//
// Created by Thoa Kim on 6/10/17.
//


#ifndef VIDEOKIT_VKDECODER_H
#define VIDEOKIT_VKDECODER_H
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#define CRAZY_DEBUG
#include "utils.h"
#define JAVA_CODING_STYLE
#include "java.h"

/**
 *
 * @param formatContext
 * @param streamIndex
 * @param wanted_stream_nb
 * @param related_stream
 * @param flags
 * @return
 */
extern AVCodecContext* vkLoadVideoCodecContext(AVFormatContext* formatContext,
                                               int* streamIndex,
                                               int wanted_stream_nb,
                                               int related_stream,
                                               int flags);

/**
 *
 * @param file_name
 * @param fmt
 * @param options
 * @return
 */
extern AVFormatContext* vkLoadFormatContext(char* file_name,AVInputFormat *fmt, AVDictionary **options);

/**
 * Decoder decode current frame for encoder save to jpg format.
 *
 * @param output Output file point to save file
 * @param outputData Output data stream for save image frame
 * @param lineSizes Line size of output image
 * @param codecContext @ref AVCodecContext point to video context for decode
 *
 */
extern int vkDecodeVideoPacket();

extern int vkDecodeAudioPacket();

#endif
