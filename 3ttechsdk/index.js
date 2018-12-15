'use strict'

const tttechnode = require('./build/Release/3TtechSDK');

let res = tttechnode.initialize("a967ac491e3acf92eed5e1b5ba641ab7");
console.log("test interface return value = ", res);

res = tttechnode.setChannelProfile(1);
console.log("setChannelProfile res = ", res);

res = tttechnode.enableVideo();
console.log("enableVideo res = ", res);

res = tttechnode.setClientRole(1, "liyong");
console.log("setClientRole res = ", res);

res = tttechnode.setupLocalVideo(2, 0);
console.log("setupLocalVideo res = ", res);

res = tttechnode.startPreview();
console.log("startPreview res = ", res);