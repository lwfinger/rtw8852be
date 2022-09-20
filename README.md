# rtw8852be
Linux driver for RTW8852BE PCIe card

---
The Bluetooth and Wifi devices are **separate interfaces on the same chip**. 
 more information is available [here](https://wikidevi.wi-cat.ru/Realtek_RTL8852BE_Combo_Module).


for WiFi: https://github.com/lwfinger/rtw8852be

for Bluetooth: https://github.com/lwfinger/rtw89-BT

---

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
For **Arch**: Install the necessary kernel headers and base-devel,

If any of the packages above are not found check how your distro installs them.

##### Requirements
```bash
build-essential 
linux-headers
bc
gcc
make
git
```
##### Installation
For all distros:
```bash
git clone https://github.com/lwfinger/rtw8852be.git
cd rtw8852be
make
sudo make install

```
##### Installation with module signing for SecureBoot
For all distros:
```bash
git clone git://github.com/lwfinger/rtw8852be.git
cd rtw8852be
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

##### How to disable/enable a Kernel module
 ```bash
sudo modprobe -rv 8852be         #This unloads the module
sudo modprobe -v 8852be          #This loads the module
```

##### Option configuration
If it turns out that your system needs one of the configuration options, then do the following:
```bash
sudo nano /etc/modprobe.d/<dev_name>.conf 
```
There, enter the line below:
```bash
options <device_name> <<driver_option_name>>=<value>
```
This driver has many options as you will see with 'modinfo 8852be'. Thus far, no one has
needed to use any of them.

***********************************************************************************************

When your kernel changes, then you need to do the following:
```bash
cd ~/rtw8852be
git pull
make
sudo make install
```

Remember, this MUST be done whenever you get a new kernel - no exceptions.

##### Installation with module signing for SecureBoot
For all distros:
```bash
git clone git://github.com/lwfinger/rtw8852be.git
cd rtw8852be
make
sudo make sign-install
```
You will be promted for a password, please keep it in mind and use it in next steps.

Reboot to activate the new installed module.
In the MOK managerment screen:
1. Select "Enroll key" and enroll the key created by above sign-install step
2. When promted, enter the password you entered when create sign key. 

If you enter wrong password, your computer won't not rebootable. In this case,
   use the BOOT menu from your BIOS, to boot into your OS then do below steps:

```bash
sudo mokutil --reset
```
Restart your computer
Use BOOT menu from BIOS to boot into your OS
In the MOK managerment screen, select reset MOK list
Reboot then retry from the step make sign-install

These drivers will not build for kernels older than 4.14. If you must use an older kernel,
submit a GitHub issue with a listing of the build errors. Without the errors, the issue
will be ignored. I am not a mind reader.

# DKMS packaging for ubuntu/debian

DKMS on debian/ubuntu simplifies the secure-boot issues, signing is
taken care of through the same mechanisms as nVidia and drivers.  You
won't need special reboot and MOK registration.

Additionally DKMS ensures new kernel installations will automatically
rebuild the driver, so you can accept normal kernel updates.

Prerequisites:

A few packages are required to build the debs from source:

``` bash
sudo apt install dkms debhelper dh-modaliases
```

Build and installation

```bash
# If you've already built as above clean up your workspace or check one out specially (otherwise some temp files can end up in your package)
git clean -xfd

dpkg-buildpackage -us -uc
sudo apt install ../rtw8852be-dkms_1.0.0_all.deb
```

That should install the package, and build the module for your
currently active kernel.  You should then be able to `modprobe` as
above.
