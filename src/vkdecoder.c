//
// Created by Thoa Kim on 6/10/17.
//
#include <libavformat/avformat.h>
#include <vkdecoder.h>

extern AVCodecContext* vkLoadCodecContext(AVFormatContext* formatContext,
                                          enum AVMediaType type,
                                          int* streamIndex,
                                          int wanted_stream_nb,
                                          int related_stream,
                                          int flags){
    int response;
    AVStream *stream = nil;
    AVCodec *codec = nil;
    AVDictionary *opts = nil;

    response = av_find_best_stream(formatContext,type,
                                   wanted_stream_nb,related_stream,nil,flags);

    if (response<0){
        fprintf(stderr, "Could not find %s with error %s!\n",
                av_get_media_type_string(type),av_err2str(AVERROR_STREAM_NOT_FOUND));
        return nil;
    } else{

        *streamIndex = response;

        printf("Stream Index: %d\n",response);

        stream = formatContext->streams[response];
        codec = avcodec_find_decoder(stream->codecpar->codec_id);

        if (!codec){
            printf("%s\n",av_err2str(AVERROR_DECODER_NOT_FOUND));
            return nil;
        }

        //TODO: Fix realloc here
        av_dict_set(&opts, "refcounted_frames", "0", 0);

        if (avcodec_open2(stream->codec, codec, &opts) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return nil;
        }
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

void vkEncodeJPG(AVCodecContext *codecContext,
                 AVFrame *frame, char * fileName,int file_nb){
    FILE *JPEGFile = nil;
    AVCodec *jpegCodec = avcodec_find_encoder(AV_CODEC_ID_JPEG2000);

    if (!jpegCodec) {
        LOGERR("Can't find JPG encoder!");
        return;
    }

    AVCodecContext *jpegContext = avcodec_alloc_context3(jpegCodec);

    if (!jpegContext) {
        LOGERR("Can't alloc JPG codec context!");
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
        LOGERR("Can't open codec!");
        return;
    }

    AVPacket packet = {.data = NULL, .size = 0};
    av_init_packet(&packet);
    int gotFrame;

    if (avcodec_encode_video2(jpegContext, &packet, frame, &gotFrame) < 0) {
        return;
    };

    char fn[255];

    sprintf(fn, "%s-%d.jpg",fileName,file_nb);
    LOGI("Save frame ",file_nb);
    JPEGFile = fopen(fn, "wb");
    fwrite(packet.data, 1, packet.size, JPEGFile);
    fclose(JPEGFile);

    av_free_packet(&packet);
    avcodec_close(jpegContext);
}
