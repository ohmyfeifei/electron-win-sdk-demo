# 3ttech.cn electron windows SDK demo
## Build and install
<pre><code>
#step 1: build electron SDK
cd 3ttechsdk
npm install
./node_modules/.bin/electron-rebuild

#step 2: build demo
npm install
npm run package-win
npm run copy-winsdk
</code></pre>
## Start test
<pre><code>
npm start
</code></pre>
