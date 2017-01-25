# vimbaGrabber
Simple grabber recording **synchronized videos** from all connected [Allied Vision Technologies](https://www.alliedvision.com/) USB 3.0 cameras using the Vimba SDK. It outputs the raw lossless h264 encoded video stream. In case your program can't read the raw frame data you need to pack it into a video container using ffmpeg. 
For example in order to pack a proper Matroska (*.mkv) container without reencoding run (the argument order is important!): 
`ffmpeg -i testrecording_run01_camera01.h246 -codec copy testrecording_run01_camera01.mkv`
## Building on Ubuntu 16.04
0. Install build essentials: `sudo apt-get install build-essential`.
1. Install video encoder dependencies: `sudo apt-get install libavcodec-dev libswscale-dev libavdevice-dev libavfilter-dev libavformat-dev`.
2. Install the [VimbaSDK](https://www.alliedvision.com/en/products/software.html). **IMPORTANT: Make shure to install both the GigE and USB3 drivers or the USB 3.0 camera discovery will be broken!**
3. Modify *VIMBASDK_DIR* in `build/Makefile` to point to the SDK's extraction folder. Alternatively you can create a symbolic link to the extracted SDK archive in this projects root folder: `ln -s /path/to/Vimba_2_0 /path/to/vimbaGrabber`.
4. Start the build with `cd build; make`. The executable will be in `build/binary/<architecture>/vimbaGrabber`.

## Usage:
<pre>Usage: vimbaGrabber ([options]) <output label>
  Output synchronized videos from every connected vimba camera.
  The output files will be placed in the current folder and 
  named "&lt;output label&gt;_runXX_cameraXX.h264".

Options:
  -w <width>, --width <width>:    Set the cameras and output videos to this width
                                   default: 640
  -h <height>, --height <height>: Set the cameras and output videos to this height
                                   default: 480
  -f <fps>, --fps <fps>:          Set the cameras and output videos to this fps
                                   default: 24
  -c <crf>, --crf <crf>:          Set the h246's constant rate factor, the higher the lossier
                                   default: 0 (lossless)
                                  </pre>
