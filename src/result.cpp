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

void Result::insertResult(std::string title, int searchScore) {
	struct result foundResult;
	foundResult.recordTitle = title;
	foundResult.recordScore = searchScore;

	searchResult.push_back(foundResult);

}

void Result::sortResult(){
	sort(searchResult.begin(), searchResult.end(), sortScore);
}