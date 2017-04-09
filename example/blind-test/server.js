var fs = require('fs');
var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var crgrep = require('bindings')('crgrep');
var elasticsearch = require('elasticsearch');

app.use(bodyParser());
app.use(express.static(__dirname + '/views'));
app.get('/', function(req, res) {
  res.render('index.html');
});

// Elastic search client
var client = new elasticsearch.Client({
  host: 'localhost:9200',
  log: 'trace'
});


client.ping({
  // ping usually has a 3000ms timeout
  requestTimeout: 1000
}, function (error) {
  if (error) {
    console.trace('elasticsearch cluster is down!');
  } else {
    console.log('All is well');
  }
});


var port = 8987;

app.post('/search', function(req, res){
  var isOutputAscendOrder = req.body.checkbox;
  var outputSize = req.body.outputSize;
  var distance = req.body.distance; // Useless in this particular benchmark
  var ascendOrder
  if (isOutputAscendOrder == "true")
    ascendOrder = 1;
  else 
    ascendOrder = 0;
  console.log("query:",req.body.query);
  searchCrgrepAsync(req.body.query, ascendOrder, outputSize, distance, res);
});

// crgrep search
function searchCrgrepAsync(query, ascendOrder, outputSize, distance, res) {
  function done (err, result) {
    if (err){
      console.log("error");
      exit(1);
    }
    // console.log("-----Result------");
    var crgrepJsonObj = JSON.stringify(result);
    searchElastic(query, ascendOrder, outputSize, distance, res, crgrepJsonObj);

    // res.end(crgrepJsonObj);
    // console.log("-----End of Result------\n");
  }

  crgrep.search(query,ascendOrder,outputSize, distance, done);
}

// elastic search
function searchElastic(query, ascendOrder, outputSize, distance, res, crgrepObj) {
  client.search({
    index: 'posts',
    body: {
      "_source": ["title"],
      "from": 0,
      "size" : outputSize,
      "query": {
        "bool": {
          "must": {
            "match": { 
              "content": {
                "query":   query,
                "operator": "or"
              }
            }
          },
          "should": {
            "match_phrase": { 
              "content": {
                "query": query,
                "slop":  1000000,
                "boost": 5
              }
            }
          }      
        }
      }
    }
  }, function(error, response){
    console.log(response.hits);
    // array.forEach( function(element, index) {
    //   // statements
    //   console.log(element);
    // });

    res.end(crgrepObj);
  });
}

process.on('SIGINT', function() {
    console.log("Caught interrupt signal");
    crgrep.freeData();
});


app.listen(port);
console.log('start at http://localhost:'+port);
console.log("-----Loading data------\nPlease do not interrupt the process");
console.log(crgrep.loadData());
console.log("-----data loaded------");


