// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#include "CameraGrabber.h"

// _____________________________________________________________________________
CameraGrabber::CameraGrabber() : frames(FRAMEBUFFER_SIZE)/*, camera(NULL) */ {
}

// _____________________________________________________________________________
CameraGrabber::~CameraGrabber() {
  std::cout << "Deinitialize..." << std::endl;
  if (camera != NULL) {
    camera->EndCapture();
    camera->FlushQueue();
    camera->RevokeAllFrames();
    camera->Close();
  }
}

// _____________________________________________________________________________
void CameraGrabber::init(AVT::VmbAPI::CameraPtr cameraPtr) {
  camera = cameraPtr;

  VmbErrorType result;

  // Get the id.
  std::string cameraId;
  if (camera->GetID(cameraId) != VmbErrorSuccess)
    throw std::runtime_error("Couldn't get the camera id!");

  // Get the camera's name.
  std::string cameraName;
  if (camera->GetName(cameraName) != VmbErrorSuccess)
    throw std::runtime_error("Couldn't get the camera's name!");

  // Open the camera.
  if (camera->Open(VmbAccessModeRead) != VmbErrorSuccess)
    throw std::runtime_error("Couldn't open the camera!");

  std::cout
    << "Opened camera " << cameraId << " (" << cameraName << ")"
    << std::endl;

  // Set to the maximum resolution that is divisible by 2.
  setMaxValueModulo2("Width");
  setMaxValueModulo2("Height");

  // Get the frame allocation size.
  AVT::VmbAPI::FeaturePtr feature;
  VmbInt64_t framesize;
  if (
      camera->GetFeatureByName("PayloadSize", feature) != VmbErrorSuccess ||
      feature->GetValue(framesize) != VmbErrorSuccess
     )
    throw std::runtime_error("Couldn't get the payload size!");

  AVT::VmbAPI::IFrameObserverPtr observer(new CameraOutput(camera));
  // Prepare the frames.
  for (AVT::VmbAPI::FramePtrVector::iterator f = frames.begin();
      f != frames.end(); f++) {
    // Allocate the frame.
    (*f).reset(new AVT::VmbAPI::Frame(framesize));

    // Register the observer for the frame.
    if ((*f)->RegisterObserver(observer) != VmbErrorSuccess)
      throw std::runtime_error("Couldn't register the observer!");

    // Assign the frame to the camera.
    if (camera->AnnounceFrame(*f) != VmbErrorSuccess)
      throw std::runtime_error("Couldn't announce the frame!");
  }

  // Start the camera's capture engine.
  if (camera->StartCapture() != VmbErrorSuccess)
    throw std::runtime_error("Couldn't start capturing!");

  // Queue all frames for data acquisition.
  for (AVT::VmbAPI::FramePtrVector::iterator f = frames.begin();
      f != frames.end(); f++)
    if (camera->QueueFrame(*f) != VmbErrorSuccess)
      throw std::runtime_error("Couldn't queue the frame!");

  // Get the feature pointers to start and stop the acquisition.
  if (
      camera->GetFeatureByName("AcquisitionStart", startAcquisitionF) != VmbErrorSuccess ||
      camera->GetFeatureByName("AcquisitionStop", stopAcquisitionF) != VmbErrorSuccess
     )
    throw std::runtime_error("Couldn't get acquisition start/stop features!");
}

// _____________________________________________________________________________
void CameraGrabber::setMaxValueModulo2(const char* const& featureName) {
  AVT::VmbAPI::FeaturePtr feature; VmbInt64_t min, max;

  if (
      camera->GetFeatureByName(featureName, feature) != VmbErrorSuccess ||
      feature->GetRange(min, max) != VmbErrorSuccess
     )
    throw std::runtime_error(featureName);

  max = (max >> 1) << 1;  // Erase the last bit.

  if (feature->SetValue(max) != VmbErrorSuccess)
    throw std::runtime_error(featureName);

  std::cout << "Set \"" << featureName << "\" to " << max << std::endl;
}
