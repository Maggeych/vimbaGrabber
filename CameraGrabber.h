// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#ifndef CAMERAGRABBER_H_
#define CAMERAGRABBER_H_

#include <iostream>

#include "VimbaCPP/Include/VimbaCPP.h"

#define FRAMEBUFFER_SIZE 3

class CameraOutput : virtual public AVT::VmbAPI::IFrameObserver {
 public:
  // ___________________________________________________________________________
  CameraOutput(AVT::VmbAPI::CameraPtr camera) : IFrameObserver(camera) {}
  virtual ~CameraOutput() {}

  // ___________________________________________________________________________
  void FrameReceived(const AVT::VmbAPI::FramePtr frame) {
    std::cout << "Got a frame!" << std::endl;
    m_pCamera->QueueFrame(frame);
  }

};

class CameraGrabber {
 public:
  // ___________________________________________________________________________
  CameraGrabber();
  ~CameraGrabber();

  // ___________________________________________________________________________
  // Initialize the grabber with a camera pointer. This opens the given camera
  // and sets all parameters.
  void init(AVT::VmbAPI::CameraPtr cameraPtr);

  // ___________________________________________________________________________
  // Start/stop image acquisition.
  inline void startAcquisition() const {
    if (startAcquisitionF->RunCommand() != VmbErrorSuccess)
      throw std::runtime_error("Couldn't start acquisition!");
  }
  inline void stopAcquisition() const {
    if (stopAcquisitionF->RunCommand() != VmbErrorSuccess)
      throw std::runtime_error("Couldn't stop acquisition!");
  }

  // ___________________________________________________________________________
  // Set the given feature to its maximum value that is divisable by two. Used
  // to set an even frame resolution.
  void setMaxValueModulo2(const char* const& featureName);

  // ___________________________________________________________________________
  AVT::VmbAPI::CameraPtr camera;  // The camera pointer.
  AVT::VmbAPI::FramePtrVector frames;  // Frame buffers.
  AVT::VmbAPI::FeaturePtr startAcquisitionF;  // Run this to start acquisition.
  AVT::VmbAPI::FeaturePtr stopAcquisitionF;  // Run this to stop acquisition.
};

#endif  // CAMERAGRABBER_H_
