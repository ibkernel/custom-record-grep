#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "result.h"


Result::Result() {
}

Result::~Result() {
  searchResult.clear();
}

/* insert comliant record to class member */
void Result::insertResult(std::string title,
                          int searchScore,
                          int searchMatchCount,
                          bool isComplianceToMustAndMustNotHave)
{
  struct result foundResult;
  if(isComplianceToMustAndMustNotHave){
    foundResult.recordTitle = title;
    foundResult.recordScore = searchScore;
    foundResult.recordMatchCount = searchMatchCount;
    foundResult.isValid = isComplianceToMustAndMustNotHave;
    searchResult.push_back(foundResult);
  }
}

/* get compliant result count */
int Result::getResultCount()
{
  int compliantCount = 0;
  for (int i=0; i<searchResult.size(); i++)
      compliantCount++;
  return compliantCount;
};

double Result::getResultScore(int i)
{
  return searchResult[i].recordScore;
};

std::string Result::getResultTitle(int i )
{
  return searchResult[i].recordTitle;
};

void Result::printResult(bool order)
{
  sort(order);
  for (auto r: searchResult) {
    if (r.isValid){
      std::cout << "Book :" << r.recordTitle << std::endl;
      std::cout << "Rank score:" << r.recordScore << std::endl;
      std::cout << "Match count:" << r.recordMatchCount << std::endl;
    }
  }
}
/* Sort result by rank score, default: descending order */
void Result::sort(bool order)
{
  if (order) {  // ascending
    std::sort(searchResult.begin(), searchResult.end(),
      [](struct result const &t1, struct result const &t2) {
          return t1.recordScore < t2.recordScore;
        }
    );
  }else{
    std::sort(searchResult.begin(), searchResult.end(),
      [](struct result const &t1, struct result const &t2) {
          return t1.recordScore > t2.recordScore;
        }
    );
  }
}

/* clear vector after each search */
void Result::reset()
{
  searchResult.clear();
}