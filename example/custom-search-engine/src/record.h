#ifndef CRGREP_RECORD_H_
#define CRGREP_RECORD_H_
#include "ranking.h"
#include "result.h"

struct record {
	char *language;
	char *id;
	char *title;
	char *content;
	int approxCharactersCount;
};

class Record {
private:
	struct record* data;
	std::vector<Ranking> rank;
	int fileCount;
	int dataCount;
	std::string inputPath;
	std::vector<std::string> rawfiles;
	std::vector<std::string> tagFiles;

	void searchId(char *id, char *recordLanguage, std::vector <std::tuple<std::string, bool, bool>> &patterns, int &searchScore, int &searchMatchCount, bool &isComplianceToMustAndMustNotHave,bool caseInsensitive = 0, unsigned int editDistance = 0);
	void searchTitle(char *title, char *recordLanguage, std::vector <std::tuple<std::string, bool, bool>> &patterns, int &searchScore, int &searchMatchCount, bool &isComplianceToMustAndMustNotHave, bool caseInsensitive = 0, unsigned int editDistance = 0);
	void searchContent(char *content, char *recordLanguage, std::vector <std::tuple<std::string, bool, bool>> &searchPatterns, int recordIndex, int &searchScore, int &searchMatchCount, bool &isComplianceToMustAndMustNotHave, bool caseInsensitive = 0, unsigned int editDistance = 0);
	char * searchFactory(char *text, char *recordLanguage, std::string pattern, bool caseInsensitive = 0, unsigned int editDistance = 0);
	void buildRecord();
	void readFileThenSetRecordAndRank();
	void checkPathAndSetFileVectors();
	void detectLanguage(const char* src, char *&recordLanguage);
	void insertAllRanksForCurrentFile(std::string &tagPath, int dataCountForCurrentFile);
	void createAndAssignDefaultStructData();
	int setPrefixAndReturnOffset(std::string &prefix, bool &isPrefixToolong, char* &line);
	void createMemoryThenInsert(char *&target, char *&source, int offset,  size_t &size);
	void incrementLocalFileDataCountAndDataCount(int &currentFileDataCount);
	void handlePrefixCases(int &dataCountForCurrentFile, size_t &read, char *&line, bool &isNewRecord);
	void handleMalformedCases(std::string malformType, int &dataCountForCurrentFile, bool &isNewRecord);
	int getRecordCharactersCount(size_t lineCharCount, int prefixCount, char *& line, char *language);
	void scoring(bool &isComplianceToMustAndMustNotHave,bool found, bool mustHave, bool mustNotHave);


public:
	Record(std::string path);
	~Record();
	
	void dubugPrintAllRecords() {
		for (int i=0; i < dataCount; i++){
			std::cout << "ID: " << data[i].id << std::endl;
			std::cout << "Title: " << data[i].title << std::endl;
			//std::cout << "Content: " << data[i].content << std::endl;
			std::cout << "Language: " << data[i].language << std::endl;
			std::cout << "Char count: " << data[i].approxCharactersCount << std::endl;
			std::cout << "--------------" << std::endl;
		}
	};

	int getRecordCount();
	int getFileCount();
	void searchAndSortWithRank(std::vector <std::string> queries, Result &searchResult, bool caseInsensitive = 0, unsigned int editDistance = 0);

};

#endif