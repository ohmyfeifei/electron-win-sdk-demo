#include <node.h>
#include <nan.h>
#include <napi.h>

#include <chrono>
#include <random>
#include <thread>
#include <iostream>
#include "streaming-worker.h"
#include <regex>

#include <unistd.h>

#include "TTTRtcEngine.hpp"
#include "TTTEventHandler.h"

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

static TTTRtcEngine *m_lpTTTEngine;
TTTEventHandler m_EventHandler;

std::queue<TaskQueue*> task_queue;
pthread_mutex_t m_mutex;

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
            pthread_mutex_lock(&m_mutex);
            if (task_queue.size() > 0) {
                pTask = task_queue.front();
                task_queue.pop();
            }
            pthread_mutex_unlock(&m_mutex);
            if (pTask != NULL) {
                switch (pTask->cmd)
                {
                    case cmd_push_queue:
                        Message tosend("sum", pTask->json);
                        writeToNode(progress, tosend);
                        break;
                }
                delete pTask;
            }
            
            sleep(0.01);
        }
        
        while (!closed()) {
            send_sample(progress, pos_dist(rd), pos_dist(rd), pos_dist(rd));
            std::this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
private:
    string name;
};

StreamingWorker * create_worker(Callback *data, Callback *complete, Callback *error_callback, v8::Local<v8::Object> & options) {
    return new Sensor(data, complete, error_callback, options);
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

    if (m_lpTTTEngine == NULL) {
        m_lpTTTEngine = TTTRtcEngine::getInstance(sname.c_str());
    }
    if (m_lpTTTEngine == NULL) {
        args.GetReturnValue().Set(Nan::New(false));
        return;
    }

    m_lpTTTEngine->setEventHandler(&m_EventHandler);
    
	args.GetReturnValue().Set(Nan::New(true));
}

void getVersion(const Nan::FunctionCallbackInfo<v8::Value>& args) {
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
	if ( m_lpTTTEngine != NULL )
		ret = m_lpTTTEngine->joinChannel(key, name, userID);
    
	args.GetReturnValue().Set(Nan::New(ret));
}

void destroy(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (m_lpTTTEngine != NULL)
		m_lpTTTEngine->destroy();
	args.GetReturnValue().Set(Nan::New(true));
}

void leaveChannel(const Nan::FunctionCallbackInfo<v8::Value>& args) {
	if (m_lpTTTEngine != NULL)
		m_lpTTTEngine->leaveChannel();
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
	int ret = -1;
	if (m_lpTTTEngine != NULL)
		ret = m_lpTTTEngine->setChannelProfile((TTT_CHANNEL_PROFILE)profile);
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
	if (m_lpTTTEngine != NULL)
		ret = m_lpTTTEngine->setClientRole((TTT_CLIENT_ROLE)role);
	args.GetReturnValue().Set(Nan::New(ret));
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
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->setVideoProfile(profile, swapWidthAndHeight);
    
    args.GetReturnValue().Set(Nan::New(true));
}

void enableVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    int ret = -1;
    if (m_lpTTTEngine != NULL)
        ret = m_lpTTTEngine->enableVideo();
    
    args.GetReturnValue().Set(Nan::New(ret));
}

void disableVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    int ret = -1;
    if (m_lpTTTEngine != NULL)
        ret = m_lpTTTEngine->disableVideo();
    
    args.GetReturnValue().Set(Nan::New(ret));
}

void startPreview(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->startPreview();
    args.GetReturnValue().Set(Nan::New(true));
}

void stopPreview(const Nan::FunctionCallbackInfo<v8::Value>& args) {
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->stopPreview();
    args.GetReturnValue().Set(Nan::New(true));
}

// void setupLocalVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	// Check the number of arguments passed.
// 	if (args.Length() < 2) {
// 		// Throw an Error that is passed back to JavaScript
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong number of arguments")));
// 		return;
// 	}

// 	// Check the argument types
// 	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong arguments")));
// 		return;
// 	}

// 	int renderMode = args[0]->NumberValue();
// 	int64_t uid = args[1]->IntegerValue();

// 	VideoCanvas_TTT canvas;
// 	canvas.renderMode = renderMode;
// 	canvas.uid = uid;
// 	canvas.devId = NULL;

// 	int ret = -1;
// 	if (m_lpTTTEngine != NULL)
// 		ret = m_lpTTTEngine->setupLocalVideo(canvas);

// 	args.GetReturnValue().Set(Nan::New(ret));
// }

// void setupRemoteVideo(const Nan::FunctionCallbackInfo<v8::Value>& args) {
//     Isolate* isolate = args.GetIsolate();

//     // Check the number of arguments passed.
//     if (args.Length() < 3) {
//         // Throw an Error that is passed back to JavaScript
//         isolate->ThrowException(Exception::TypeError(
//             String::NewFromUtf8(isolate, "Wrong number of arguments")));
//         return;
//     }

//     // Check the argument types
//     if (!args[0]->IsNumber()||!args[1]->IsNumber()) {
//         isolate->ThrowException(Exception::TypeError(
//             String::NewFromUtf8(isolate, "Wrong arguments")));
//         return;
//     }
//     int renderMode = args[0]->NumberValue();
//     int64_t uid = args[1]->IntegerValue();

//     Nan::Utf8String utf8_value(args[2]);
//     int len = utf8_value.length();
//     std::string devID(*utf8_value, len);
//     //info[3]and [4] for view handle, and priv data

//     VideoCanvas_TTT canvas;
//     canvas.renderMode = renderMode;
//     canvas.uid = uid;
//     canvas.devId = devID.data();

//     int ret = -1;
//     if (m_lpTTTEngine != NULL)
//         ret = m_lpTTTEngine->setupRemoteVideo(canvas);

//     args.GetReturnValue().Set(Nan::New(ret));
// }

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
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->enableAudioVolumeIndication(interval, smooth);
    
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
    if (m_lpTTTEngine != NULL)
        ret = m_lpTTTEngine->setHighQualityAudioParameters(mute, mute, mute);
    
    args.GetReturnValue().Set(Nan::New(ret));
}

// void setVideoCompositingLayout(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	int ret = -1;
// 	VideoCompositingLayout_TTT sei;
// 	//TODO 参数怎么组织
// 	Nan::Utf8String utf8_value(args[0]);
// 	int len = utf8_value.length();
// 	std::string url(*utf8_value, len);
	
// 	if (m_lpTTTEngine != NULL)
// 		ret = m_lpTTTEngine->setVideoCompositingLayoutSei(url.data(), url.length());

// 	args.GetReturnValue().Set(Nan::New(ret));
// }

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
    
    TTT_PublisherConfiguration profile;
    profile.publishUrl = url;
    
    if (m_lpTTTEngine != NULL)
        ret = m_lpTTTEngine->configPublisher(profile);
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
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->muteLocalAudioStream(mute);
    
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
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->muteAllRemoteAudioStreams(mute);
    
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
    int64_t uid = args[0]->NumberValue();
    bool mute = args[1]->BooleanValue();
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->muteRemoteAudioStream(uid, mute);
    
    args.GetReturnValue().Set(Nan::New(true));
}

// void muteLocalVideoStream(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	// Check the number of arguments passed.
// 	if (args.Length() < 1) {
// 		// Throw an Error that is passed back to JavaScript
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong number of arguments")));
// 		return;
// 	}

// 	// Check the argument types
// 	if (!args[0]->IsBoolean()) {
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong arguments")));
// 		return;
// 	}
// 	bool mute = args[0]->BooleanValue();

// 	if (m_lpTTTEngine != NULL)
// 		m_lpTTTEngine->muteLocalVideoStream(mute);

// 	args.GetReturnValue().Set(Nan::New(true));
// }

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
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->muteAllRemoteVideoStreams(mute);
    
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
    int64_t uid = args[0]->NumberValue();
    bool mute = args[1]->BooleanValue();
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->muteRemoteVideoStream(uid, mute);
    
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
    int64_t uid = args[0]->NumberValue();
    bool mute = args[1]->BooleanValue();
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->muteRemoteSpeaking(uid, mute);
    
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
    int64_t uid = args[0]->NumberValue();
    
    if (m_lpTTTEngine != NULL)
        m_lpTTTEngine->kickChannelUser(uid);
    
    args.GetReturnValue().Set(Nan::New(true));
}

// void startAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	// Check the number of arguments passed.
// 	if (args.Length() < 4) {
// 		// Throw an Error that is passed back to JavaScript
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong number of arguments")));
// 		return;
// 	}

// 	// Check the argument types
// 	if (!args[3]->IsNumber()) {
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong arguments")));
// 		return;
// 	}

// 	Nan::Utf8String utf8_value(args[0]);
// 	int len = utf8_value.length();
// 	std::string filepath(*utf8_value, len);
// 	bool loopback = args[1]->BooleanValue();
// 	bool replace = args[2]->BooleanValue();
// 	int  cycle = args[3]->NumberValue();

// 	int ret = -1;
// 	if (m_lpTTTEngine != NULL)
// 		ret = m_lpTTTEngine->startAudioMixing(filepath.data(), loopback, replace, cycle);

// 	args.GetReturnValue().Set(Nan::New(ret));
// }

// void stopAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	if (m_lpTTTEngine != NULL)
// 		m_lpTTTEngine->stopAudioMixing();

// 	args.GetReturnValue().Set(Nan::New(true));
// }

// void pauseAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	if (m_lpTTTEngine != NULL)
// 		m_lpTTTEngine->pauseAudioMixing();

// 	args.GetReturnValue().Set(Nan::New(true));
// }

// void resumeAudioMixing(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	if (m_lpTTTEngine != NULL)
// 		m_lpTTTEngine->resumeAudioMixing();

// 	args.GetReturnValue().Set(Nan::New(true));
// }

// void adjustAudioMixingVolume(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	// Check the number of arguments passed.
// 	if (args.Length() < 1) {
// 		// Throw an Error that is passed back to JavaScript
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong number of arguments")));
// 		return;
// 	}

// 	// Check the argument types
// 	if (!args[0]->IsNumber()) {
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong arguments")));
// 		return;
// 	}

// 	int volume = args[0]->NumberValue();

// 	if (m_lpTTTEngine != NULL)
// 		m_lpTTTEngine->adjustAudioMixingVolume(volume);

// 	args.GetReturnValue().Set(Nan::New(true));
// }

// void getAudioMixingDuration(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	int duration = 0;
// 	if (m_lpTTTEngine != NULL)
// 		duration = m_lpTTTEngine->getAudioMixingDuration();

// 	args.GetReturnValue().Set(Nan::New(duration));
// }

// void getAudioMixingCurrentPosition(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	int pos = 0;
// 	if (m_lpTTTEngine != NULL)
// 		pos = m_lpTTTEngine->getAudioMixingCurrentPosition();
// 	args.GetReturnValue().Set(Nan::New(pos));
// }

// void setAudioMixingPosition(const Nan::FunctionCallbackInfo<v8::Value>& args) {
// 	Isolate* isolate = args.GetIsolate();

// 	// Check the number of arguments passed.
// 	if (args.Length() < 1) {
// 		// Throw an Error that is passed back to JavaScript
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong number of arguments")));
// 		return;
// 	}

// 	// Check the argument types
// 	if (!args[0]->IsNumber()) {
// 		isolate->ThrowException(Exception::TypeError(
// 			String::NewFromUtf8(isolate, "Wrong arguments")));
// 		return;
// 	}
// 	int pos = args[0]->NumberValue();
// 	if (m_lpTTTEngine != NULL)
// 		m_lpTTTEngine->setAudioMixingPosition(pos);

// 	args.GetReturnValue().Set(Nan::New(true));
// }

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
     int64_t  uid = args[0]->NumberValue();
     int chattype = args[1]->NumberValue();

     Nan::Utf8String utf8_value(args[2]);
     int len = utf8_value.length();
     std::string msg(*utf8_value, len);

     char* buf = new char[msg.length() + 1];
     memcpy(buf, msg.data(), msg.length());

     int ret = -1;
     if (m_lpTTTEngine != NULL)
         ret = m_lpTTTEngine->sendChatMessage(uid, chattype, "", buf);

     delete buf;
     args.GetReturnValue().Set(Nan::New(ret));
 }

// Init
void Init(v8::Local<v8::Object> exports) {
    pthread_mutex_init(&m_mutex, NULL);
    
    StreamWorkerWrapper::Init(exports);
    
	exports->Set(Nan::New("initialize").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(initialize)->GetFunction());

    exports->Set(Nan::New("destroy").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(destroy)->GetFunction());
    
	exports->Set(Nan::New("getVersion").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(getVersion)->GetFunction());

	exports->Set(Nan::New("joinChannel").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(joinChannel)->GetFunction());

    exports->Set(Nan::New("leaveChannel").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(leaveChannel)->GetFunction());

    exports->Set(Nan::New("setChannelProfile").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(setChannelProfile)->GetFunction());

    exports->Set(Nan::New("setClientRole").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(setClientRole)->GetFunction());
    
    exports->Set(Nan::New("setVideoProfile").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(setVideoProfile)->GetFunction());
    
    exports->Set(Nan::New("enableVideo").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(enableVideo)->GetFunction());
    
    exports->Set(Nan::New("disableVideo").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(disableVideo)->GetFunction());

     exports->Set(Nan::New("startPreview").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(startPreview)->GetFunction());

     exports->Set(Nan::New("stopPreview").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(stopPreview)->GetFunction());

//     exports->Set(Nan::New("setupLocalVideo").ToLocalChecked(),
//         Nan::New<v8::FunctionTemplate>(setupLocalVideo)->GetFunction());

//     exports->Set(Nan::New("setupRemoteVideo").ToLocalChecked(),
//         Nan::New<v8::FunctionTemplate>(setupRemoteVideo)->GetFunction());

//     exports->Set(Nan::New("setVideoCompositingLayout").ToLocalChecked(),
//         Nan::New<v8::FunctionTemplate>(setVideoCompositingLayout)->GetFunction());
    
     exports->Set(Nan::New("configPublisher").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(configPublisher)->GetFunction());
    
    exports->Set(Nan::New("setHighQualityAudioParameters").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(setHighQualityAudioParameters)->GetFunction());
    
    exports->Set(Nan::New("enableAudioVolumeIndication").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(enableAudioVolumeIndication)->GetFunction());

     exports->Set(Nan::New("muteLocalAudioStream").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(muteLocalAudioStream)->GetFunction());

    exports->Set(Nan::New("muteRemoteAudioStream").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(muteRemoteAudioStream)->GetFunction());
    
     exports->Set(Nan::New("muteAllRemoteAudioStreams").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(muteAllRemoteAudioStreams)->GetFunction());

	// exports->Set(Nan::New("muteLocalVideoStream").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(muteLocalVideoStream)->GetFunction());

     exports->Set(Nan::New("muteRemoteVideoStream").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(muteRemoteVideoStream)->GetFunction());

     exports->Set(Nan::New("muteAllRemoteVideoStreams").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(muteAllRemoteVideoStreams)->GetFunction());

     exports->Set(Nan::New("muteRemoteSpeaking").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(muteRemoteSpeaking)->GetFunction());
    
     exports->Set(Nan::New("kickChannelUser").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(kickChannelUser)->GetFunction());

	// exports->Set(Nan::New("startAudioMixing").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(startAudioMixing)->GetFunction());

	// exports->Set(Nan::New("stopAudioMixing").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(stopAudioMixing)->GetFunction());

	// exports->Set(Nan::New("pauseAudioMixing").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(pauseAudioMixing)->GetFunction());

	// exports->Set(Nan::New("resumeAudioMixing").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(resumeAudioMixing)->GetFunction());

	// exports->Set(Nan::New("adjustAudioMixingVolume").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(adjustAudioMixingVolume)->GetFunction());

	// exports->Set(Nan::New("getAudioMixingDuration").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(getAudioMixingDuration)->GetFunction());

	// exports->Set(Nan::New("getAudioMixingCurrentPosition").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(getAudioMixingCurrentPosition)->GetFunction());

	// exports->Set(Nan::New("setAudioMixingPosition").ToLocalChecked(),
	// 	Nan::New<v8::FunctionTemplate>(setAudioMixingPosition)->GetFunction());

     exports->Set(Nan::New("sendChatMessage").ToLocalChecked(),
         Nan::New<v8::FunctionTemplate>(sendChatMessage)->GetFunction());
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init);

