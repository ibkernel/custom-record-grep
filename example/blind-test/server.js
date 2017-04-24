var fs = require('fs');
var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var crgrep = require('bindings')('crgrep');
var elasticsearch = require('elasticsearch');

var extend = require('util')._extend

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
  var isMustHave = req.body.isMustHave;
  var ascendOrder
  if (isOutputAscendOrder == "true")
    ascendOrder = 1;
  else 
    ascendOrder = 0;
  console.log("query:",req.body.query);
  searchCrgrepAsync(req.body.query, ascendOrder, outputSize, distance,isMustHave, res);
});

// crgrep search
function searchCrgrepAsync(query, ascendOrder, outputSize, distance,isMustHave, res) {
  function done (err, result) {
    if (err){
      console.log("error");
      exit(1);
    }
    // console.log("-----Result------");
    var crgrepJsonObj = JSON.stringify(result);
    searchElastic(query, ascendOrder, outputSize, distance, res, isMustHave, crgrepJsonObj);

    // res.end(crgrepJsonObj);
    // console.log("-----End of Result------\n");
  }

  crgrep.search(query,ascendOrder,outputSize, distance, done);
}

// elastic search
function searchElastic(query, ascendOrder, outputSize, distance, res, isMustHave, crgrepObj) {
  outputSize = (outputSize != -1 ? outputSize : 10);
  operator = "or";
  if (isMustHave == "true")
    operator = "and";
  query = query.replace('+', '').replace('-', '');
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
                "query":   "安娜",
                "operator": "and"
              }
            }
          },
          "should": {
            "match_phrase": { 
              "content": {
                "query": "安娜",
                "slop":  1000000,
                "boost": 5
              }
            }
          }      
        }
      }
    }
  }, function(error, response){
    var arr = response.hits.hits;
    var esObj = [];
    esObj.push({resultCount: response.hits.total});
    esObj.push({src: "elasticsearch"});
    for (var i = 0, len = arr.length; i < len; i++) {
      esObj.push({title: arr[i]['_source'].title.replace('@title:', '').replace('\n',''),score: arr[i]['_score']});
    }
    var randomObj = {};
    esObj = JSON.stringify(esObj);
    if (Math.random() >= 0.5){ // random bool
      randomObj['1'] = crgrepObj;
      randomObj['2'] = esObj;
    }else {
      randomObj['1'] = esObj;
      randomObj['2'] = crgrepObj;
    }

    res.end(JSON.stringify(randomObj));
  });
}

process.on('SIGINT', function() {
    console.log("Caught interrupt signal");
    crgrep.freeData();
    gracefulShutdown(); // Weird behavior
});

var gracefulShutdown = function() {
  console.log("Shutting down gracefully.");
  process.exit();

  // server.close(function() {
  //   console.log("Closed out remaining connections.");
  //   process.exit()
  // });
  
  //  // if after 
  //  setTimeout(function() {
  //      console.error("Could not close connections in time, forcefully shutting down");
  //      process.exit()
  // }, 1000);
}

process.on('uncaughtException', function (err) {
  console.log('Error happened, please check if your elasticsearch is running.\nTerminating process');
  crgrep.freeData();
  gracefulShutdown();
});

var server = app.listen(port);
console.log('start at http://localhost:'+port);
console.log("-----Loading data------\nPlease do not interrupt the process");
console.log(crgrep.loadData("../../data/formattedData", ""));
console.log("-----data loaded------");


