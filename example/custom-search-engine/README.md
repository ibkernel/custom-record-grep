# Custom search engine using crgrep
Thanks to node.js' NAN module, it is now simplier to build a web-based search engine. This is a demo using crgrep as the searching backend. 


**The default path to formatted dir is `data/` and you can manually change it in the file `src/crgrep.cc` (after changing, you should rebuild the addon by typing `node-gyp rebuild`). You could use crgrep to generate formatted files if you don't have any**

*There are some code file added in order to make it a native addon.*

**You may want to rewrite the web page, because I just pick one of my old react.js stuff and put it online. It is badly coded**

## Requirement

- node-gyp

## Usage
Install require module
```
npm install -g node-gyp
npm install
```

build cld library
```
make build-libcld:
```

Build node native c++ addon
```
node-gyp build
```
Run server
```
npm start
```
Time to play with our newly created search engine at http:://localhost:8888/ 

## Node.js addon
In case you wonder how I turn the c++ crgrep into node.js add-on, you can go check out these files:
- `src/crgrep.cc`
- `src/search_worker.cc`
- `src/search_worker.h`
