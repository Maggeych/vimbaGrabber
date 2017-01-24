// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#include <iostream>
#include <sstream>
#include <iomanip>
#include <exception>

#include "VimbaCPP/Include/VimbaCPP.h"
#include "CameraGrabber.h"
#include "FFmpegOutput.h"

int width = 640;
int height = 480;
std::string crf = "0";  // 0 means lossless. The higher the lossier.

int main(int argc, char* argv[]) {
  int fps = 24;
  std::string pixelFormat = "Mono8";

  int ret = EXIT_SUCCESS;
  // Startup Vimba.
  AVT::VmbAPI::VimbaSystem& system = AVT::VmbAPI::VimbaSystem::GetInstance();
  try {
    // Needed in order to set ffmpegs mutexes. We use Vimba in asynchronous
    // mode, so frames are received from different threads. Therefore ffmpeg
    // needs to be multi-thread safe and we need to init its mutexes.
    FFmpegOutput::initFFmpeg();

    if (system.Startup() != VmbErrorSuccess)
      throw std::runtime_error("Could not start VimbaSystem!");

    // Discover cameras.
    AVT::VmbAPI::CameraPtrVector cameras;
    if (system.GetCameras(cameras) != VmbErrorSuccess)
      throw std::runtime_error("Could not get the camera list!");

    // At least one camera is required.
    std::cout << "Found " << cameras.size() << " Camera(s)." << std::endl;
    if (cameras.empty()) throw std::runtime_error("No cameras connected!");

    // Initialize every camera. This takes a while, which is why we call
    // startAcquisition separately.
    std::vector<AVT::VmbAPI::IFrameObserverPtr> receivers(cameras.size());
    std::vector<CameraGrabber> grabbers(cameras.size());
    for (size_t i = 0; i < cameras.size(); ++i) {
      // Generate the output filename.
      std::ostringstream fn;
      fn << "vimba_camera" << std::setfill('0') << std::setw(2) << i << ".h264";

      // Needed to tell libav how to read in the raw camera frame data.
      AVPixelFormat framePixFmtInAvCodec;
      int frameLineSize[3] = {0, 0, 0};
      if (pixelFormat == "Mono8") {
        framePixFmtInAvCodec = AV_PIX_FMT_GRAY8;
        frameLineSize[0] = width;
      } else {
        throw std::runtime_error("Don't know how to read in camera pixel "
            "format in ffmpeg!");
      }

      // Create the ffmpeg output instance.
      receivers[i] =
        AVT::VmbAPI::IFrameObserverPtr(new FFmpegOutput(fn.str(), width, height,
              fps, crf, cameras[i], framePixFmtInAvCodec, frameLineSize));

      // Initialize the camera controller.
      grabbers[i].init(cameras[i], pixelFormat, width, height, fps, receivers[i]);
    }

    // Start recording. These calls are super fast, so we assume this guarantees
    // synchronized images.
    for (size_t i = 0; i < cameras.size(); ++i) grabbers[i].startAcquisition();

    // Wait for <enter>.
    std::cout << "Recording. Press <enter> to stop..." << std::endl;
    getchar();

    // Stop recording.
    std::cout << "Stopping..." << std::endl;
    for (size_t i = 0; i < cameras.size(); ++i) grabbers[i].stopAcquisition();
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    ret = EXIT_FAILURE;
  }

  // Cleanup.
  FFmpegOutput::closeFFmpeg();
  system.Shutdown();

  return ret;
}
