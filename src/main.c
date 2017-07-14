#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>

#define VD_FILE "/Users/anonymousjp/Desktop/big.mp4"
#define AD_FILE "/Users/anonymousjp/Desktop/test.mp3"
#define OP_FILE "/Users/anonymousjp/Desktop/out.mp4"

#define CRAZY_DEBUG
#include <vkdecoder.h>

int main(){
    
    int response;
    AVOutputFormat* outputFormat = nil;
    AVFormatContext * audioFormatContext = nil,
                    * videoFormatContext = nil,
                    * outputFormatContext = nil;
    int videoStreamIndex = -1, audioStreamIndex = -1, index = 0;
    int ouputVideoStreamIndex = -1, outputAudioStreamIndex = -1;
    AVPacket pkt;
    pkt.size = 0;
    pkt.data = nil;
    int frame_index = 0;
    int64_t cur_pts_v = 0, cur_pts_a = 0;

    av_register_all();

    if ((response = avformat_open_input(&audioFormatContext, AD_FILE, 0, 0)) < 0) {
        LOGS("LOGS","Could not open input file.");
        return response;
    }

    if ((response = avformat_find_stream_info(audioFormatContext, 0)) < 0) {
        LOGS("LOGS","Failed to retrieve input stream information");
        return response;
    }

    if ((response = avformat_open_input(&videoFormatContext, VD_FILE, 0, 0)) < 0) {
        LOGS("LOGS","Could not open input file.");
        return response;
    }
    if ((response = avformat_find_stream_info(videoFormatContext, 0)) < 0) {
        LOGS("LOGS","Failed to retrieve input stream information");
        return response;
    }
    //av_dump_format(audioFormatContext,0,nil,0);
    //av_dump_format(videoFormatContext,0,nil,0);

    avformat_alloc_output_context2(&outputFormatContext,nil,nil,OP_FILE);

    if (!outputFormatContext) {
        LOGS("ERROR","Could not create output context");
        response = AVERROR_UNKNOWN;
        return response;
    }

    outputFormat = outputFormatContext->oformat;


    for (index = 0; index < videoFormatContext->nb_streams; index++) {
        if (videoFormatContext->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = index;
            AVStream *inStream = videoFormatContext->streams[index];
            AVStream *outStream = avformat_new_stream(outputFormatContext, inStream->codec->codec);
            if (!outStream) {
                LOGS("","Failed allocating output stream\n");
                response = AVERROR_UNKNOWN;
                return response;
            }

            ouputVideoStreamIndex = outStream->index;

            if (avcodec_copy_context(outStream->codec, inStream->codec) < 0) {
                LOGS("ERROR","Failed to copy context from input to output stream codec context\n");
                return 0;
            }

            outStream->codec->codec_tag = 0;
            if (outputFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
                outStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    for(index = 0;index<audioFormatContext->nb_streams;index++){
        if (audioFormatContext->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audioStreamIndex = index;

            AVStream *in_stream = audioFormatContext->streams[index];
            AVStream *out_stream = avformat_new_stream(outputFormatContext, in_stream->codec->codec);

            if (!out_stream) {
                LOGS("ERROR","Failed allocating output stream");
                response = AVERROR_UNKNOWN;
                return response;
            }

            outputAudioStreamIndex = out_stream->index;

            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
                LOGS("ERROR","Failed to copy context from input to output stream codec context");
                return response;
            }


            out_stream->codec->codec_tag = 0;

            if (outputFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

            break;
        }
    }

    if (!(outputFormat->flags & AVFMT_NOFILE)) {
        if (avio_open(&outputFormatContext->pb, OP_FILE, AVIO_FLAG_WRITE) < 0) {
            LOGERR("Could not open output file '%s'", OP_FILE);
            return 0;
        }
    }

    if (avformat_write_header(outputFormatContext, NULL) < 0) {
        LOGERR("Error occurred when opening output file");
        return 0;
    }

    while(1){
        AVFormatContext * inpFormatContext;
        int stream_index = 0;
        AVStream *in_stream, *out_stream;
        if(av_compare_ts(cur_pts_v,videoFormatContext->streams[videoStreamIndex]->time_base,
                         cur_pts_a,audioFormatContext->streams[audioStreamIndex]->time_base)<=0){
            inpFormatContext = videoFormatContext;
            stream_index = ouputVideoStreamIndex;
            if (av_read_frame(inpFormatContext, &pkt) >= 0) {
                do {
                    if (pkt.stream_index == videoStreamIndex) {
                        cur_pts_v = pkt.pts;
                        break;
                    }
                } while (av_read_frame(inpFormatContext, &pkt) >= 0);
            } else {
                break;
            }
        }else {
            inpFormatContext = audioFormatContext;
            stream_index = outputAudioStreamIndex;
            if (av_read_frame(inpFormatContext, &pkt) >= 0) {
                do {
                    if (pkt.stream_index == audioStreamIndex) {
                        cur_pts_a = pkt.pts;
                        break;
                    }
                } while (av_read_frame(inpFormatContext, &pkt) >= 0);
            } else {
                break;
            }

        }
        in_stream = inpFormatContext->streams[pkt.stream_index];
        out_stream = outputFormatContext->streams[stream_index];

        if (pkt.pts == AV_NOPTS_VALUE) {
            //Write PTS
            AVRational time_base1 = in_stream->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
            //Parameters
            pkt.pts = (double) (frame_index * calc_duration) / (double) (av_q2d(time_base1) * AV_TIME_BASE);
            pkt.dts = pkt.pts;
            pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
            frame_index++;
        }

        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (enum AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (enum AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index = stream_index;

        printf("Write 1 Packet. size:%5d\tpts:%8d\n", pkt.size, pkt.pts);
        // Write
        if (av_interleaved_write_frame(outputFormatContext, &pkt) < 0) {
            LOGS("Error","Error muxing packet");
            break;
        }
        av_free_packet(&pkt);
    }
    av_write_trailer(outputFormatContext);

    LOGS("Runtime","Finish");

    avformat_close_input(&videoFormatContext);
    avformat_close_input(&audioFormatContext);

    if (outputFormatContext && !(outputFormat->flags & AVFMT_NOFILE))
        avio_close(outputFormatContext->pb);
    avformat_free_context(outputFormatContext);

    if (response < 0 && response != AVERROR_EOF) {
        LOGS("ERROR","Error occurred.");
        return -1;
    }

    return 0;

    /*
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


    formatContext = vkLoadFormatContext(VD_FILE,nil,nil);
    codecContext = vkLoadCodecContext(formatContext,AVMEDIA_TYPE_VIDEO,&videoStreamIndex,-1,-1,0);

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

    av_dump_format(formatContext,0,VD_FILE,0);

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

                if (gotFrameCount && count < 100){
                    vkEncodeJPG(codecContext, imageFrame,"decode", count);
                    count++;
                }

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
    }
    return 0;*/
}

/*int WriteJPEG (AVCodecContext *pCodecCtx, AVFrame *pFrame, int FrameNo){
    AVCodecContext         *pOCodecCtx;
    AVCodec                *pOCodec;
    uint8_t                *Buffer;
    int                     BufSiz;
    int                     BufSizActual;
    int                     ImgFmt = pOCodecCtx->pix_fmt;
    FILE                   *JPEGFile;
    char                    JPEGFName[256];

    BufSiz = avpicture_get_size (
            ImgFmt,pCodecCtx->width,pCodecCtx->height );

    Buffer = (uint8_t *)malloc ( BufSiz );
    if ( Buffer == NULL )
        return ( 0 );
    memset ( Buffer, 0, BufSiz );

    pOCodecCtx = avcodec_alloc_context3(pOCodec);
    if ( !pOCodecCtx ) {
        free ( Buffer );
        return ( 0 );
    }

    pOCodecCtx->bit_rate      = pCodecCtx->bit_rate;
    pOCodecCtx->width         = pCodecCtx->width;
    pOCodecCtx->height        = pCodecCtx->height;
    pOCodecCtx->pix_fmt       = ImgFmt;
    pOCodecCtx->codec_id      = AV_CODEC_ID_JPEGLS;
    pOCodecCtx->codec_type    = AVMEDIA_TYPE_VIDEO;
    pOCodecCtx->time_base.num = pCodecCtx->time_base.num;
    pOCodecCtx->time_base.den = pCodecCtx->time_base.den;

    pOCodec = avcodec_find_encoder ( pOCodecCtx->codec_id );
    if ( !pOCodec ) {
        free ( Buffer );
        return ( 0 );
    }
    if ( avcodec_open2( pOCodecCtx, pOCodec ,NULL) < 0 ) {
        free ( Buffer );
        return ( 0 );
    }

    pOCodecCtx->mb_lmin        = pOCodecCtx->lmin =
            pOCodecCtx->qmin * FF_QP2LAMBDA;
    pOCodecCtx->mb_lmax        = pOCodecCtx->lmax =
            pOCodecCtx->qmax * FF_QP2LAMBDA;
    pOCodecCtx->flags          = CODEC_FLAG_QSCALE;
    pOCodecCtx->global_quality = pOCodecCtx->qmin * FF_QP2LAMBDA;

    pFrame->pts     = 1;
    pFrame->quality = pOCodecCtx->global_quality;
    BufSizActual = avcodec_encode_video2(
            pOCodecCtx,Buffer,BufSiz,pFrame);

    sprintf ( JPEGFName, "%06d.jpg", FrameNo );
    JPEGFile = fopen ( JPEGFName, "wb" );
    fwrite ( Buffer, 1, BufSizActual, JPEGFile );
    fclose ( JPEGFile );

    avcodec_close ( pOCodecCtx );
    free ( Buffer );
    return ( BufSizActual );
}*/
