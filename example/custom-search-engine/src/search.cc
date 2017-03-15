
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include "search.h"
#include "load_data.h"
#include <nan.h>

using namespace std;

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

// int search(string pattern, struct record *&data, int dataCount, vector<struct resultFormat> &result){
// 	struct resultFormat currentData;
// 	int currentScore = 0;
// 	for (int i=0; i < dataCount; i++) {
// 		if(strstr(data[i].id, pattern.c_str())>0){
// 			currentScore += 1;
// 		}
// 		if(strstr(data[i].title, pattern.c_str())>0){
// 			currentScore += 10;
// 		}
// 		if(strstr(data[i].content, pattern.c_str())>0){
// 			currentScore += 3;
// 		}
// 		//TODO: store current data into a self-managed-order data structure
// 		if (currentScore > 0){
// 				currentData.id = i;
// 				currentData.score = currentScore;
// 				result.push_back(currentData);
// 				currentScore = 0;
// 		}
// 	}
// 	// TODO: Let it be customizable in the future (ascend/descend)
// 	sort(result.begin(), result.end(), sortScore);
// 	return result.size();
// }

bool sortScore(struct resultFormat a, struct resultFormat b){
	return (a.score > b.score) ? true : false;
}

// void Search(const Nan::FunctionCallbackInfo<v8::Value>& info) {

// 	if (info.Length() != 1) {
// 		Nan::ThrowTypeError("Wrong number of argument");
// 	}
// 	if (!info[0]->IsString()){
// 		Nan::ThrowTypeError("Wrong type of argument");
// 	}
// 	Nan::Utf8String q(info[0]);
// 	std::string str(*q);
// 	//Nan::MaybeLocal<v8::String> query = Nan::To<v8::String>(info[0]);
// 	// Local<String> arg1;
// 	// if (query.ToLocal(&arg1)!= true){
// 	// 	 Nan::ThrowError("Error converting second argument to string");
// 	// }
// 	// std::string to = std::string(param1);
// 	//cout << query << endl;
// 	vector<struct resultFormat> result;
// 	int resultCount = search(str, data, dataCount, result);
// 	v8::Local<v8::Array> returnArr = Nan::New<v8::Array>(resultCount);
// 	v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();
// 	Nan::Set(resultObj, Nan::New("resultCount").ToLocalChecked(), Nan::New(resultCount));
// 	Nan::Set(returnArr, 0, resultObj);
//   for(int i=0; i<resultCount; i++){
//   	v8::Local<v8::Object> vobj = Nan::New<v8::Object>();
//   	Nan::Set(vobj, Nan::New("id").ToLocalChecked(), Nan::New(result[i].id));
//   	Nan::Set(vobj, Nan::New("score").ToLocalChecked(), Nan::New(result[i].score));
//   	Nan::Set(returnArr, i+1, vobj);
//   }

// 	info.GetReturnValue().Set(returnArr);
// }

// class SearchWorker : public AsyncWorker {
// 	public:
		SearchWorker::SearchWorker(Callback *callback, string query, int dataCount, struct record *&data)
			: AsyncWorker(callback), query(query), searchResult(vector<struct resultFormat>()), dataCount(dataCount), resultCount(0), data(data){}
		// SearchWorker::~SearchWorker() {}

		void SearchWorker::Execute() {
				struct resultFormat currentData;
				int currentScore = 0;
				for (int i=0; i < dataCount; i++) {
					if(strstr(data[i].id, query.c_str())>0){
						currentScore += 1;
					}
					if(strstr(data[i].title, query.c_str())>0){
						currentScore += 10;
					}
					if(strstr(data[i].content, query.c_str())>0){
						currentScore += 3;
					}
					//TODO: store current data into a self-managed-order data structure
					if (currentScore > 0){
							currentData.id = i;
							currentData.score = currentScore;
							searchResult.push_back(currentData);
							currentScore = 0;
					}
				}
				// TODO: Let it be customizable in the future (ascend/descend)
				sort(searchResult.begin(), searchResult.end(), sortScore);
				resultCount = searchResult.size();
		}

		void SearchWorker::HandleOKCallback() {
			HandleScope scope;

			int outputCount = ((resultCount < 10) ? resultCount : 10);
			v8::Local<v8::Array> returnArr = Nan::New<v8::Array>(outputCount+1);
			v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();
			Nan::Set(resultObj, Nan::New("resultCount").ToLocalChecked(), Nan::New(outputCount));
			Nan::Set(returnArr, 0, resultObj);
			for(int i=0; i<outputCount; i++){
				v8::Local<v8::Object> vobj = Nan::New<v8::Object>();
				Nan::Set(vobj, Nan::New("id").ToLocalChecked(), Nan::New(searchResult[i].id));
				Nan::Set(vobj, Nan::New("score").ToLocalChecked(), Nan::New(searchResult[i].score));
				Nan::Set(returnArr, i+1, vobj);
			}

			Local<Value> argv[] = {
					Null()
				,	v8::Local<v8::Array>(returnArr)
			};

			callback->Call(2, argv);
		}

// 	private:
// 		int dataCount;
// 		string query;
// 		vector <struct resultFormat> searchResult;
// 		int resultCount;
// 		struct record *data;
// };

// // Asynchronous access to the `Estimate()` function
// NAN_METHOD(SearchAsync) {
//   int points = To<int>(info[0]).FromJust();
//   Callback *callback = new Callback(info[1].As<Function>());

//   AsyncQueueWorker(new PiWorker(callback, points));
// }
