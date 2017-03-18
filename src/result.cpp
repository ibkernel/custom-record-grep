#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "result.h"


Result::Result() {
}

Result::~Result() {
	searchResult.clear();
}

void Result::insertResult(std::string title, int searchScore, int searchMatchCount) {
	struct result foundResult;
	foundResult.recordTitle = title;
	foundResult.recordScore = searchScore;
	foundResult.recordMatchCount = searchMatchCount;

	searchResult.push_back(foundResult);

}

int Result::getResultCount(){
	return searchResult.size();
};

double Result::getResultScore(int i){
	return searchResult[i].recordScore;
};

std::string Result::getResultTitle(int i ){
	return searchResult[i].recordTitle;
};

void Result::printResult(bool order) {
	sort(order);
	for (auto r: searchResult) {
		std::cout << "Book :" << r.recordTitle << std::endl;
		std::cout << "Rank score:" << r.recordScore << std::endl;
		std::cout << "Match count:" << r.recordMatchCount << std::endl;
	}
}

void Result::sort(bool order) {
	if (order) {	// ascending
		std::sort(searchResult.begin(), searchResult.end(),
			[](struct result const &t1, struct result const &t2) {
					return t1.recordScore < t2.recordScore;
				}
		);
	}else{
		std::sort(searchResult.begin(), searchResult.end(),
			[](struct result const &t1, struct result const &t2) {
					return t1.recordScore > t2.recordScore;
				}
		);
	}
}

void Result::reset() {
	searchResult.clear();
}