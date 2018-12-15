# 3ttech.cn electron windows SDK & demo
[3ttech electron windows SDKs](https://github.com/santiyun/electron-win-sdk) <br>
_The 3ttechsdk folder also is eletron windows SDKs for your._<br>
[Offical 3ttech website](http://3ttech.cn/) <br>

## Environment and dependencies
The SDKs were built for win32 bits, then you are node have to 32-bit version.

## Build and install
<pre><code>
#step 1: build electron SDK
cd 3ttechsdk
npm install
./node_modules/.bin/electron-rebuild
cd ..

#step 2: build demo
npm install
npm run package-win
npm run copy-winsdk
</code></pre>
## Start test
<pre><code>
npm start
</code></pre>

# Sample code
~~~~ javascript
const path = require("path");
// The 3techSDK addons path.
const addon_path = path.join(__dirname, "../../3ttechsdk/build/Release/3TtechSDK");
// the make_stream in the SDKs folder.
const make_stream = require('../../3ttechsdk/3tstream');
// the tttechnode are 3tech addon
const tttechnode = require(addon_path);
// The varible for received data from 3tech SDKs.
const tttdata = make_stream(tttechnode, { name: "Head Mounted Display" });

// call 3ttech addons function
// like this line.
let res = tttechnode.initialize(appkey);

// receive stream data(some events and stream-video-audio data) from 3ttech.
tttdata.from.on('sum', (sample) => {
    let data = JSON.parse(sample);
    switch (data.callbackname) {
        case 'onRemoteVideoDecoded':
        // TODO.
            break;
        case 'onUserJoined':
        // TODO.
            break;
        case 'onUserYuvPush':
        // TODO.
            break;
        case 'onJoinChannelSuccess':
        // TODO.
            break;
        //.....
    }
});
~~~~