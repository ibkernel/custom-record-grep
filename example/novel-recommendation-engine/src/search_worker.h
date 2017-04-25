#ifndef CRGREP_SEARCH_WORKER_H_
#define CRGREP_SEARCH_WORKER_H_

#include <nan.h>
#include "record.h"
#include <map>
#include <utility>

//int search(std::string pattern, struct record *&data, int dataCount, std::vector<struct resultFormat> &result);
bool sortScore(struct resultFormat a, struct resultFormat b);

class SearchWorker : public Nan::AsyncWorker {
	public:
		SearchWorker(Nan::Callback *callback, std::string query, 
			Record *&data, bool isAscend, int outputSize, unsigned int distance, std::vector<std::string>& vectorBookName, std::string dirPath, std::vector <double>& vectorBookScore);
		~SearchWorker() {}

		void Execute();
		void HandleOKCallback();
		void mergeESandCrgrepResults();
		void createEsMapping();
		void multiplyEsScoreWithCrgrepScore();
		void sortVectorPair();

	private:
		Result searchResult;
		std::string query;
		int resultCount;
		Record *data;
		bool isAscend;
		unsigned int distance;
		std::string dirPath;
		std::vector <std::string> bookNames;
		std::vector <double> esScores;
		double crgrep_min_score;
		int last_index;

		std::vector <std::pair<std::string, double>> mergedResult;
		std::map <std::string, double> sumMap;
};



#endif

