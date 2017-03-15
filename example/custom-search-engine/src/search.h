#ifndef CRGREP_SEARCH_H_
#define CRGREP_SEARCH_H_

#include <nan.h>

struct resultFormat {
	int id;
	int score;
};

//int search(std::string pattern, struct record *&data, int dataCount, std::vector<struct resultFormat> &result);
bool sortScore(struct resultFormat a, struct resultFormat b);

class SearchWorker : public Nan::AsyncWorker {
	public:
		SearchWorker(Nan::Callback *callback, std::string query, int dataCount, struct record *&data);
		~SearchWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		int dataCount;
		std::string query;
		std::vector <struct resultFormat> searchResult;
		int resultCount;
		struct record *data;
};



#endif

