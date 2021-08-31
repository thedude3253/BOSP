@echo off
wsl make
qemu-system-x86_64 -drive format=raw,file=./out/file.img,index=0,if=floppy -drive if=floppy -soundhw pcspk