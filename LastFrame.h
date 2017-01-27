// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#ifndef LASTFRAME_H_
#define LASTFRAME_H_

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}
#include <cxcore.hpp>

#include <mutex>

class LastFrame {
 public:
  LastFrame() {}
  void update(VmbUchar_t* img, int width, int height,
      AVPixelFormat inputPixFmt) {
    if (inputPixFmt == AV_PIX_FMT_GRAY8) {
      std::lock_guard<std::mutex> guard(accessLock);
      cv::Mat vimbaCVFrame(height, width, CV_8UC1, img);
      vimbaCVFrame.copyTo(frame);
    } else {
      // TODO: More input pixel formats.
      throw std::runtime_error("Don't know how to read the camera's pixel format in OpenCV!");
    }
  }
  void get(cv::Mat& frameCpy) {
    std::lock_guard<std::mutex> guard(accessLock);
    frame.copyTo(frameCpy);
  }
 private:
  std::mutex accessLock;
  cv::Mat frame;
};

#endif  // LASTFRAME_H_
