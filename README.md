
                                               Lenovo Legion 
                                                    
Unofficial control application for Legion Pro 7 16IRX9H with Core™ i9-14900HX and NVIDIA GeForce RTX 4090 GPU.


This project is not affiliated with Lenovo in any way
Lenovo Legion brings additional drivers and tools for Legion Pro 7 16IRX9H with Core™ i9-14900HX and NVIDIA GeForce RTX 4090 GPU to Linux. It is the alternative to Lenovo Vantage or Legion Zone (both Windows only).
It allows you to control features like the fan curve, power mode, power limits,  and more. This has been achieved through reverse engineering and disassembling the ACPI firmware, as well as the firmware and memory of the embedded controller (EC).


🚀 Features

![image](https://github.com/user-attachments/assets/4864c3d2-06b6-4261-afde-6bc229db9e01)
![image](https://github.com/user-attachments/assets/660fb791-4e77-41b0-8ed3-cda80b5ccfdd)
![image](https://github.com/user-attachments/assets/8f44be6f-4f90-49d0-9218-9f53bb4cda57)

- GUI replacement for Lenovo Vantage: Fan control (Fan curve), Power Control (GPU, CPU)
- Set a fully featured custom fan curve with up to 10 points
- Set temperature points for when the fan speed (level)
- Use CPU, GPU to control the fan all at the same it
- Set the fan speed (RPM) at each level
- Switch power mode (quiet, balanced, performance, custom) using software
- Changing with Fn+Q is also possible
- Switch between different fan profiles depending on the power profile
- Monitor fan speeds, power consumtion and temperatures (CPU, GPU)
- Frequency Control
- CPU Control enable/disable SMT enable cores and set governor

📣 Overview
- it comes with a driver lenovo_legion (kernel module) that implements the Linux standard interfaces (sysfs, hwmon)
- it comes with a LenovoLegion-Daemon (daemon under root privileges) the provide for LenovoLegion (gui part) data
- it comes with a LenovoLegion (GUI) the provide UI for the user

📦 Available Packages

Arch Base Distros:
  - 

📌 Confirmed Compatible Models
  - Legion Pro 7 16IRX9H with Core™ i9-14900HX and NVIDIA GeForce RTX 4090 GPU.

💡 Instructions

Please do the following:

  -  follow installation instructions

Requirements

You will need to install the following to download and build it. If there is an error of any package, find the alternative name in your distribution and install them.

"linux-headers" "qt6-base" "qt6-5compat" "dkms" "make" "gcc" "qt6-charts" 

Arch/Manjaro/EndeavourOS
  sudo pacman -S linux-headers qt6-base qt6-5compat dkms make gcc qt6-charts 
  sudo pacman -S dkms openssl mokutil

Build

For all distribution

1) Download [https://github.com/bolekjar/LenovoLegion/releases/tag/release%2F1.0.0](https://github.com/bolekjar/LenovoLegion/archive/refs/tags/release/1.0.0.tar.gz) and unpack
2) Go to LenovoLegion-release-1.0.0/LenovoLegion-dkms
3) run make
4) Kernel module is lenovo_legion.ko
5) If you want to install lenovo_legion.ko:
   - for current kernel: run make install
   - like dkms: run make dkms
   - you should see in dmesg for sucessfully installed and detected supported laptop this:
   - kernel: lenovo-legion VPC2004:00: Legion Pro 7 16IRX9H platform driver probing:
   - kernel: lenovo-legion VPC2004:00:         Read identifying information: DMI_SYS_VENDOR: LENOVO; DMI_PRODUCT_NAME: 83DE; DMI_BIOS_VERSION:N2CN27WW
   - kernel: lenovo-legion VPC2004:00:         Supported model
   - kernel: lenovo-legion VPC2004:00:         Platform profile interface was created
   - kernel: lenovo-legion VPC2004:00:         WMI interface was created
   - kernel: lenovo-legion VPC2004:00:         SysFs interface was created
   - kernel: lenovo-legion VPC2004:00:         HWMon interface was created
   - kernel: lenovo-legion VPC2004:00: Legion Pro 7 16IRX9H platform driver was loaded   
7) Go to LenovoLegion-release-1.0.0/LenovoLegion
8) run (qmake or qmake6) && make
9) The application is in folder LenovoLegion-release-1.0.0/LenovoLegion/Installation

Arch/Manjaro/EndeavourOS
- download package from aur or install with yay utility

 




