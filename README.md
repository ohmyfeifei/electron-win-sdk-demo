# 3ttech.cn electron windows SDK demo
[3ttech electron windows SDKs](https://github.com/santiyun/electron-win-sdk) <br>
[Offical 3ttech website](http://3ttech.cn/)

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
