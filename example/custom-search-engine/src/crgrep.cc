#include <nan.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "load_data.h"
#include "search.h"


using namespace std;
using v8::String;
using namespace v8;
using namespace Nan;

struct record *data = NULL;
int dataCount = 0;

void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(Nan::New("world").ToLocalChecked());
}

void LoadData(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	string dataPath = string("../../crgrep/data/formattedData/");
	dataCount = loadData(dataPath, data);
	info.GetReturnValue().Set(Nan::New(dataCount));
}

// void Search(const Nan::FunctionCallbackInfo<v8::Value>& info) {
// 	Nan::Utf8String q(info[0]);
// 	std::string str(*q);
// 	Callback *callback = new Callback(info[1].As<Function>());

// 	AsyncQueueWorker(new SearchWorker(callback, q));
// }

NAN_METHOD(Search) {
	Nan::Utf8String q(info[0]);
	std::string str(*q);
	Callback *callback = new Callback(info[1].As<Function>());

	AsyncQueueWorker(new SearchWorker(callback, str, dataCount, data));
}


void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("loadData").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(LoadData)->GetFunction());
  exports->Set(Nan::New("search").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Search)->GetFunction());
}

NODE_MODULE(crgrep, Init)