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
#include "record.h"
#include "ranking.h"
#include "utils.h"

using namespace std;

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
	char *text = NULL;
	char *found = NULL;

	for (int i=0; i < fileCount; i++){
		int foundLocation, score = 0, count_time = 0;
		bool foundIdFlag = false, foundTitleFlag = false, foundContentFlag = false;
		text = data[i].content;

		if((found = strstr(data[i].id, pattern.c_str()))>0){
			score += 0;
			foundIdFlag = true;
			//std::cout << "Book :" << data[i].title;
			//std::cout << "Found at title, location: " << found - data[i].id << std::endl;
		}
		if((found = strstr(data[i].title, pattern.c_str()))>0){
			score += 100;
			foundIdFlag = false;
			//std::cout << "Book :" << data[i].title;
			//std::cout << "Found at id, location: " << found - data[i].title << std::endl;
		}
		while((found = strstr(text, pattern.c_str()))>0){
			foundContentFlag = true;
			foundLocation = found - data[i].content;
			score += rank[i].getRankingScore(foundLocation);
			text = found + pattern.length();
			count_time ++;
		}
		if (foundIdFlag || foundTitleFlag || foundContentFlag){
			std::cout << "Book :" << data[i].title << "Count time:" << count_time << std::endl;
			std::cout << "Ranking score: " << score << std::endl;
		}
		text = found = NULL;
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
						if (newFilePath.substr(newFilePath.length() - 4) == ".txt"){
							rawfiles.push_back(newFilePath);
							//tagFiles.push_back(newFilePath.substr(0, newFilePath.size()-3)+"info");
							tagFiles.push_back(newFilePath.substr(0, newFilePath.size()-3)+"tags");
							fileCount++;
						}
					}
				}
				closedir(dir);
			}
		}
		else {
			rawfiles.push_back(inputPath);
			//tagFiles.push_back(inputPath.substr(0, inputPath.size()-3)+"info");
			tagFiles.push_back(inputPath.substr(0, inputPath.size()-3)+"tags");
			fileCount++;
		}
	}else {
		std::cout << "File doesn't exists !" << std::endl;
		exit(1);
	}
}



