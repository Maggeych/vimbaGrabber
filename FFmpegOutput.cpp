// Copyright © 2017, Freiburg.
// Author: Markus Frey. All Rights Reserved.

#include "FFmpegOutput.h"

#include <iostream>

// _____________________________________________________________________________
FFmpegOutput::FFmpegOutput(std::string fn, AVCodecID codecId, int width,
    int height, int fps, int bitRate, AVT::VmbAPI::CameraPtr camera,
    AVPixelFormat cameraPixFmt, int* cameraLineSize) :
    IFrameObserver(camera),
    inputPixFmt(cameraPixFmt),
    inputLineSize(cameraLineSize) {
  pthread_mutex_init(&lock, NULL);
  // Get the codec handle.
  AVCodec *codec;
  codec = (AVCodec*) avcodec_find_encoder(codecId);
  if (!codec) throw std::runtime_error("Codec not found!");

  // Create the codec's parameters.
  c = avcodec_alloc_context3(codec);
  if (!c) throw std::runtime_error("Could not allocate video codec context!");
  c->bit_rate = bitRate;
  c->width = width;
  c->height = height;
  c->time_base.num = 1;
  c->time_base.den = fps;
  c->gop_size = 10;
  c->max_b_frames = 1;
  c->pix_fmt = AV_PIX_FMT_YUV420P;
  outputPixFmt = c->pix_fmt;

  // Start the codec with set parameters.
  if (codecId == AV_CODEC_ID_H264)
    av_opt_set(c->priv_data, "preset", "slow", 0);
  if (avcodec_open2(c, codec, NULL) < 0)
    throw std::runtime_error("Could not open codec!");

  // Open the file for the output.
  file = fopen(fn.c_str(), "wb");
  if (!file) throw std::runtime_error("Could not open the output file!");

  // Create the output frame buffer.
  ffmpegFrame = av_frame_alloc();
  if (!ffmpegFrame) throw std::runtime_error("Could not allocate the video frame!");
  ffmpegFrame->format = c->pix_fmt;
  ffmpegFrame->width  = c->width;
  ffmpegFrame->height = c->height;
  if (av_image_alloc(
        ffmpegFrame->data, ffmpegFrame->linesize, c->width, c->height, c->pix_fmt, 32
      ) < 0) throw std::runtime_error("Could not allocate the output buffer!");
}

// _____________________________________________________________________________
FFmpegOutput::~FFmpegOutput() {
  // Flush the encoder output.
  int gotOutput = 0;
  do {
    if (avcodec_encode_video2(c, &pkt, NULL, &gotOutput) < 0) {
      // Don't throw in a destructor.
      std::cerr << "Error encoding a frame!" << std::endl;
      break;
    }
    if (gotOutput) {
      fwrite(pkt.data, 1, pkt.size, file);
      av_packet_unref(&pkt);
    }
  } while (gotOutput);

  // End the output file.
  uint8_t endCode[] = {0, 0, 1, 0xb7};
  fwrite(endCode, 1, sizeof(endCode), file);

  // Cleanup.
  pthread_mutex_destroy(&lock);
  fclose(file);
  avcodec_close(c);
  av_free(c);
  av_freep(&ffmpegFrame->data[0]);
  av_frame_free(&ffmpegFrame);
}

// _____________________________________________________________________________
void FFmpegOutput::FrameReceived(const AVT::VmbAPI::FramePtr vimbaFrame) {
  // Check if the received frame is valid.
	VmbFrameStatusType receiveStatus;
	if (vimbaFrame->GetReceiveStatus(receiveStatus) != VmbErrorSuccess)
	  throw std::runtime_error("Could not get frame recieve status!");
	if (receiveStatus != VmbFrameStatusComplete)
	  throw std::runtime_error("Received an incomplete frame!");

  // Lock for the other vimba frame buffer threads.
  pthread_mutex_lock(&lock);

  // Fill the ffmpeg frame buffer with the outputPixFmt type image.
  VmbUchar_t* img;
  if (vimbaFrame->GetImage(img) != VmbErrorSuccess)
	  throw std::runtime_error("Could not get the frame's image data!");

  fillFFmpegFrameFromData(img);

  // Receive the encoded frame data.
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;
  int gotOutput;
  if (avcodec_encode_video2(c, &pkt, ffmpegFrame, &gotOutput) < 0)
    throw std::runtime_error("Error encoding frame!");

  // If there's encoded data write it to the file.
  if (gotOutput) {
    fwrite(pkt.data, 1, pkt.size, file);
    av_packet_unref(&pkt);
  }

  // Unlock, requeue.
  pthread_mutex_unlock(&lock);
  m_pCamera->QueueFrame(vimbaFrame);
}

// _____________________________________________________________________________
void FFmpegOutput::fillFFmpegFrameFromData(uint8_t *img) {
  // Convert the input data to match the ffmpeg encoder input format.
  sws_context = sws_getCachedContext(sws_context,
      c->width, c->height, inputPixFmt,
      c->width, c->height, outputPixFmt,
      0, 0, 0, 0);
  sws_scale(sws_context, (const uint8_t * const *)&img, inputLineSize, 0,
      c->height, ffmpegFrame->data, ffmpegFrame->linesize);
}





// _____________________________________________________________________________
// _____________________________________________________________________________
// _____________________________________________________________________________
// libav multithread safety.
// _____________________________________________________________________________
// _____________________________________________________________________________
// _____________________________________________________________________________

static int lockmgr(void **mtx, enum AVLockOp op) {
  switch(op) {
    case AV_LOCK_CREATE:
      *mtx = malloc(sizeof(pthread_mutex_t));
      if(!*mtx)
        return 1;
      return !!pthread_mutex_init((pthread_mutex_t*) *mtx, NULL);
    case AV_LOCK_OBTAIN:
      return !!pthread_mutex_lock((pthread_mutex_t*) *mtx);
    case AV_LOCK_RELEASE:
      return !!pthread_mutex_unlock((pthread_mutex_t*) *mtx);
    case AV_LOCK_DESTROY:
      pthread_mutex_destroy((pthread_mutex_t*) *mtx);
      free(*mtx);
      return 0;
  }
  return 1;
}

// _____________________________________________________________________________
void FFmpegOutput::initMultiThreadSafe() {
  if(av_lockmgr_register(lockmgr))
    throw std::runtime_error("Couldn't make ffmpeg multi-thread safe!");
}
// _____________________________________________________________________________
void FFmpegOutput::closeMultiThreadSafe() {
  av_lockmgr_register(NULL);
}
