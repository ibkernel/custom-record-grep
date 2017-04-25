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
  std::vector <std::string> vectorBookName;
  std::vector <double> vectorBookScore;
  Nan::Utf8String pathToDir(info[5]);
  Nan::Utf8String topNbooksFromES(info[6]);
  Nan::Utf8String topNscoresFromES(info[7]);
  std::string topResults(*topNbooksFromES);
  std::string dirPath(*pathToDir);
  std::string topResultsScores(*topNscoresFromES);
  if (dirPath.empty()){
    std::cout << "missing dir path argument" << std::endl;
    exit(1);
  }

  if (!topResults.empty()){
    stringstream ss(topResults), sscore(topResultsScores);
    while( ss.good() && sscore.good())
    {
        std::string novel_name, novel_score;
        getline( ss, novel_name, ',' );
        getline( sscore, novel_score, ',' );
        if (!novel_score.empty() &&  !novel_name.empty()){
          double score  = std::stod(novel_score);
          vectorBookName.push_back(novel_name);
          vectorBookScore.push_back(score);
        }
    }
  }

	Callback *callback = new Callback(info[4].As<Function>());
	AsyncQueueWorker(new SearchWorker(callback, str, records, isAscend, outputSize, distance, vectorBookName, dirPath, vectorBookScore));
}


void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("search").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Search)->GetFunction());
}

NODE_MODULE(crgrep, Init)
