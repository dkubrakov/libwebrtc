#include "rtc_rtp_sender_impl.h"
#include "rtc_types.h"

static std::map<cricket::MediaType, libwebrtc::MediaType>
        media_type_map = {
        {cricket::MEDIA_TYPE_AUDIO, libwebrtc::MEDIA_TYPE_AUDIO},
        {cricket::MEDIA_TYPE_VIDEO, libwebrtc::MEDIA_TYPE_VIDEO},
        {cricket::MEDIA_TYPE_DATA,  libwebrtc::MEDIA_TYPE_DATA, }};

namespace libwebrtc {

    RTCRtpSenderImpl::RTCRtpSenderImpl(rtc::scoped_refptr<webrtc::RtpSenderInterface> rtcRtpSender)
            : rtc_rtp_sender_(rtcRtpSender) {
        RTC_LOG(INFO) << __FUNCTION__ << ": ctor ";

        Initialize();
    }

    RTCRtpSenderImpl::~RTCRtpSenderImpl() {
        RTC_LOG(INFO) << __FUNCTION__ << ": dtor ";
    }

    MediaType convertMediaType(const cricket::MediaType& kind) {
        if (kind == cricket::MediaType::MEDIA_TYPE_AUDIO) {
            return MEDIA_TYPE_AUDIO;
        } else if (kind == cricket::MediaType::MEDIA_TYPE_VIDEO) {
            return MEDIA_TYPE_VIDEO;
        } else if (kind == cricket::MediaType::MEDIA_TYPE_DATA) {
            return MEDIA_TYPE_DATA;
        }
        FATAL();
        // Not reachable; avoids compile warning.
        return static_cast<MediaType>(-1);
    }

    RtpCodecParameters convertCodec(const webrtc::RtpCodecParameters &webrtcCodec) {
        RtpCodecParameters codec;
        codec.payload_type = webrtcCodec.payload_type;
        strcpy_s(codec.name, webrtcCodec.name.c_str());
        codec.kind = convertMediaType(webrtcCodec.kind); // codec.kind = media_type_map[webrtcCodec.kind];
        codec.clock_rate = webrtcCodec.clock_rate.value_or(0);
        codec.num_channels = webrtcCodec.num_channels.value_or(0);
// TODO       codec.parameters = webrtcCodec.parameters;
        return codec;
    }

    RtpHeaderExtensionParameters convertHeaderExtension(const webrtc::RtpHeaderExtensionParameters &webrtcExtension) {
        RtpHeaderExtensionParameters extension;
        extension.id = webrtcExtension.id;
        extension.encrypted = webrtcExtension.encrypt;
        strcpy_s(extension.uri, webrtcExtension.uri.c_str());
        return extension;
    }

    scoped_refptr<RtpEncodingParameters> convertEncoding(const webrtc::RtpEncodingParameters &webrtcEncoding) {
        scoped_refptr<RtpEncodingParameters> encoding = new  RefCountedObject<RtpEncodingParameters>();
        strcpy_s(encoding->rid, webrtcEncoding.rid.c_str());
        encoding->active = webrtcEncoding.active;
        encoding->max_bitrate_bps = webrtcEncoding.max_bitrate_bps.value_or(0);
        encoding->min_bitrate_bps = webrtcEncoding.min_bitrate_bps.value_or(0);
        encoding->max_framerate = webrtcEncoding.max_framerate.value_or(0);
        encoding->num_temporal_layers = webrtcEncoding.num_temporal_layers.value_or(0);
        encoding->scale_resolution_down_by = webrtcEncoding.scale_resolution_down_by.value_or(0.0);
        encoding->scale_framerate_down_by = webrtcEncoding.scale_framerate_down_by.value_or(0.0);
        encoding->ssrc = webrtcEncoding.ssrc.value_or(0);
        return encoding;
    }

    bool RTCRtpSenderImpl::Initialize() {
        RTC_DCHECK(rtc_rtp_sender_.get() != nullptr);

        // init parameters1_
        parameters1_ = new RtpParameters();


        std::string msg;

        strncpy(id_, rtc_rtp_sender_->id().c_str(), sizeof(id_));

//        track_
        rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> temp_track = rtc_rtp_sender_->track();
        const std::string &kind = temp_track->kind();

        if (kind == webrtc::MediaStreamTrackInterface::kAudioKind) {
            webrtc::AudioTrackInterface* rtc_audio_track = (webrtc::AudioTrackInterface*) temp_track.get();
            track_ = scoped_refptr<AudioTrackImpl>(new RefCountedObject<AudioTrackImpl>(rtc_audio_track));
        }
        else if (kind == webrtc::MediaStreamTrackInterface::kVideoKind) {
            webrtc::VideoTrackInterface* rtc_video_track = (webrtc::VideoTrackInterface*) temp_track.get();
            track_ = scoped_refptr<VideoTrackImpl>(new RefCountedObject<VideoTrackImpl>(rtc_video_track));
        }
        else {
            RTC_LOG(LS_ERROR) << "RTCRtpSenderImpl::track() with invalid kind: " << kind;
        }

//        RtpParameters
        const webrtc::RtpParameters &webrtcRtpParameters = rtc_rtp_sender_->GetParameters();

        strcpy_s(parameters_.transaction_id, webrtcRtpParameters.transaction_id.c_str());
        strcpy_s(parameters_.mid, webrtcRtpParameters.mid.c_str());

        msg += " codecs size=" + std::to_string(webrtcRtpParameters.codecs.size());
//        RtpCodecParameters
//        for (webrtc::RtpCodecParameters element : webrtcRtpParameters.codecs) {
//            parameters_.codecs.push_back(convertCodec(element));
//        }

        msg += " header_extensions size=" + std::to_string(webrtcRtpParameters.header_extensions.size());
//        RtpHeaderExtensionParameters
//        for (webrtc::RtpHeaderExtensionParameters element : webrtcRtpParameters.header_extensions) {
//            parameters_.header_extensions.push_back(convertHeaderExtension(element));
//        }

        msg += " encodings size=" + std::to_string(webrtcRtpParameters.encodings.size());
        strncpy(message_, msg.c_str(), sizeof(message_));

//        RtpEncodingParameters
        for (const webrtc::RtpEncodingParameters& element : webrtcRtpParameters.encodings) {
//            scoped_refptr<RtpEncodingParameters> encoding = convertEncoding(element);
            scoped_refptr<RtpEncodingParameters> encoding = new RefCountedObject<RtpEncodingParameters>();
            strcpy_s(encoding->rid, element.rid.c_str());
            encoding->active = element.active;
            encoding->max_bitrate_bps = element.max_bitrate_bps.value_or(0);
            encoding->min_bitrate_bps = element.min_bitrate_bps.value_or(0);
            encoding->max_framerate = element.max_framerate.value_or(0);
            encoding->num_temporal_layers = element.num_temporal_layers.value_or(0);
            encoding->scale_resolution_down_by = element.scale_resolution_down_by.value_or(0.0);
            encoding->scale_framerate_down_by = element.scale_framerate_down_by.value_or(0.0);
            encoding->ssrc = element.ssrc.value_or(0);
            parameters_.encodings.push_back(encoding);
            encodings_.push_back(encoding);
        }

// TEST
        scoped_refptr<RtpEncodingParameters> encoding = new RefCountedObject<RtpEncodingParameters>();
        strcpy_s(encoding->rid, "webrtcEncoding.rid.1");
        encoding->max_bitrate_bps = 100;
        encoding->min_bitrate_bps = 10;
        encoding->max_framerate = 500;
        encodings_.push_back(encoding);
        parameters_.encodings.push_back(encoding); // TEST
        parameters1_->encodings.push_back(encoding); // TEST
// TEST
        encoding = new RefCountedObject<RtpEncodingParameters>();
        strcpy_s(encoding->rid, "webrtcEncoding.rid.2");
        encoding->max_bitrate_bps = 1000;
        encoding->min_bitrate_bps = 100;
        encoding->max_framerate = 5000;
        encodings_.push_back(encoding);
        parameters_.encodings.push_back(encoding); // TEST
        parameters1_->encodings.push_back(encoding); // TEST

//        RtcpParameters
        parameters_.rtcp.ssrc = webrtcRtpParameters.rtcp.ssrc.value_or(0);
        parameters_.rtcp.mux = webrtcRtpParameters.rtcp.mux;
        parameters_.rtcp.reduced_size = webrtcRtpParameters.rtcp.reduced_size;
        strcpy_s(parameters_.rtcp.cname, webrtcRtpParameters.rtcp.cname.c_str());

        rtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmfSender = rtc_rtp_sender_->GetDtmfSender();
        dtmfSender_.duration = dtmfSender->duration();
        dtmfSender_.interToneGap = dtmfSender->inter_tone_gap();

        return true;
    }

}