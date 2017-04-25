
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <nan.h>
#include <map>
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



template <typename T1, typename T2>
struct great_second {
    typedef pair<T1, T2> type;
    bool operator ()(type const& a, type const& b) const {
        return a.second > b.second;
    }
};

SearchWorker::SearchWorker(Callback *callback, string query, Record *&data, bool isAscend, int outputSize, unsigned int distance, std::vector <std::string> &vectorBookName, std::string dirPath, std::vector <double>& vectorBookScore)
	: AsyncWorker(callback), query(query), searchResult({outputSize}), isAscend(isAscend),
	 resultCount(0), data(nullptr), distance(distance), bookNames{vectorBookName}, dirPath{dirPath}, esScores{vectorBookScore}, crgrep_min_score(1), last_index(0), mergedResult(), sumMap(){}

void SearchWorker::Execute() {
	std::vector <std::string> bookPath;
	if (bookNames.empty()) {
		bookPath.push_back(dirPath);
	}else {
		for (auto book_name : bookNames) {
			bookPath.push_back(dirPath + book_name + ".txt");
		}
	}

	data = new Record(bookPath);
	std::vector <std::string> queries = parseInteractiveSearchQuery(query);
	data->searchAndSortWithRank(queries, searchResult,0,distance);
	resultCount = searchResult.getResultCount();
	searchResult.sort(isAscend);

	std::cout << "result count: " << resultCount << std::endl;
	queries.clear();
	mergeESandCrgrepResults();
}

void SearchWorker::mergeESandCrgrepResults() {
	createEsMapping();
	multiplyEsScoreWithCrgrepScore();
	sortVectorPair();
}

void SearchWorker::createEsMapping() {
	last_index = searchResult.getResultCount() -1;
	if (last_index >= 0)
		crgrep_min_score = searchResult.getResultScore(last_index);
	// Multiply all scores at first
	for (int i=0; i<bookNames.size(); i++) {
		sumMap.insert ( std::pair<std::string,double>(bookNames[i],esScores[i] * crgrep_min_score));
	}
}

void SearchWorker::multiplyEsScoreWithCrgrepScore() {
	for (int i=0; i< searchResult.getResultCount(); i++) {
		// for those crgrep had found value, divide min score to
		// restore es's score
		sumMap[searchResult.getResultTitle(i)] /= crgrep_min_score;
		sumMap[searchResult.getResultTitle(i)] *= searchResult.getResultScore(i);
	}
}



void SearchWorker::sortVectorPair() {
	std::vector<std::pair<std::string, double>> tmpResult(sumMap.begin(), sumMap.end());
	sort(tmpResult.begin(), tmpResult.end(), great_second<std::string, double>());
	mergedResult = tmpResult;
}


void SearchWorker::HandleOKCallback() {
	HandleScope scope;
	int outputCount = ((resultCount < 10) ? 10 : resultCount);
	v8::Local<v8::Array> returnArr = Nan::New<v8::Array>(outputCount+2);
	v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();
	v8::Local<v8::Object> srcObj = Nan::New<v8::Object>();
	std::string src = "merged result";
	Nan::Set(resultObj, Nan::New("resultCount").ToLocalChecked(), Nan::New(outputCount));
	Nan::Set(srcObj, Nan::New("src").ToLocalChecked(), Nan::New(src.c_str()).ToLocalChecked());
	Nan::Set(returnArr, 0, resultObj);
	Nan::Set(returnArr, 1, srcObj);
	std::string title;
	for(int i=0; i<outputCount; i++){
		
		v8::Local<v8::Object> vobj = Nan::New<v8::Object>();
		title = mergedResult[i].first;
		Nan::Set(vobj, Nan::New("title").ToLocalChecked(), Nan::New(title.c_str()).ToLocalChecked());
		Nan::Set(vobj, Nan::New("score").ToLocalChecked(), Nan::New(mergedResult[i].second));
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

