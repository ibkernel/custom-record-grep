
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <nan.h>
#include "record.h"
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


SearchWorker::SearchWorker(Callback *callback, string query, Record *&data)
	: AsyncWorker(callback), query(query), searchResult(std::vector <std::tuple <std::string, int>>()),
	 resultCount(0), data(data){}

void SearchWorker::Execute() {
	searchResult = data->searchAndSortWithRank(query);
	resultCount = searchResult.size();
}


void SearchWorker::HandleOKCallback() {
	HandleScope scope;
	int outputCount = ((resultCount < 10) ? resultCount : 10);
	v8::Local<v8::Array> returnArr = Nan::New<v8::Array>(outputCount+1);
	v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();
	Nan::Set(resultObj, Nan::New("resultCount").ToLocalChecked(), Nan::New(outputCount));
	Nan::Set(returnArr, 0, resultObj);
	std::string title;
	for(int i=0; i<outputCount; i++){
		v8::Local<v8::Object> vobj = Nan::New<v8::Object>();
		title = std::get<0>(searchResult[i]);
		Nan::Set(vobj, Nan::New("title").ToLocalChecked(), Nan::New(title.c_str()).ToLocalChecked());
		Nan::Set(vobj, Nan::New("score").ToLocalChecked(), Nan::New(std::get<1>(searchResult[i])));
		Nan::Set(returnArr, i+1, vobj);
	}

	Local<Value> argv[] = {
			Null()
		,	v8::Local<v8::Array>(returnArr)
	};

	callback->Call(2, argv);
}

