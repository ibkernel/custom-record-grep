var fs = require('fs');
var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var crgrep = require('bindings')('crgrep');
//var crgrep = require('./build/Release/crgrep.node')

var port = 8888;

app.use(bodyParser());
app.use(express.static(__dirname + '/views'));


app.get('/', function(req, res) {
  res.render('index.html');
});

app.post('/search', function(req, res){
	var isOutputAscendOrder = req.body.checkbox;
	var outputSize = req.body.outputSize;
	var distance = req.body.distance;
	var ascendOrder
	if (isOutputAscendOrder == "true")
		ascendOrder = 1;
	else 
		ascendOrder = 0;
	console.log("query:",req.body.query);
	searchAsync(req.body.query, ascendOrder, outputSize, distance, res)
});


function searchAsync(query, ascendOrder, outputSize, distance, res) {
	function done (err, result) {
		if (err){
			console.log("error");
			exit(1);
		}
		console.log("-----Result------");
		var jsonObj = JSON.stringify(result);
		console.log(jsonObj);
		res.end(jsonObj);
		console.log("-----End of Result------\n");
	}

	crgrep.search(query,ascendOrder,outputSize, distance, done);
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


app.listen(port);
console.log('start at http://localhost:'+port);

console.log(crgrep.loadData());
console.log("-----data loaded------");

