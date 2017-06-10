//
// Created by Thoa Kim on 6/10/17.
//
#include "vkdecoder.h"

int vkExtractPacket(){

}

int vkDecodeVideoPacket(FILE* output,const uint8_t *outputData[4],
                        int lineSizes[4],
                        AVCodecContext* codecContext,
                        AVFrame* frame,AVPacket* packet,
                        int * gotFrame, int width, int height,
                        enum AVPixelFormat pixelFormat){
    int response;

    response = avcodec_decode_video2(codecContext, frame, gotFrame, packet);

    if (response < 0) {
        fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(response));
        return response;
    }

    if (*gotFrame) {
        if (frame->width != width || frame->height != height ||
            frame->format != pixelFormat) {
            /* To handle this change, one could call av_image_alloc again and
             * decode the following frames into another rawvideo file. */
            fprintf(stderr, "Error: Width, height and pixel format have to be "
                            "constant in a rawvideo file, but the width, height or "
                            "pixel format of the input video changed:\n"
                            "old: width = %d, height = %d, format = %s\n"
                            "new: width = %d, height = %d, format = %s\n",
                    width, height, av_get_pix_fmt_name(pixelFormat),
                    frame->width, frame->height,
                    av_get_pix_fmt_name(frame->format));
            return -1;
        }
        av_image_copy(outputData, lineSizes,
                      (const uint8_t **)(frame->data), frame->linesize,
                      pixelFormat, width, height);
        /* write to rawvideo file */
        fwrite(outputData[0], 1, 1024 /*TODO: Change with real buffer size */, output);
    }
}
