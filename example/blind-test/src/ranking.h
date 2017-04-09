#ifndef CRGREP_RANKING_H_
#define CRGREP_RANKING_H_
#include <unordered_map>

const int chapterTagWeight = 3,  titleTagWeight = 10, paragraphTagWeight = 4, sentenseTagWeight = 5;
const int idWeight = 0, titleWeight = 300000, contentWeight = 3;

struct rankTreeNode {
  int *lowerBoundLocationOfChildTags;
  int childTagCount;
  struct rankTreeNode **tagNodes;
};

class Ranking {
public:
  Ranking(std::string tagFilePath);
  ~Ranking();

  int getAdvancedRankingScore(std::vector <std::vector <std::tuple <int,int,int>>>  &patternLocationTuple) const;
  std::tuple <int, int, int> getRankTreeTuple(int foundLocation) const;
  bool isDefaultRanking() const;

private:
  struct rankTreeNode *root;
  std::string pathToTagFile;
  int chapter_num;
  int chapter_size;
  int paragraph_num;
  int paragraph_size;
  bool isDefaultRankingBool;

  int getBelongingNodeIndexWithFoundLocation(int *&lowerBound,
                                             int arrayLength,
                                             int foundLocation) const;

  int getPatternScore(std::unordered_map<std::string, int> &foundMap,
                      std::tuple<int,int,int>&singleLocation,
                      int patternNum) const;

  void insertTag(char tagType, int lowerBound, int upperBound);
  void buildRank();
};

#endif