#pragma once

class CTTTEngineEventHandler :
	public TTTRtcEngineEventHandler
{
public:
	CTTTEngineEventHandler(void);
	~CTTTEngineEventHandler(void);

	//void SetMsgReceiver(HWND hWnd = NULL);
	//void SetMsgReceiver(IMeetingFrameEvent* pEvent);
	//HWND GetMsgReceiver() {return m_hMainWnd;};

	virtual void onJoinChannelSuccess(const char* channel, uid_ttt uid);
	virtual void onError(int err, const char* msg);
	virtual void onAudioVolumeIndication(long nUserID, int audioLevel, int audioLevelFullRange);
	virtual void onLeaveChannel(const RtcStats_TTT& stat);
	virtual void onRtcStats(const RtcStats_TTT& stat);
	virtual void onLastmileQuality(int quality);
	virtual void onFirstLocalVideoFrame(int width, int height, int elapsed);
	virtual void onFirstRemoteVideoDecoded(uid_ttt uid, int width, int height, int elapsed);
	virtual void onFirstRemoteVideoFrame(uid_ttt uid, int width, int height, int elapsed);

	virtual void onRejoinChannelSuccess(const char* channel, uid_ttt uid, int elapsed);
	virtual void onWarning(int warn, const char* msg);
	virtual void onAudioQuality(uid_ttt uid, int quality, unsigned short delay, unsigned short lost);
	virtual void onMediaEngineEvent(int evt);

	virtual void onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState);
	virtual void onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState);

	virtual void onNetworkQuality(int quality);
	virtual void OnChatMessageRecived(long nSrcUserID, int type, const char* sSeqID, const char* strData);
	virtual void onUserJoined(uid_ttt uid, int elapsed);
	virtual void onUserOffline(uid_ttt uid, USER_OFFLINE_REASON_TYPE_TTT reason);
	virtual void onUserMuteAudio(uid_ttt uid, bool muted);
	virtual void onUserMuteVideo(uid_ttt uid, bool muted);
	virtual void onApiCallExecuted(const char* api, int error);

	virtual void onLocalVideoStats(const LocalVideoStats_TTT& stats);
	virtual void onRemoteVideoStats(const RemoteVideoStats_TTT& stats);
	virtual void onCameraReady();
	virtual void onVideoStopped();
	virtual void onConnectionLost();
	virtual void onConnectionInterrupted();

	virtual void onUserEnableVideo(uid_ttt uid, bool enabled);
	virtual void onRemoteVideoDecoded(long long uid, VideoFrame_TTT& mVideoFrame);
	virtual void onRemoteAudioDecoded(long long uid, AudioFrame_TTT& mAudioFrame);
	virtual void OnOtherAnchorLink(long long roomID, long long operUserID);
	virtual void OnOtherAnchorLinked(long long roomID, long long operUserID, const char* operDevID, int nError);
	virtual void OnOtherAnchorUnlink(long long roomID, long long operUserID);
	virtual void OnOtherAnchorUnLinked(long long roomID, long long operUserID);
	virtual void onUserYuvPush(uid_ttt uid, unsigned char* Yuv, int width, int height);
	virtual void OnUserH264Push(const char* data, int len, const char* devId, long long ts, int width, int height, VideoFrameType_TTT frameType);

private:
	//HWND		m_hMainWnd;
	//IMeetingFrameEvent* m_pEvent;
	int m_nSecondaryIndex;
};
