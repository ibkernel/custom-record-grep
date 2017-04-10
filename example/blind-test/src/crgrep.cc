#include <nan.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "record.h"
#include "utils.h"
#include "search_worker.h"


using namespace std;
using v8::String;
using namespace v8;
using namespace Nan;

Record* records = 0;

void LoadData(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  string dataPath = string("../../data/formattedData/"); // from server.js's perspective
	//string dataPath = string("data/"); // from server.js's perspective
	records = new Record(dataPath);
	info.GetReturnValue().Set(Nan::New(records->getFileCount()));
}

NAN_METHOD(Search) {
	Nan::Utf8String q(info[0]);
	std::string str(*q);
  int a = info[1]->NumberValue();
  bool isAscend = (a == 1) ? true : false;
  int outputSize = info[2]->NumberValue();
  unsigned int distance = info[3]->Int32Value();
  
	Callback *callback = new Callback(info[4].As<Function>());
	AsyncQueueWorker(new SearchWorker(callback, str, records, isAscend, outputSize, distance));
}

void FreeData(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  delete records;
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("loadData").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(LoadData)->GetFunction());
  exports->Set(Nan::New("search").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Search)->GetFunction());
  exports->Set(Nan::New("freeData").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(FreeData)->GetFunction());
}

NODE_MODULE(crgrep, Init)