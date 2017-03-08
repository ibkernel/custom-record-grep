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
#include "load_data.h"

using namespace std;

/*
void WeightedTreeNode::insert(char tag, int left, int right){
	switch (tag){
		case 'c':
			std::cout << "You got c: " << left << "," << right << std::endl;
			break;
		case 't':
			std::cout << "You got t: " << left << "," << right << std::endl;
			break;
		case 'p':
			std::cout << "You got p: " << left << "," << right << std::endl;
			break;
		case 's':
			std::cout << "You got s: " << left << "," << right << std::endl;
			break;
		default:
			std::cout << "error tag" << std::endl;
			break;
	}
};*/

Ranking::~Ranking(){};
Ranking::Ranking(std::string tagFilePath){
		pathToTagFile = tagFilePath;
		root = NULL;
		chapter_num = chapter_size  = paragraph_size = 0;
		paragraph_num = 1;
		buildRank();
};

void Ranking::insertTag(char tagType, int lowerBound, int upperBound){
	if (root == NULL){
		root = (struct node*) malloc(sizeof(struct node) * 1);
		root->weight = 0;
		root->left = (int *) malloc(sizeof(int) * 1);
		root->chapterNodes = (struct node **)malloc(sizeof(struct node *) * 1);
		root->left[0] = lowerBound;
		root->chapterNodes[0] = (struct node *)malloc(sizeof(struct node));
		cout << "root->chapterNodes[0]:" <<root->chapterNodes[chapter_num] << endl;
		chapter_num += 1;
	}else {
		struct node **moreData = NULL;
		int*	evenMoreData = NULL;

		switch (tagType){
			case 'c':
				chapter_size = 0;
				cout << "c: " << chapter_num << endl;
				moreData = (struct node **) realloc(root->chapterNodes, sizeof(struct node *)*(chapter_num+1));
				if (moreData!= NULL){
					root->chapterNodes = moreData;
					root->chapterNodes[chapter_num] = (struct node *)malloc(sizeof(struct node) * 1);
					evenMoreData = (int *) realloc(root->left, sizeof(int)*(chapter_num+1));
					root->left = evenMoreData;
					root->left[chapter_num] = lowerBound;

					cout << "chapter_num:" <<chapter_num << endl;
					cout << "root->chapterNodes[chapter_num]:" <<root->chapterNodes[chapter_num] << endl;
					cout << "root->left:" <<root->left[chapter_num] << endl;

					std::cout << "You got c: " << lowerBound << "," << upperBound << std::endl;

					//root->chapterNodes[chapter_num]->chapterNodes = (struct node **) malloc(sizeof(struct node));
				}

				chapter_num += 1;
				break;
			case 't':
				cout << "t: " << chapter_num << endl;
				//moreData = (struct node **)realloc( root->chapterNodes[chapter_num-1]->chapterNodes ,sizeof(struct node *) *1);
				//root->chapterNodes[(chapter_num-1)]->chapterNodes = moreData;
				// root->chapterNodes[chapter_num-1]->chapterNodes[0] = (struct node *)malloc(sizeof(struct node));
				//root->chapterNodes[chapter_num-1]->chapterNodes[0]->weight = 10; // TITLE WEIGHT
				// root->chapterNodes[chapter_num-1]->chapterNodes[0]->left = (int *)malloc(sizeof(int)*1);
				// root->chapterNodes[chapter_num-1]->chapterNodes[0]->left[0] = lowerBound;
				root->chapterNodes[chapter_num-1]->left = (int *)malloc(sizeof(int)*1);
				root->chapterNodes[chapter_num-1]->left[0] = lowerBound;
				root->chapterNodes[chapter_num-1]->chapterNodes = (struct node **)malloc(sizeof(struct node*)*1);
				root->chapterNodes[chapter_num-1]->chapterNodes[0] = (struct node *)malloc(sizeof(struct node));
				root->chapterNodes[chapter_num-1]->chapterNodes[0]->weight = 10;
				std::cout << "You got t: " << lowerBound << "," << upperBound << std::endl;
				chapter_size += 1;
				break;
			case 'p':
				moreData = (struct node **) realloc(root->chapterNodes[chapter_num-1]->chapterNodes, sizeof(struct node *)*(chapter_size+1));
				if(moreData != NULL){
					root->chapterNodes[chapter_num-1]->chapterNodes = moreData;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size] = (struct node *)malloc(sizeof(struct node));
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->weight = 3;
					evenMoreData = (int *)realloc(root->chapterNodes[chapter_num-1]->left, sizeof(int)*(chapter_size+1));
					root->chapterNodes[chapter_num-1]->left = evenMoreData;
					root->chapterNodes[chapter_num-1]->left[chapter_size] = lowerBound;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->chapterNodes = (struct node**)malloc(sizeof(struct node*) *1);
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->left = (int *)malloc(sizeof(int)*1);
					//root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->left = evenMoreData;
					//root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size]->left[0] = lowerBound;
				}else {
					puts("Error reallocating memory");
					exit(1);
				}
				chapter_size += 1;
				paragraph_num += 1;
				paragraph_size = 1;
				//std::cout << "You got p: " << lowerBound << "," << upperBound << std::endl;
				break;
			case 's':
				moreData = (struct node **) realloc(root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes, sizeof(struct node *)*(paragraph_size));
				if (moreData != NULL){
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes = moreData;
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes[paragraph_size-1] = (struct node*)malloc(sizeof(struct node)* 1);
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->chapterNodes[paragraph_size-1]->weight = 1;
					evenMoreData = (int *) realloc(root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->left, sizeof(int)*(paragraph_size));
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->left = evenMoreData;
						//root->chapterNodes[chapter_num-1]->chapterNodes[paragraph_num-1]->left[0] = (int *)malloc(sizeof(int));
					root->chapterNodes[chapter_num-1]->chapterNodes[chapter_size-1]->left[paragraph_size] = lowerBound;
				}else {
					puts("Error reallocating memory");
					exit(1);
				}
				paragraph_size += 1;
				//std::cout << "You got s: " << lowerBound << "," << upperBound << std::endl;
				break;
			default:
				std::cout << "error tag" << std::endl;
				break;
		}
	}
};

void Ranking::buildRank(){
	// std::cout << pathToTagFile << std::endl;
	std::ifstream infile(pathToTagFile);
	std::string tagName;
	int left, right;
	while(infile >> tagName >> left >> right){
		// std::cout << tagName <<"\t"<<left<<"\t" << right << std::endl;
		insertTag(tagName.at(0), left, right);
	}
};

Record::~Record(){};
Record::Record(std::string path){
	inputPath = path;
	fileCount = 0;
	buildRecord();
};


void Record::printRecord(){
	// std::cout << "heelo\n" ;
	for(int i=0; i< fileCount; i++){
		// std::cout << data[i].id << std::endl;
		// std::cout << data[i].title << std::endl;
		std::cout << data[i].content << std::endl;
	}
};

void Record::search(std::string pattern) {
	char *found;
	for (int i=0; i < fileCount; i++){
		if((found = strstr(data[i].id, pattern.c_str()))>0){
			std::cout << "Found at title, location: " << found - data[i].id << std::endl;
		}
		if((found = strstr(data[i].title, pattern.c_str()))>0){
			std::cout << "Found at id, location: " << found - data[i].title << std::endl;
		}
		if((found = strstr(data[i].content, pattern.c_str()))>0){
			std::cout << "Found at content, location: " << found - data[i].content << std::endl;
		}
	}
};

void Record::buildRecord(){
	checkPathAndSetFileVectors();
	readFileThenSetRecordAndRank();
};

void Record::readFileThenSetRecordAndRank(){
	FILE *fptr;
	char *line = NULL;
	char prefix[5];
	size_t len = 0;
	ssize_t read;
	int dataCount = 0;
	struct record *moreData = NULL;
	data = (struct record*) malloc(sizeof(struct record));

	for (int i=0; i<fileCount; i++){
		fptr = fopen(rawfiles[i].c_str(), "r");
		while((read = getline(&line, &len, fptr)) != -1){ //char **restrict lineptr
			memcpy( prefix, line, 4);
			prefix[4] = '\0';
			if(strcmp(prefix, "@id:")==0){
					dataCount++;
					moreData = (struct record *) realloc(data, dataCount*sizeof(struct record));
					if (moreData != NULL){
						data = moreData;
						data[dataCount-1].id = (char *) malloc(read-4);
						strcpy(data[dataCount-1].id, (line+4));
					}else {
						free(data);
						puts ("Error (re)allocating memory");
	       		exit (1);
					}
			}else if (strcmp(prefix, "@tit")==0){
					data[dataCount-1].title = (char *) malloc(read-7);
					strcpy(data[dataCount-1].title, (line+7));
			}else if (strcmp(prefix, "@con")==0){
					data[dataCount-1].content = (char *) malloc(read-9);
					strcpy(data[dataCount-1].content, (line+9));
			}else {
					std::cout << "File " +rawfiles[i]+ " did not obeyed input format" << std::endl;
					break;
			}
		}
		fclose(fptr);
		Ranking currentRank(tagFiles[i]);
		rank.push_back(currentRank);
	}
}

void Record::checkPathAndSetFileVectors(){
	DIR *dir;
	struct dirent *ent;
	if (exists(inputPath)){
		if(isDir(inputPath)){
			if ((dir = opendir(inputPath.c_str())) != NULL) {
				if (inputPath[inputPath.length()-1] != '/')
					inputPath = inputPath+"/";
				while ((ent = readdir(dir)) != NULL) {
					std::string newFilePath = inputPath+ent->d_name;
					if (!isDir(newFilePath)) {
						rawfiles.push_back(newFilePath);
						tagFiles.push_back(newFilePath.substr(0, newFilePath.size()-3)+"info");
						fileCount++;
					}
				}
				closedir(dir);
			}
		}
		else {
			rawfiles.push_back(inputPath);
			tagFiles.push_back(inputPath.substr(0, inputPath.size()-3)+"info");
			fileCount++;
		}
	}else {
		std::cout << "File doesn't exists !" << std::endl;
		exit(1);
	}
}


inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int isDir(const std::string &name) {
	struct stat buffer;
	stat(name.c_str(), &buffer);
	if ( !S_ISDIR(buffer.st_mode) )
	{
		return 0;
	}
	return 1;
}

