#include "stdafx.h"

#include <chrono>
#include <thread>
#include <iostream>
#include "TTTRtcEngine.h"
#include "TTTEngineEventHandler.h"
#include <windows.h>
#include "JSON\json\json.h"
#include "JSON\json\json-forwards.h"
extern "C"
{
#include "base64.h"
};

extern std::queue<TaskQueue*> task_queue;
extern CRITICAL_SECTION m_queue_cs;
CTTTEngineEventHandler::CTTTEngineEventHandler(void)
{
	m_nSecondaryIndex = 1;
}

CTTTEngineEventHandler::~CTTTEngineEventHandler(void)
{

}


void CTTTEngineEventHandler::onJoinChannelSuccess(const char* channel, uid_ttt uid)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	std::string s_channel = "";
	if (channel) {
		s_channel = channel;
	}
	root["channel"] = s_channel;
	root["uid"] = uid;
	root["callbackname"] = "onJoinChannelSuccess";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onError(int err, const char* msg)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	std::string s_msg = "";
	if (msg) {
		s_msg = msg;
	}
	root["err"] = err;
	root["msg"] = s_msg;
	root["callbackname"] = "onError";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onAudioVolumeIndication(long nUserID, int audioLevel, int audioLevelFullRange)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["nUserID"] = nUserID;
	root["audioLevel"] = audioLevel;
	root["audioLevelFullRange"] = audioLevelFullRange;
	root["callbackname"] = "onAudioVolumeIndication";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onLeaveChannel(const RtcStats_TTT& stat)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;

	root["callbackname"] = "onLeaveChannel";


	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onRtcStats(const RtcStats_TTT& stat)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;

	root["callbackname"] = "onRtcStats";
	root["cpuAppUsage"] = stat.cpuAppUsage;
	root["cpuTotalUsage"] = stat.cpuTotalUsage;
	root["rxAudioKBitRate"] = stat.rxAudioKBitRate;
	root["rxBytes"] = stat.rxBytes;

	root["rxVideoKBitRate"] = stat.rxVideoKBitRate;

	root["txAudioKBitRate"] = stat.txAudioKBitRate;
	root["txBytes"] = stat.txBytes;
	root["txVideoKBitRate"] = stat.txVideoKBitRate;
	root["users"] = stat.users;

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onLastmileQuality(int quality) {
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;

	root["callbackname"] = "onLastmileQuality";
	root["quality"] = quality;

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onFirstLocalVideoFrame(int width, int height, int elapsed)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;

	root["callbackname"] = "onFirstLocalVideoFrame";
	root["width"] = width;
	root["elapsed"] = elapsed;

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onFirstRemoteVideoDecoded(uid_ttt uid, int width, int height, int elapsed)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["width"] = width;
	root["height"] = height;
	root["elapsed"] = elapsed;
	root["callbackname"] = "onFirstRemoteVideoDecoded";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);

}
void CTTTEngineEventHandler::onRejoinChannelSuccess(const char* channel, uid_ttt uid, int elapsed)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	std::string s_channel = "";
	if (channel) {
		s_channel = channel;
	}
	root["channel"] = s_channel;
	root["uid"] = uid;
	root["elapsed"] = elapsed;
	root["callbackname"] = "onRejoinChannelSuccess";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onWarning(int warn, const char* msg)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	std::string s_str =  "";
	if (msg) {
		s_str = msg;
	}
	root["warn"] = warn;
	root["msg"] = s_str;
	root["callbackname"] = "onWarning";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}



void CTTTEngineEventHandler::onAudioQuality(uid_ttt uid, int quality, unsigned short delay, unsigned short lost)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["uid"] = uid;
	root["quality"] = quality;
	root["delay"] = delay;
	root["lost"] = lost;

	root["callbackname"] = "onAudioQuality";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onMediaEngineEvent(int evt)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["evt"] = evt;
	root["callbackname"] = "onMediaEngineEvent";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);

}

void CTTTEngineEventHandler::onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	std::string s_str = "";
	if (deviceId) {
		s_str = deviceId;
	}
	Json::Value root;
	root["deviceType"] = deviceType;
	root["deviceState"] = deviceState;
	root["callbackname"] = "onAudioDeviceStateChanged";


	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	std::string s_str = "";
	if (deviceId) {
		s_str = deviceId;
	}	
	Json::Value root;
	root["deviceType"] = deviceType;
	root["deviceState"] = deviceState;
	root["callbackname"] = "onVideoDeviceStateChanged";


	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onNetworkQuality(int quality)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	root["quality"] = quality;
	root["callbackname"] = "onNetworkQuality";


	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onFirstRemoteVideoFrame(uid_ttt uid, int width, int height, int elapsed)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	root["uid"] = uid;
	root["width"] = width;
	root["height"] = height;
	root["elapsed"] = elapsed;

	root["callbackname"] = "onFirstRemoteVideoFrame";


	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::OnChatMessageRecived(long nSrcUserID, int type, const char* sSeqID, const char* strData) {
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	std::string s_str1 = "";
	if (sSeqID) {
		s_str1 = sSeqID;
	}
	std::string s_str2 = "";
	if (strData) {
		s_str2 = strData;
	}

	Json::Value root;
	root["nSrcUserID"] = nSrcUserID;
	root["type"] = type;
	root["sSeqID"] = s_str1;
	root["strData"] = s_str2;

	root["callbackname"] = "onChatMessageRecived";


	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onUserJoined(uid_ttt uid, int elapsed)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	root["uid"] = uid;
	root["elapsed"] = elapsed;
	root["callbackname"] = "onUserJoined";


	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onUserOffline(uid_ttt uid, USER_OFFLINE_REASON_TYPE_TTT reason)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	root["uid"] = uid;
	root["reason"] = reason;
	root["callbackname"] = "onUserOffline";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onUserMuteAudio(uid_ttt uid, bool muted)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	root["uid"] = uid;
	root["muted"] = muted;
	root["callbackname"] = "onUserMuteAudio";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onUserMuteVideo(uid_ttt uid, bool muted)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	root["uid"] = uid;
	root["muted"] = muted;
	root["callbackname"] = "onUserMuteVideo";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onApiCallExecuted(const char* api, int error)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	std::string s_str1 = "";
	if (api) {
		s_str1 = api;
	}
	Json::Value root;
	root["api"] = s_str1;
	root["error"] = error;
	root["callbackname"] = "onApiCallExecuted";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onLocalVideoStats(const LocalVideoStats_TTT& stats)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["sentBitrate"] = stats.sentBitrate;
	root["sentFrameRate"] = stats.sentFrameRate;
	root["callbackname"] = "onLocalVideoStats";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onRemoteVideoStats(const RemoteVideoStats_TTT& stats)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	std::string s_str1 = "";
	if (stats.devid) {
		s_str1 = stats.devid;
	}

	root["sentFrameRate"] = stats.delay;
	root["devid"] = s_str1;
	root["sentFrameRate"] = stats.height;
	root["sentFrameRate"] = stats.receivedBitrate;
	root["sentFrameRate"] = stats.receivedFrameRate;
	root["sentFrameRate"] = stats.uid;
	root["sentFrameRate"] = stats.width;
	root["callbackname"] = "onRemoteVideoStats";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onCameraReady()
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;

	root["callbackname"] = "onCameraReady";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onVideoStopped()
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["callbackname"] = "onVideoStopped";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::onConnectionLost()
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["callbackname"] = "onConnectionLost";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
}

void CTTTEngineEventHandler::onConnectionInterrupted()
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["callbackname"] = "onConnectionInterrupted";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
}

void CTTTEngineEventHandler::onUserEnableVideo(uid_ttt uid, bool enabled)
{
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["uid"] = uid;
	root["enabled"] = enabled;
	root["callbackname"] = "onUserEnableVideo";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
}
void CTTTEngineEventHandler::onRemoteVideoDecoded(long long uid, VideoFrame_TTT& mVideoFrame)
 {
	OutputDebugString("onRemoteVideoDecodered data func!");
		TaskQueue *pTask = new TaskQueue();
		pTask->cmd = cmd_push_queue;
		Json::Value root;
		int width = mVideoFrame.width;
		int height = mVideoFrame.height;
		int len = width * height * 3; //yuv 3/2 rgb 3
		int b64strlen = base64_enclen(len);
		std::string result;
		result.resize(b64strlen);
		base64_encode(&((uint8_t*)mVideoFrame.buf)[0], len, (uint8_t*)&result[0]);
		result = std::move(result);

		root["width"] = mVideoFrame.width;
		root["height"] = mVideoFrame.height;
		root["data"] = result;
		root["uid"] = uid;
		root["callbackname"] = "onRemoteVideoDecoded";
		int dataLen = base64_declen(result.length());


		std::string sJson = root.toStyledString();
		pTask->json = sJson;


		EnterCriticalSection(&m_queue_cs);
		task_queue.push(pTask);
		LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onRemoteAudioDecoded(long long uid, AudioFrame_TTT& mAudioFrame) {
}
void CTTTEngineEventHandler::OnOtherAnchorLink(long long roomID, long long operUserID) {

	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	root["roomID"] = roomID;
	root["operUserID"] = operUserID;

	root["callbackname"] = "onOtherAnchorLink";
	std::string sJson = root.toStyledString();
	pTask->json = sJson;


	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::OnOtherAnchorLinked(long long roomID, long long operUserID, const char* operDevID, int nError) {
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	std::string s_str1 = "";
	if (operDevID) {
		s_str1 = operDevID;
	}
	Json::Value root;
	root["roomID"] = roomID;
	root["operUserID"] = operUserID;
	root["operDevID"] = s_str1;
	root["nError"] = nError;
	root["callbackname"] = "onOtherAnchorLink";
	std::string sJson = root.toStyledString();
	pTask->json = sJson;


	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::OnOtherAnchorUnlink(long long roomID, long long operUserID) {
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;

	root["roomID"] = roomID;
	root["operUserID"] = operUserID;
	root["callbackname"] = "onOtherAnchorUnlink";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}

void CTTTEngineEventHandler::OnOtherAnchorUnLinked(long long roomID, long long operUserID) {
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;
	Json::Value root;
	root["callbackname"] = "onOtherAnchorUnLinked";

	root["roomID"] = roomID;
	root["operUserID"] = operUserID;

	std::string sJson = root.toStyledString();
	pTask->json = sJson;
	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::onUserYuvPush(uid_ttt uid, unsigned char* Yuv, int width, int height) {
	TaskQueue *pTask = new TaskQueue();
	pTask->cmd = cmd_push_queue;

	Json::Value root;
	int len = width * height * 3 / 2;
	int b64strlen = base64_enclen(len);
	std::string result;
	result.resize(b64strlen);
	base64_encode(&Yuv[0], len, (uint8_t*)&result[0]);
	result = std::move(result);

	root["width"] = width;
	root["height"] = height;
	root["data"] = result;
	root["len"] = len;
	root["uid"] = uid;
	root["callbackname"] = "onUserYuvPush";

	std::string sJson = root.toStyledString();
	pTask->json = sJson;


	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
void CTTTEngineEventHandler::OnUserH264Push(const char* data, int len, const char* devId, long long ts, int width, int height, VideoFrameType_TTT frameType)
{
	TaskQueue *pTask = new TaskQueue();

	pTask->cmd = cmd_push_queue;
	pTask->length = len;

	Json::Value root;
	Json::FastWriter writer;
	Json::Value canvas;
	Json::Value arrayObj;



	int b64strlen = base64_enclen(len);
	std::string result;
	result.resize(b64strlen);
	base64_encode(&data[0], len,  (uint8_t*)&result[0]);
	result = std::move(result);
	root["callbackname"] = "OnUserH264Push";

	std::string strId = devId;
	root["width"] = width;
	root["height"] = height;
	root["data"] = result;
	root["len"] = len;
	root["uid"] = strId;
	std::string sJson = root.toStyledString();
	pTask->json = sJson;

	int nMsgLen;
	if (devId != NULL) {
		nMsgLen = sJson.length() + 1;// strlen(devId) + 1;
		pTask->devId = new char[nMsgLen];
		strcpy_s(pTask->devId, nMsgLen, sJson.c_str());
	}
	else {
		pTask->devId = NULL;
	}


	pTask->width = width;
	pTask->height = height;
	pTask->VideoFrameType = int(frameType);

	EnterCriticalSection(&m_queue_cs);
	task_queue.push(pTask);
	LeaveCriticalSection(&m_queue_cs);
}
