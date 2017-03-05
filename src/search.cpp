#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "search.h"
#include "load_data.h"

using namespace std;

int search(string pattern, struct record *&data, int dataCount, vector<struct resultFormat> &result){
	struct resultFormat currentData;
	int currentScore = 0;
	for (int i=0; i < dataCount; i++) {
		if(strstr(data[i].id, pattern.c_str())>0){
			currentScore += 1;
		}
		if(strstr(data[i].title, pattern.c_str())>0){
			currentScore += 10;
		}
		if(strstr(data[i].content, pattern.c_str())>0){
			currentScore += 3;
		}
		//TODO: store current data into a self-managed-order data structure
		if (currentScore > 0){
				currentData.id = i;
				currentData.score = currentScore;
				result.push_back(currentData);
				currentScore = 0;
		}
	}
	return result.size();
}