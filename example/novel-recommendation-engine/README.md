# novel-recommendation-engine
A recommendation website using elasticsearch and crgrep as search-engine

## Usage

Edit `src/crgrep.cc`'s dataPath to your data's directory.

Build cld library
```
cd src
make build-libcld
```

Build node native c++ addon
```
cd ..
node-gyp build
```

Run Elasticsearch then run server
```
npm start
```
## Elasticsearch search settings

[TODO]

## Demo page
You can visit my novel recommendation website [here](findmynovel.csie.io)
