#!/bin/bash

# UTILITY SCRIPT FOR BUILDING KERNEL AND RUNNING IT WITH QEMU

cd kernel
make
cd ..

cp kernel/kernel.bin sysroot/boot
grub-mkrescue -o Carbon-x86.iso sysroot

qemu-system-i386 -cdrom Carbon-x86.iso