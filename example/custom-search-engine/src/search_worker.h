#ifndef CRGREP_SEARCH_WORKER_H_
#define CRGREP_SEARCH_WORKER_H_

#include <nan.h>
#include "record.h"


//int search(std::string pattern, struct record *&data, int dataCount, std::vector<struct resultFormat> &result);
bool sortScore(struct resultFormat a, struct resultFormat b);

class SearchWorker : public Nan::AsyncWorker {
	public:
		SearchWorker(Nan::Callback *callback, std::string query, 
			Record *&data, bool isAscend, int outputSize, unsigned int distance);
		~SearchWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		Result searchResult;
		std::string query;
		int resultCount;
		Record *data;
		bool isAscend;
		unsigned int distance;
};



#endif

