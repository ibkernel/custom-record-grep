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

app.listen(port);
console.log('start at http://localhost:'+port);

console.log(crgrep.loadData());
console.log("-----data loaded------");



// var obj = crgrep.search("書本");
// var jsonObj = JSON.stringify(obj);
// console.log(jsonObj);