##!/bin/sh

ddi="$(find /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/DeviceSupport 2>/dev/null | grep "8.4/.*.dmg$" || echo './data/DeveloperDiskImage.dmg' | head -1)"

### Ddi mount function ###

function mount_ddi(){
echo "Mounting DDI..."
./bin/ideviceimagemounter "$ddi" >/dev/null || echo "Couldn't mount DDI. Not an issue if Xcode's running, an issue if it isn't."
}

function fetchsymbols_armv7()
{
  ./bin/fetchsymbols -f "$(./bin/fetchsymbols -l 2>&1 | (grep armv7) | tr ':' '\n'|tr -d ' '|head -1)" ./tmp/cache
}

function make_run()
{
  #cd ./data/dyldmagic_amfid
  #./make.sh
  #cd ../../
  #./bin/afcclient put ./data/dyldmagic_amfid/magic64_amfid.dylib PhotoData/KimJongCracks/Library/PrivateFrameworks/GPUToolsCore.framework/GPUToolsCore
  # just test...
  cd ./data/dyldmagic
  ./make-run.sh
  cd ../../
}

mount_ddi && make_run&&fetchsymbols_armv7
