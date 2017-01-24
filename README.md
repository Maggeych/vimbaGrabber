# vimbaGrabber
Simple grabber recording **synchronized videos** from all connected [Allied Vision Technologies](https://www.alliedvision.com/) USB 3.0 cameras using the Vimba SDK.
## Building on Ubuntu 16.04
0. Install build essentials: `sudo apt-get install build-essential`.
1. Install video encoder dependencies: `sudo apt-get install libavcodec-dev libswscale-dev libavdevice-dev libavfilter-dev libavformat-dev`.
2. Install the [VimbaSDK](https://www.alliedvision.com/en/products/software.html). **IMPORTANT: Make shure to install both the GigE and USB3 drivers or the USB 3.0 camera discovery will be broken!**
3. Modify *VIMBASDK_DIR* in `build/Makefile` to point to the SDK's extraction folder. Alternatively you can create a symbolic link to the extracted SDK archive in this projects root folder: `ln -s /path/to/Vimba_2_0 /path/to/vimbaGrabber`.
4. Start the build with `cd build; make`. The executable will be in `build/binary/<architecture>/vimbaGrabber`.
