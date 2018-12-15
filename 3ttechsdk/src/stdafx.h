
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif
#include <queue>
enum Cmd
{
	cmd_push_queue
};
class TaskQueue {
public:
	TaskQueue() {
		cmd = 0;
		length = 1;
		data = NULL;
	}
	~TaskQueue() {
		cmd = 0;
		length = 1;
		if (data != NULL)
		{
			delete data;
			data = NULL;
		}
	}
	int  cmd;
	long long id;
	int  length;
	char* devId;
	std::string json;
	int width;
	int height;
	int VideoFrameType;
	uint8_t *data;
};



