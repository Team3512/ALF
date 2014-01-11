# ALF - Application Loading Framework

The Application Loading Framework (ALF) provides us with a means to remotely reboot the cRIO as well as reload our robot code's kernel module without having to reboot the cRIO. Being able to update the currently running C++ code without waiting for the cRIO to reboot drastically speeds up our development cycle and increases our productivity.


## Dependencies

There are no dependencies for this module. It can be built with either Wind River or the [C++11 Toolchain](http://firstforge.wpi.edu/sf/projects/c--11_toochain) as both builds function properly.


Getting Started
---------------
Follow these steps to get ALF running on your FIRST FRC cRIO controller.

### Compile the Software

This software has been tested using both the FIRST FRC Wind River Workbench (GCC 3.4.4 for powerpc-wrs-vxworks) toolchain and the new [GCC 4.8.2 toolchain for powerpc-wrs-vxworks](http://firstforge.wpi.edu/sf/projects/c--11_toochain). Makefiles for the latter are included in the project. The project can be built in Wind River by importing the project's folder into a workspace and selecting "Build Project" from the "Project" drop-down menu.

### Upload the Binary

Building the software will generate a binary with the extension ".out" . This binary should uploaded by FTP to the path "/c/ni-rt/system/ALF.out" on the cRIO controller.

### Edit ni-rt.ini

An entry must also be added into the "/c/ni-rt.ini" configuration file on the cRIO controller for the module to be loaded at startup. The ni-rt.ini file should be downloaded, edited, and the modified version uploaded. The file should be modified as follows:

Locate the "LVRT" section.
```
[LVRT]
StartupDLLs="debug.o;nisysrpc.out;NiRioRpc.out;nivissvc.out;nivision.out;visa32.out;niserial.out;NiFpgaLv.out;FRC_FPGA.out;FRC_NetworkCommunication.out;FRC_UserProgram.out;"
```

Add ALF.out to the StartupDLLs value.
```
StartupDLLs="debug.o;nisysrpc.out;NiRioRpc.out;nivissvc.out;nivision.out;visa32.out;niserial.out;NiFpgaLv.out;FRC_FPGA.out;FRC_NetworkCommunication.out;ALF.out;FRC_UserProgram.out;"
```


### Restart the System

ALF should begin running at the system's next restart.


## Usage

To call commands from ALF, one can connect to the robot on port 3512 using any Telnet client, such as [PuTTY](http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html). The following commands are supported:

    1) reboot
        * Reboots the cRIO
    2) reload
        * Reloads the currently running robot code kernel module
    3) save
        * Saves the currently running robot code kernel module for falling back to it later
    4) fallback
        * Loads the robot code kernel module previously saved by ALF's 'save' command
    5) listTasks
        * Lists the tasks currently running on the cRIO with some extra information

