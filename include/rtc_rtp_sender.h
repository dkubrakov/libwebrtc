#ifndef LIBWEBRTC_RTC_RTP_SENDER_HXX
#define LIBWEBRTC_RTC_RTP_SENDER_HXX

#include "rtc_media_track.h"

#include "rtc_types.h"

namespace libwebrtc {

    enum MediaType { MEDIA_TYPE_AUDIO, MEDIA_TYPE_VIDEO, MEDIA_TYPE_DATA };

    struct RtpHeaderExtensionParameters {
    public:
        char uri[kMaxStringLength]{};
        int id = 0;
        bool encrypted = false;
    };

    struct RtpCodecParameters {
    public:
        int payload_type = 0;
        char name[kMaxStringLength]{};
        MediaType kind;
        int clock_rate;
        int num_channels;
// TODO       std::unordered_map<std::string, std::string> parameters{};
    };

    struct  RtpEncodingParameters : public RefCountInterface {
    public:
        char rid[kMaxStringLength]{};
        bool active = true;
        int max_bitrate_bps;
        int min_bitrate_bps;
        int max_framerate;
        int num_temporal_layers;
        double scale_resolution_down_by;
        double scale_framerate_down_by;
        int ssrc;
    };

    typedef Vector<scoped_refptr<RtpEncodingParameters>> RtpEncodingParametersVector;

    struct RtcpParameters {
    public:
        uint32_t ssrc;
        char cname[kMaxStringLength]{};
        bool reduced_size = false;
        bool mux = true;
    };

    struct RtpParameters {
    public:
        char transaction_id[kMaxStringLength]{};
        char mid[kMaxStringLength]{};
//        Vector<RtpCodecParameters> codecs{};
//        Vector<scoped_refptr<RtpHeaderExtensionParameters>> header_extensions{};
        RtpEncodingParametersVector encodings;
        RtcpParameters rtcp{};
    };

    struct DtmfSender {
    public:
        int interToneGap;   // @(dtmf.interToneGap / 1000.0) - Objective-C
        int duration;       // @(dtmf.duration / 1000.0) - Objective-C
    };

    class RTCRtpSender : public RefCountInterface{

    public:
        // senderId
        virtual const char* id() const = 0;

        virtual scoped_refptr<RTCMediaTrack> track() = 0;

        virtual bool ownsTrack() = 0;

        virtual RtpParameters parameters() = 0;

        virtual RtpParameters* parameters1() = 0;

        // not use in flutter - only 'sendDtmf'
        virtual DtmfSender dtmfSender() = 0;

        virtual RtpEncodingParametersVector encodings() = 0;

        virtual const char* message() = 0;

    protected:
        virtual ~RTCRtpSender() {};
    };

}

#endif //LIBWEBRTC_RTC_RTP_SENDER_HXX

