#ifndef __TTT_RTCENGINE_PANDA_H__  //潘达项目
#define __TTT_RTCENGINE_PANDA_H__
#ifdef RTCENGINE_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include "TTTstruct.h"
/**
* 主体SDK的回调接收基类，用户可自定义类继承此类，实现SDK回调的接收
*/
class TTTRtcEngineEventHandler
{
public:
    virtual ~TTTRtcEngineEventHandler() {}
	/**
	* 加入频道回调，表示客户端已经登入服务器
	*
	* @param channel 频道名称
	* @param uid     登录的用户ID
	*/
	virtual void onJoinChannelSuccess(const char* channel, uid_ttt uid) {
		(void)channel;
		(void)uid;
	}

	/**
	* 表示SDK运行时出现了（网络或媒体相关的）错误。
	*
	* @param err 错误码 
	*/
	virtual void onError(int err, const char* msg) {
		(void)err;
		(void)msg;
	}

	/**
	* 说话声音音量提示回调。默认禁用。可以通过 enableAudioVolumeIndication 方法设置。
	*
	* @param nUserID             说话者的用户ID
	* @param audioLevel          说话者的音量，在0-9之间
	* @param audioLevelFullRange 说话者的音量，音量范围更大0-32767.5之间
	*/
	virtual void onAudioVolumeIndication(long nUserID, int audioLevel, int audioLevelFullRange) {
		(void)nUserID;
		(void)audioLevel;
		(void)audioLevelFullRange;
	}




	/**
	* 应用程序调用leaveChannel()方法时，SDK提示应用程序离开频道成功。
	*
	* @param stats 通话相关的统计信息。
	*/
	virtual void onLeaveChannel(const RtcStats_TTT& stats) {
		(void)stats;
	}

	/**
	* 统计数据回调。该回调定期上报 Engine 的运行时的状态，每两秒触发一次
	*
	* @param stats 通话相关的统计信息<br/>
	*              totalDuration: 通话时长（秒），累计值<br/>
	*              txBytes: 发送字节数（bytes), 累计值<br/>
	*              rxBytes: 接收字节数（bytes), 累计值<br/>
	*              txKBitRate: 发送码率（kbps), 瞬时值<br/>
	*              rxKBitRate: 接收码率（kbps), 瞬时值<br/>
	*              lastmileQuality: 客户端接入网络质量<br/>
	*              cpuTotalQuality: 当前系统的CPU使用率（%）<br/>
	*              cpuAppQuality: 当前应用程序的CPU使用率（%）<br/>
	*/
	virtual void onRtcStats(const RtcStats_TTT& stats) {
		(void)stats;
	}
	/**
	* 报告测试网络质量
	* @param quality
	* 网络质量评分 0 ~ 5 越高越好
	*/
	virtual void onLastmileQuality(int quality) {
		(void)quality;
	}
	/**
	* 本地视频显示回调，提示第一帧本地视频画面已经显示在屏幕上。
	*
	* @param width  该用户视频显示的宽度
	* @param height 该用户视频显示的高度
	*/
	virtual void onFirstLocalVideoFrame(int width, int height, int elapsed) {
		(void)width;
		(void)height;
		(void)elapsed;
	}

	/**
	* 收到第一帧远程视频流并解码成功时，触发此调用。应用程序可以在此回调中设置该用户的view。
	*
	* @param uid    加入频道的用户ID
	* @param width  视频流宽
	* @param height 视频流高
	*/
	virtual void onFirstRemoteVideoDecoded(uid_ttt uid, int width, int height, int elapsed) {
		(void)uid;
		(void)width;
		(void)height;
		(void)elapsed;
	}

	/**
	* 远端视频显示回调,第一帧远程视频显示在视图上时，触发此调用
	*
	* @param uid    加入频道的用户ID
	* @param width  该用户视频显示的宽度
	* @param height 该用户视频显示的高度
	*/
	virtual void onFirstRemoteVideoFrame(uid_ttt uid, int width, int height, int elapsed) {
		(void)uid;
		(void)width;
		(void)height;
		(void)elapsed;
	}


	/**
	* 其他用户加入当前频道回调
	*
	* @param uid      加入频道的用户ID
	* @param identity 加入频道的用户的身份，副播或观众
	*/
	virtual void onUserJoined(uid_ttt uid, int elapsed) {
		(void)uid;
		(void)elapsed;
	}


	/**
	* 其他用户离开当前频道回调，主动离开或掉线
	*
	* @param uid    离开频道的用户ID
	* @param reason 该用户离开频道的原因
	*/
	virtual void onUserOffline(uid_ttt uid, USER_OFFLINE_REASON_TYPE_TTT reason) {
		(void)uid;
		(void)reason;
	}
	


	/**
	* 提示有其他用户启用/关闭了视频功能。关闭视频功能是指该用户只能进行语音通话，不能显示、
	* 发送自己的视频，也不能接收、显示别人的视频。
	*
	* @param uid     用户ID
	* @param enabled 用户已启用/关闭了视频功能
	*/
	virtual void onUserEnableVideo(uid_ttt uid, bool enabled) {
		(void)uid;
		(void)enabled;
	}
	/**
	* 本地视频统计回调，报告更新本地视频统计信息，该回调函数每两秒触发一次
	*
	* @param stats 本地视频相关的统计信息，包含:
	*              sentBitrate:（上次统计后）发送的码率(kbps)
	*              sentFrameRate:（上次统计后）发送的帧率(fps)
	*/
	virtual void onLocalVideoStats(const LocalVideoStats_TTT& stats) {
		(void)stats;
	}

	/**
	* 远端视频统计回调，报告更新远端视频统计信息，该回调函数每两秒触发一次
	*
	* @param stats 远端视频的统计信息，包含:
	*              uid: 用户ID，指定是哪个用户的视频流
	*              receivedBitrate: 接收码率(kbps)
	*              receivedFrameRate: 接收帧率(fps)
	*/
	virtual void onRemoteVideoStats(const RemoteVideoStats_TTT& stats) {
		(void)stats;
	}

	/**
	* 连接丢失回调
	*/
	virtual void onConnectionLost() {}



	/**
	* 聊天消息发送的回调
	*
	* @param sSeqID 消息的ID
	* @param error  发送的状态码
	*/

	virtual void OnChatMessageSent(const char* sSeqID, int error) {
		(void)sSeqID;
		(void)error;
	}


	/**
	* 聊天消息接收的回调
	*
	* @param nSrcUserID 该消息的发送者ID
	* @param type       消息类型(语音,图片和文字),图片暂不支持
	* @param sSeqID     消息ID
	* @param strData    消息体XML
	*/
	virtual void OnChatMessageRecived(long nSrcUserID, int type, const char* sSeqID, const char* strData) {
		(void)nSrcUserID;
		(void)type;
		(void)sSeqID;
		(void)strData;
	}

	/********************
	以下是多流功能提供的回调
	*/

	/**
	* 其他用户加入当前频道回调
	*
	* @param uid      加入频道的用户ID
	* @param uid      加入频道的用户的设备id.有的用户会有多个设备
	* @param identity 加入频道的用户的身份，副播或观众
	*/
	virtual void onUserJoined(uid_ttt uid, const char* devId,  int elapsed) {
		(void)uid;
		(void)devId;
		(void)elapsed;
	}
	/**
	* 收到每个用户yuv数据
	* @return  0代表方法调用成功，其他代表失败。
	*/
	virtual void onRemoteVideoDecoded(long long uid,VideoFrame_TTT& mVideoFrame) {
		(void)uid;
		(void)mVideoFrame;
	}

	/**
	* 收到每个用户pcm数据
	* @return  0代表方法调用成功，其他代表失败。
	*/
	virtual void onRemoteAudioDecoded(long long uid, AudioFrame_TTT& mAudioFrame) {
		(void)uid;
		(void)mAudioFrame;
	}
	/**
	* 远端开始共享mp4视频视频的回调
	*
	* @param uid 共享mp4视频用户的uid
	*/		
	virtual void onMp4Joined(uid_ttt uid) {
		(void)uid;
	}
	/**
	* 远端停止共享mp4视频视频的回调
	*
	* @param uid 停止共享mp4视频用户的uid
	*/
	virtual void onMp4Offline(uid_ttt uid) {
		(void)uid;
	}

	//add by liyong at 2010.10.11 不同直播间之间的连麦功能
	/**
	* 收到其他房间用户连麦请求
	* @param roomID  请求连麦的房间号
	* @param operUserID 请求连麦的用户ID
	*/
	virtual void OnOtherAnchorLink(long long roomID, long long operUserID) {
		(void)roomID;
		(void)operUserID;
	}

	/**
	* 用户发起连麦请求后的回调方法
	* @param roomID  远端房间的ID，
	* @param operUserID 远端房间的用户ID
	* @param operDevID 远端用户的设备描述，列表
	* @param nError 0表示成功，其他表示失败
	*/
	virtual void OnOtherAnchorLinked(long long roomID, long long operUserID, const char* operDevID, int nError) {

	}

	/**
	* 收到其他房间用户断开连麦的请求
	* @param roomID  请求断麦的房间号
	* @param operUserID 请求断麦的用户ID
	*/
	virtual void OnOtherAnchorUnlink(long long roomID, long long operUserID) {

	}

	/**
	* 收到用户断开连麦的响应
	* @param roomID  请求断麦的房间号
	* @param operUserID 请求断麦的用户ID
	*/
	virtual void OnOtherAnchorUnLinked(long long roomID, long long operUserID) {

	}

	/**
	* 視頻流回调输出给第三方
	*
	* @param uid      加入频道的用户ID
	* @param identity 加入频道的用户的身份，副播或观众
	*/
	virtual void onUserYuvPush(uid_ttt uid, unsigned char* Yuv, int width, int height) {
		(void)uid;
		(void)Yuv;
		(void)width;
		(void)height;
	}


	
};
struct RtcEngineContext_TTT
{
	TTTRtcEngineEventHandler* eventHandler;
	const char* appId;
	RtcEngineContext_TTT()
		:eventHandler(NULL)
		, appId(NULL)
	{}
};
class DLL_EXPORT TTTRtcEngine
{
public:
    /**
     *  获取接口对象，singleton
     *
     *  @return 接口对象
     */
	static TTTRtcEngine* createInstance();

	/**
	* 初始化SDK，在程序生命周期中只需要调用一次，即便执行destroy函数也不需要调用
	*/
    virtual int initialize( RtcEngineContext_TTT& context) = 0;

	/**
	* 查询 SDK 版本号
	*
	* @return 返回 SDK 版本号字符串
	*/
	virtual char* getVersion() = 0;
	/**
	* 加入视频通信房间，需要异步调用
	*
	* @param channelKey  频道key
	* @param channelName 频道名字
	* @param optionalUid 用户ID
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int joinChannel(char *channelKey, char *channelName, uid_ttt optionalUid) = 0;

	/**
	* SDK的反初始化操作
	*/
	virtual void destroy() = 0;

	/**
	* 离开视频通信房间
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int leaveChannel() = 0;

	/**
	* 设置频道模式
	*
	* @param profile 频道模式
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setChannelProfile(RoomMode_TTT profile) = 0;

	/**
	* 设置用户角色。在加入频道前，用户需要通过本方法设置观众(默认)或主播模式。在加入频道后，用户可以通过本方法切换用户模式。
	*
	* @param role          直播场景里的用户角色。
	* @param permissionKey 连麦鉴权功能暂时没开放，将其设置为空。
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setClientRole(int role, const char* permissionKey) = 0;

	/**
	* 开启本地视频预览画面
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int startPreview() = 0;


	/**
	* 关闭本地视频预览画面
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int stopPreview() = 0;

	/**
	* 设置本地视频属性
	*
	* @param profile            预设的视频质量
	* @param swapWidthAndHeight 是否将视频的宽和高互调
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setVideoProfile(int profile, bool swapWidthAndHeight) = 0;


	/**
	* 设置远端视频显示属性
	*
	* @param remote 视频显示属性对象
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setupRemoteVideo(const VideoCanvas_TTT& canvas) = 0;


	/**
	* 设置本地视频显示属性
	*
	* @param local               视频显示属性对象
	* @param activityOrientation activity的显示方向
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setupLocalVideo(const VideoCanvas_TTT& canvas) = 0;


	/**
	* 启动网络测试
	* 如果成功或错误代码 返回0
	*/
	virtual int enableLastmileTest() = 0;

	/**
	* 停止网络测试
	* 如果成功或错误代码返回0
	*/
	virtual int disableLastmileTest() = 0;

	/**
	* 启用视频模式
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int enableVideo() = 0;

	/**
	* 禁用视频模式
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int disableVideo() = 0;


	/**
	* 禁用音频模式
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int disableAudio() = 0;

	/**
	* 启用音频模式
	*
	* @return 0代表方法调用成功，其他代表失败
	*/
	virtual int enableAudio() = 0;

	virtual int startScreenCapture(const Rect_TTT *rect) = 0;
	virtual int stopScreenCapture() = 0;

	/**
	* 设置画中画布局
	*
	* @param layout the layout
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setVideoCompositingLayout(const VideoCompositingLayout_TTT& sei) = 0;

	virtual int setVideoCompositingLayoutSei(const char*  sei, int len) = 0;

	/**
	* 该方法用于在加入频道前为引擎创建一份推流设置。我们提供一个 Builder 类方便配置旁路直播推流，例如:
	*
	* @param config 封装配置信息的类
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int configPublisher(const PublisherConfiguration_TTT& config) = 0;

	/**
	* 静音/取消静音。该方法用于允许/禁止往网络发送本地音频流。
	*
	* @param muted 静音/取消静音
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int muteLocalAudioStream(bool muted) = 0;

	/**
	* 该方法用于允许/禁止播放远端用户的音频流，即对所有远端用户进行静音与否。
	*
	* @param muted True麦克风静音，False取消静音
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int muteAllRemoteAudioStreams(bool mute) = 0;

	/**
	* 静音指定远端用户/对指定远端用户取消静音。本方法用于允许/禁止播放远端用户的音频流。
	*
	* @param uid   指定用户
	* @param muted true麦克风静音，false取消静音
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int muteRemoteAudioStream(uid_ttt uid, bool mute) = 0;

	/**
	* 暂停/继续发送本地视频流
	*
	* @param muted True: 不发送本地视频流，False: 发送本地视频流
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int muteLocalVideoStream(bool muted) = 0;

	/**
	* 暂停所有远端视频流
	*
	* @param muted True: 停止播放用户的视频流，False: 允许播放用户的视频流
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int muteAllRemoteVideoStreams(bool mute) = 0;

	/**
	* 该方法允许SDK定期向应用程序反馈当前谁在说话以及说话者的音量。
	*
	* @param interval 指定音量提示的时间间隔。小于0禁用音量提示功能，单位为毫秒
	* @param smooth   平滑系数。默认可以设置为3。
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int enableAudioVolumeIndication(int interval, int smooth) = 0;

	/**
	* 开始客户端本地混音。指定本地音频文件来和麦克风采集的音频流进行混音和替换(用音频文件替换麦克风采集的音频流)，
	* 可以通过参数选择是否让对方听到本地播放的音频和指定循环播放的次数。
	*
	* @param filePath 指定需要混音的本地音频文件名和文件路径。支持以下音频格式：mp3, aac, m4a, 3gp, wav, flac。
	* @param loopback True只有本地可以听到混音或替换后的音频流，False本地和对方都可以听到混音或替换后的音频流。
	* @param replace  True音频文件内容将会替换本地录音的音频流，False音频文件内容将会和麦克风采集的音频流进行混音
	* @param cycle    指定音频文件循环播放的次数。正整数代表循环的次数
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int startAudioMixing(const char* filePath, bool loopback, bool replace, int cycle) = 0;


	/**
	* 使用该方法停止伴奏播放，请在频道内调用该方法。
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int stopAudioMixing() = 0;

	/**
	* 使用该方法暂停伴奏播放。请在频道内调用该方法。
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int pauseAudioMixing() = 0;

	/**
	* 使用该方法恢复混音，继续播放伴奏。请在频道内调用该方法。.
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int resumeAudioMixing() = 0;

	/**
	* 使用该方法调节混音里伴奏与人声的音量比例大小。请在频道内调用该方法。
	*
	* @param volume 伴奏与人声音量比例范围为0~100。0代表无伴奏声音，100代表无人声。
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int adjustAudioMixingVolume(int volume) = 0;

	/**
	* 使用该方法获取伴奏时长，单位为毫秒。请在频道内调用该方法。
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int getAudioMixingDuration() = 0;

	/**
	* 使用该方法获取当前伴奏播放进度，单位为毫秒。请在频道内调用该方法。
	*
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int getAudioMixingCurrentPosition() = 0;

	/**
	* 设置当前播放的进度。
	*/
	virtual int setAudioMixingPosition(int volume) = 0;

	/**
	* 设置音频高音质选项。切勿在加入频道后再次调用本方法。
	*
	* @param enable true启用，false禁用
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setHighQualityAudioParameters(bool enable) = 0;




	/**
	* 所有视频充满窗体
	*/
	virtual int resizeRreview() = 0;

	/**
	* 暂停指定用户的远端视频流
	*
	* @param uid   用户ID
	* @param muted True: 停止播放指定用户的视频流，False: 允许播放指定用户的视频流
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int muteRemoteVideoStream(uid_ttt uid, bool mute) = 0;

	/**
	* 将某个远端用户静音，其他所有用户都无法听到该用户的说话
	*
	* @param uid       要被静音的用户ID
	* @param isDisable true代表用户静音，false代表不静音
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int muteRemoteSpeaking(uid_ttt uid, bool isDisable) = 0;

	/**
	* 将某个用户请出直播房间
	*
	* @param uid 被请出的用户ID
	* @return 是否成功将用户请出
	*/
	virtual bool kickChannelUser(uid_ttt uid) = 0;

	/**
	* 请求和远端房间连麦
	*
	* @param sessionId 远端房间号
	* @param userId 远端主播号码
	*/
	virtual void  linkOtherAnchor(uid_ttt sessionId, uid_ttt userId) = 0;

	/**
	* 断开与远端房间的连麦
	*
	* @param sessionId 远端房间号
	* @param userId 远端主播号码
	* @param devId   远端视频设备ID
	*/
	virtual void  unlinkOtherAnchor(long long sessionId, long long userId, const char* devId) = 0;

	/**
	* 使用该方法设定扬声器音量。
	*
	* @param volume 设定音量，最小为0，最大为255
	* @return 0代表方法调用成功，其他代表失败。
	*/
	virtual int setSpeakerphoneVolume(int volume) = 0;

	virtual int sendChatMessage(uid_ttt nDstUserID,int type,char*  sData) = 0;


	/**
	* 测试麦克风设备测试录音音频设备，知道它是否能正常工作。
	* @param [in] indicationInterval
	*        本地采集声音音量上报回调的时间间隔
	* @return  0代表方法调用成功，其他代表失败。
	*/
	virtual int startRecordingDeviceTest(int indicationInterval) = 0;
	
	/**
	* 停止录制音频设备测试
	* @return  0代表方法调用成功，其他代表失败。
	*/
	virtual int stopRecordingDeviceTest() = 0;

	virtual int startPlaybackDeviceTest(const char* testAudioFilePath) = 0;
	virtual int stopPlaybackDeviceTest() = 0;
	virtual int startVideotDeviceTest(view_ttt hwnd) = 0;
	virtual int stopVideoDeviceTest() = 0;
	/**
	* 设置远端mp4视频显示属性,收到mp4开始的回调后调用
	*
	* @param canvas 视频显示属性对象
	* @return 0代表方法调用成功，其他代表失败。
	*/	
	virtual int setupRemoteMp4(const VideoCanvas_TTT& canvas) = 0;	
	/**
	* 开始共享mp4视频 ，进入房间后调用
	*
	* @param hVideoWnd 本地显示mp4视频的窗口句柄
	* @return 0代表方法调用成功，其他代表失败。
	*/	
	virtual int startPlayMp4(const char* filePath, void* hVideoWnd) = 0;
	/**
	* 停止共享mp4视频
	*/	
	virtual int stopPlayMp4() = 0;

};
#endif
