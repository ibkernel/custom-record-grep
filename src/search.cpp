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

void testingLocations(string pattern, struct record *&data, int dataCount, vector<struct resultFormat> &result){
	char *found;
	for (int i=0; i < dataCount; i++){
		if((found = strstr(data[i].id, pattern.c_str()))>0){
			cout << "Found at title, location: " << found - data[i].id << endl;
		}
		if((found = strstr(data[i].title, pattern.c_str()))>0){
			cout << "Found at id, location: " << found - data[i].title << endl;
		}
		if((found = strstr(data[i].content, pattern.c_str()))>0){
			cout << "Found at content, location: " << found - data[i].content << endl;
		}
	}

	return;
}