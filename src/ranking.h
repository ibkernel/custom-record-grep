#ifndef CRGREP_RANKING_H_
#define CRGREP_RANKING_H_
#include <map>

struct node {
	int *left;
	int count;
	struct node **chapterNodes;
};

class Ranking {
private:
	struct node	*root;
	std::string pathToTagFile;
	int chapter_num;
	int chapter_size;
	int paragraph_num;
	int paragraph_size;

	void buildRank();
	void insertTag(char tagType, int lowerBound, int upperBound);
	int getBelongingInterval(int *&left, int arrayLength, int foundLocation);
	int getPatternScore(std::map<std::string, int> &foundMap, std::tuple<int,int,int> &singleLocation, int patternNum);
public:
	std::tuple <int, int, int> getRankTreeTuple(int foundLocation);
	int getAdvancedRankingScore(std::vector <std::vector <std::tuple <int,int,int>>>  &patternLocationTuple);
	Ranking(std::string tagFilePath);
	~Ranking();
	// NOTE: REVISE IT AFTER TESTING
	int getRankingScore(int foundLocation);
	void printTag();
};

#endif