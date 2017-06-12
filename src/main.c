/*
 * @author: Hoang Tuan
 * @date:
 * @filename: main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>

#define CRAZY_DEBUG
#include <vkdecoder.h>

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
        response = av_image_alloc(desData,desLineSize,codecContext->width,
                                  codecContext->height,codecContext->pix_fmt,1);

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
                response = avcodec_decode_video2(codecContext, imageFrame,
                                                 &gotFrameCount, &sendingPacket);

                if (gotFrameCount && count > 0 && count < 100)
                    vkEncodeJPG(formatContext, codecContext, imageFrame,"LOL", count);

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