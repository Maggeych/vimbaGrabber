# vimbaGrabber
Simple grabber recording **synchronized videos** from all connected Vimba cameras.
## Building
1. Install the [VimbaSDK](https://www.alliedvision.com/en/products/software.html).
2. Modify **VIMBASDK_DIR** in `build/Makefile` to point to the SDK's installation folder. Alternatively you can extract the SDK archive to this projects root folder.
3. Start the build with `cd build; make`. The executable will be in `build/binary/<architecture>/vimbaGrabber`.
