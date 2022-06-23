rtw8852be
===========
### A repo for the newest Realtek wifi device.

This repository includes drivers for the following card:

RTW8852BE

If you are looking for a driver for chips such as 
RTL8188EE, RTL8192CE, RTL8192CU, RTL8192DE, RTL8192EE, RTL8192SE, RTL8723AE, RTL8723BE, or RTL8821AE,
these should be provided by your kernel. If not, then you should go to the Backports Project
(https://backports.wiki.kernel.org/index.php/Main_Page) to obtain the necessary code.

If you see a line such as:
make[1]: *** /lib/modules/5.17.5-300.fc36.x86_64/build: No such file or directory. Stop.
that indicates that you have NOT installed the kernel headers. Use the following instructions for that step.

### Installation instruction
##### Requirements
You will need to install "make", "gcc", "kernel headers", "kernel build essentials", and "git".
You can install them with the following command, on **Ubuntu**:
```bash
sudo apt-get update
sudo apt-get install make gcc linux-headers-$(uname -r) build-essential git
```
If any of the packets above are not found check if your distro installs them like that. 

##### Installation
For all distros:
```bash
git clone git://github.com/lwfinger/rtw8852be.git
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

##### Blacklisting (needed if you want to use these modules)
Some distros provide `RTL8723DE` drivers. To use this driver, that one MUST be
blacklisted. How to do that is left as an exercise as learning that will be very beneficial.

If your system has ANY conflicting drivers installed, you must blacklist them as well. For kernels
5.6 and newer, this will include drivers such as rtw88_xxxx.
Here is a useful [link](https://askubuntu.com/questions/110341/how-to-blacklist-kernel-modules) on how to blacklist a module

Once you have reached this point, then reboot. Use the command `lsmod | grep rtw` and check if there are any
conflicting drivers. The correct ones are:
- `rtw_8723de  rtw_8723d  rtw_8822be  rtw_8822b  rtw_8822ce  rtw_8822c  rtw_core  and rtw_pci`

If you have other modules installed, see if you blacklisted them correctly.

##### How to disable/enable a Kernel module
 ```bash
sudo modprobe -rv 8852be         #This unloads the module
sudo modprobe -v 8852be            #This loads the module
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
***********************************************************************************************

When your kernel changes, then you need to do the following:
```bash
cd ~/rtw8852be
git pull
make
sudo make install
```

Remember, this MUST be done whenever you get a new kernel - no exceptions.

