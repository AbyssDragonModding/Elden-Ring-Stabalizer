## COPIED FROM NEXUS MODS DESCRIPTION

## [Building from source]
Note: This requires downloading and setting up a few development based tools to build from source yourself, it will take a couple minutes, if you rather not deal with it, just download the prebuilt versions, We will be using Winget (windows package manager) to install of these.
﻿Tools we will install: we will need to install 3 things to build from source a C++ compiler (GCC), CMake, and Git

## ﻿[Installing CMake and Git]
- Open up command prompt 
- and type in without quotes "winget install Git.Git"
- next type in "winget install Kitware.CMake"

 ## [Installing GCC]
﻿- Follow this guide: https://phoenixnap.com/kb/install-gcc-windows

## [Building the project]
- Close the command prompt window
- In explorer choose a location to ﻿clone my repository too for example your downloads folder
- On the empty space in that directory Shift + Right Click > Open In terminal
- type in git clone https://github.com/AbyssDragonModding/Elden-Ring-Stabalizer.git "EldenRingStabalizer"
the above command will use git and clone the repository into a folder named EldenRingStabalizer
- Open the cloned repository and run the "Build_Project.bat" 
﻿
﻿This will make a build directory, and tell cmake to build the project files and the DLL into that folder. The DLL is located in the following directory "./build/Release", follow the install instructions and copy that DLL into the mod folder

## [Installation]
﻿- Install Mod Engine 3 Manager (GUI for ME3): Mod Engine 3 Manager
﻿﻿- Follow the steps when you run it to install ME3
- Download the prebuilt binaries (or the ones you built from source yourself, you will need "EldenRingStabalizer.dll and "restore_ssdp_service.bat" if you want to restore the service after if needed.)
- Extract this dll and the bat file (to restore the service if needed in the future) into a subdirectory in your mods folder
- In ME3 UI add a new "External DLL Mod" (4th icon next to the Launch Elden Ring button that has a little cog on it)
﻿- Navigate to your mod folder for EldenRingStabalizer and click on EldenRingStabalizer.dll
﻿- Make sure the mod is enabled
- Close ME3 Manager and restart it with administrator privileges (the functions in the windows API need admin rights to disable the service, If you don't run with admin rights only the CPU affinity will be set)
- Launch the game and everything will be applied.


## [Q.A]
Q: What does the code actually do under the hood?
A: In non programmer terms. When you run the game with the DLL attached, it will start a new CPU thread process and wait 1 second before processing any changes (otherwise it will crash the game immediately). It will then set the CPU Affinity settings (disable core 0 for ER) and disable the SSDPSRV service. It will then check every 2 minutes if the service somehow got started again (I noticed it randomly happens sometimes because if windows requests it, it will restart the service) and if it is running again it will disable it again.
