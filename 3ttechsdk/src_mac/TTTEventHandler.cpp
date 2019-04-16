//
//  TTTEventHandler.cpp
//
//  Created by doubon on 2019/3/11.
//

#include "TTTEventHandler.h"

extern std::queue<TaskQueue*> task_queue;
extern pthread_mutex_t m_mutex;

TTTEventHandler::TTTEventHandler()
{
}

TTTEventHandler::~TTTEventHandler()
{
    pthread_mutex_destroy(&m_mutex);
}

void TTTEventHandler::pushJsonToTaskQueue(std::string sJson)
{
    TaskQueue *pTask = new TaskQueue();
    pTask->cmd = cmd_push_queue;
    pTask->json = sJson;
    pthread_mutex_lock(&m_mutex);
    task_queue.push(pTask);
    pthread_mutex_unlock(&m_mutex);
}

void TTTEventHandler::onError(TTT_ERROR_CODE code, const char *message)
{
    Json::Value root;
    std::string s_msg = "";
    if (message) {
        s_msg = message;
    }
    root["err"] = code;
    root["msg"] = s_msg;
    root["callbackname"] = "onError";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onConnectionLost()
{
    Json::Value root;
    root["callbackname"] = "onConnectionLost";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onJoinChannelSuccess(const char* channel, long long uid, int elapsed)
{
    Json::Value root;
    std::string s_channel = "";
    if (channel) {
        s_channel = channel;
    }
    root["channel"] = s_channel;
    root["uid"] = uid;
    root["callbackname"] = "onJoinChannelSuccess";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onLeaveChannel(const TTT_RtcStats &stats)
{
    Json::Value root;
    root["callbackname"] = "onLeaveChannel";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onUserJoined(long long uid, int elapsed)
{
    Json::Value root;
    root["uid"] = uid;
    root["elapsed"] = elapsed;
    root["callbackname"] = "onUserJoined";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onUserOffline(long long uid, TTT_USER_OFFLINE_REASON reason)
{
    Json::Value root;
    root["uid"] = uid;
    root["reason"] = reason;
    root["callbackname"] = "onUserOffline";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onReportAudioVolumeIndication(long long userID, unsigned int audioLevel, unsigned int audioLevelFullRange)
{
    Json::Value root;
    root["nUserID"] = userID;
    root["audioLevel"] = audioLevel;
    root["audioLevelFullRange"] = audioLevelFullRange;
    root["callbackname"] = "onAudioVolumeIndication";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onReportRtcStats(const TTT_RtcStats &stats)
{
    Json::Value root;
    root["txBytes"] = stats.txBytes;
    root["rxBytes"] = stats.rxBytes;
    root["txAudioKBitRate"] = stats.txAudioKBitrate;
    root["rxAudioKBitRate"] = stats.rxAudioKBitrate;
    root["txVideoKBitRate"] = stats.txVideoKBitrate;
    root["rxVideoKBitRate"] = stats.rxVideoKBitrate;
    root["users"] = stats.users;
    root["duration"] = stats.duration;
    root["cpuAppUsage"] = 0;
    root["cpuTotalUsage"] = 0;
    root["callbackname"] = "onRtcStats";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onLocalVideoStats(const TTT_LocalVideoStats &stats)
{
    Json::Value root;
    root["encodedBitrate"] = stats.sentBitrate;
    root["sentBitrate"] = stats.sentBitrate;
    root["sentFrameRate"] = stats.sentFrameRate;
    root["receivedBitrate"] = stats.receivedBitrate;
    root["sentLossRate"] = stats.sentLossRate;
    root["callbackname"] = "onLocalVideoStats";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onLocalAudioStats(const TTT_LocalAudioStats &stats)
{
//    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onRemoteVideoStats(const TTT_RemoteVideoStats &stats)
{
    Json::Value root;
    std::stringstream ss;
    ss << stats.uid;
    root["devid"] = ss.str();
    root["uid"] = stats.uid;
    root["delay"] = stats.delay;
    root["width"] = stats.width;
    root["height"] = stats.height;
    root["receivedBitrate"] = stats.receivedBitrate;
    root["receivedFrameRate"] = stats.receivedFrameRate;
    root["receivedFrames"] = stats.receivedFrames;
    root["lostFrames"] = stats.lostFrames;
    root["callbackname"] = "onRemoteVideoStats";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onRemoteAudioStats(const TTT_RemoteAudioStats &stats)
{
//    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onVideoEnabled(int64_t uid, bool enabled)
{
//    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onAudioMuted(long long uid, bool muted)
{
    Json::Value root;
    root["uid"] = uid;
    root["muted"] = muted;
    root["callbackname"] = "onUserMuteAudio";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

#if TARGET_OS_IOS
void TTTEventHandler::onAudioRouteChanged(AUDIO_ROUTE routing)
{
    pushJsonToTaskQueue(sJson);
}
#endif

void TTTEventHandler::onAudioMixingFinished()
{
//    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onChatMessageSent(const TTT_ChatInfo &chatInfo, TTT_ERROR_CODE errorCode)
{
//    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onChatMessageReceived(long long userID, const TTT_ChatInfo &chatInfo)
{
//    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onChatAudioFinishPlaying(const char *fileName)
{
//    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onFirstLocalVideoFrame(int width, int height, int elapsed)
{
    Json::Value root;
    root["width"] = width;
    root["height"] = height;
    root["elapsed"] = elapsed;
    root["callbackname"] = "onFirstLocalVideoFrame";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onFirstRemoteVideoFrameDecoded(int64_t uid, int width, int height, int elapsed)
{
    Json::Value root;
    root["uid"] = uid;
    root["width"] = width;
    root["height"] = height;
    root["elapsed"] = elapsed;
    root["callbackname"] = "onFirstRemoteVideoDecoded";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onFirstRemoteVideoFrame(int64_t uid, int width, int height, int elapsed)
{
    Json::Value root;
    root["uid"] = uid;
    root["width"] = width;
    root["height"] = height;
    root["elapsed"] = elapsed;
    root["callbackname"] = "onFirstRemoteVideoFrame";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onLocalVideoFrameCaptured(const TTT_VideoFrame &videoFrame)
{
    Json::Value root;
    root["width"] = videoFrame.width;
    root["height"] = videoFrame.height;
    root["data"] = std::string(videoFrame.base64EncodedData.c_str());
    root["len"] = videoFrame.width * videoFrame.height * 3 / 2;
    root["uid"] = videoFrame.uid;
    root["callbackname"] = "onUserYuvPush";
    std::string sJson = root.toStyledString();
    
    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onRemoteVideoFrameDecoded(int64_t uid, const TTT_VideoFrame &videoFrame)
{
    Json::Value root;
    root["uid"] = uid;
    root["width"] = videoFrame.width;
    root["height"] = videoFrame.height;
    root["data"] = videoFrame.base64EncodedData;
    root["callbackname"] = "onRemoteVideoDecoded";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onCameraReady()
{
    Json::Value root;
    root["callbackname"] = "onCameraReady";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}

void TTTEventHandler::onVideoStopped()
{
    Json::Value root;
    root["callbackname"] = "onVideoStopped";
    std::string sJson = root.toStyledString();

    pushJsonToTaskQueue(sJson);
}
