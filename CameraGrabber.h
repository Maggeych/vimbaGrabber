// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#ifndef CAMERAGRABBER_H_
#define CAMERAGRABBER_H_

#include <iostream>

#include "VimbaCPP/Include/VimbaCPP.h"

class CameraGrabber {
 public:
  // ___________________________________________________________________________
  CameraGrabber();
  ~CameraGrabber();

  // ___________________________________________________________________________
  // Initialize the grabber with a camera pointer and a frame receiver.
  // This opens the given camera and sets all parameters.
  void init(AVT::VmbAPI::CameraPtr cam, std::string pixelFormat,
      VmbInt64_t width, VmbInt64_t height, AVT::VmbAPI::IFrameObserverPtr rec);

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

 private:
  // ___________________________________________________________________________
  // Set the given feature to its maximum value that is divisable by two. Used
  // to set an even frame resolution.
  void setMaxValueModulo2(const char* const& featureName);

  // ___________________________________________________________________________
  void setFeatureToValue(const char* const& featureName, const char* const& value);
  void setFeatureToValue(const char* const& featureName, VmbInt64_t value);
  void getFeature(const char* const& featureName, VmbInt64_t& val) const;
  void getFeature(const char* const& featureName, std::string& val) const;

  // ___________________________________________________________________________
  AVT::VmbAPI::CameraPtr camera;  // The camera pointer.
  AVT::VmbAPI::FramePtrVector frames;  // Frame buffers.
  AVT::VmbAPI::FeaturePtr startAcquisitionF;  // Run this to start acquisition.
  AVT::VmbAPI::FeaturePtr stopAcquisitionF;  // Run this to stop acquisition.
};

#endif  // CAMERAGRABBER_H_
