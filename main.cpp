// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#include <iostream>
#include <exception>

#include "VimbaCPP/Include/VimbaCPP.h"
#include "CameraGrabber.h"

int main(int argc, char* argv[]) {
  // Startup Vimba.
  AVT::VmbAPI::VimbaSystem& system = AVT::VmbAPI::VimbaSystem::GetInstance();
  try {
    if (system.Startup() != VmbErrorSuccess)
      throw std::runtime_error("Couldn't start VimbaSystem!");

    // Discover cameras.
    AVT::VmbAPI::CameraPtrVector cameras;
    if (system.GetCameras(cameras) != VmbErrorSuccess)
      throw std::runtime_error("Couldn't get the camera list!");

    // At least one camera is required.
    std::cout << "Found " << cameras.size() << " Camera(s)." << std::endl;
    if (cameras.empty()) throw std::runtime_error("No cameras connected!");

    // Initialize every camera. This takes a while, which is why we call
    // startAcquisition separately.
    std::vector<CameraGrabber> grabbers(cameras.size());
    for (size_t i = 0; i < cameras.size(); ++i) grabbers[i].init(cameras[i]);

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
    system.Shutdown();
    return EXIT_FAILURE;
  }

  system.Shutdown();
  return EXIT_SUCCESS;
}
