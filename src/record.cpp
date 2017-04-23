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
#include "record.h"
#include "ranking.h"
#include "search.h"
#include "utils.h"
#include "result.h"

using namespace std;

/* free record pointer on destruction */
Record::~Record()
{
  std::cout << "Freeing memory" << std::endl;
  std::cout << "dataCount : " << dataCount << std::endl;
  for (int i=0; i<dataCount; i++) {
    if(data[i].language)
      free(data[i].language);
    if(data[i].id)
      free(data[i].id);
    if(data[i].title)
      free(data[i].title);
    if(data[i].content)
      free(data[i].content);
  }
  if(data)
    free(data);
  for (auto rk: rank) {
    delete rk;
  }
};

/* construct record */
Record::Record(std::string path)
{
  std::cout << "Loading record..." << std::endl;
  inputPath = path;
  fileCount = 0;
  dataCount = 0;
  buildRecord();
  std::cout << "Data loaded" << std::endl;
};

/* get count of files */
int Record::getFileCount() const{
  return fileCount;
}

/* get count of all record */
int Record::getRecordCount() const{
  return dataCount;
};


/**
 * searchFactory - choose the search algorithm to implement
 * @text: haystack text
 * @recordLanguage: Either 'ChineseT' or not chineseT
 * @pattern: needle string
 * @caseInsensitive: NOT Implemented yet
 * @editDistance: edit distance tolerance
 *
 * return the pointer to the founded position if successful
 * 
 */
char * Record::searchFactory(char *text,
                             char *recordLanguage,
                             std::string pattern,
                             bool caseInsensitive,
                             unsigned int editDistance)
{
  if (strcmp(recordLanguage, "FORMAT_ERROR")==0) // record malformed
    return NULL;
  else if ((strcmp(recordLanguage,"ChineseT")==0 )|| editDistance == 0){

    return strstr(text, pattern.c_str());
  }
  else
    return toleranceSearch(text, pattern.c_str(), editDistance);
}

/* update flag if not compliance, only compliant record will be stored */
void Record::checkComplianceToMustAndMustNotHave(bool &isComplianceToMustAndMustNotHave,
                     bool found,
                     bool mustHave,
                     bool mustNotHave) const
{
  if (mustHave){
    if(!found)
      isComplianceToMustAndMustNotHave = false;
  }else if (mustNotHave){
    if(found)
      isComplianceToMustAndMustNotHave = false;
  }
}

/**
 * searchId - search id
 * @id: record title text
 * @recordLanguage: Either 'ChineseT', not chineseT or FORMAT_ERROR
 * @searchPatterns: processed search patterns
 * @recordIndex: index of current processing record
 * @searchScore: the record search score
 * @searchMatchCount: the patterns match count
 * @isComplianceToMustAndMustNotHave: flag to 
 * @caseInsensitive: NOT Implemented yet
 * @editDistance: edit distance tolerance
 *
 * The record language are passed down to searchFactory to distinguish whether
 * fuzzy search is available (Not available for chineseT)
 *
 * NOTE: must / must not have are not implemented on id/title
 */
void Record::searchId(char *id,
                      char *recordLanguage,
                      std::vector<std::tuple<std::string, bool, bool>> &searchPatterns,
                      int &searchScore,
                      int &searchMatchCount,
                      bool &isComplianceToMustAndMustNotHave,
                      bool caseInsensitive,
                      unsigned int editDistance)
{
  for (auto pattern: searchPatterns){
    if((searchFactory(id, recordLanguage, std::get<0>(pattern).c_str(), caseInsensitive, editDistance))>0){
      searchScore += idWeight;
      searchMatchCount++;
    }
  }
}
/**
 * searchTitle - search title
 * @title: record title text
 * @recordLanguage: Either 'ChineseT' or not chineseT
 * @searchPatterns: processed search patterns
 * @recordIndex: index of current processing record
 * @searchScore: the record search score
 * @searchMatchCount: the patterns match count
 * @isComplianceToMustAndMustNotHave: flag to 
 * @caseInsensitive: NOT Implemented yet
 * @editDistance: edit distance tolerance
 *
 * The record language are passed down to searchFactory to distinguish whether
 * fuzzy search is available (Not available for chineseT)
 * NOTE: must / must not have are not implemented on id/title
 */
void Record::searchTitle(char *title,
                         char *recordLanguage,
                         std::vector<std::tuple<std::string, bool, bool>> &searchPatterns,
                         int &searchScore,
                         int &searchMatchCount,
                         bool &isComplianceToMustAndMustNotHave,
                         bool caseInsensitive,
                         unsigned int editDistance)
{
  for (auto pattern: searchPatterns){
    if((searchFactory(title, recordLanguage, std::get<0>(pattern).c_str(), caseInsensitive, editDistance))!=NULL){
      searchScore += titleWeight;
      searchMatchCount++;
    }
  }
}

/**
 * searchContent - search content
 * @content: record content text
 * @recordLanguage: Either 'ChineseT' or not chineseT
 * @searchPatterns: processed search patterns
 * @recordIndex: index of current processing record
 * @searchScore: the record search score
 * @searchMatchCount: the patterns match count
 * @isComplianceToMustAndMustNotHave: flag to 
 * @caseInsensitive: NOT Implemented yet
 * @editDistance: edit distance tolerance
 *
 * The record language are passed down to searchFactory to distinguish whether
 * fuzzy search is available (Not available for chineseT)
 * 
 */
void Record::searchContent(char *content,
                           char *recordLanguage,
                           std::vector <std::tuple<std::string, bool, bool>> &searchPatterns,
                           int recordIndex,
                           int &searchScore,
                           int &searchMatchCount,
                           bool &isComplianceToMustAndMustNotHave,
                           bool caseInsensitive,
                           unsigned int editDistance)
{
  char *text, *found;
  int foundLocation;
  double tmpScore;
  // NOTE: array of size searchPatterns.size();
  std::vector <std::tuple <int, int, int>> foundTuple;
  std::vector <std::vector <std::tuple <int,int,int>>> patternLocationTuples;

  for (auto pattern: searchPatterns){
    text = content;
    found = NULL;
    bool foundFlag = false;
    while((found = searchFactory(text, recordLanguage, std::get<0>(pattern).c_str(), caseInsensitive, editDistance))!=NULL){
      foundLocation = found - content;
      if (!rank[recordIndex]->isDefaultRanking())
        foundTuple.push_back(rank[recordIndex]->getRankTreeTuple(foundLocation));
      text = found + std::get<0>(pattern).length();
      searchMatchCount++;
      foundFlag = true;
    }
    patternLocationTuples.push_back(foundTuple);
    foundTuple.clear();
    checkComplianceToMustAndMustNotHave(isComplianceToMustAndMustNotHave, foundFlag, std::get<1>(pattern), std::get<2>(pattern));
  }


  if (rank[recordIndex]->isDefaultRanking())
    tmpScore= searchMatchCount * contentWeight;
  else
    tmpScore= rank[recordIndex]->getAdvancedRankingScore(patternLocationTuples);

  // TODO: Adjust tmpScore according to the % of the pattern in the content
  //       NOT ACCURATE AT ALL, it will ignore little match count
  //tmpScore = double(titleWeight * searchMatchCount / data[recordIndex].approxCharactersCount);
  //std::cout << searchMatchCount << std::endl;
  //std::cout << data[recordIndex].approxCharactersCount << std::endl;
  searchScore += tmpScore;
}

/**
 * searchAndSortWithRank - search with ranking
 * @queries: list of queries, still need to parse for + -
 * @searchResult: class to store results
 * @caseInsensitive: NOT implemented yet
 * @editDistance: edit distance tolerance default to 0
 *
 * queries need to be processed after received for detecting
 * special ones: must have (+) and must not have (-)
 */
void Record::searchAndSortWithRank(std::vector<std::string> queries,
                                   Result &searchResult,
                                   bool caseInsensitive,
                                   unsigned int editDistance)
{
  std::vector <std::tuple<std::string, bool, bool>> searchPatterns = parseSearchQuery(queries);

  for (int i=0; i < dataCount; i++){
      int searchScore = 0, searchMatchCount = 0;
      bool isComplianceToMustAndMustNotHave = true;
      searchId(data[i].id, data[i].language, searchPatterns, searchScore, searchMatchCount, isComplianceToMustAndMustNotHave, caseInsensitive, editDistance);
      searchTitle(data[i].title, data[i].language, searchPatterns, searchScore, searchMatchCount, isComplianceToMustAndMustNotHave, caseInsensitive, editDistance);
      searchContent(data[i].content, data[i].language, searchPatterns, i, searchScore, searchMatchCount, isComplianceToMustAndMustNotHave, caseInsensitive, editDistance);

      if (searchMatchCount > 0 && searchScore > 0){
          std::string bookTitle(data[i].title);
          searchResult.insertResult(bookTitle, searchScore, searchMatchCount, isComplianceToMustAndMustNotHave);
      }
  }

}


/* Update file paths and tags paths then start loading into memory */
void Record::buildRecord(){
  checkPathAndSetFileVectors();
  readFileThenSetRecordAndRank();
};



/* Initiate record loading and rank tree building */
void Record::readFileThenSetRecordAndRank()
{
  FILE *fptr;
  char *line = NULL;
  size_t len = 0, read;
  data = (struct record*) malloc(sizeof(struct record));
  bool isNewRecord = true;

  for (int i=0; i<fileCount; i++){
    // NOTE: the first char of rawfile will get changed into '\0' randomly
    // I don't know why this happen. 
    if (rawfiles[i].c_str()[0] == '\0'){
      rawfiles[i].at(0) = '.';
    }
    fptr = fopen(rawfiles[i].c_str(), "r");
    int dataCountForCurrentFile = 0;
    while((read = getline(&line, &len, fptr))!=-1){
      switch (line[0]) {
        case '@':
          handlePrefixCases(dataCountForCurrentFile, read, line, isNewRecord);
          break;
        default:
          handleMalformedCases("Error: No prefix before data!", dataCountForCurrentFile, isNewRecord);
          break;
      }
    }
    fclose(fptr);
    insertAllRanksForCurrentFile(tagFiles[i], dataCountForCurrentFile);
  }
  free(line);
}

/**
 * handlePrefixCases - select corresponding prefix and update its data
 * @isNewRecord: bool to reset new file condition                             
 *
 */
void Record::handlePrefixCases(int &dataCountForCurrentFile,
                               size_t &read,
                               char *&line,
                               bool &isNewRecord)
{
  bool isPrefixToolong = false;
  std::string prefix = "@";
  int offset = setPrefixAndReturnOffset(prefix, isPrefixToolong, line);

  if(isPrefixToolong){
    handleMalformedCases("Error: Prefix too long!", dataCountForCurrentFile, isNewRecord);
  }else {
    
    if (prefix == "@id") {
      incrementLocalFileDataCountAndDataCount(dataCountForCurrentFile);
      createAndAssignDefaultStructData();
      createMemoryThenInsert(data[dataCount-1].id, line, offset, read);
      isNewRecord = false;
    }else if (prefix == "@title" && !isNewRecord){
      createMemoryThenInsert(data[dataCount-1].title, line ,offset, read);
    }else if (prefix == "@content" && !isNewRecord){
      createMemoryThenInsert(data[dataCount-1].content, line, offset, read);
      detectLanguage((line+offset), data[dataCount-1].language);
      data[dataCount-1].approxCharactersCount = getRecordCharactersCount(read, offset, line, data[dataCount-1].language);
      isNewRecord = true;
    }else {
      // NOTE: allow custom prefix in the future
      handleMalformedCases("Error: Unknown prefix!", dataCountForCurrentFile, isNewRecord);
    }
  }
}

/* get approximate utf8 character count */
int Record::getRecordCharactersCount(size_t lineCharCount,
                                     int prefixCount,
                                     char *& line,
                                     char *language) const
{
  if (strcmp(language, "ChineseT") == 0)
    return (lineCharCount - prefixCount)/3; // assume all chinese characters
  else
    return  countWords(line); // will count the prefix as a redundant word
}

/* increment global record count and local-to-file record count */
void Record::incrementLocalFileDataCountAndDataCount(int &currentFileDataCount)
{
  dataCount += 1;
  currentFileDataCount += 1;
}

/**
 * handleMalformedCases - create dummy data for malformed record
 * @malformType: the malform error string
 * @dataCountForCurrentFile: record count of the current file
 * @isNewRecord: bool to reset outer condition                             
 *
 */
void Record::handleMalformedCases(std::string malformType,
                                  int &dataCountForCurrentFile,
                                  bool &isNewRecord)
{
  incrementLocalFileDataCountAndDataCount(dataCountForCurrentFile);
  createAndAssignDefaultStructData();
  std::cout << malformType << std::endl;
  isNewRecord = true; 
}


/* insert source into new target memory */
void Record::createMemoryThenInsert(char *&target,
                                    char *&source,
                                    size_t offset,
                                    size_t &size)
{
  // must free target because we had already assign a default value
  free(target);
  target= (char *) malloc(size-offset+1);
  if (target == NULL){
    std::cout << "Error allocating memory" << std::endl;
    exit(1);
  }
  if (size == offset) { // empty source
    target[0] = '\0';
  }else {
    strcpy(target, (source+offset));
    target[size-offset-1] = '\0';
  }
}

/* get the prefix string before character ':' */
int Record::setPrefixAndReturnOffset(std::string &prefix,
                                     bool &isPrefixToolong,
                                     char *&line) const
{
  int offset=1;
  while(line[offset]!=':'){
    prefix += line[offset];
    offset++;
    if (offset == 30) { // Limitation for long offset
      isPrefixToolong = true;
      break;
    }
  }
  offset += 1; // skip ':'
  return offset;
}

/* assign default value for current record */
void Record::createAndAssignDefaultStructData()
{
  struct record *moreData = NULL;
  moreData = (struct record *) realloc(data, dataCount*sizeof(struct record));
  if (moreData == NULL){
    std::cout << "Error allocating memory" << std::endl;
    exit(1);
  }
  else {
    data = moreData;
    data[dataCount-1].id = strdup("FORMAT_ERROR");
    data[dataCount-1].title = strdup("FORMAT_ERROR");
    data[dataCount-1].content = strdup("FORMAT_ERROR");
    data[dataCount-1].language = strdup("FORMAT_ERROR");
  }
}

/**
 * insertAllRanksForCurrentFile - update rank tree after loading a file
 * @tagPath: the indexed tag path of the file
 * @dataCountForCurrentFile: record count for the file (each file may have
 *                           multiple record, however the tagfile is only
 *                           corresponded to the first record )
 *
 * Must be inplemented before the next file load up.
 */
void Record::insertAllRanksForCurrentFile(std::string &tagPath, int dataCountForCurrentFile)
{
  // Rule: if there is more than one record in the file
  // The index-file (.tags) must be assigned to the first record
  // Ranking newRank(tagPath);
  Ranking* newRank = new Ranking(tagPath);
  rank.push_back(newRank);
  for (int i=0; i < dataCountForCurrentFile-1; i++) {
    // Ranking* defaultRank = new Ranking("NO-INDEX_FILE");
    rank.push_back(newRank);
  }
}

/* check inputPath then insert file paths toclass vector members */
void Record::checkPathAndSetFileVectors() 
{
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
            if (newFilePath.substr(newFilePath.length() - 4) == ".txt"){
              rawfiles.push_back(newFilePath);
              tagFiles.push_back(newFilePath.substr(0, newFilePath.size()-3)+"tags");
              fileCount++;
              // if(fileCount > 100) break;
            }
          }
        }
        closedir(dir);
      }
    }
    else {
      rawfiles.push_back(inputPath);
      tagFiles.push_back(inputPath.substr(0, inputPath.size()-3)+"tags");
      fileCount++;
    }
  }else {
    std::cout << "File doesn't exists !" << std::endl;
    exit(1);
  }
}
