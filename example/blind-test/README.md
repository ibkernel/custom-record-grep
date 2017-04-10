# Blind test with crgrep and Elastic search

This example is specifically created to compare crgrep and elasticsearch in a blind test manner.

**Because data used in my comparison are all chinese books, therefor the error tolerant search is not used**

## Requirement

- node-gyp
- elasticsearch

## Elastic search settings

Create index `posts` into es
```
curl -XPUT localhost:9200/posts
```

Define `posts`' mapping
```
curl -XPOST localhost:9200/posts/cjk_book/_mapping -d'
{
    "cjk_book": {
             "_all": {
            "analyzer": "cjk",
            "search_analyzer": "cjk",
            "term_vector": "no",
            "store": "false"
        },
        "properties": {
            "content": {
                "type": "text",
                "analyzer": "cjk",
                "search_analyzer": "cjk",
                "include_in_all": "true"
            },
            "title": {
                "type": "text",
                "analyzer": "cjk",
                "search_analyzer": "cjk",
                "include_in_all": "true",
                "boost": 8
            }
        }
    }
}'
```
Insert your own data into elasticsearch.

```
python es.py
```
You can check out `es.py` to see my implementation using python.


## Usage

**The default path to formatted dir is `data/` and you can manually change it in the file `src/crgrep.cc` (after changing, you should rebuild the addon by typing `node-gyp rebuild`**

After elasticsearch is set up, you can now build the node.js addon and start
the server.

```
npm install .

cd src
make build-libcld
cd ..

node-gyp build
npm start
```

Go `http://localhost:8987/` test it out!

## Elastic search settings

```
"query": {
  "bool": {
    "must": {
      "match": { 
        "content": {
          "query":   "keyword1  keyword2...",
          "operator": "and" // or
        }
      }
    },
    "should": {
      "match_phrase": { 
        "content": {
          "query": "keyword1  keyword2...",
          "slop":  1000000,
          "boost": 5
        }
      }
    }      
  }
```

## Node.js addon

In case you wonder how I turn the c++ crgrep into node.js add-on, you can go check out these files:

- `src/crgrep.cc`
- `src/search_worker.cc`
- `src/search_worker.h`

## Result
See my post [here](https://cwayne.github.io/2017/04/09/More-on-crgrep) for the full comparison result.

