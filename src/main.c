/*
 * @author: Hoang Tuan
 * @date:
 * @filename: main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <vkdecoder.h>

#define CRAZY_DEBUG
#include "utils.h"
#define JAVA_CODING_STYLE
#include "java.h"

int open_codec_context(AVFormatContext*,int*,enum AVMediaType);

void save_frame_as_jpeg(AVFormatContext* formatContext,AVCodecContext *codecContext, AVFrame *frame, int frameno) {
    AVCodec *jpegCodec = avcodec_find_encoder(AV_CODEC_ID_JPEG2000);
    if (!jpegCodec) {
        return;
    }
    AVCodecContext *jpegContext = avcodec_alloc_context3(jpegCodec);
    if (!jpegContext) {
        return;
    }

    jpegContext->pix_fmt = codecContext->pix_fmt;
    jpegContext->height = frame->height;
    jpegContext->width = frame->width;
    jpegContext->sample_aspect_ratio = codecContext->sample_aspect_ratio;
    jpegContext->time_base = codecContext->time_base;
    jpegContext->compression_level = 100;
    jpegContext->thread_count = 1;
    jpegContext->prediction_method = 1;
    jpegContext->flags2 = 0;
    jpegContext->rc_max_rate = jpegContext->rc_min_rate = jpegContext->bit_rate = 80000000;

    if (avcodec_open2(jpegContext, jpegCodec, NULL) < 0) {
        return;
    }

    FILE *JPEGFile;
    char JPEGFName[256];

    AVPacket packet = {.data = NULL, .size = 0};
    av_init_packet(&packet);
    int gotFrame;

    if (avcodec_encode_video2(jpegContext, &packet, frame, &gotFrame) < 0) {
        return;
    };

    sprintf(JPEGFName, "dvr-%06d.jpg",frameno);
    JPEGFile = fopen(JPEGFName, "wb");
    fwrite(packet.data, 1, packet.size, JPEGFile);
    fclose(JPEGFile);

    av_free_packet(&packet);
    avcodec_close(jpegContext);

}


int main(){
    AVFormatContext *formatContext = nil;
    AVCodecContext *codecContext = nil;
    AVStream* videoStream = nil;
    AVFrame* imageFrame;
    AVPacket sendingPacket;
    int videoStreamIndex = 0, response = 0;
    uint8_t *desData[4] = {nil};
    int desLineSize[4];
    int buffSize = 0;
    int gotFrameCount;

    av_register_all();

    formatContext = vkLoadFormatContext(IP_FILE,nil,nil);
    codecContext = vkLoadVideoCodecContext(formatContext,&videoStreamIndex,-1,-1,0);

    if(codecContext){
        response = av_image_alloc(desData,desLineSize,codecContext->width,codecContext->height,codecContext->pix_fmt,1);

        if (response < 0) {
            fprintf(stderr, "Could not allocate raw video buffer\n");
            return 0;
        }

        buffSize = response;
    } else{
        LOL
    }
    LOGI("BuffSize",buffSize);

    av_dump_format(formatContext,0,IP_FILE,0);

    imageFrame = av_frame_alloc();
    if (!imageFrame) {
        fprintf(stderr, "Could not allocate frame\n");
        response = AVERROR(ENOMEM);
        return 0;
    }

    av_init_packet(&sendingPacket);
    sendingPacket.data = nil;
    sendingPacket.size = 0;
    int count = 0;
    while (av_read_frame(formatContext, &sendingPacket) >= 0) {
        AVPacket orig_pkt = sendingPacket;
        do {
            if (sendingPacket.stream_index == videoStreamIndex) {
                response = avcodec_decode_video2(codecContext, imageFrame, &gotFrameCount, &sendingPacket);
                if (gotFrameCount && count > 0 && count < 100)
                    save_frame_as_jpeg(formatContext, codecContext, imageFrame, count);
                if (response < 0) {
                    fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(response));
                    return response;
                }
            }

            if (response < 0)
                break;
            sendingPacket.data += response;
            sendingPacket.size -= response;

        } while (sendingPacket.size > 0);
        av_packet_unref(&orig_pkt);
        count++;
    }
    return 0;
}

int open_codec_context(AVFormatContext* formatContext,int* streamIndex,enum AVMediaType type){

    int response;
    AVStream *stream;
    AVCodec *codec;
    AVDictionary * opts;

    response = av_find_best_stream(formatContext,type,-1,-1,nil,0);

    if (response<0){
        fprintf(stderr, "Could not find %s with error %s!\n",
                av_get_media_type_string(type),av_err2str(AVERROR_STREAM_NOT_FOUND));
        return response;
    }

    *streamIndex = response;

    LOGI("Response Code",response);

    stream = formatContext->streams[response];

    codec = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!codec){
        printf("%s\n",av_err2str(AVERROR_DECODER_NOT_FOUND));
        return AVERROR_DECODER_NOT_FOUND;
    }

    LOGS("Decoder",codec->long_name);

    av_dict_set(&opts, "refcounted_frames", "0", 0);

    if (avcodec_open2(stream->codec, codec, &opts) < 0) {
        fprintf(stderr, "Failed to open %s codec\n",
                av_get_media_type_string(type));
        return response;
    }

    return response;
}