//
//  TTTEventHandler.h
//
//  Created by doubon on 2019/3/11.
//

#ifndef TTTEventHandler_h
#define TTTEventHandler_h

#include <stdio.h>
#include <pthread.h>

#include "JSON/json/json.h"
#include "JSON/json/json-forwards.h"
#include "stdafx.h"
#include "TTTRtcEngine.hpp"

class TTTEventHandler : public TTTRtcEngineEventHandler
{
private:
    void pushJsonToTaskQueue(std::string sJson);
public:
    TTTEventHandler();
    virtual ~TTTEventHandler();
    
    void onError(TTT_ERROR_CODE code, const char *message) override;
    void onConnectionLost() override;
    void onJoinChannelSuccess(const char *channel, long long uid, int elapsed) override;
    void onLeaveChannel(const TTT_RtcStats &stats) override;
    void onUserJoined(long long uid, int elapsed) override;
    void onUserOffline(long long uid, TTT_USER_OFFLINE_REASON reason) override;
    
    void onReportAudioVolumeIndication(long long userID, unsigned int audioLevel, unsigned int audioLevelFullRange) override;
    void onReportRtcStats(const TTT_RtcStats &stats) override;
    void onLocalVideoStats(const TTT_LocalVideoStats &stats) override;
    void onLocalAudioStats(const TTT_LocalAudioStats &stats) override;
    void onRemoteVideoStats(const TTT_RemoteVideoStats &stats) override;
    void onRemoteAudioStats(const TTT_RemoteAudioStats &stats) override;
    
    void onVideoEnabled(int64_t uid, bool enabled) override;
    void onAudioMuted(long long uid, bool muted) override;
#if TARGET_OS_IOS
    void onAudioRouteChanged(AUDIO_ROUTE routing) override;
#endif
    void onAudioMixingFinished() override;

    void onChatMessageSent(const TTT_ChatInfo &chatInfo, TTT_ERROR_CODE errorCode) override;
    void onChatMessageReceived(long long userID, const TTT_ChatInfo &chatInfo) override;
    void onChatAudioFinishPlaying(const char *fileName) override;
    
    void onFirstLocalVideoFrame(int width, int height, int elapsed) override;
    void onFirstRemoteVideoFrameDecoded(int64_t uid, int width, int height, int elapsed) override;
    void onFirstRemoteVideoFrame(int64_t uid, int width, int height, int elapsed) override;
    void onLocalVideoFrameCaptured(const TTT_VideoFrame &videoFrame) override;
    void onRemoteVideoFrameDecoded(int64_t uid, const TTT_VideoFrame &videoFrame) override;
    void onCameraReady() override;
    void onVideoStopped() override;
};

#endif /* TTTEventHandler_h */
