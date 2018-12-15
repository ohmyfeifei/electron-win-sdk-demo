{
  "targets": [
    {
      "target_name": "3TtechSDK",
      "sources": [
        "src/3Ttechapi.cc",
        "src/TTTRtcEngine.h",
        "src/TTTEngineEventHandler.h",
        "src/TTTstruct.h",
        "src/TTTEngineEventHandler.cpp",
        "src/TTTstruct.h",
        "src/JSON/jsoncpp.cpp",
        "src/base64.c",
        "inc/MyAudioApi.h",
        "inc/MyVideoApi.h"
      ],
      "cflags!": [
        "-fno-exceptions"
      ],
      "cflags_cc!": [
        "-fno-exceptions"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<!(node -e \"require('nan')\")",
        "<!(node -e \"require('streaming-worker-sdk')\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "conditions": [
        [
          "OS==\"win\"",
          {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "ExceptionHandling": 1
              }
            },
            "libraries": [
              "<(module_root_dir)/lib/myaudiodll.lib",
              "<(module_root_dir)/lib/wstechapi.lib",
              "<(module_root_dir)/lib/GSVideo.lib"
            ],
            "copies": [
              {
                "destination": "<(module_root_dir)/build/Release/",
                "files": [
                  "<(module_root_dir)/lib/AudioDecoder.dll",
                  "<(module_root_dir)/lib/avcodec-57.dll",
                  "<(module_root_dir)/lib/avformat-57.dll",
                  "<(module_root_dir)/lib/avutil-55.dll",
                  "<(module_root_dir)/lib/capturescr.dll",
                  "<(module_root_dir)/lib/GSVideo.dll",
                  "<(module_root_dir)/lib/libfaac.dll",
                  "<(module_root_dir)/lib/libgcc_s_dw2-1.dll",
                  "<(module_root_dir)/lib/libNet.dll",
                  "<(module_root_dir)/lib/libwinpthread-1.dll",
                  "<(module_root_dir)/lib/libx264-152.dll",
                  "<(module_root_dir)/lib/msvcp100.dll",
                  "<(module_root_dir)/lib/msvcr100.dll",
                  "<(module_root_dir)/lib/myaudiodll.dll",
                  "<(module_root_dir)/lib/opencv_core249.dll",
                  "<(module_root_dir)/lib/opencv_imgproc249.dll",
                  "<(module_root_dir)/lib/pthreadVC2.dll",
                  "<(module_root_dir)/lib/swresample-2.dll",
                  "<(module_root_dir)/lib/wstechapi.dll",
                  "<(module_root_dir)/lib/msvcp140.dll",
                  "<(module_root_dir)/lib/vcruntime140.dll",
                  "<(module_root_dir)/lib/wstechapi.lib",
                  "<(module_root_dir)/lib/GSVideo.lib",
                  "<(module_root_dir)/lib/myaudiodll.lib",
                  "<(module_root_dir)/lib/log_config.xml"
                ]
              }
            ]
          }
        ],
        [
          "OS==\"mac\"",
          {
            "xcode_settings": {
              "CLANG_CXX_LIBRARY": "libc++",
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "MACOSX_DEPLOYMENT_TARGET": "10.7"
            }
          }
        ]
      ]
    }
  ]
}