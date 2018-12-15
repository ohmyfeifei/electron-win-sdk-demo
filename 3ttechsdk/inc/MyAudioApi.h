#ifndef __MY_AUDIO_API_H__
#define __MY_AUDIO_API_H__

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

	int SetSpeakerVolume(unsigned int volume);
	int SetMicVolume(unsigned int volume);
	int GetNumOfPlayoutDevices(int* devices);
	int GetNumOfRecordingDevices(int* devices);
	int getRecordingDefaultDevice(char deviceId[128]);
	int getPlaybackDefaultDevice(char deviceId[128]);
	int getRecordingDevice(int index, char deviceName[128], char deviceId[128]);
	int getPlaybackDevice(int index, char deviceName[128], char deviceId[128]);
	int setRecordingDevice(const char deviceId[128]);
	int setPlaybackDevice(const char deviceId[128]);
#ifdef __cplusplus
};
#endif

#endif
