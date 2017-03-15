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
	int fileCount;
	std::string inputPath;
	std::vector<std::string> rawfiles;
	std::vector<std::string> tagFiles;

	void searchId(char *id, std::string pattern, int &searchScore, int &searchMatchCount,bool caseInsensitive = 0, unsigned int editDistance = 0);
	void searchTitle(char *title, std::string pattern, int &searchScore, int &searchMatchCount, bool caseInsensitive = 0, unsigned int editDistance = 0);
	void searchContent(char *content, std::vector <std::string> &searchPatterns, int recordIndex, int &searchScore, int &searchMatchCount, bool caseInsensitive = 0, unsigned int editDistance = 0);
	char * searchFactory(char *text, std::string pattern, bool caseInsensitive = 0, unsigned int editDistance = 0);
	void buildRecord();
	void readFileThenSetRecordAndRank();
	void checkPathAndSetFileVectors();

public:
	Record(std::string path);
	~Record();

	std::vector <std::tuple <std::string, int>> searchAndSortWithRank(std:: string pattern, bool caseInsensitive = 0, unsigned int editDistance = 0);

};

#endif