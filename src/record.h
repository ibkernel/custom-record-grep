#ifndef CRGREP_RECORD_H_
#define CRGREP_RECORD_H_
#include "ranking.h"

struct record {
	char *id;
	char *title;
	char *content;
};

class Record {
private:
	struct record* data;
	std::vector<Ranking> rank;
	//Ranking** rank;
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

	void search(std::string pattern);
	std::vector <std::tuple <std::string, int>> searchAndSortWithRank(std:: string pattern);

};

#endif