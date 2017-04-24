
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <nan.h>
#include "record.h"
#include "result.h"
#include "utils.h"
#include "search_worker.h"


using namespace std;

using v8::String;
using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;


SearchWorker::SearchWorker(Callback *callback, string query, Record *&data, bool isAscend, int outputSize, unsigned int distance, std::vector <std::string> &vectorDataPath)
	: AsyncWorker(callback), query(query), searchResult({outputSize}), isAscend(isAscend),
	 resultCount(0), data(nullptr), distance(distance), bookPath{vectorDataPath}{}

void SearchWorker::Execute() {
	data = new Record(bookPath);
	std::vector <std::string> queries = parseInteractiveSearchQuery(query);
	data->searchAndSortWithRank(queries, searchResult,0,distance);
	resultCount = searchResult.getResultCount();
	searchResult.sort(isAscend);

	std::cout << "result count: " << resultCount << std::endl;
	queries.clear();
}


void SearchWorker::HandleOKCallback() {
	HandleScope scope;
	int outputCount = ((resultCount < 10) ? resultCount : 10);
	v8::Local<v8::Array> returnArr = Nan::New<v8::Array>(outputCount+2);
	v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();
	v8::Local<v8::Object> srcObj = Nan::New<v8::Object>();
	std::string src = "crgrep";
	Nan::Set(resultObj, Nan::New("resultCount").ToLocalChecked(), Nan::New(outputCount));
	Nan::Set(srcObj, Nan::New("src").ToLocalChecked(), Nan::New(src.c_str()).ToLocalChecked());
	Nan::Set(returnArr, 0, resultObj);
	Nan::Set(returnArr, 1, srcObj);
	std::string title;
	for(int i=0; i<outputCount; i++){
		
		v8::Local<v8::Object> vobj = Nan::New<v8::Object>();
		title = searchResult.getResultTitle(i);
		std::cout << title << std::endl;
		std::cout << searchResult.getResultScore(i) << std::endl;
		Nan::Set(vobj, Nan::New("title").ToLocalChecked(), Nan::New(title.c_str()).ToLocalChecked());
		Nan::Set(vobj, Nan::New("score").ToLocalChecked(), Nan::New(searchResult.getResultScore(i)));
		Nan::Set(returnArr, i+2, vobj);
	}
	std::cout << "time to return" << std::endl;

	Local<Value> argv[] = {
			Null()
		,	v8::Local<v8::Array>(returnArr)
	};
	delete data;
	callback->Call(2, argv);
}

