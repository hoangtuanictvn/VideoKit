//
// Created by Thoa Kim on 6/10/17.
//
#include <libavformat/avformat.h>
#include "vkdecoder.h"

extern AVCodecContext* vkLoadVideoCodecContext(AVFormatContext* formatContext,
                                   int* streamIndex,
                                   int wanted_stream_nb,
                                   int related_stream,
                                   int flags){
    int response;
    AVStream *stream;
    AVCodec *codec;
    AVDictionary *opts;

    response = av_find_best_stream(formatContext,AVMEDIA_TYPE_VIDEO,wanted_stream_nb,related_stream,nil,flags);

    if (response<0){
        fprintf(stderr, "Could not find %s with error %s!\n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO),av_err2str(AVERROR_STREAM_NOT_FOUND));
        return nil;
    }

    *streamIndex = response;

    LOGI("Response Code",response);

    stream = formatContext->streams[response];
    codec = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!codec){
        printf("%s\n",av_err2str(AVERROR_DECODER_NOT_FOUND));
        return nil;
    }

    //TODO: Fix realloc here
    //av_dict_set(&opts, "refcounted_frames", "0", 0);

    if ((response = avcodec_open2(stream->codec, codec, nil)) < 0) {
        fprintf(stderr, "Failed to open %s codec\n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
        return nil;
    }

    return stream->codec;
}

AVFormatContext* vkLoadFormatContext(char* file_name,AVInputFormat *fmt, AVDictionary **options){
    AVFormatContext* res;

    LOGS("Start parse file to format context",file_name);

    if(avformat_open_input(&res,file_name,fmt,options)<0){
        printf("Can't open %s\n",file_name);
        return nil;
    }

    if (avformat_find_stream_info(res, options) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return nil;
    }

    LOGS("Parse progress","Finish!");

    return res;
}


int vkDecodeVideoPacket(){

}
