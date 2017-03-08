#ifndef CRGREP_LOAD_H_
#define CRGREP_LOAD_H_
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
#include <fstream>
struct record {
	char *id;
	char *title;
	char *content;
};

class WeightedTree {
private:
	WeightedTree **chapterNodes;
	int **left;
	int weight;

public:
	WeightedTree();
	~WeightedTree();
	void insert(char tag, int left, int right);
	int search(int foundLocation); // return score
};




class Ranking {
private:
	WeightedTree	*root;
	std::string pathToTagFile;

public:
	Ranking(std::string tagFilePath);
	~Ranking();

	void buildRank();

};

class RawRecord {
	char *id;
	char *title;
	char *content;
};

class Record {
private:
	struct record* data;
	//Ranking* rank;
	int fileCount;
	std::string inputPath;
	std::vector<std::string> rawfiles;
	std::vector<std::string> tagFiles;

	void buildRecord();
	void readFileThenSetRecordAndRank();
	void checkPathAndSetFileVectors();

public:
	Record(std::string path);
	~Record();

	void printRecord();
	void search(std::string pattern);

};


inline bool exists(const std::string& name);
int isDir(const std::string &name);


int loadData(std::string path, struct record *&data);
int loadData2(std::string path, struct record **data);

#endif