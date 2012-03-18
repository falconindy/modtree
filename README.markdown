# modtree

modtree is a dependency graph viewer for kernel modules. It borrows the excellent
tree table (tt) library from util-linux, and various pieces of code from kmod.

    $ modtree aes
    NAME            PATH
    padlock_aes     /lib/modules/3.2.11-1-ARCH/kernel/drivers/crypto/padlock-aes.ko.gz
    └─aes_generic   /lib/modules/3.2.11-1-ARCH/kernel/crypto/aes_generic.ko.gz
    aes_generic     /lib/modules/3.2.11-1-ARCH/kernel/crypto/aes_generic.ko.gz
    aesni_intel     /lib/modules/3.2.11-1-ARCH/kernel/arch/x86/crypto/aesni-intel.ko.gz
    ├─cryptd        /lib/modules/3.2.11-1-ARCH/kernel/crypto/cryptd.ko.gz
    ├─aes_x86_64    /lib/modules/3.2.11-1-ARCH/kernel/arch/x86/crypto/aes-x86_64.ko.gz
    │ └─aes_generic /lib/modules/3.2.11-1-ARCH/kernel/crypto/aes_generic.ko.gz
    └─aes_generic   /lib/modules/3.2.11-1-ARCH/kernel/crypto/aes_generic.ko.gz
    aes_x86_64      /lib/modules/3.2.11-1-ARCH/kernel/arch/x86/crypto/aes-x86_64.ko.gz
    └─aes_generic   /lib/modules/3.2.11-1-ARCH/kernel/crypto/aes_generic.ko.gz
