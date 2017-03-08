#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include "load_data.h"
#include "search.h"



using namespace std;

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

void WeightedTree::insert(char tag, int left, int right){
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
};

Ranking::~Ranking(){};
Ranking::Ranking(std::string tagFilePath){
		pathToTagFile = tagFilePath;
		root = NULL;
};

void Ranking::buildRank(){
	// std::cout << pathToTagFile << std::endl;
	std::ifstream infile(pathToTagFile);
	std::string tagName;
	int left, right;
	while(infile >> tagName >> left >> right){
		// std::cout << tagName <<"\t"<<left<<"\t" << right << std::endl;
		root->insert(tagName.at(0), left, right);
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
		//rank[dataCount-1]  = new Ranking(tagFiles[i]);
		//rank[dataCount-1]->buildRank();
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

/* OLD METHOD


int loadData(string path, struct record *&data) {
	FILE *fptr;
	char *line = NULL;
	char prefix[5];
	size_t len = 0;
	ssize_t read;
	int dataCount = 0, fileCount = 0;
	DIR *dir;
	struct dirent *ent;
	std::vector<string> files;
	std::vector<string> tagFiles;

	struct record *moreData = NULL;
	if (exists(path)){
		if(isDir(path)){
			if ((dir = opendir(path.c_str())) != NULL) {
				if (path[path.length()-1] != '/')
					path = path+"/";
				while ((ent = readdir(dir)) != NULL) {
					string newFilePath = path+ent->d_name;
					if (!isDir(newFilePath)) {
						files.push_back(newFilePath);
						tagFiles.push_back(newFilePath.substr(0, newFilePath.size()-3)+"info");
						fileCount++;
					}
				}
				closedir(dir);
			}
		}
		else {
			files.push_back(path);
			tagFiles.push_back(path.substr(0, path.size()-3)+"info");
			fileCount++;
		}
	}else {
		cout << "File doesn't exists !" << endl;
		exit(1);
	}
	cout << tagFiles[0] << endl;

	// NOTE: A better way to handle format inconsistency
	for (int i=0; i<fileCount; i++){
		fptr = fopen(files[i].c_str(), "r");
		while((read = getline(&line, &len, fptr)) != -1){ //char **restrict lineptr
			memcpy( prefix, line, 4);
			prefix[4] = '\0';
			if(strcmp(prefix, "@id:")==0){
					dataCount++;
					moreData = (struct record *) realloc(data, dataCount*sizeof(struct record));
					if (moreData != NULL){
						data = moreData;
						// data[dataCount-1].score = 0;
						data[dataCount-1].id = (char *) malloc(read-4);
						strcpy(data[dataCount-1].id, (line+4));
						// cout << (line+4);
						// printf("line: %s\n", line);
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
					cout << "File " +files[i]+ " did not obeyed input format" << endl;
					break;
			}
		}
		fclose(fptr);
		//Ranking recordRank(tagFiles[i]);
		//recordRank.buildRank();
		Ranking *recordRank  = new Ranking(tagFiles[i]);
		recordRank->buildRank();
	}

	return dataCount;
}

int loadData2(string path, struct record **data) {
	FILE *fptr;
	char *line = NULL;
	char prefix[5];
	size_t len = 0;
	ssize_t read;
	int dataCount = 0, fileCount = 0;
	DIR *dir;
	struct dirent *ent;
	std::vector<string> files;
	struct record *moreData = NULL;

	if (exists(path)){
		if(isDir(path)){
			if ((dir = opendir(path.c_str())) != NULL) {
				if (path[path.length()-1] != '/')
					path = path+"/";
				while ((ent = readdir(dir)) != NULL) {
					string newFilePath = path+ent->d_name;
					if (!isDir(newFilePath)) {
						files.push_back(newFilePath);
						fileCount++;
					}
				}
				closedir(dir);
			}
		}
		else {
			files.push_back(path);
			fileCount++;
		}
	}else {
		cout << "File doesn't exists !" << endl;
		exit(1);
	}

	// NOTE: A better way to handle format inconsistency
	for (int i=0; i<fileCount; i++){
		fptr = fopen(files[i].c_str(), "r");
		while((read = getline(&line, &len, fptr)) != -1){ //char **restrict lineptr
			memcpy( prefix, line, 4);
			prefix[4] = '\0';
			if(strcmp(prefix, "@id:")==0){
					dataCount++;
					moreData = (struct record *) realloc(*data, dataCount*sizeof(struct record));
					if (moreData != NULL){
						*data = moreData;
						// (*data+dataCount-1)->score = 0;
						(*data+dataCount-1)->id = (char *) malloc(read-4);
						strcpy((*data+dataCount-1)->id, (line+4));
					}else {
						free(*data);
						puts ("Error (re)allocating memory");
	       		exit (1);
					}
			}else if (strcmp(prefix, "@tit")==0){
					(*data+dataCount-1)->title = (char *) malloc(read-7);
					strcpy((*data+dataCount-1)->title, (line+7));
			}else if (strcmp(prefix, "@con")==0){
					(*data+dataCount-1)->content = (char *) malloc(read-9);
					strcpy((*data+dataCount-1)->content, (line+9));
			}else {
					cout << "File " +files[i]+ " did not obeyed input format" << endl;
					break;
			}
		}
		fclose(fptr);
	}

	return dataCount;
}
*/