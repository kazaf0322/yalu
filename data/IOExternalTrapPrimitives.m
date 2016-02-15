#ifndef IOExternalTrapPrimitives_h
#define IOExternalTrapPrimitives_h
#import <IOKit/IOKitLib.h>
#import "mach/vm.h" // run cat mach/vm.h to see if this is a real file.
//the exploit hinted towereds it.
#import "YaluKernel.h"
struct YaluPrimitives {
    uint32_t (*read32)(struct YaluPrimitives*, vm_address_t);
    void (*write32)(struct YaluPrimitives*, vm_address_t, uint32_t);
    uint64_t (*read64)(struct YaluPrimitives*, vm_address_t);
    void (*write64)(struct YaluPrimitives*, vm_address_t, uint64_t);
    void (*invalidateTLB)(struct YaluPrimitives*);
    void (*flushCache)(struct YaluPrimitives*);
    void * ref;
};
void IOExternalTrapPrimitivesFromKernelAndSmashedClient(struct YaluPrimitives* primitives, struct YaluKernel* kernel, io_connect_t smashed);
#endif /* IOExternalTrapPrimitives_h */


//
//  IOExternalTrapPrimitives.m
//  Copyright © 2016 qwertyoruiop. All rights reserved.
