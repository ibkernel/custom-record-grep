var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var crgrep = require('bindings')('crgrep');
//var crgrep = require('./build/Release/crgrep.node')

var port = 8888;

app.use(bodyParser());
app.get('/', function(req, res){
	q = "魔法 火球  冰霜 閃電";
	searchAsync(q, res);
});

app.post('/search', function(req, res){
	console.log("query:",req.body.query);
	q = "校花 裝逼 打臉 小白臉";
	searchAsync(q, res);
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
		res.end(jsonObj);
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