#include "stdafx.h"

#include <node.h>
#include <nan.h>
#include <napi.h>

#include <chrono>
#include <random>
#include <thread>
#include <iostream>
#include "streaming-worker.h"
#include <windows.h>
#include <regex>

#include "TTTRtcEngine.h"
#include "TTTEngineEventHandler.h"
#include "..\inc\MyAudioApi.h"
#include "..\inc\MyVideoApi.h"

#include ".\JSON\json\json.h"

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

static	TTTRtcEngine		*m_lpTTTEngine_TTT;
CTTTEngineEventHandler       m_EngineEventHandler;

std::queue<TaskQueue*> task_queue;
CRITICAL_SECTION m_queue_cs;

class Sensor : public StreamingWorker {
  public:
    Sensor(Callback *data, Callback *complete, Callback *error_callback,  v8::Local<v8::Object> & options) 
          : StreamingWorker(data, complete, error_callback){

      name = "default sensor";
      if (options->IsObject() ) {
        v8::Local<v8::Value> name_ = options->Get(New<v8::String>("name").ToLocalChecked());
        if ( name_->IsString() ) {
          v8::String::Utf8Value s(name_);
          name = *s;
        }
      }
    }
     
    void send_sample(const AsyncProgressWorker::ExecutionProgress& progress, double x, double y, double z) {

        Message tosend("position_sample", name);
        writeToNode(progress, tosend);
    }

    void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
      std::random_device rd;
      std::uniform_real_distribution<double> pos_dist(-1.0, 1.0);

		while (true) {
			TaskQueue *pTask = NULL;
			EnterCriticalSection(&m_queue_cs);
			pTask = NULL;
			if (task_queue.size() > 0) {
				pTask = task_queue.front();
				task_queue.pop();

			}
			LeaveCriticalSection(&m_queue_cs);
			if (pTask != NULL) {
				switch (pTask->cmd)
				{
				case cmd_push_queue:
					std::string str7 = pTask->json;

					Message tosend("sum", (str7));
					writeToNode(progress, tosend);
					break;
				}
				delete pTask;
			}
			Sleep(10);
		}
		
      while (!closed()) {
        send_sample(progress, pos_dist(rd), pos_dist(rd), pos_dist(rd));
        std::this_thread::sleep_for(chrono::milliseconds(50));
      }
    }
  private:
    string name;
};


StreamingWorker * create_worker(Callback *data
    , Callback *complete
    , Callback *error_callback, v8::Local<v8::Object> & options) {
 return new Sensor(data, complete, error_callback, options);
}


namespace ws_techapi
{
	int gettimeofday(struct timeval *tp, void *tzp)
	{
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear - 1900;
		tm.tm_mon = wtm.wMonth - 1;
		tm.tm_mday = wtm.wDay;
		tm.tm_hour = wtm.wHour;
		tm.tm_min = wtm.wMinute;
		tm.tm_sec = wtm.wSecond;
		tm.tm_isdst = -1;
		clock = mktime(&tm);
		tp->tv_sec = clock;
		tp->tv_usec = wtm.wMilliseconds * 1000;

		return 0;
	}
	// internal functions
	std::string ws2s(const std::wstring& ws)
	{
		char* ps = NULL;
		std::string strRet;

		int bufSize = WideCharToMultiByte(CP_ACP,
			0,
			ws.c_str(),
			(int)ws.length(),
			ps,
			0,
			NULL,
			NULL);

		ps = new char[bufSize + 1];
		memset(ps, 0, bufSize + 1);

		WideCharToMultiByte(CP_ACP,
			0,
			ws.c_str(),
			(int)ws.length(),
			ps,
			bufSize,
			NULL,
			NULL);

		strRet = ps;
		delete[] ps;

		return strRet;
	}
	std::wstring s2wss(const std::string& s, wchar_t* pws)
	{
		//wchar_t* 
		pws = NULL;
		std::wstring wstrRet;

		int bufSize = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			s.c_str(),
			(int)s.length(),
			pws,
			0);

		pws = new wchar_t[bufSize + 1];
		memset(pws, 0, (bufSize + 1) * sizeof(wchar_t));

		MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			s.c_str(),
			(int)s.length(),
			pws,
			bufSize);
		return L"";
	}
	std::wstring s2ws(const std::string& s)
	{
		wchar_t* pws = NULL;
		std::wstring wstrRet;

		int bufSize = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			s.c_str(),
			(int)s.length(),
			pws,
			0);

		pws = new wchar_t[bufSize + 1];
		memset(pws, 0, (bufSize + 1) * sizeof(wchar_t));

		MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			s.c_str(),
			(int)s.length(),
			pws,
			bufSize);

		wstrRet = pws;
		delete[] pws;

		return wstrRet;
	}
	std::string ws2utf8(const std::wstring &ws)
	{
		char* ps = NULL;
		std::string strRet;

		int bufSize = WideCharToMultiByte(CP_UTF8,
			0,
			ws.c_str(),
			(int)ws.length(),
			ps,
			0,
			NULL,
			NULL);

		ps = new char[bufSize + 1];
		memset(ps, 0, bufSize + 1);

		WideCharToMultiByte(CP_UTF8,
			0,
			ws.c_str(),
			(int)ws.length(),
			ps,
			bufSize,
			NULL,
			NULL);

		strRet = ps;
		delete[] ps;

		return strRet;
	}

	std::wstring utf82ws(const std::string &utf8)
	{
		wchar_t* pws = NULL;
		std::wstring wstrRet;

		int bufSize = MultiByteToWideChar(CP_UTF8,
			0,
			utf8.c_str(),
			(int)utf8.length(),
			pws,
			0);

		pws = new wchar_t[bufSize + 1];
		memset(pws, 0, (bufSize + 1) * sizeof(wchar_t));

		MultiByteToWideChar(CP_UTF8,
			0,
			utf8.c_str(),
			(int)utf8.length(),
			pws,
			bufSize);

		wstrRet = pws;
		delete[] pws;

		return wstrRet;
	}
	std::string s2utf8(const std::string &s)
	{
		std::wstring ws = s2ws(s);
		return ws2utf8(ws);
	}

	std::string utf82s(const std::string &utf8)
	{
		std::wstring ws = utf82ws(utf8);
		return ws2s(ws);
	}

	std::string llToString(long long llvalue)
	{
		std::stringstream str;
		str << llvalue;
		return str.str();
	}

	std::string doubleToString(double d)
	{
		char tmp[256] = { 0 };
		_snprintf_s(tmp, sizeof(tmp), "%.6lf", d);
		return std::string(tmp);
	}
}

//  info[0]   Venderf appID
void initialize(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string sname(*utf8_value, len);

	bool ret = false; 

	if (m_lpTTTEngine_TTT == NULL)
		m_lpTTTEngine_TTT = TTTRtcEngine::createInstance();
	if (m_lpTTTEngine_TTT == NULL) {
		args.GetReturnValue().Set(Nan::New(false));
		return;
	}

	RtcEngineContext_TTT ctx;
	LPCTSTR lpVendorKey = sname.c_str();
	ctx.appId = lpVendorKey;
	ctx.eventHandler = (&m_EngineEventHandler);

	m_lpTTTEngine_TTT->initialize(ctx);

	args.GetReturnValue().Set(Nan::New(true));
}

void getVersion(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();
	std::string ver = "1.0.1";
	args.GetReturnValue().Set(Nan::New(ver.data()).ToLocalChecked());
}

// info[0]: channelkey
// info[1]: channelname
// info[2]: userID
void joinChannel(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();
	
	// Check the number of arguments passed.
	if (args.Length() < 3) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsString()||!args[2]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}


	//char channelkey[256];
	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string channelkey(*utf8_value, len);
	
	Nan::Utf8String value2(args[1]);
	len = value2.length();
	std::string channelname(*value2, len);

	int64_t userID = args[2]->IntegerValue();

	char key[256];
	sprintf(key, "%s", channelkey.data());
	char name[256];
	sprintf(name, "%s", channelname.data());

	int ret = -2;
	if ( m_lpTTTEngine_TTT != NULL ) 
		ret = m_lpTTTEngine_TTT->joinChannel(key, name, userID);

	args.GetReturnValue().Set(Nan::New(ret));
}

void destroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->destroy();
	args.GetReturnValue().Set(Nan::New(true));
}

void leaveChannel(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->leaveChannel();
	args.GetReturnValue().Set(Nan::New(true));
}

void setChannelProfile(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int32_t profile = args[0]->NumberValue();
	RoomMode_TTT mode = (RoomMode_TTT)profile;
	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->setChannelProfile(mode);
	args.GetReturnValue().Set(Nan::New(ret));
}

void setClientRole(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int32_t role = args[0]->NumberValue();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->setClientRole(role, NULL);
	args.GetReturnValue().Set(Nan::New(ret));
}

void startPreview(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->startPreview();
	args.GetReturnValue().Set(Nan::New(true));
}

void stopPreview(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->stopPreview();

	args.GetReturnValue().Set(Nan::New(true));
}

void setVideoProfile(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 2) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int profile = args[0]->NumberValue();
	bool swapWidthAndHeight = args[1]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->setVideoProfile(profile, swapWidthAndHeight);

	args.GetReturnValue().Set(Nan::New(true));
}

void setupRemoteVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 3) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()||!args[1]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int renderMode = args[0]->NumberValue();
	int64_t uid = args[1]->IntegerValue();

	Nan::Utf8String utf8_value(args[2]);
	int len = utf8_value.length();
	std::string devID(*utf8_value, len);
	//info[3]and [4] for view handle, and priv data

	VideoCanvas_TTT canvas;
	canvas.renderMode = renderMode;
	canvas.uid = uid;
	canvas.devId = devID.data();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->setupRemoteVideo(canvas);

	args.GetReturnValue().Set(Nan::New(ret));
}

void setupLocalVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 2) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	int renderMode = args[0]->NumberValue();
	int64_t uid = args[1]->IntegerValue();

	VideoCanvas_TTT canvas;
	canvas.renderMode = renderMode;
	canvas.uid = uid;
	canvas.devId = NULL;

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->setupLocalVideo(canvas);

	args.GetReturnValue().Set(Nan::New(ret));
}

void enableLastmileTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->enableLastmileTest();
	args.GetReturnValue().Set(Nan::New(true));
}

void disableLastmileTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->disableLastmileTest();
	args.GetReturnValue().Set(Nan::New(true));
}

void enableVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->enableVideo();

	args.GetReturnValue().Set(Nan::New(ret));
}

void disableVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->disableVideo();

	args.GetReturnValue().Set(Nan::New(ret));
}

void disableAudio(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->disableAudio();

	args.GetReturnValue().Set(Nan::New(ret));
}

void enableAudio(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->enableAudio();

	args.GetReturnValue().Set(Nan::New(ret));
}

void startScreenCapture(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 4) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	int left = args[0]->NumberValue();
	int top = args[1]->NumberValue();
	int right = args[2]->NumberValue();
	int bottom = args[3]->NumberValue();

	Rect_TTT rect;
	rect.left = left;
	rect.top = top;
	rect.right = right;
	rect.bottom = bottom;

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->startScreenCapture(&rect);

	args.GetReturnValue().Set(Nan::New(true));
}

void stopScreenCapture(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->stopScreenCapture();

	args.GetReturnValue().Set(Nan::New(ret));
}

void setVideoCompositingLayout(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int ret = -1;
	VideoCompositingLayout_TTT sei;
	//TODO 参数怎么组织

	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->setVideoCompositingLayout(sei);

	args.GetReturnValue().Set(Nan::New(ret));
}

void configPublisher(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	int ret = -1;
	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string url(*utf8_value, len);

	PublisherConfiguration_TTT profile;

	memcpy(profile.publishUrl, url.data(), url.length());

	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->configPublisher(profile);
	args.GetReturnValue().Set(Nan::New(ret));
}

void muteLocalAudioStream(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsBoolean()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	bool mute = args[0]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->muteLocalAudioStream(mute);

	args.GetReturnValue().Set(Nan::New(true));
}

void muteAllRemoteAudioStreams(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsBoolean()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	bool mute = args[0]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->muteAllRemoteAudioStreams(mute);

	args.GetReturnValue().Set(Nan::New(true));
}

void muteRemoteAudioStream(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 2) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()|| !args[1]->IsBoolean()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	uid_ttt uid = args[0]->NumberValue();
	bool mute = args[1]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->muteRemoteAudioStream(uid, mute);

	args.GetReturnValue().Set(Nan::New(true));
}

void muteLocalVideoStream(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsBoolean()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	bool mute = args[0]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->muteLocalVideoStream(mute);

	args.GetReturnValue().Set(Nan::New(true));
}

void muteAllRemoteVideoStreams(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsBoolean()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	bool mute = args[0]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->muteAllRemoteVideoStreams(mute);

	args.GetReturnValue().Set(Nan::New(true));
}

void enableAudioVolumeIndication(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 2) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()|| !args[1]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int interval = args[0]->NumberValue();
	int smooth = args[1]->NumberValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->enableAudioVolumeIndication(interval, smooth);

	args.GetReturnValue().Set(Nan::New(true));
}

void startAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 4) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[3]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string filepath(*utf8_value, len);
	bool loopback = args[1]->BooleanValue();
	bool replace = args[2]->BooleanValue();
	int  cycle = args[3]->NumberValue();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->startAudioMixing(filepath.data(), loopback, replace, cycle);

	args.GetReturnValue().Set(Nan::New(ret));
}

void stopAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->stopAudioMixing();

	args.GetReturnValue().Set(Nan::New(true));
}

void pauseAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->pauseAudioMixing();

	args.GetReturnValue().Set(Nan::New(true));
}

void resumeAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->resumeAudioMixing();

	args.GetReturnValue().Set(Nan::New(true));
}

void adjustAudioMixingVolume(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	int volume = args[0]->NumberValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->adjustAudioMixingVolume(volume);

	args.GetReturnValue().Set(Nan::New(true));
}

void getAudioMixingDuration(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int duration = 0;
	if (m_lpTTTEngine_TTT != NULL)
		duration = m_lpTTTEngine_TTT->getAudioMixingDuration();

	args.GetReturnValue().Set(Nan::New(duration));
}

void getAudioMixingCurrentPosition(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int pos = 0;
	if (m_lpTTTEngine_TTT != NULL)
		pos = m_lpTTTEngine_TTT->getAudioMixingCurrentPosition();
	args.GetReturnValue().Set(Nan::New(pos));
}

void setAudioMixingPosition(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int pos = args[0]->NumberValue();
	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->setAudioMixingPosition(pos);

	args.GetReturnValue().Set(Nan::New(true));
}

void setHighQualityAudioParameters(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsBoolean()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	bool mute = args[0]->BooleanValue();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->setHighQualityAudioParameters(mute);

	args.GetReturnValue().Set(Nan::New(ret));
}

void resizeRreview(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->resizeRreview();

	args.GetReturnValue().Set(Nan::New(true));
}

void muteRemoteVideoStream(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 2) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	uid_ttt uid = args[0]->NumberValue();
	bool mute = args[1]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->muteRemoteVideoStream(uid, mute);

	args.GetReturnValue().Set(Nan::New(true));
}

void muteRemoteSpeaking(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 2) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	uid_ttt uid = args[0]->NumberValue();
	bool mute = args[1]->BooleanValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->muteRemoteSpeaking(uid, mute);

	args.GetReturnValue().Set(Nan::New(true));
}

void kickChannelUser(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	uid_ttt uid = args[0]->NumberValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->kickChannelUser(uid);

	args.GetReturnValue().Set(Nan::New(true));
}

void setSpeakerphoneVolume(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int volume = args[0]->NumberValue();

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->setSpeakerphoneVolume(volume);

	args.GetReturnValue().Set(Nan::New(ret));
}

void sendChatMessage(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 3) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()|| !args[1]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	uid_ttt  uid = args[0]->NumberValue();
	int chattype = args[1]->NumberValue();

	Nan::Utf8String utf8_value(args[2]);
	int len = utf8_value.length();
	std::string msg(*utf8_value, len);

	char* buf = new char[msg.length() + 1];
	memcpy(buf, msg.data(), msg.length());

	int ret = -1;
	if (m_lpTTTEngine_TTT != NULL)
		ret = m_lpTTTEngine_TTT->sendChatMessage(uid, chattype, buf);

	delete buf;
	args.GetReturnValue().Set(Nan::New(ret));
}

void startRecordingDeviceTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber() ) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int indicationInterval = args[0]->NumberValue();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->startRecordingDeviceTest(indicationInterval);

	args.GetReturnValue().Set(Nan::New(true));
}

void stopRecordingDeviceTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->stopRecordingDeviceTest();

	args.GetReturnValue().Set(Nan::New(true));
}

void startPlaybackDeviceTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	Nan::Utf8String utf8_value(args[2]);
	int len = utf8_value.length();
	std::string filepath(*utf8_value, len);

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->startPlaybackDeviceTest(filepath.data());

	args.GetReturnValue().Set(Nan::New(true));
}

void stopPlaybackDeviceTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->stopPlaybackDeviceTest();

	args.GetReturnValue().Set(Nan::New(true));
}

void starVideotDeviceTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->starVideotDeviceTest(NULL);

	args.GetReturnValue().Set(Nan::New(true));
}
void stopVideoDeviceTest(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (m_lpTTTEngine_TTT != NULL)
		m_lpTTTEngine_TTT->stopVideoDeviceTest();

	args.GetReturnValue().Set(Nan::New(true));
}


//*****************************************
//   add by liy for GSVideo interface 
//*****************************************

/**
* @brief 获取本地可用的视频设备个数
*
* @return int,本地可用视频设备的个数，0表示没有枚举到可用的本地视频设备
*/
//int GS_GetNumOfVideoDevices(void);
void GetVideoDevices(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();
	int num = GS_GetNumOfVideoDevices();
	args.GetReturnValue().Set(Nan::New(num));
}

/**
* @brief 获取可用视频设备的名称
*
* @param [in]index 视频设备索引 0- GS_GetNumOfVideoDevices()-1
* @param [out] nameWchar 视频设备名称
* @param [out] deviceType 视频设备类型
*/
//int GS_GetVideoDeviceName(unsigned int index, wchar_t* nameWchar, VIDEODEV_TYPE* deviceType);
void GetVideoDeviceName(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int index = args[0]->NumberValue();
	VIDEODEV_TYPE vtype;
	wchar_t* nameWchar = new wchar_t[128];
	int ret = GS_GetVideoDeviceName(index, nameWchar, &vtype);

	
	std::wstring output = nameWchar;
	std::string out2 = ws_techapi::ws2s(output);

	//TODO want to return a json string to node.js
	Json::Value root;
	root["errCode"] = ret;
	if (ret == 0) {
		root["name"] = out2;
		root["deviceType"] = vtype;
	}

	std::string value = root.toStyledString();
	
	args.GetReturnValue().Set(Nan::New(value.data()).ToLocalChecked());
}

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
//int GS_SetDeviceCaptureParams(const wchar_t* nameWchar, unsigned int iWidth, unsigned int iHeight, unsigned int iFramerate, unsigned int iBitrate);
void SetVideoCaptureParams(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 5) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[1]->IsNumber()||!args[2]->IsNumber()||!args[3]->IsNumber()||!args[4]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	char name[256];

	int iWidth = 0;
	int iHeight = 0;
	int iFramerate = 0;
	int iBiterate = 0;
	size_t result = 0;
	
	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string sname(*utf8_value, len);
	std::wstring wsname = ws_techapi::s2ws(sname);

	iWidth = args[1]->NumberValue();
	iHeight = args[2]->NumberValue();
	iFramerate = args[3]->NumberValue();
	iBiterate = args[4]->NumberValue();

	int  num = GS_SetDeviceCaptureParams(wsname.data(), iWidth, iHeight, iFramerate, iBiterate);

	args.GetReturnValue().Set(Nan::New(num));
}

/**
* @brief	设置默认的视频设备名称
*
* @param	[in]nameWchar  	视频设备名称 抓屏的名称为:"dd919183-b1bb-4107-b216-968f0ebe6d4a-ScreenCapture"
*
* @return int 0表示成功，否则错误码
*/
//int GS_SetDefaultDevice(const wchar_t* nameWchar);
void SetDefaultDevice(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string sname(*utf8_value, len);
	std::wstring wsname = ws_techapi::s2ws(sname);
	int num = GS_SetDefaultDevice(wsname.data());

	args.GetReturnValue().Set(Nan::New(num));
}

//GSAudio interface
//int SetSpeakerVolume(unsigned int volume);
void Set_SpeakerVolume(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	int volume = args[0]->NumberValue();
	int num = SetSpeakerVolume(volume);

	args.GetReturnValue().Set(Nan::New(num));
}

////int SetMicVolume(unsigned int volume);
void Set_MicVolume(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	int volume = args[0]->NumberValue();
	int num = SetMicVolume(volume);

	args.GetReturnValue().Set(Nan::New(num));
}

////int GetNumOfPlayoutDevices(int* devices);
void GetNumOfSpeakers(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int devices = 0;
	double num = GetNumOfPlayoutDevices(&devices);

	if (num < 0)
		args.GetReturnValue().Set(Nan::New(num));
	else
		args.GetReturnValue().Set(Nan::New(devices));
}

////int GetNumOfRecordingDevices(int* devices);
void GetNumOfMicphones(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	int devices = 0;
	double num = GetNumOfRecordingDevices(&devices);

	Json::Value root;
	root["errCode"] = num;
	if (num == 0) {
		root["deviceNum"] = devices;
	}

	std::string value = root.toStyledString();

	args.GetReturnValue().Set(Nan::New(value.data()).ToLocalChecked());
}

////int getRecordingDefaultDevice(char deviceId[128]);
void getDefaultMicphoneID(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	char deviceID[128];
	int num = getRecordingDefaultDevice(deviceID);

	Json::Value root;
	root["errCode"] = num;
	if (num == 0) {
		root["deviceID"] = deviceID;
	}

	std::string value = root.toStyledString();

	args.GetReturnValue().Set(Nan::New(value.data()).ToLocalChecked());

}

////int getPlaybackDefaultDevice(char deviceId[128]);
void getDefaultSpeakerID(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	char deviceID[128];
	int num = getPlaybackDefaultDevice(deviceID);

	Json::Value root;
	root["errCode"] = num;
	if (num == 0) {
		root["deviceID"] = deviceID;
	}

	std::string value = root.toStyledString();

	args.GetReturnValue().Set(Nan::New(value.data()).ToLocalChecked());
}

////int getRecordingDevice(int index, char deviceName[128], char deviceId[128]);
void getMicphoneInfoByIndex(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	int index = args[0]->NumberValue();
	char deviceID[128];
	char deviceName[128];
	int num = getRecordingDevice(index, deviceName, deviceID);

	Json::Value root;
	root["errCode"] = num;
	if (num == 0) {
		root["deviceName"] = deviceName;
		root["deviceID"] = deviceID;
	}

	std::string value = root.toStyledString();
	args.GetReturnValue().Set(Nan::New(value.data()).ToLocalChecked());

}

////int gePlaybackDevice(int index, char deviceName[128], char deviceId[128]);
void getSpeakerInfoByIndex(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	int index = args[0]->NumberValue();
	char deviceID[128];
	char deviceName[128];
	int num = getPlaybackDevice(index, deviceName, deviceID);

	Json::Value root;
	root["errCode"] = num;
	if (num == 0) {
		root["deviceName"] = deviceName;
		root["deviceID"] = deviceID;
	}

	std::string value = root.toStyledString();

	args.GetReturnValue().Set(Nan::New(value.data()).ToLocalChecked());
}

////int setRecordingDevice(const char deviceId[128]);
void setMicphoneDevice(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string string_copy(*utf8_value, len);
	//std::string value = str;
	char name[128] = { 0 };
	sprintf(name, "%s", string_copy.data());

	int num = setRecordingDevice(name);

	args.GetReturnValue().Set(Nan::New(num));
}

//int setPlaybackDevice(const char deviceId[128]);
void setSpeakerDevice(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();

	// Check the number of arguments passed.
	if (args.Length() < 1) {
		// Throw an Error that is passed back to JavaScript
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	// Check the argument types
	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	Nan::Utf8String utf8_value(args[0]);
	int len = utf8_value.length();
	std::string string_copy(*utf8_value, len);
	//std::string value = str;
	char name[128] = { 0 };
	sprintf(name, "%s", string_copy.data());
	int num = -1;
	try
	{
		num = setPlaybackDevice(name);
	}
	catch (const std::exception&)
	{
		num = -99;
	}
	args.GetReturnValue().Set(Nan::New(num));
}


// Init
void Init(v8::Local<v8::Object> exports) {
	InitializeCriticalSection(&m_queue_cs);
    StreamWorkerWrapper::Init(exports);
	exports->Set(Nan::New("initialize").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(initialize)->GetFunction());

	exports->Set(Nan::New("getVersion").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getVersion)->GetFunction());

	exports->Set(Nan::New("joinChannel").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(joinChannel)->GetFunction());

	exports->Set(Nan::New("destroy").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(destroy)->GetFunction());

	exports->Set(Nan::New("leaveChannel").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(leaveChannel)->GetFunction());

	exports->Set(Nan::New("setChannelProfile").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setChannelProfile)->GetFunction());

	exports->Set(Nan::New("setClientRole").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setClientRole)->GetFunction());

	exports->Set(Nan::New("startPreview").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(startPreview)->GetFunction());

	exports->Set(Nan::New("stopPreview").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(stopPreview)->GetFunction());

	exports->Set(Nan::New("setVideoProfile").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setVideoProfile)->GetFunction());

	exports->Set(Nan::New("setupRemoteVideo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setupRemoteVideo)->GetFunction());

	exports->Set(Nan::New("setupLocalVideo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setupLocalVideo)->GetFunction());

	exports->Set(Nan::New("enableLastmileTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(enableLastmileTest)->GetFunction());

	exports->Set(Nan::New("disableLastmileTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(disableLastmileTest)->GetFunction());

	exports->Set(Nan::New("enableVideo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(enableVideo)->GetFunction());

	exports->Set(Nan::New("disableVideo").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(disableVideo)->GetFunction());


	exports->Set(Nan::New("disableAudio").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(disableAudio)->GetFunction());

	exports->Set(Nan::New("enableAudio").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(enableAudio)->GetFunction());

	exports->Set(Nan::New("startScreenCapture").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(startScreenCapture)->GetFunction());

	exports->Set(Nan::New("stopScreenCapture").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(stopScreenCapture)->GetFunction());

	exports->Set(Nan::New("setVideoCompositingLayout").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setVideoCompositingLayout)->GetFunction());

	exports->Set(Nan::New("configPublisher").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(configPublisher)->GetFunction());

	exports->Set(Nan::New("muteLocalAudioStream").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(muteLocalAudioStream)->GetFunction());

	exports->Set(Nan::New("muteAllRemoteAudioStreams").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(muteAllRemoteAudioStreams)->GetFunction());

	exports->Set(Nan::New("muteRemoteAudioStream").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(muteRemoteAudioStream)->GetFunction());

	exports->Set(Nan::New("muteLocalVideoStream").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(muteLocalVideoStream)->GetFunction());

	exports->Set(Nan::New("muteAllRemoteVideoStreams").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(muteAllRemoteVideoStreams)->GetFunction());

	exports->Set(Nan::New("enableAudioVolumeIndication").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(enableAudioVolumeIndication)->GetFunction());

	exports->Set(Nan::New("startAudioMixing").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(startAudioMixing)->GetFunction());

	exports->Set(Nan::New("stopAudioMixing").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(stopAudioMixing)->GetFunction());

	exports->Set(Nan::New("pauseAudioMixing").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(pauseAudioMixing)->GetFunction());

	exports->Set(Nan::New("resumeAudioMixing").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(resumeAudioMixing)->GetFunction());

	exports->Set(Nan::New("adjustAudioMixingVolume").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(adjustAudioMixingVolume)->GetFunction());

	exports->Set(Nan::New("getAudioMixingDuration").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getAudioMixingDuration)->GetFunction());

	exports->Set(Nan::New("getAudioMixingCurrentPosition").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getAudioMixingCurrentPosition)->GetFunction());

	exports->Set(Nan::New("setAudioMixingPosition").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setAudioMixingPosition)->GetFunction());

	exports->Set(Nan::New("setHighQualityAudioParameters").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setHighQualityAudioParameters)->GetFunction());

	exports->Set(Nan::New("resizeRreview").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(resizeRreview)->GetFunction());

	exports->Set(Nan::New("muteRemoteVideoStream").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(muteRemoteVideoStream)->GetFunction());

	exports->Set(Nan::New("muteRemoteSpeaking").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(muteRemoteSpeaking)->GetFunction());

	exports->Set(Nan::New("kickChannelUser").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(kickChannelUser)->GetFunction());

	exports->Set(Nan::New("setSpeakerphoneVolume").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setSpeakerphoneVolume)->GetFunction());

	exports->Set(Nan::New("sendChatMessage").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(sendChatMessage)->GetFunction());

	exports->Set(Nan::New("startRecordingDeviceTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(startRecordingDeviceTest)->GetFunction());

	exports->Set(Nan::New("stopRecordingDeviceTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(stopRecordingDeviceTest)->GetFunction());

	exports->Set(Nan::New("startPlaybackDeviceTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(startPlaybackDeviceTest)->GetFunction());

	exports->Set(Nan::New("stopPlaybackDeviceTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(stopPlaybackDeviceTest)->GetFunction());

	exports->Set(Nan::New("starVideotDeviceTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(starVideotDeviceTest)->GetFunction());

	exports->Set(Nan::New("stopVideoDeviceTest").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(stopVideoDeviceTest)->GetFunction());

	//add by liy for GSVideo interface 
	exports->Set(Nan::New("GS_GetNumOfVideoDevices").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetVideoDevices)->GetFunction());

	exports->Set(Nan::New("GS_GetVideoDeviceName").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetVideoDeviceName)->GetFunction());

	exports->Set(Nan::New("GS_SetDeviceCaptureParams").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(SetVideoCaptureParams)->GetFunction());  

	exports->Set(Nan::New("GS_SetDefaultDevice").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(SetDefaultDevice)->GetFunction());  //GS_SetDefaultDevice

	////add by liy for myaudiodll
	exports->Set(Nan::New("setSpeakerVolume").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Set_SpeakerVolume)->GetFunction());

	exports->Set(Nan::New("setMicVolume").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Set_MicVolume)->GetFunction());

	exports->Set(Nan::New("getNumOfRecordingDevices").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetNumOfMicphones)->GetFunction());

	exports->Set(Nan::New("getNumOfPlayoutDevices").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(GetNumOfSpeakers)->GetFunction());

	exports->Set(Nan::New("getRecordingDefaultDevice").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getDefaultMicphoneID)->GetFunction());

	exports->Set(Nan::New("getPlaybackDefaultDevice").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getDefaultSpeakerID)->GetFunction());

	exports->Set(Nan::New("getRecordingDevice").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getMicphoneInfoByIndex)->GetFunction());

	exports->Set(Nan::New("getPlaybackDevice").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getSpeakerInfoByIndex)->GetFunction());

	exports->Set(Nan::New("setRecordingDevice").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setMicphoneDevice)->GetFunction());

	exports->Set(Nan::New("setSpeakerDevice").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(setSpeakerDevice)->GetFunction());

	//destroy
	//return exports;
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init);

