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
#include "search.h"
#include "utils.h"

using namespace std;

Record::~Record(){};
Record::Record(std::string path){
	inputPath = path;
	fileCount = 0;
	buildRecord();
};

//TODO: caseInsensitive not implemented yet
char * Record::searchFactory(char *text, std::string pattern, bool caseInsensitive, unsigned int editDistance) {
	if (editDistance == 0){
		return strstr(text, pattern.c_str());
	}else {
		return fuzzySearch(text, pattern.c_str(), editDistance);
	}
}

void Record::searchId(char *id, std::string pattern, int &searchScore, int &searchMatchCount, 
											bool caseInsensitive, unsigned int editDistance){
	if((searchFactory(id, pattern.c_str(), caseInsensitive, editDistance))>0){
				searchScore += 0;
				searchMatchCount++;
	}
}

void Record::searchTitle(char *title, std::string pattern, int &searchScore, int &searchMatchCount,
												bool caseInsensitive, unsigned int editDistance){
	if((searchFactory(title, pattern.c_str(), caseInsensitive, editDistance))!=NULL){
				searchScore += 300000;
				searchMatchCount++;
	}
}

void Record::searchContent(char *content, std::vector <std::string> &searchPatterns, int recordIndex, int &searchScore, int &searchMatchCount,
													bool caseInsensitive, unsigned int editDistance){
	char *text, *found;
	int foundLocation;
	// NOTE: array of size searchPatterns.size();
	std::vector <std::tuple <int, int, int>> foundTuple;
	std::vector <std::vector <std::tuple <int,int,int>>> patternLocationTuples;

	for (auto searchPattern: searchPatterns){
		text = content;
		found = NULL;
		while((found = searchFactory(text, searchPattern.c_str(), caseInsensitive, editDistance))!=NULL){
			foundLocation = found - content;
			foundTuple.push_back(rank[recordIndex].getRankTreeTuple(foundLocation));
			//searchScore += rank[recordIndex].getRankingScore(foundLocation);
			text = found + searchPattern.length();
			searchMatchCount++;
		}
		patternLocationTuples.push_back(foundTuple);
		foundTuple.clear();
	}
	
	searchScore += rank[recordIndex].getAdvancedRankingScore(patternLocationTuples);

}

std::vector <std::tuple <std::string, int>> Record::searchAndSortWithRank(std::string pattern,bool caseInsensitive, unsigned int editDistance){
	int searchScore, searchMatchCount;
	std::vector <std::tuple <std::string, int>> result;
	std::vector <std::string> searchPatterns = parseSearchQuery(pattern);
	for (int i=0; i < fileCount; i++){
			searchScore = searchMatchCount = 0;
			for (auto searchPattern: searchPatterns){
				searchId(data[i].id, searchPattern, searchScore, searchMatchCount, caseInsensitive, editDistance);
				searchTitle(data[i].title, searchPattern, searchScore, searchMatchCount, caseInsensitive, editDistance);
			}
			searchContent(data[i].content, searchPatterns, i, searchScore, searchMatchCount, caseInsensitive, editDistance);

			if (searchMatchCount > 0 && searchScore > 0){
					std::string bookTitle(data[i].title);
					result.push_back(std::make_tuple(bookTitle, searchScore));
			}
	}
	std::sort(result.begin(), result.end(),
		[](std::tuple<std::string, int> const &t1, tuple<std::string, int> const &t2) {
				return std::get<1>(t1) > std::get<1>(t2);
			}
	);

	return result;
}

void Record::buildRecord(){
	checkPathAndSetFileVectors();
	readFileThenSetRecordAndRank();
};


// TODO: remove last character:newline
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
			cout << flush;
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



