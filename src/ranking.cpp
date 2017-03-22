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
#include <map>
#include <cmath>
#include "utils.h"
#include "ranking.h"



using namespace std;

Ranking::~Ranking(){};
Ranking::Ranking(std::string tagFilePath)
{
    pathToTagFile = tagFilePath;
    root = NULL;
    isDefaultRankingBool = true;
    chapter_num = chapter_size  = paragraph_size = 0;
    paragraph_num = 1;
    buildRank();
};

// TODO: change score to double?
// For multi-pattern searching
int Ranking::getAdvancedRankingScore(std::vector <std::vector <std::tuple <int,int,int>>>  &patternLocationTuples)
{
  int score = 0, patternNum = 0;
  std::map<std::string, int> foundMap;
  for (auto patternLocations: patternLocationTuples){
    for (auto singleLocation: patternLocations){
        score += getPatternScore(foundMap, singleLocation,patternNum);
    }
    patternNum += 1;
  }
  foundMap.clear();
  return score;
}

// TODO: need fix overflow error
int Ranking::getPatternScore(std::map<std::string, int> &foundMap,
                             std::tuple<int,int,int>&singleLocation,
                             int patternNum)
{
  std::map<std::string, int>::iterator it;
  std::string chapterIndex = std::to_string(std::get<0>(singleLocation));
  std::string paragraphIndex = std::to_string(std::get<1>(singleLocation));
  std::string sentenseIndex = std::to_string(std::get<2>(singleLocation));

  it = foundMap.find(chapterIndex);
  if (it != foundMap.end()){
    foundMap[chapterIndex] += sqrt(pow(chapterTagWeight, patternNum));
  }else{
    foundMap.insert(std::pair<std::string, int>(chapterIndex, 1));
  }

  it = foundMap.find(chapterIndex+'_'+paragraphIndex);
  if (it != foundMap.end()){
    foundMap[chapterIndex+'_'+paragraphIndex] += sqrt(pow(paragraphTagWeight, patternNum));
  }else{
    foundMap.insert(std::pair<std::string, int>(chapterIndex+'_'+paragraphIndex, 1));
  }

  it = foundMap.find(chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex);
  if (it != foundMap.end()){
    foundMap[chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex] += sqrt(pow(sentenseTagWeight, patternNum));
  }else{
    foundMap.insert(std::pair<std::string, int>(chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex, 1));
  }
  int score = foundMap[chapterIndex]*foundMap[chapterIndex+'_'+paragraphIndex]*foundMap[chapterIndex+'_'+paragraphIndex+'_'+sentenseIndex];

  if (paragraphIndex == "0") // title
    return score * titleTagWeight;
  else
    return score * paragraphTagWeight;
}

std::tuple <int, int, int> Ranking::getRankTreeTuple(int foundLocation)
{
  std::tuple <int, int, int> location;
  int chapter_array_index = getBelongingNodeIndexWithFoundLocation(root->lowerBoundLocationOfChildTags, root->childTagCount, foundLocation);
  int paragraph_array_index = getBelongingNodeIndexWithFoundLocation(root->tagNodes[chapter_array_index]->lowerBoundLocationOfChildTags,
   root->tagNodes[chapter_array_index]->childTagCount, foundLocation);
  if (paragraph_array_index == 0){
    return std::make_tuple(chapter_array_index, paragraph_array_index, 0);
  }
  int sentence_array_index = getBelongingNodeIndexWithFoundLocation(
      root->tagNodes[chapter_array_index]->tagNodes[paragraph_array_index]->lowerBoundLocationOfChildTags,
       root->tagNodes[chapter_array_index]->tagNodes[paragraph_array_index]->childTagCount,
        foundLocation);
  return std::make_tuple(chapter_array_index, paragraph_array_index, sentence_array_index);
}

int Ranking::getBelongingNodeIndexWithFoundLocation(int *&lowerBound,
                                                    int arrayLength,
                                                    int foundLocation)
{
  if (arrayLength == 0) // NOTE: It should never happen, however we need more testing to verify
    return -1;
  int mid = arrayLength/2, left =0, right = arrayLength - 1;
  int minDiff = lowerBound[arrayLength-1], curDiff = 0, inMid = 0;
  if (foundLocation > lowerBound[arrayLength-1])
    return arrayLength-1;
  if (foundLocation < lowerBound[0])
    return 0;

  while(left <= right){
    if (lowerBound[mid] == foundLocation)
      return mid;
    if (lowerBound[mid] > foundLocation){
      curDiff = lowerBound[mid] - foundLocation;
      if (curDiff < minDiff){
        inMid = mid;
        minDiff = curDiff;
      }
      right = mid -1;
      mid = (left+right)/2;
    }else {
      curDiff = foundLocation - lowerBound[mid];
      if (curDiff < minDiff){
        inMid = mid;
        minDiff = curDiff;
      }
      left = mid + 1;
      mid = (left+right)/2;
    }
  }

  return mid;
}


void Ranking::insertTag(char tagType, int lowerBound, int upperBound)
{
  if (root == NULL){
    root = (struct rankTreeNode*) malloc(sizeof(struct rankTreeNode) * 1);
    root->lowerBoundLocationOfChildTags = (int *) malloc(sizeof(int) * 1);
    root->tagNodes = (struct rankTreeNode **)malloc(sizeof(struct rankTreeNode *) * 1);
    root->lowerBoundLocationOfChildTags[0] = lowerBound;
    root->tagNodes[0] = (struct rankTreeNode *)malloc(sizeof(struct rankTreeNode));
    root->childTagCount = 1;
    chapter_num += 1;
  }else {
    struct rankTreeNode **moreData = NULL;
    int*  evenMoreData = NULL;

    switch (tagType){
      case 'c':
        chapter_size = 0;
        moreData = (struct rankTreeNode **) realloc(root->tagNodes, sizeof(struct rankTreeNode *)*(chapter_num+1));
        evenMoreData = (int *) realloc(root->lowerBoundLocationOfChildTags, sizeof(int)*(chapter_num+1));
        if (moreData!= NULL && evenMoreData != NULL){
          root->tagNodes = moreData;
          root->tagNodes[chapter_num] = (struct rankTreeNode *)malloc(sizeof(struct rankTreeNode) * 1);
          root->lowerBoundLocationOfChildTags = evenMoreData;
          root->lowerBoundLocationOfChildTags[chapter_num] = lowerBound;
          root->childTagCount += 1;
        }
        chapter_num += 1;
        break;
      case 't':
        root->tagNodes[chapter_num-1]->childTagCount = 0;
        root->tagNodes[chapter_num-1]->childTagCount += 1;
        root->tagNodes[chapter_num-1]->lowerBoundLocationOfChildTags = (int *)malloc(sizeof(int)*1);
        root->tagNodes[chapter_num-1]->lowerBoundLocationOfChildTags[0] = lowerBound;
        root->tagNodes[chapter_num-1]->tagNodes = (struct rankTreeNode **)malloc(sizeof(struct rankTreeNode*)*1);
        root->tagNodes[chapter_num-1]->tagNodes[0] = (struct rankTreeNode *)malloc(sizeof(struct rankTreeNode));
        root->tagNodes[chapter_num-1]->tagNodes[0]->childTagCount = 0;
        chapter_size += 1;
        break;
      case 'p':
        if(!root->tagNodes[chapter_num-1]->childTagCount)
          root->tagNodes[chapter_num-1]->childTagCount = 1;
        root->tagNodes[chapter_num-1]->childTagCount += 1;
        moreData = (struct rankTreeNode **) realloc(root->tagNodes[chapter_num-1]->tagNodes, sizeof(struct rankTreeNode *)*(chapter_size+1));
        evenMoreData = (int *)realloc(root->tagNodes[chapter_num-1]->lowerBoundLocationOfChildTags, sizeof(int)*(chapter_size+1));
        if(moreData != NULL && evenMoreData != NULL){
          root->tagNodes[chapter_num-1]->tagNodes = moreData;
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size] = (struct rankTreeNode *)malloc(sizeof(struct rankTreeNode));
          root->tagNodes[chapter_num-1]->lowerBoundLocationOfChildTags = evenMoreData;
          root->tagNodes[chapter_num-1]->lowerBoundLocationOfChildTags[chapter_size] = lowerBound;
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size]->tagNodes = (struct rankTreeNode**)malloc(sizeof(struct rankTreeNode*) *1);
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size]->lowerBoundLocationOfChildTags = (int *)malloc(sizeof(int)*1);
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size]->childTagCount = 0;
        }else {
          puts("Error reallocating memory");
          exit(1);
        }
        chapter_size += 1;
        paragraph_num += 1;
        paragraph_size = 1;
        break;
      case 's':
        root->tagNodes[chapter_num-1]->tagNodes[chapter_size-1]->childTagCount += 1;
        moreData = (struct rankTreeNode **) realloc(root->tagNodes[chapter_num-1]->tagNodes[chapter_size-1]->tagNodes, sizeof(struct rankTreeNode *)*(paragraph_size));
        evenMoreData = (int *) realloc(root->tagNodes[chapter_num-1]->tagNodes[chapter_size-1]->lowerBoundLocationOfChildTags, sizeof(int)*(paragraph_size));
        if (moreData != NULL && evenMoreData != NULL){
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size-1]->tagNodes = moreData;
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size-1]->tagNodes[paragraph_size-1] = (struct rankTreeNode*)malloc(sizeof(struct rankTreeNode)* 1);
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size-1]->lowerBoundLocationOfChildTags = evenMoreData;
          root->tagNodes[chapter_num-1]->tagNodes[chapter_size-1]->lowerBoundLocationOfChildTags[paragraph_size-1] = lowerBound;
        }else {
          puts("Error reallocating memory");
          exit(1);
        }
        paragraph_size += 1;
        break;
      default:
        std::cout << "error tag" << std::endl;
        break;
    }
  }
};

void Ranking::buildRank()
{
  if(exists(pathToTagFile)) {
    std::ifstream infile(pathToTagFile);
    std::string tagName;
    int lowerBoundLocation, upperBoundLocation;
    while(infile >> tagName >> lowerBoundLocation >> upperBoundLocation){
      insertTag(tagName.at(0), lowerBoundLocation, upperBoundLocation);
    }
    isDefaultRankingBool = false;
  } else {
    std::cout << "index file doesn't exist! Using default ranking" << std::endl;
  }
};

bool Ranking::isDefaultRanking()
{
  return isDefaultRankingBool;
}

// For testing
void Ranking::printTag()
{
  cout << "Total chapter tags: " << root->childTagCount << endl;
  for (int i=0; i < root->childTagCount; i++){
    cout << "Current chapter: " << root->childTagCount << "\tTotal paragraph tags:" << root->tagNodes[i]->childTagCount << endl;
    for (int j=0; j < root->tagNodes[i]->childTagCount; j++)
      cout << "Total sentense tags:" << root->tagNodes[i]->tagNodes[j]->childTagCount << endl;
  }
}