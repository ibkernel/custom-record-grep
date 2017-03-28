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
	var c1 = req.body.checkbox;
  var c2 = req.body.method; // NOT DONE YET
  var c3 = "-L "+req.body.list;
  var c4 = "-R "+req.body.ranking;
  var c5 = req.body.outputSize;
  var c6 = req.body.distance;
  console.log("@C:"+req.body.checkbox);
  console.log("1:"+req.body.query,"2:",c1,"3:",c2,"4:", c3,"5:", c4,"6:", c5,"7:", c6);
	console.log("query:",req.body.query);
	// q = "校花 裝逼 打臉 小白臉";
	searchAsync(req.body.query, res);
	//res.send('ok');
});


function searchAsync(query, res) {
	function done (err, result) {
		if (err){
			console.log("error");
			exit(1);
		}
		console.log("-----Result------");
		var jsonObj = JSON.stringify(result);
		//console.log(jsonObj);
		// res.end(jsonObj);
		console.log("-----End of Result------\n");
	}

	crgrep.search(query, done);
}

app.listen(port);
console.log('start at http://localhost:'+port);

console.log(crgrep.loadData());
console.log("-----data loaded------");



// var obj = crgrep.search("書本");
// var jsonObj = JSON.stringify(obj);
// console.log(jsonObj);