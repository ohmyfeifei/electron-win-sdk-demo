#ifndef TTTRTCENGINE_HPP
#define TTTRTCENGINE_HPP

#include <string>
#include <vector>

#if defined(__APPLE__)
#define TTT_API __attribute__((visibility("default"))) extern "C"
#elif defined(__ANDROID__) || defined(__linux__) || defined(__linux)
#define TTT_API extern "C" __attribute__((visibility("default")))
#else
#define TTT_API extern "C"
#endif

enum TTT_ERROR_CODE
{
    ERROR_NOERROR              = 0,    // 没有错误
    ERROR_INVALID_CHANNEL_NAME = 9000, // 无效的房间名称
    ERROR_ENTER_TIMEOUT        = 9001, // 超时,10秒未收到服务器返回结果
    ERROR_ENTER_FAILED         = 9002, // 无法连接服务器
    ERROR_ENTER_VERIFYFAILED   = 9003, // 验证码错误
    ERROR_ENTER_BADVERSION     = 9004, // 版本错误
    ERROR_ENTER_UNKNOWN        = 9005, // 未知错误
    ERROR_ENTER_NOANCHOR       = 9006, // 房间内没有主播
    ERROR_NO_AUDIODATA         = 9101, // 长时间没有上行音频数据
    ERROR_NO_VIDEODATA         = 9102, // 长时间没有上行视频数据
    ERROR_NO_RECEIVEDAUDIODATA = 9111, // 长时间没有下行音频数据
    ERROR_NO_RECEIVEDVIDEODATA = 9112, // 长时间没有下行视频数据
    ERROR_INVALID_CHANNELKEY   = 9200, // 无效的channelKey
    ERROR_UNKNOWN              = 9999  // 未知错误
};

enum TTT_LOG_FILTER
{
    LOG_FILTER_OFF      = 0,
    LOG_FILTER_DEBUG    = 0x080f,
    LOG_FILTER_INFO     = 0x000f,
    LOG_FILTER_WARNING  = 0x000e,
    LOG_FILTER_ERROR    = 0x000c,
    LOG_FILTER_CRITICAL = 0x0008,
};

enum TTT_CHANNEL_PROFILE {
    CHANNEL_PROFILE_COMMUNICATION    = 0, // 通信
    CHANNEL_PROFILE_LIVEBROADCASTING = 1, // 直播
    CHANNEL_PROFILE_GAME_FREEMODE    = 2, // 游戏（自由发言模式）
};

enum TTT_CLIENT_ROLE
{
    CLIENT_ROLE_ANCHOR      = 1, // 主播
    CLIENT_ROLE_BROADCASTER = 2, // 副播
    CLIENT_ROLE_AUDIENCE    = 3, // 观众(默认)
};

enum TTT_USER_OFFLINE_REASON
{
    USER_OFFLINE_QUIT            = 1, // 用户主动离开
    USER_OFFLINE_DROPPED         = 2, // 因过长时间收不到对方数据包，超时掉线。
    USER_OFFLINE_BECOME_AUDIENCE = 3, // 当用户身份从主播切换为观众时触发
};

/**
 *  视频编码属性
 */
enum TTT_VIDEO_PROFILE
{
                               //    res    fps   kbps
    VIDEO_PROFILE_120P  =  0,  //  176x128   15     65
    VIDEO_PROFILE_180P  = 10,  //  320x192   15    140
    VIDEO_PROFILE_240P  = 20,  //  320x240   15    200
    VIDEO_PROFILE_360P  = 30,  //  640x352   15    400
    VIDEO_PROFILE_480P  = 40,  //  848x480   15    600
    VIDEO_PROFILE_720P  = 50,  // 1280x720   15   1130
    VIDEO_PROFILE_1080P = 60,  // 1920x1088  15   2080
};

/**
 *  视频帧格式
 */
enum TTT_VIDEO_FRAME_FORMAT
{
    VIDEO_FRAME_FORMAT_TEXTURE = 0,
    VIDEO_FRAME_FORMAT_I420    = 1,
    VIDEO_FRAME_FORMAT_NV12    = 2,
    VIDEO_FRAME_FORMAT_NV21    = 3,
    VIDEO_FRAME_FORMAT_RGBA    = 4,
    VIDEO_FRAME_FORMAT_BGRA    = 5,
    VIDEO_FRAME_FORMAT_ARGB    = 6,
};

enum TTT_AUDIO_ROUTE
{
    AUDIO_ROUTE_HEADSET   = 0, // 耳机或蓝牙
    AUDIO_ROUTE_SPEAKERE  = 1, // 扬声器
    AUDIO_ROUTE_HEADPHONE = 2, // 手机听筒
};

enum TTT_CHAT_TYPE
{
    TEXT    = 1, // 文字
    PICTURE = 2, // 图片
    AUDIO   = 3, // 音频
    CUSTOM  = 4, // 自定义
};

/**
 *  直播推流配置
 */
struct TTT_PublisherConfiguration {
    unsigned int bitrate;   // 旁路直播输出码流的码率。默认设置为 500 Kbps
    std::string publishUrl; // 合图推流地址
    bool isPureAudio;       // 是否推送纯音频流
};

struct TTT_RtcStats
{
    unsigned int duration;        // 通话时长，单位为秒，累计值
    unsigned int txBytes;         // 发送字节数，累计值
    unsigned int rxBytes;         // 接收字节数，累计值
    unsigned int txAudioKBitrate; // 音频发送码率 (kbps)，瞬时值
    unsigned int rxAudioKBitrate; // 音频接收码率 (kbps)，瞬时值
    unsigned int txVideoKBitrate; // 视频发送码率 (kbps)，瞬时值
    unsigned int rxVideoKBitrate; // 视频接收码率 (kbps)，瞬时值
    unsigned int users;           // 房间内的瞬时人数
};

/**
 *  本地视频统计信息
 */
struct TTT_LocalVideoStats
{
    unsigned int encodedBitrate;  // 编码的码率(kbps)
    unsigned int sentBitrate;     // 发送的码率(kbps)
    unsigned int sentFrameRate;   // 发送的帧率(fps)
    unsigned int receivedBitrate; // 接收的码率(kbps)
    float sentLossRate;           // 发送的丢包率
    int bufferDuration;           // 视频缓冲区大小
};

/**
 *  本地音频统计信息
 */
struct TTT_LocalAudioStats
{
    unsigned int encodedBitrate;  // 编码的码率(kbps)
    unsigned int sentBitrate;     // 发送的码率(kbps)
    unsigned int receivedBitrate; // 接收的码率(kbps)
    unsigned int captureDataSize; // push数据量
};

/**
 *  远端视频统计信息
 */
struct TTT_RemoteVideoStats
{
    int64_t uid;
    unsigned int delay;
    unsigned int width;
    unsigned int height;
    unsigned int receivedBitrate;   // 接收的码率
    unsigned int receivedFrameRate; // 接收的帧率
    unsigned int receivedFrames;    // 接收的帧数
    unsigned int lostFrames;        // 丢掉的帧数
};

/**
 *  远端音频统计信息
 */
struct TTT_RemoteAudioStats
{
    int64_t uid;
    unsigned int receivedBitrate; // 接收码率
    unsigned int loseRate;        // 丢包率
    unsigned int bufferDuration;  // 缓存时长
    unsigned int delay;
    unsigned int audioCodec;
};

/**
 *  视频帧
 */
struct TTT_VideoFrame
{
    int64_t uid;
    TTT_VIDEO_FRAME_FORMAT format; // 视频帧的格式
    int width;
    int height;
    //uint8_t *dataBuffer;
    std::string base64EncodedData;
};

struct TTT_ChatInfo
{
    TTT_CHAT_TYPE chatType;     // 聊天类型
    std::string seqID;          // 唯一标识
    std::string chatData;       // 聊天内容
    unsigned int audioDuration; // 音频时长（单位“秒”，chatType为“AUDIO”）
};

class TTTRtcEngineEventHandler;

class TTTRtcEngine
{
private:
    TTTRtcEngine(const char *appID);
    virtual ~TTTRtcEngine();
    
    TTTRtcEngineEventHandler *_eventHandler;
public:
    static TTTRtcEngine* getInstance(const char *appID);
    static void destroy();
    
    /**
     *  获取SDK版本号
     *
     *  @return SDK版本号
     */
    static const char *getVersion();
    
    /**
     *  获取错误描述
     *  @param code 错误代码
     *
     *  @return 错误代码对应的错误描述
     */
    static std::string getErrorDescription(TTT_ERROR_CODE code);
    
    /**
     *  设置回调处理句柄
     */
    virtual void setEventHandler(TTTRtcEngineEventHandler *eventHandler);
    
    /**
     *  设置频道模式
     *
     *  @param profile 频道模式
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setChannelProfile(TTT_CHANNEL_PROFILE profile);
    
    /**
     *  设置用户角色
     *
     *  @param role 用户角色
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setClientRole(TTT_CLIENT_ROLE role);
    
    /**
     *  设置视频编码属性
     *
     *  @param profile            每个Profile对应一套视频参数，如分辨率、帧率、码率等。
     *  @param swapWidthAndHeight 是否交换宽和高
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setVideoProfile(TTT_VIDEO_PROFILE profile, bool swapWidthAndHeight);
    
    /**
     *  设置日志文件过滤器
     *
     *  @param filter 日志过滤器
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setLogFilter(TTT_LOG_FILTER filter);
    
    /**
     *  设置SDK输出的日志文件 应用程序必须保证指定的目录存在而且可写
     *
     *  @param filePath 日志文件的完整路径。该日志文件为UTF-8编码。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setLogFile(const char *filePath);
    
    /**
     *  该方法用于开启视频模式。
     *  可以在加入频道前或者通话中调用，在加入频道前调用，则自动开启视频模式，在通话中调用则由音频模式切换为视频模式。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int enableVideo();
    
    /**
     *  该方法用于关闭视频，开启纯音频模式。
     *  可以在加入频道前或者通话中调用，在加入频道前调用，则自动开启纯音频模式，在通话中调用则由视频模式切换为纯音频频模式。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int disableVideo();
    
    /**
     *  启动本地视频预览
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int startPreview();
    
    /**
     *  停止本地视频预览
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int stopPreview();
    
    /**
     *  设置音频高音质选项
     *
     *  @param fullband    全频带编解码器（48kHz采样率）
     *  @param stereo      立体声编解码器
     *  @param fullBitrate 高码率模式，建议仅在纯音频模式下使用。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setHighQualityAudioParameters(bool fullband, bool stereo, bool fullBitrate);
    
    /**
     *  配置旁路直播推流
     *
     *  @param config 直播推流配置
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int configPublisher(TTT_PublisherConfiguration config);
    
    /**
     *  加入频道，在同一个频道内的用户可以互相通话。
     *
     *  @param channelName 标识通话的频道名称
     *  @param uid  用户 ID
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int joinChannel(const char *channelKey, const char *channelName, int64_t uid);
    
    /**
     *  离开频道
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int leaveChannel();
    
    /**
     *  允许/禁止往网络发送本地音频流
     *
     *  @param mute true: 麦克风静音，false: 取消静音。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int muteLocalAudioStream(bool mute);
    
    /**
     *  允许/禁止播放远端用户的音频流，即对所有远端用户进行静音与否。
     *
     *  @param mute true: 停止播放所有收到的音频流，false: 允许播放所有收到的音频流。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int muteAllRemoteAudioStreams(bool mute);
    
    /**
     *  允许/禁止播放远端用户的音频流
     *
     *  @param uid 用户 ID
     *  @param mute true: 停止播放指定用户的音频流，false: 允许播放指定用户的音频流。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int muteRemoteAudioStream(int64_t uid, bool mute);
    
    /**
     *  暂停所有远端视频流
     *
     *  @param mute YES: 停止播放接收到的所有视频流，NO: 允许播放接收到的所有视频流。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int muteAllRemoteVideoStreams(bool mute);
    
    /**
     *  允许/禁止播放指定的远端视频流
     *
     *  @param uid  用户ID
     *  @param mute YES: 停止播放接收到的视频流，NO: 允许播放接收到的视频流。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int muteRemoteVideoStream(int64_t uid, bool mute);
    
    /**
     *  禁言指定远端用户/对指定远端用户取消禁言。
     *
     *  @param mute YES: 禁止发言, NO: 允许发言。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int muteRemoteSpeaking(int64_t uid, bool mute);
    
#if TARGET_OS_IOS
    /**
     *  打开外放(扬声器)
     *
     *  @param speakerphone true: 切换到从外放(扬声器)出声，false: 语音会根据默认路由出声。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setEnableSpeakerphone(bool speakerphone);
    
    /**
     *  修改默认的语音路由
     *
     *  @param speakerphone true: 默认路由改为外放(扬声器)，false: 默认路由改为听筒。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int setDefaultAudioRouteToSpeakerphone(bool speakerphone);
#endif
    
    /**
     *  启用/禁用说话者音量提示
     *
     *  @param interval 指定音量提示的时间间隔（<=0: 禁用音量提示功能；>0: 提示间隔，单位为毫秒。建议设置到大于200毫秒。）
     *  @param smooth   平滑系数。默认可以设置为3。
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int enableAudioVolumeIndication(int interval, int smooth);
    
    /**
     *  踢出房间
     *  角色为“CLIENT_ROLE_ANCHOR”调用有效
     *
     *  @param uid 被踢者userID
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int kickChannelUser(int64_t uid);
    
    /**
     *  开始客户端本地混音
     *
     *  @param filePath 指定需要混音的本地音频文件名和文件路径
     *  @param loopback true: 只有本地可以听到混音或替换后的音频流，false: 本地和对方都可以听到混音或替换后的音频流。
     *  @param replace  true: 音频文件内容将会替换本地录音的音频流，false: 音频文件内容将会和麦克风采集的音频流进行混音。
     *  @param cycle    指定音频文件循环播放的次数
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int startAudioMixing(const char *filePath, bool loopback, bool replace, int cycle);
    
    /**
     *  停止客户端本地混音
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int stopAudioMixing();
    
    /**
     *  暂停播放伴奏
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int pauseAudioMixing();
    
    /**
     *  恢复播放伴奏
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int resumeAudioMixing();
    
    /**
     * adjust mixing audio file volume
     *
     * @param volume range from 0 to 100
     * @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int adjustAudioMixingVolume(int volume);
    
    /**
     * get the duration of the specified mixing audio file
     *
     * @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int getAudioMixingDuration();
    
    /**
     * get the current playing position of the specified mixing audio file
     *
     * @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int getAudioMixingCurrentPosition();
    
    /**
     *  发送聊天消息
     *
     *  @param userID   用户ID
     *  @param chatType 聊天消息类型
     *  @param seqID    唯一标识
     *  @param data     消息内容
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int sendChatMessage(int64_t userID, int chatType, const char *seqID, const char *data);
    
#if TARGET_OS_IOS
    /**
     *  开始采集语音消息
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int startRecordChatAudio();
    
    /**
     *  停止采集并开始发送消息
     *
     *  @param userID 目标用户ID，0表示发送给房间内的所有人。
     *  @param seqID  消息唯一标识
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int stopRecordAndSendChatAudio(int64_t userID, const char *seqID);
    
    /**
     *  取消语音消息录制
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int cancelRecordChatAudio();
    
    /**
     *  开始播放语音消息
     *
     *  @param fileName 语音消息文件名
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int startPlayChatAudio(const char *fileName);
    
    /**
     *  停止播放语音消息
     *
     *  @return 0: 方法调用成功，<0: 方法调用失败。
     */
    virtual int stopPlayChatAudio();
    
    /**
     *  是否正在播放语音消息
     *
     *  @return YES: 正在播放，NO: 不在播放。
     */
    virtual bool isChatAudioPlaying();
#endif
    
    /**
     * trigger the SDK event working according to vertical synchronization such as onUpdate(Cocos2d)
     *
     */
    virtual void poll();
private:
    static std::vector<std::string> splitMsg(const std::string &msg, char delim);
};

/**
 *  事件回调接口
 */
class TTTRtcEngineEventHandler
{
public:
    virtual ~TTTRtcEngineEventHandler() {}
    
    /**
     *  发生错误
     *
     *  @param code    错误代码
     *  @param message 错误描述
     */
    virtual void onError(TTT_ERROR_CODE code, const char *message) {}
    
    /**
     *  网络连接丢失
     */
    virtual void onConnectionLost() {}
    
    /**
     *  加入频道成功
     *
     *  @param channel 频道名
     *  @param uid     用户ID
     *  @param elapsed 从joinChannel开始到该事件产生的延迟（毫秒）
     */
    virtual void onJoinChannelSuccess(const char *channel, int64_t uid, int elapsed) {}
    
    /**
     *  成功离开频道
     *
     *  @param stats 统计数据
     */
    virtual void onLeaveChannel(const TTT_RtcStats &stats) {}
    
    /**
     *  用户加入
     *
     *  @param uid     用户ID
     *  @param elapsed 加入频道开始到该回调触发的延迟（毫秒)
     */
    virtual void onUserJoined(int64_t uid, int elapsed) {}
    
    /**
     *  用户离线
     *
     *  @param uid    用户ID
     *  @param reason 离线原因
     */
    virtual void onUserOffline(int64_t uid, TTT_USER_OFFLINE_REASON reason) {}
    
    /**
     *  上报远端用户音量
     *  提示谁在说话及其音量，默认禁用。可通过enableAudioVolumeIndication方法设置。
     *
     *  @param userID              用户ID
     *  @param audioLevel          非线性区间[0,9]
     *  @param audioLevelFullRange 线性区间[0,32768]
     */
    virtual void onReportAudioVolumeIndication(int64_t userID, unsigned int audioLevel, unsigned int audioLevelFullRange) {}
    
    /**
     *  上报统计数据
     *
     *  @param stats 统计数据
     */
    virtual void onReportRtcStats(const TTT_RtcStats &stats) {}
    
    /**
     *  本地视频统计回调
     *
     *  @param stats 本地视频的统计信息
     */
    virtual void onLocalVideoStats(const TTT_LocalVideoStats &stats) {}
    
    /**
     *  本地音频统计回调
     *
     *  @param stats 本地音频的统计信息
     */
    virtual void onLocalAudioStats(const TTT_LocalAudioStats &stats) {}
    
    /**
     *  远端视频统计回调
     *
     *  @param stats 远端视频的统计信息
     */
    virtual void onRemoteVideoStats(const TTT_RemoteVideoStats &stats) {}
    
    /**
     *  远端音频统计回调
     *
     *  @param stats 远端音频的统计信息
     */
    virtual void onRemoteAudioStats(const TTT_RemoteAudioStats &stats) {}

    /**
     *  用户启用/关闭视频回调
     *
     *  @param enabled YES: 该用户已启用了视频功能，NO: 该用户已关闭了视频功能。
     *  @param uid     用户ID
     */
    virtual void onVideoEnabled(int64_t uid, bool enabled) {}
    
    /**
     *  用户音频静音
     *
     *  @param uid   用户ID
     *  @param muted true: 静音，false: 取消静音。
     */
    virtual void onAudioMuted(int64_t uid, bool muted) {}
    
#if TARGET_OS_IOS
    /**
     *  音频输出路由发生变化
     *
     *  @param routing 当前音频输出路由
     */
    virtual void onAudioRouteChanged(AUDIO_ROUTE routing) {}
#endif
    
    /**
     *  客户端本地混音完成
     */
    virtual void onAudioMixingFinished() {}
    
    /**
     *  发送聊天消息的回调
     *
     *  @param chatInfo  聊天信息
     *  @param errorCode 错误代码
     */
    virtual void onChatMessageSent(const TTT_ChatInfo &chatInfo, TTT_ERROR_CODE errorCode) {}
    
    /**
     *  收到聊天消息的回调
     *
     *  @param userID   用户ID
     *  @param chatInfo 聊天信息
     */
    virtual void onChatMessageReceived(int64_t userID, const TTT_ChatInfo &chatInfo) {}
    
    /**
     *  语音消息播放完成的回调
     *
     *  @param fileName 语音消息文件名
     */
    virtual void onChatAudioFinishPlaying(const char *fileName) {}
    
    /**
     *  本地首帧视频显示回调
     *
     *  @param width   视频流宽度
     *  @param height  视频流高度
     *  @param elapsed 加入频道开始到该回调触发的延迟（毫秒)
     */
    virtual void onFirstLocalVideoFrame(int width, int height, int elapsed) {}
    
    /**
     *  远端首帧视频接收解码回调
     *
     *  @param uid     用户ID，指定是哪个用户的视频流
     *  @param width   视频流宽度
     *  @param height  视频流高度
     *  @param elapsed 加入频道开始到该回调触发的延迟（毫秒)
     */
    virtual void onFirstRemoteVideoFrameDecoded(int64_t uid, int width, int height, int elapsed) {}
    
    /**
     *  已显示远端视频首帧回调
     *
     *  @param uid     用户ID，指定是哪个用户的视频流
     *  @param width   视频流宽度
     *  @param height  视频流高度
     *  @param elapsed 加入频道开始到该回调触发的延迟（毫秒)
     */
    virtual void onFirstRemoteVideoFrame(int64_t uid, int width, int height, int elapsed) {}
    
    /**
     *  本地视频采集回调
     *
     *  @param videoFrame 视频帧
     */
    virtual void onLocalVideoFrameCaptured(const TTT_VideoFrame &videoFrame) {}

    /**
     *  远端视频接收解码回调
     *
     *  @param uid        用户ID，指定是哪个用户的视频流
     *  @param videoFrame 视频帧
     */
    virtual void onRemoteVideoFrameDecoded(int64_t uid, const TTT_VideoFrame &videoFrame) {}
    
    /**
     *  摄像头启用回调
     */
    virtual void onCameraReady() {}
    
    /**
     *  视频功能停止回调
     */
    virtual void onVideoStopped() {}
};

#endif
