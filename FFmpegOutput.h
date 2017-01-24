// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#ifndef FFMPEGOUTPUT_H_
#define FFMPEGOUTPUT_H_

#include "VimbaCPP/Include/VimbaCPP.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <string>

class FFmpegOutput : virtual public AVT::VmbAPI::IFrameObserver {
 public:
  // FFmpegOutput writes each received frame of <camera> to a video file
  // <filename> using h264.
  FFmpegOutput(std::string fn, int width, int height, int fps, std::string crf,
      AVT::VmbAPI::CameraPtr camera, AVPixelFormat cameraPixFmt,
      int* cameraLineSize);
  virtual ~FFmpegOutput();

  void FrameReceived(const AVT::VmbAPI::FramePtr vimbaFrame);

 private:
  pthread_mutex_t lock;
  AVPixelFormat outputPixFmt;
  AVPixelFormat inputPixFmt;
  int* inputLineSize;

  AVCodecContext *c = NULL;
  AVFrame *ffmpegFrame;
  AVPacket pkt;
  FILE *file;
  struct SwsContext *sws_context = NULL;

  void fillFFmpegFrameFromData(uint8_t *img);

 public:
  // ___________________________________________________________________________
  // These need to be called at program start and end if the FrameReceived calls
  // come from different threads. It enables ffmpegs mutex system.
  static void initFFmpeg();
  static void closeFFmpeg();
  // ___________________________________________________________________________
};


#endif  // FFMPEGOUTPUT_H_
