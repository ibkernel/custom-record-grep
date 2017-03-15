#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <map>
#include <cmath>
#include "utils.h"
#include "ranking.h"

using namespace std;

Ranking::~Ranking(){};
Ranking::Ranking(std::string tagFilePath){
		pathToTagFile = tagFilePath;
		root = NULL;
		chapter_num = chapter_size  = paragraph_size = 0;
		paragraph_num = 1;
		buildRank();
};

// TODO: change score to double?
// For multi-pattern searching
int Ranking::getAdvancedRankingScore(std::vector <std::vector <std::tuple <int,int,int>>>  &patternLocationTuples){
	int score = 0, patternNum = 0;
	std::map<std::string, int> foundMap;
	for (auto patternLocations: patternLocationTuples){
		for (auto singleLocation: patternLocations){
				score += getPatternScore(foundMap, singleLocation,patternNum);
		}
		patternNum += 1;
	}
	foundMap.clear();
	return score;
}

int Ranking::getPatternScore(std::map<std::string, int> &foundMap, std::tuple<int,int,int>&singleLocation, int patternNum){
	const int weight = 5;
	std::map<std::string, int>::iterator it;
	std::string chapterIndex = std::to_string(std::get<0>(singleLocation));
	std::string paragraphIndex = std::to_string(std::get<1>(singleLocation));
	std::string sentenseIndex = std::to_string(std::get<2>(singleLocation));

	it = foundMap.find(chapterIndex);
	if (it != foundMap.end()){
		foundMap[chapterIndex] += pow(weight, patternNum);
	}else{
		foundMap.insert(std::pair<std::string, int>(chapterIndex, 1));
	}

	it = foundMap.find(chapterIndex+'_'+paragraphIndex);
	if (it != foundMap.end()){
		foundMap[chapterIndex+'_'+paragraphIndex] += pow(weight, patternNum);
	}else{
		foundMap.insert(std::pair<std::string, int>(chapterIndex+'_'+paragraphIndex, 1));
	}

	it = foundMap.find(chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex);
	if (it != foundMap.end()){
		foundMap[chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex] += pow(weight, patternNum);
	}else{
		foundMap.insert(std::pair<std::string, int>(chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex, 1));
	}
	int score = foundMap[chapterIndex]*foundMap[chapterIndex+'_'+paragraphIndex]*foundMap[chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex];

	if (paragraphIndex == "0") // title
		return score * 10;
	else
		return score * 3;
}


int Ranking::getRankingScore(int foundLocation){
	int score = 0;
	int chapter_array_index = getBelongingInterval(root->left, root->count, foundLocation);
	score += root->chapterNodes[chapter_array_index]->weight;
	int paragraph_array_index = getBelongingInterval(root->chapterNodes[chapter_array_index]->left,
	 root->chapterNodes[chapter_array_index]->count, foundLocation);
	score += root->chapterNodes[chapter_array_index]->chapterNodes[paragraph_array_index]->weight;
	if(paragraph_array_index == 0){
		// In title no need to find 's' tag
		return score;
	}
	else{
		int sentence_array_index = getBelongingInterval(
			root->chapterNodes[chapter_array_index]->chapterNodes[paragraph_array_index]->left,
			 root->chapterNodes[chapter_array_index]->chapterNodes[paragraph_array_index]->count,
			  foundLocation);
		if (sentence_array_index == -1){ // NOTE: It should never happen, however we need more testing to verify
			std::cout << "Weird things happen at: " << chapter_array_index << ":" <<  paragraph_array_index << ":" << sentence_array_index << std::endl;
			return 0;
		}
		score += root->chapterNodes[chapter_array_index]->chapterNodes[paragraph_array_index]->chapterNodes[sentence_array_index]->weight;
	}
	return score; // not found, not supposed to go here
};

std::tuple <int, int, int> Ranking::getRankTreeTuple(int foundLocation){
	std::tuple <int, int, int> location;
	int chapter_array_index = getBelongingInterval(root->left, root->count, foundLocation);
	int paragraph_array_index = getBelongingInterval(root->chapterNodes[chapter_array_index]->left,
	 root->chapterNodes[chapter_array_index]->count, foundLocation);
	if (paragraph_array_index == 0){
		return std::make_tuple(chapter_array_index, paragraph_array_index, 0);
	}
	int sentence_array_index = getBelongingInterval(
			root->chapterNodes[chapter_array_index]->chapterNodes[paragraph_array_index]->left,
			 root->chapterNodes[chapter_array_index]->chapterNodes[paragraph_array_index]->count,
			  foundLocation);
	return std::make_tuple(chapter_array_index, paragraph_array_index, sentence_array_index);
}

int Ranking::getBelongingInterval(int *&lowerBound, int arrayLength, int foundLocation) {
	int leftLength = arrayLength;
	int mid = leftLength/2, left =0, right = leftLength - 1;
	int minDiff = lowerBound[leftLength-1], curDiff = 0, inMid = 0;
	if (arrayLength == 0) // NOTE: It should never happen, however we need more testing to verify
		return -1;
	if (foundLocation > lowerBound[leftLength-1])
		return leftLength-1;
	if (foundLocation < lowerBound[0])
		return 0;

	while(left <= right){
		if (lowerBound[mid] == foundLocation)
			return mid;
		if (lowerBound[mid] > foundLocation){
			curDiff = lowerBound[mid] - foundLocation;
			if (curDiff < minDiff){
				inMid = mid;
				minDiff = curDiff;
			}
			right = mid -1;
			mid = (left+right)/2;
		}else {
			curDiff = foundLocation - lowerBound[mid];
			if (curDiff < minDiff){
				inMid = mid;
				minDiff = curDiff;
			}
			left = mid + 1;
			mid = (left+right)/2;
		}
	}

	return mid;
}


void Ranking::insertTag(char tagType, int lowerBound, int upperBound){
	if (root == NULL){
		root = (struct node*) malloc(sizeof(struct node) * 1);
		root->weight = 0;
		root->left = (int *) malloc(sizeof(int) * 1);
		root->chapterNodes = (struct node **)malloc(sizeof(struct node *) * 1);
		root->left[0] = lowerBound;
		root->chapterNodes[0] = (struct node *)malloc(sizeof(struct node));
		root->count = 1;
		chapter_num += 1;
	}else {
		struct node **moreData = NULL;
		int*	evenMoreData = NULL;

		switch (tagType){
			case 'c':
				//NOTE: WEIRD
				//cout << "p & l count: " <<  (sizeof(root->chapterNodes[chapter_num-1]->left)/sizeof(int)) << endl;
				chapter_size = 0;
				moreData = (struct node **) realloc(root->chapterNodes, sizeof(struct node *)*(chapter_num+1));
				evenMoreData = (int *) realloc(root->left, sizeof(int)*(chapter_num+1));
				if (moreData!= NULL && evenMoreData != NULL){
					root->chapterNodes = moreData;
					root->chapterNodes[chapter_num] = (struct node *)malloc(sizeof(struct node) * 1);
					root->left = evenMoreData;
					root->left[chapter_num] = lowerBound;
					root->chapterNodes[chapter_num]->weight = 0;
					root->count += 1;
				}
				chapter_num += 1;
				break;
			case 't':
				root->chapterNodes[chapter_num-1]->count = 0;
				root->chapterNodes[chapter_num-1]->count += 1;
				root->chapterNodes[chapter_num-1]->left = (int *)malloc(sizeof(int)*1);
				root->chapterNodes[chapter_num-1]->left[0] = lowerBound;
				root->chapterNodes[chapter_num-1]->chapterNodes = (struct node **)malloc(sizeof(struct node*)*1);
				root->chapterNodes[chapter_num-1]->chapterNodes[0] = (struct node *)malloc(sizeof(struct node));
				root->chapterNodes[chapter_num-1]->chapterNodes[0]->weight = 10;
				root->chapterNodes[chapter_num-1]->chapterNodes[0]->count = 0;
				chapter_size += 1;
				break;
			case 'p':
				if(!root->chapterNodes[chapter_num-1]->count)
					root->chapterNodes[chapter_num-1]->count = 1;
				root->chapterNodes[chapter_num-1]->count += 1;
				moreData = (struct node **) realloc(root->chapterNodes[chapter_num-1]->chapterNodes, sizeof(struct node *)*(chapter_size+1));
				evenMoreData = (int *)realloc(root->chapterNodes[chapter_num-1]->left, sizeof(int)*(chapter_size+1));
				if(moreData != NULL && evenMoreData != NULL){
					root->chapterNodes[chapter_num-1]->chapterNodes = moreData;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size] = (struct node *)malloc(sizeof(struct node));
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->weight = 3;
					root->chapterNodes[chapter_num-1]->left = evenMoreData;
					root->chapterNodes[chapter_num-1]->left[chapter_size] = lowerBound;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->chapterNodes = (struct node**)malloc(sizeof(struct node*) *1);
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->left = (int *)malloc(sizeof(int)*1);
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->count = 0;
				}else {
					puts("Error reallocating memory");
					exit(1);
				}
				chapter_size += 1;
				paragraph_num += 1;
				paragraph_size = 1;
				break;
			case 's':
				root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->count += 1;
				moreData = (struct node **) realloc(root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes, sizeof(struct node *)*(paragraph_size));
				evenMoreData = (int *) realloc(root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->left, sizeof(int)*(paragraph_size));
				if (moreData != NULL && evenMoreData != NULL){
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes = moreData;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes[paragraph_size-1] = (struct node*)malloc(sizeof(struct node)* 1);
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes[paragraph_size-1]->weight = 1;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->left = evenMoreData;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->left[paragraph_size-1] = lowerBound;
				}else {
					puts("Error reallocating memory");
					exit(1);
				}
				paragraph_size += 1;
				break;
			default:
				std::cout << "error tag" << std::endl;
				break;
		}
	}
};

// For testing
void Ranking::printTag() {
	cout << "Total chapter tags: " << root->count << endl;
	for (int i=0; i < root->count; i++){
		cout << "Current chapter: " << root->count << "\tTotal paragraph tags:" << root->chapterNodes[i]->count << endl;
		for (int j=0; j < root->chapterNodes[i]->count; j++)
			cout << "Total sentense tags:" << root->chapterNodes[i]->chapterNodes[j]->count << endl;
	}
}

void Ranking::buildRank(){
	// std::cout << pathToTagFile << std::endl;
	std::ifstream infile(pathToTagFile);
	std::string tagName;
	int left, right;
	while(infile >> tagName >> left >> right){
		insertTag(tagName.at(0), left, right);
	}
	//cout << "Current book: " << pathToTagFile << endl;
	//printTag();
};
