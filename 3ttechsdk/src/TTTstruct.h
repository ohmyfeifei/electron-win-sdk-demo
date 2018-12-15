#ifndef __TTTSTRUCT_H__
#define __TTTSTRUCT_H__
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
typedef long long uid_ttt;
typedef void* view_ttt;

typedef enum VideoFrameType_TTT
{
	FRAMETYPE_INVALID_TTT,
	FRAMETYPE_SPS_PPS_TTT,
	FRAMETYPE_I_TTT,
	FRAMETYPE_P_TTT
} VideoFrameType_TTT;

enum CLIENT_ROLE_TYPE_TTT
{
    CLIENT_ROLE_BROADCASTER_TTT = 1,
    CLIENT_ROLE_ANCHOR_TTT = 2,
	/**
	* 观众角色(默认)
	*/
	CLIENT_ROLE_AUDIENCE_TTT = 3,
};

enum USER_OFFLINE_REASON_TYPE_TTT
{
    USER_OFFLINE_QUIT_TTT = 0,
    USER_OFFLINE_DROPPED_TTT = 1,
    USER_OFFLINE_BECOME_ANCHOR_TTT = 2,
};



struct RtcStats_TTT
{
	/**
	* 通话时长(秒)，累计值
	*/
	unsigned int duration;
	/**
	* 发送字节数(bytes), 累计值
	*/
	unsigned int txBytes;
	/**
	* 接收字节数(bytes)，累计值
	*/
	unsigned int rxBytes;

	/**
	* 接受音频码率(kbps)，瞬时值
	*/
	unsigned short rxAudioKBitRate;
	/**
	* 发送音频码率(kbps)，瞬时值
	*/
	unsigned short txAudioKBitRate;
	/**
	* 接受音频码率(kbps)，瞬时值
	*/
	unsigned short rxVideoKBitRate;
	/**
	* 发送视频码率(kbps)，瞬时值
	*/
	unsigned short txVideoKBitRate;
	unsigned int users;
	double cpuAppUsage;
	double cpuTotalUsage;
};


struct LocalVideoStats_TTT
{
	/**
	* （上次统计后）发送的码率(kbps)
	*/
	int sentBitrate;
	/**
	* (上次统计后）发送的帧率(fps)
	*/
	int sentFrameRate;
};

enum CHANNEL_PROFILE_TYPE_TTT
{
	CHANNEL_PROFILE_COMMUNICATION_TTT = 0,
	CHANNEL_PROFILE_LIVE_BROADCASTING_TTT = 1,
    CHANNEL_PROFILE_GAME_TTT = 2,
};
typedef enum RoomMode_TTT
{
	ROOM_MODE_COMMUNICATION_TTT,    // 互动模式
	ROOM_MODE_LIVE_TTT,             // 直播模式
	ROOM_MODE_GAME_FREE_TTT,        // 游戏自由模式
	ROOM_MODE_GAME_COMMAND_TTT,     // 游戏指挥模式
	ROOM_MODE_CONFERENCE_TTT,       // 会议模式
	ROOM_MODE_MANUAL_TTT,           // 手动模式
	ROOM_MODE_UNFINE_TTT,           // 未定义模式

}RoomMode_TTT;

struct RemoteVideoStats_TTT
{
	uid_ttt uid;
    char* devid;
	int delay;
	int width;
	int height;
	int receivedBitrate;
	int receivedFrameRate;
};
enum RENDER_MODE_TYPE_TTT
{
	RENDER_MODE_HIDDEN_TTT = 1,
	RENDER_MODE_FIT_TTT = 2,
	RENDER_MODE_ADAPTIVE_TTT = 3,
};


enum RTMP_STREAM_LIFE_CYCLE_TYPE_TTT
{
	RTMP_STREAM_LIFE_CYCLE_BIND2CHANNEL_TTT = 1,
	RTMP_STREAM_LIFE_CYCLE_BIND2OWNER_TTT = 2,
};

/**
* 配置旁路直播推流的类
*/
struct PublisherConfiguration_TTT {
	/**
	* Description: 设置推流的地址.
	*
	* @param url 推流的地址
	* @return the builder
	*/
	char publishUrl[256] = {0};
};

struct VideoCanvas_TTT
{
	/**
	* 视频显示视窗
	*/
	view_ttt view;
	/**
	* 设置视频显示的模式
	*/
	int renderMode;
	/**
	* 该视频属性类所属用户ID
	*/
	uid_ttt uid;
	const char* devId = NULL;
	void *priv; // private data (underlying video engine denotes it)

	/**
	* 视频属性类的构造函数
	*
	* @param uid   该视频属性类所属用户ID
	* @param renderMode 设置视频显示的模式 
	* @param view  视频显示视窗
	*/
	VideoCanvas_TTT()
		: view(NULL)
		, renderMode(RENDER_MODE_HIDDEN_TTT)
		, uid(0)
		, priv(NULL)
	{}
	VideoCanvas_TTT(view_ttt v, int m, uid_ttt u)
		: view(v)
		, renderMode(m)
		, uid(u)
		, priv(NULL)
	{}
};



struct ScreenRecordConfig_TTT
{
	int mRecordWidth;
	int mRecordHeight;
	int mRecordBitRate;
	int mRecordFrameRate;
	int mRecordAudioBitRate;
	int mTop;
	int mLeft;

	//ScreenRecordConfig_TTT(int RecordWidth, int RecordHeight, int RecordBitRate, int RecordFrameRate, int RecordAudioBitRate) {
	//	mRecordWidth = RecordWidth;
	//	mRecordHeight = RecordHeight;
	//	mRecordBitRate = RecordBitRate;
	//	mRecordFrameRate = RecordFrameRate;
	//	mRecordAudioBitRate = RecordAudioBitRate;
	//}
};
/**
* 画中画布局
*/
struct Region_TTT {
	/**
	* 小视频的用户ID
	*/
	uid_ttt uid;
	const char* devid = NULL;// [128];
	/**
	* 小视频X坐标
	*/
	double x;//[0,1]
			 /**
			 * 小视频Y坐标
			 */
	double y;//[0,1]
			 /**
			 * 小视频宽度
			 */
	double width;//[0,1]
				 /**
				 * 小视频高度
				 */
	double height;//[0,1]
				  /**
				  * 小视频层级关系
				  */
	int zOrder;

	double alpha;
	int isRegin = 54321;


};
struct VideoCompositingLayout_TTT
{

	int canvasWidth;
	int canvasHeight;
	/**
	* 视频背景颜色，RGB值，[0-255 , 0-255 , 0-255]
	*/
	const char* backgroundColor;//e.g. "#C0C0C0" in RGB
	/**
	* 小视频窗口数据集合
	*/
	Region_TTT* regions;
	int regionCount;
	const char* appData;
	int appDataLength;

};
typedef struct Rect_TTT {
	int top;
	int left;
	int bottom;
	int right;
} Rect_TTT;

enum VIDEO_PROFILE_TYPE
{
	VIDEO_PROFILE_120P = 110,
	/**
	* 320x180,15fps,140kbps
	*/
	VIDEO_PROFILE_180P = 111,
/**
* 320x240,15fps,200kbps
*/
VIDEO_PROFILE_240P = 112,
/**
* 640x360,15fps,400kbps
*/
VIDEO_PROFILE_360P = 113,
/**
* 640x480,15fps,500kbps
*/
VIDEO_PROFILE_480P = 114,
/**
* 1280x720,15fps,1130kbps
*/
VIDEO_PROFILE_720P = 115,
/**
* 1920x1080,15fps,2080kbps
*/
VIDEO_PROFILE_1080P = 116,

};
enum VIDEO_FRAME_TYPE_TTT {
	FRAME_TYPE_YUV420 = 0,  //YUV 420 格式
};
struct VideoFrame_TTT {
	VIDEO_FRAME_TYPE_TTT type;
	int width;  //视频帧宽
	int height;  //视频帧的高度
	void* buf; 
};
enum AUDIO_FRAME_TYPE_TTT {
	FRAME_TYPE_PCM16 = 0,  //PCM 16bit little endian
};
struct AudioFrame_TTT {
	AUDIO_FRAME_TYPE_TTT type;
	int bytesPerSample;  //number of bytes per sample: 2 for PCM16
	int channels;  //number of channels (data are interleaved if stereo)
	int samplesPerSec;  //sampling rate
	void* buffer;  //data buffer
};

#endif
