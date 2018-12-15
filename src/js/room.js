'use strict'

const $ = require('jquery');
const {dialog} = require('electron');

const path = require("path");
// The 3techSDK addons path
const addon_path = path.join(__dirname, "../../3ttechsdk/build/Release/3TtechSDK");
// the make_stream in the SDKs folder.
const make_stream = require('../../3ttechsdk/3tstream');
// the tttechnode are 3tech addon
const tttechnode = require(addon_path);
// The varible for received data from 3tech SDKs.
const tttdata = make_stream(tttechnode, { name: "Head Mounted Display" });

const videoDisplay = require('../../3ttechsdk/videoDisplay');
const displayImage = new videoDisplay();
let uids = new Map();

function init() {
    let res = tttechnode.initialize("a967ac491e3acf92eed5e1b5ba641ab7");
    console.log("test interface return value = ", res);
}

// receive stream data(some event and stream-video-audio data) from 3ttech.
function streamData() {
    tttdata.from.on('sum', (sample) => {
        let d = JSON.parse(sample);
        // console.log(d);
        switch (d.callbackname) {
            case 'OnUserH264Push':
                // let dh264 = base64.decode(sample.data);
                // appendToBuffer(dh264);
                break;
            case 'onRemoteVideoDecoded':
                break;
            case 'onUserJoined':
                if (!uids.has(d.uid)) {
                    uids.set(d.uid, d.uid);
                    $(".grid-container").append('<canvas id="' + d.uid + '" class="grid-item"></canvas>');
                }
                break;
            case 'onUserYuvPush':
                drawUserData(d, 'yuv');
                break;
            case 'onJoinChannelSuccess':
                if (!uids.has(d.uid)) {
                    uids.set(d.uid, d.uid);
                    $(".grid-container").append('<canvas id="' + d.uid + '" class="grid-item"></canvas>');
                }
                break;
        }
    });
}

function drawUserData(sample, type) {
    let canvas = document.getElementById(sample.uid);
    if (type == 'rgba') {
        displayImage.drawImage(sample.data, sample.width, sample.height, canvas);
    } else {
        displayImage.yuv2canvas(sample.data, sample.width, sample.height, canvas)
    }
}

function joinRoom() {
    let url = new URL(window.location.href);
    const chanid = url.searchParams.get('chanid');

    init();
    // start recive the stream data from node addins
    streamData();

    //bind button events.
    bindEvents();

    if(chanid === '' || chanid === 'undefined') {
        return;
    } 

    let res = tttechnode.setChannelProfile(1);
    console.log("setChannelProfile res = ", res);

    res = tttechnode.enableVideo();
    console.log("enableVideo res = ", res);

    res = tttechnode.setClientRole(1, "liyong");
    console.log("setClientRole res = ", res);

    res = tttechnode.setupLocalVideo(2, 0);
    console.log("setupLocalVideo res = ", res);

    res = tttechnode.setVideoProfile(112, false);

    res = tttechnode.startPreview();
    console.log("startPreview res = ", res);

    let num = tttechnode.joinChannel("chID", chanid, 0);
    console.log("joidChannel num = ", num);
}

const test_info_area = $('#info');

function bindEvents() {
    $('#start_screen_share_bt').bind('click', () => {
        tttechnode.startScreenCapture(0, 0, 500, 500);
    });
    
    $('#stop_screen_share_bt').bind('click', () => {
        tttechnode.stopScreenCapture();
    });
    
    $('#enable_camera_bt').bind('click', () => {
        tttechnode.startPreview();
    })
    
    $('#disable_camera_bt').bind('click', () => {
        tttechnode.stopPreview();
    })

    $("#open_mix_bt").bind('click', () => {
        let path = dialog.showOpenDialog({properties: ['openFile', 'openDirectory', 'multiSelections']});
    });

    $('#close_mix_bt').bind('click', () => {

    });

    $('#vmute_bt').bind('click', () => {
        tttechnode.muteLocalVideoStream(true);
    });

    $('#unvmut_bt').bind('click', () => {
        tttechnode.muteLocalVideoStream(false);
    });

    $('#mute_bt').bind('click', () => {
        tttechnode.muteLocalAudioStream(true);
    });

    $('#unmute_bt').bind('click', () => {
        tttechnode.muteLocalAudioStream(false);
    });

    $('#startVideoDeviceTest_bt').bind('click', () => {
        let info = tttechnode.startVideoDeviceTest();
        test_info_area.val(test_info_area.val() + info + "\n");
    });
    
    $('#stopVideoDeviceTest_bt').bind('click', ()=> {
        let info = tttechnode. stopVideoDeviceTest(false);
        test_info_area.val(test_info_area.val() + info + "\n");
    });
    
    $('#getPlaybackDefaultDevice_bt').bind('click', () => {
        let info = tttechnode.getPlaybackDefaultDevice();
        test_info_area.val(test_info_area.val() + info + "\n");
    });

    $('#getRecordingDevice_bt').bind('click', () => {
        let info = tttechnode.getRecordingDevice(0);
        test_info_area.val(test_info_area.val() + info + "\n");
    });

    $('#getNumOfPlayoutDevices_bt').bind('click', () => {
        let info = tttechnode.getNumOfPlayoutDevices();
        test_info_area.val(test_info_area.val() + info + "\n");
    });

    $('#mute_all_bt').bind('click', () => {
        let info = tttechnode.muteAllRemoteAudioStream(true);
        test_info_area.val(test_info_area.val() + info + "\n");
    });

    $('#vmute_all_bt').bind('click', () => {
        let info = tttechnode.muteAllRemoteVideoStreams(true);
        test_info_area.val(test_info_area.val() + info + "\n");
    });

    $('#unmute_all_bt').bind('click', () => {
        let info = tttechnode.muteAllRemoteAudioStream(false);
        test_info_area.val(test_info_area.val() + info + "\n");
    });

    $('#unvmute_all_bt').bind('click', () => {
        let info = tttechnode.muteAllRemoteVideoStreams(false);
        test_info_area.val(test_info_area.val() + info + "\n");
    });

    $('#close_mick_bt').bind('click', () => {

    });

}

joinRoom();