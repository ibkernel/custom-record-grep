#ifndef CRGREP_RANKING_H_
#define CRGREP_RANKING_H_


struct node {
	int *left;
	int weight;
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
public:
	Ranking(std::string tagFilePath);
	~Ranking();
	// NOTE: REVISE IT AFTER TESTING
	int getRankingScore(int foundLocation);
};

#endif