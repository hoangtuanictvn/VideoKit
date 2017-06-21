//
// Created by Thoa Kim on 6/10/17.
//


#ifndef VIDEOKIT_VKDECODER_H
#define VIDEOKIT_VKDECODER_H
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#define CRAZY_DEBUG
#include <utils.h>
#include <java.h>

/**
 *
 * @param formatContext
 * @param type
 * @param streamIndex
 * @param wanted_stream_nb
 * @param related_stream
 * @param flags
 * @return
 */
extern AVCodecContext* vkLoadCodecContext(AVFormatContext* formatContext,
                                          enum AVMediaType type,
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
extern AVFormatContext* vkLoadFormatContext(char* file_name,AVInputFormat *fmt,
                                            AVDictionary **options);

/**
 *
 * @param codecContext
 * @param frame
 * @param fileName
 * @param file_nb
 */
extern void vkEncodeJPG(AVCodecContext *codecContext,
                       AVFrame *frame, char * fileName,int file_nb);

#endif
