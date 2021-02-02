#ifndef LIBWEBRTC_RTC_RTP_SENDER_IMPL_HXX
#define LIBWEBRTC_RTC_RTP_SENDER_IMPL_HXX


#include <api/scoped_refptr.h>
#include <api/rtp_sender_interface.h>

#include <include/rtc_rtp_sender.h>
#include <include/rtc_media_track.h>

#include "rtc_audio_track_impl.h"
#include "rtc_video_track_impl.h"

#include "rtc_types.h"

#include <string>

namespace libwebrtc {

    class RTCRtpSenderImpl : public RTCRtpSender {

    public:
        RTCRtpSenderImpl(rtc::scoped_refptr<webrtc::RtpSenderInterface> rtcRtpSender);

        virtual ~RTCRtpSenderImpl();

        virtual const char*  id() const override {
            return id_;
        };

        virtual bool ownsTrack() override {
            return true;
        };

        virtual RtpParameters parameters() override {
            return parameters_;
        };

        virtual RtpParameters* parameters1() override {
            return parameters1_;
        };

        virtual DtmfSender dtmfSender() override {
            return dtmfSender_;
        };

        virtual scoped_refptr<RTCMediaTrack> track() override {
            return track_;
        };

        rtc::scoped_refptr<webrtc::RtpSenderInterface> rtc_rtp_sender() {
            return rtc_rtp_sender_;
        }

        RtpEncodingParametersVector encodings() override {
            return encodings_;
        };

        virtual const char* message() override {
            return message_;
        };

    private:
        bool Initialize();

    protected:
        rtc::scoped_refptr<webrtc::RtpSenderInterface> rtc_rtp_sender_;
        scoped_refptr<RTCMediaTrack> track_;
        DtmfSender dtmfSender_;
        RtpParameters parameters_;
        RtpParameters* parameters1_;
        char id_[kMaxStringLength] = "";
        RtpEncodingParametersVector encodings_;
        char message_[kMaxStringLength] = "";
    };
}


#endif //LIBWEBRTC_RTC_RTP_SENDER_IMPL_HXX
