
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147
148
149
150
151
152
153
	
#!/bin/sh
 
### Initial vars ###
 
SCRIPTPATH=`dirname $0`
ddi="$(find /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/DeviceSupport 2>/dev/null | grep "9.2/.*.dmg$" || echo './data/DeveloperDiskImage.dmg' | head -1)"
 
cd $SCRIPTPATH
 
### Functions ###
 
function abort() {
echo "Error. Exiting..." >&2
exit 254;
}
 
 
### Ddi mount function ###
 
function mount_ddi(){
echo "Mounting DDI..."
./bin/ideviceimagemounter "$ddi" >/dev/null || echo "Couldn't mount DDI. Not an issue if Xcode's running, an issue if it isn't."
}
 
### Device detection function ###
 
function wait_for_device() {
echo "Waiting for device..."
while ! (./bin/afcclient deviceinfo | grep -q FSTotalBytes); do sleep 5; done 2>/dev/null
}
 
### Jailbreak Functions ###
# Stage 1:
# set-up environment, install app and swap binaries
 
function stage1(){
echo "Setting up environment..."
 
./bin/afcclient put ./data/WWDC_Info_TOC.plist /yalu.plist | grep Uploaded || abort
 
echo "
Installing app & swapping binaries..."
 
./bin/mobiledevice install_app ./data/WWDC-TOCTOU.ipa || abort
 
echo "
Please wait..."
 
sleep 5
./bin/afcclient put ./data/WWDC_Info_TOU.plist /yalu.plist | grep Uploaded || abort
 
echo
}
 
# Stage 0:
# Important stuff
 
stage0() {
echo "DISABLE FIND MY PHONE"
# Waiting for device
wait_for_device
 
echo "Recreating temp directory..."
rm -rf tmp
mkdir tmp
 
(
echo "Creating dirs on device">&2
./bin/afcclient mkdir PhotoData/KimJongCracks
./bin/afcclient mkdir PhotoData/KimJongCracks/a
./bin/afcclient mkdir PhotoData/KimJongCracks/a/a
./bin/afcclient mkdir PhotoData/KimJongCracks/Library
./bin/afcclient mkdir PhotoData/KimJongCracks/Library/PrivateFrameworks
./bin/afcclient mkdir PhotoData/KimJongCracks/Library/PrivateFrameworks/GPUToolsCore.framework
 
# Stage 1
stage1 || abort
 
# Backup device data
 
echo "Backing up, could take several minutes..." >&2
./bin/idevicebackup2 backup tmp || abort
udid="$(ls tmp | head -1)"
 
echo "Mounting ddi and copying files to backup directory">&2
 
mkdir tmp_ddi
hdiutil attach -nobrowse -mountpoint tmp_ddi "$ddi"
cp tmp_ddi/Applications/MobileReplayer.app/MobileReplayer tmp/MobileReplayer
cp tmp_ddi/Applications/MobileReplayer.app/Info.plist tmp/MobileReplayerInfo.plist
hdiutil detach tmp_ddi
rm -rf tmp_ddi
 
echo "Compiling and copying binary file to device...">&2
 
lipo tmp/MobileReplayer -thin armv7s -output ./tmp/MobileReplayer
./bin/mbdbtool tmp $udid CameraRollDomain rm Media/PhotoData/KimJongCracks/a/a/MobileReplayer
./bin/mbdbtool tmp $udid CameraRollDomain put ./tmp/MobileReplayer Media/PhotoData/KimJongCracks/a/a/MobileReplayer || abort
)
 
# Restore modified backup
echo "Restoring modified backup..."
(
./bin/idevicebackup2 restore tmp --system --reboot || abort
)>/dev/null
 
# ZZZZZZ....
echo "Sleeping until device reboot..."
sleep 20
 
# Wait for device
wait_for_device
read -p "Press [Enter] key when your device finishes restoring."
echo
 
# Mount ddi
mount_ddi
 
echo "Fetching symbols..."
./bin/fetchsymbols -f "$(./bin/fetchsymbols -l 2>&1 | (grep dyld$ || abort ) | tr ':' '\n'|tr -d ' '|head -1)" ./tmp/dyld.fat
lipo -info ./tmp/dyld.fat | grep arm64 >/dev/null && ./bin/fetchsymbols -f "$(./bin/fetchsymbols -l 2>&1 | (grep arm64 || abort ) | tr ':' '\n'|tr -d ' '|head -1)" ./tmp/cache64
./bin/fetchsymbols -f "$(./bin/fetchsymbols -l 2>&1 | (grep armv7 || abort ) | tr ':' '\n'|tr -d ' '|head -1)" ./tmp/cache
 
echo "Compiling jailbreak files..."
cd tmp
lipo -info dyld.fat | grep arm64 >/dev/null &&  lipo dyld.fat -thin arm64 -output dyld64
lipo -info dyld.fat | grep Non-fat >/dev/null || (lipo dyld.fat -thin "$(lipo -info dyld.fat | tr ' ' '\n' | grep v7)" -output dyld; mv dyld dyld.fat) && mv dyld.fat dyld
$SCRIPTPATH./bin/jtool -e IOKit cache
$SCRIPTPATH./bin/jtool -e libsystem_kernel.dylib cache
lipo -info dyld64 | grep arm64 >/dev/null && (
$SCRIPTPATH./bin/jtool -e libdyld.dylib cache64
cd $SCRIPTPATH./data/dyldmagic_amfid
./make.sh
cd ../..
)
cd $SCRIPTPATH./data/dyldmagic
./make.sh
 
echo "Copying files to device..."
cd ../../
./bin/afcclient put ./data/dyldmagic/magic.dylib PhotoData/KimJongCracks/Library/PrivateFrameworks/GPUToolsCore.framework/GPUToolsCore
./bin/afcclient put ./data/untether/untether drugs
gzcat ./data/bootstrap.tgz > ./tmp/bootstrap.tar
./bin/afcclient put ./tmp/bootstrap.tar PhotoData/KimJongCracks/bootstrap.tar
./bin/afcclient put ./data/tar PhotoData/KimJongCracks/tar
 
echo "Tap on the jailbreak icon to crash the kernel (or 0wn it if you're in luck!)"
}
 
# Let's do this!
stage0 || abort
 
exit 0
