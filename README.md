rtw89 - branch bluetooth

===========
### A repo for the newest Realtek bluetooth codes.

This branch was created from the code in the 5.19 kernel plus the modifications
to the USB_DEVICE tables noted in the rtw89 - wifi issues. These changes will be
submitted to the kernel, but this branch is established to avoid complete builds
of a new kernel.


The drivers btusb.ko and btrtl.ko replace those of the kernel. No blacklisting will
be needed. 

It is not known what versions of older kernels are supported. If you get build errors,
please report them and I will try to fix any incompatibilites.

This repository includes drivers for the USB/Bluetooth part of the following cards:

Realtek 8852AE, 8852BE, and 8852CE

### Installation instruction
##### Requirements
You will need to install "make", "gcc", "kernel headers", "kernel build essentials", and "git".

For **Ubuntu**: You can install them with the following command
```bash
sudo apt-get update
sudo apt-get install make gcc linux-headers-$(uname -r) build-essential git
```
For **Fedora**: You can install them with the following command
```bash
sudo dnf install kernel-headers kernel-devel
sudo dnf group install "C Development Tools and Libraries"
```
For **openSUSE**: Install necessary headers with
```bash
sudo zypper install make gcc kernel-devel kernel-default-devel git libopenssl-devel
```
##### Installation
For all distros:
```bash
git clone git://github.com/lwfinger/rtw89.git -b bluetooth
cd rtw89
make
sudo make install
```

##### Installation with module signing for SecureBoot
For all distros:
```bash
git clone git://github.com/lwfinger/rtw89.git -b bluetooth
cd rtw89
make
sudo make sign-install
```
You will be promted a password, please keep it in mind and use it in next steps.
Reboot to activate the new installed module.
In the MOK managerment screen:
1. Select "Enroll key" and enroll the key created by above sign-install step
2. When promted, enter the password you entered when create sign key. 
3. If you enter wrong password, your computer won't not bebootable. In this case,
   use the BOOT menu from your BIOS, to boot into your OS then do below steps:
```bash
sudo mokutil --reset
```
Restart your computer
Use BOOT menu from BIOS to boot into your OS
In the MOK managerment screen, select reset MOK list
Reboot then retry from the step make sign-install

***********************************************************************************************

When your kernel changes, then you need to do the following:
```bash
cd ~/rtw89
git pull
make clean
make
sudo make install
;or
sudo make sign-install
```

Remember, this MUST be done whenever you get a new kernel - no exceptions.

