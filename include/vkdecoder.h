//
// Created by Thoa Kim on 6/10/17.
//

#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#ifndef VIDEOKIT_VKDECODER_H
#define VIDEOKIT_VKDECODER_H

extern int vkDecodePacket();

/**
 * Decoder decode current frame for encoder save to jpg format.
 *
 * @param output Output file point to save file
 * @param outputData Output data stream for save image frame
 * @param lineSizes Line size of output image
 * @param codecContext @ref AVCodecContext point to video context for decode
 *
 */
extern int vkDecodeVideoPacket(FILE* output,const uint8_t *outputData[4],
                               int lineSizes[4],
                               AVCodecContext* codecContext,
                               AVFrame* frame,AVPacket* packet,
                               int * gotFrame, int width, int height,
                               enum AVPixelFormat pixelFormat);
extern int vkDecodeAudioPacket();

#endif //VIDEOKIT_VKDECODER_H
