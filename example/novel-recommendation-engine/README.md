# novel-recommendation-engine
A recommendation website using elasticsearch and crgrep as search-engine

## Usage

Edit `server.js crgrep.loadData`'s argument to your data's directory. The first argument is the data directory path and the second argument is a string of multiple sequence of specified books name separated with a `,`. If an empty string is provided, crgrep will look for all files in the directory path. 

```
crgrep.loadData('../path/must/have/last/slash/', 'book_name1,book_name2,book_name3')
```


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

Analyzer
```
cjk_analyzer
```

## Demo page
You can take a quick tour at my novel recommendation website [here](http://findmynovel.csie.io)
