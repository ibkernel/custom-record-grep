#include <nan.h>
#include <sstream>
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

NAN_METHOD(Search) {
  Nan::Utf8String q(info[0]);
  std::string str(*q);
  int a = info[1]->NumberValue();
  bool isAscend = (a == 1) ? true : false;
  int outputSize = info[2]->NumberValue();
  unsigned int distance = info[3]->Int32Value();
  std::vector <std::string> vectorDataPath;
  Nan::Utf8String pathToDir(info[5]);
  Nan::Utf8String top30booksFromES(info[6]);
  std::string top30(*top30booksFromES);
  std::string dirPath(*pathToDir);
  if (dirPath.empty()){
    std::cout << "missing dir path argument" << std::endl;
    exit(1);
  }

  if (top30.empty()){
    vectorDataPath.push_back(dirPath);
  }
  else {
    stringstream ss(top30);
    while( ss.good() )
    {
        std::string novel_name;
        getline( ss, novel_name, ',' );
        vectorDataPath.push_back( dirPath + novel_name + ".txt");
    }
  }

	Callback *callback = new Callback(info[4].As<Function>());
	AsyncQueueWorker(new SearchWorker(callback, str, records, isAscend, outputSize, distance, vectorDataPath));
}


void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("search").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Search)->GetFunction());
}

NODE_MODULE(crgrep, Init)
