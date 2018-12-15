/**
 * @file MyVideo.h
*/
#ifndef __MYVIDEOAPILIB_H__
#define __MYVIDEOAPILIB_H__

/**
 视频设备类型
 */
typedef enum VIDEODEV_TYPE
{
	VIDEODEV_TYPE_VIDEO         = 1, //摄像头
	VIDEODEV_TYPE_CAMERA        = 2, //云台
	VIDEODEV_TYPE_FILE          = 3, //文件源
	VIDEODEV_TYPE_VIDEOMIXER    = 4, //混屏
	VIDEODEV_TYPE_SCREENCAPTURE = 9  //屏幕采集
} VIDEODEV_TYPE;

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

/**
* @brief 获取本地可用的视频设备个数
*
* @return int,本地可用视频设备的个数，0表示没有枚举到可用的本地视频设备
*/
int GS_GetNumOfVideoDevices(void) ;

/**
* @brief 获取可用视频设备的名称
*
* @param [in]index 视频设备索引 0- GS_GetNumOfVideoDevices()-1
* @param [out] nameWchar 视频设备名称
* @param [out] nameLen 视频设备名称长度
* @return int 0表示成功，否则错误码
*/
int GS_GetVideoDeviceName(unsigned int index, wchar_t* nameWchar, VIDEODEV_TYPE* deviceType);

/**
 * @brief	设置本地采集参数
 *
 * @param	[in]nameWcar  	视频设备名称
 * @param	[in]iWidth	    采集视频宽度
 * @param	[in]iHeight     采集视频高度
 * @param   [in]iFramerate  采集视频帧率
 * @param   [in]iBitrate    采集视频码率
 * @param   [in]iBitrate    采集视频码率
 *
 * @return int 0表示成功，否则错误码
 */

int GS_SetDeviceCaptureParams(const wchar_t* nameWchar,unsigned int iWidth,unsigned int iHeight,unsigned int iFramerate,unsigned int iBitrate);

/**
 * @brief	设置默认的视频设备名称
 *
 * @param	[in]nameWchar  	视频设备名称 抓屏的名称为:"dd919183-b1bb-4107-b216-968f0ebe6d4a-ScreenCapture"
 *
 * @return int 0表示成功，否则错误码
 */
int GS_SetDefaultDevice(const wchar_t* nameWchar);


#ifdef __cplusplus
};
#endif

#endif
 