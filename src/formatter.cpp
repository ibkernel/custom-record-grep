#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <ctime>
#include <vector>
#include <deque>
#include <tuple>

#include "formatter.h"
#include "utils.h"

std::tuple<std::string, int> getTagTuple(std::string tagType, long tagLocation) 
{
  return std::make_tuple(tagType, tagLocation); // Always works
};
/* initialize class member*/
Formatter::Formatter(std::string pathSource, std::string pathDest, std::string pathStopWords)
{
  pathToFormattedDir = pathDest;
  pathToRawData = pathSource;
  pathToStopWords = pathStopWords;
  std::ifstream stopWordsFile(pathToStopWords);
  std::string words;
  while (std::getline(stopWordsFile, words)){
    stopWords.push_back(words);
  }
  stopWordsFile.close();
  stopWords.push_back("\r");
  if (isDir(pathSource)){
    readPathsInDirAndProcessEachPath();
    // insertFilesPathInDirIntoVector(pathToRawData, singleFilePaths);
    // processFile();
    // insertDirPathInRawDir();
    // processFile(isConcatFile);
  }else {
    singleFilePaths.push_back(pathSource);
    processFile();
  }
};

/* initialize class member */
Formatter::Formatter(std::string pathSource, std::string pathDest)
{
  pathToFormattedDir = pathDest;
  pathToRawData = pathSource;
  pathToStopWords = "none";
  stopWords.push_back("\r");
  // Refactor to process each single path at a time
  // if is file -> process single file
  // if is dir -> process dir
  if (isDir(pathSource)){
    readPathsInDirAndProcessEachPath();
    // insertFilesPathInDirIntoVector(pathToRawData, singleFilePaths);
    // processFile();
    // insertDirPathInRawDir();
    // processFile(isConcatFile);
  }else {
    singleFilePaths.push_back(pathSource);
    processFile();
  }
};

void Formatter::readPathsInDirAndProcessEachPath()
{
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(pathToRawData.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL){
      std::string pathToChildDir;
      char end = pathToRawData.back();
      if (end == '/' || end == '\\') {
        pathToChildDir = pathToRawData+ent->d_name;
      }else {
        pathToChildDir = pathToRawData+'/'+ent->d_name;
      }
      if(isDir(pathToChildDir)){
        if (strcmp(ent->d_name, ".")!=0 && strcmp(ent->d_name, "..")!=0){
          newProcessFile(pathToChildDir, true);
          // concatFilePaths.push_back(pathToChildDir);
        }
      }else {
        newProcessFile(pathToChildDir, false);
      }
    }
  }else {
    std::cout << "error: readPathsInDirAndProcessEachPath: " << std::endl;
  }
  closedir(dir);
}

void Formatter::newProcessFile(std::string pathToChildDir, bool concatFlag)
{
  if (!concatFlag){
    int chapter_num = 0, title_num = 0, paragraph_num = 0, sentense_num =0;
    long char_count = 0;
    std::string dataTitle = removePrefixPath(pathToChildDir);
    ReplaceStringInPlace(dataTitle, ".txt", "");
    writeRecordHeaderToFile(dataTitle);
    formatThenMerge(pathToChildDir, char_count, chapter_num, title_num, paragraph_num, sentense_num, dataTitle);
  }else {
    int chapter_num = 0, title_num = 0, paragraph_num = 0, sentense_num =0;
    long char_count = 0;
    std::string dataTitle = removePrefixPath(pathToChildDir);
    writeRecordHeaderToFile(dataTitle);
    processConcatFile(pathToChildDir, char_count, chapter_num, title_num, paragraph_num, sentense_num, dataTitle);
  }

};



/**
 * processFile - process independent file and concatenating file
 * @concatFlag: whether the following data a to-be-concat file
 * 
 * Single file: 1.If the user submitted a path to a FILE, the file
 *              is obviously a single file. 
 *              2.If the user submitted a path to a DIRECTORY, all the
 *              file children will be treated as single file.
 * Concat file: Some slightly difference compare to the above 2.
 *              the rest of the children's (which are directories) children
 *              are considered to be related concat files.
 */
void Formatter::processFile(bool concatFlag)
{
  if (!concatFlag){
    for (auto path: singleFilePaths) {
      int chapter_num = 0, title_num = 0, paragraph_num = 0, sentense_num =0;
      long char_count = 0;
      std::string dataTitle = removePrefixPath(path);
      ReplaceStringInPlace(dataTitle, ".txt", "");

      writeRecordHeaderToFile(dataTitle);
      formatThenMerge(path, char_count, chapter_num, title_num, paragraph_num, sentense_num, dataTitle);
    }
  }else {
    for (auto path: concatFilePaths) {
      int chapter_num = 0, title_num = 0, paragraph_num = 0, sentense_num =0;
      long char_count = 0;
      std::string dataTitle = removePrefixPath(path);
      writeRecordHeaderToFile(dataTitle);
      processConcatFile(path, char_count, chapter_num, title_num, paragraph_num, sentense_num, dataTitle);
    }
  }

};

/* process concatFile */
void Formatter::processConcatFile(std::string pathToDir,
                                  long &char_count,
                                  int &chapter_num,
                                  int &title_num,
                                  int &paragraph_num,
                                  int &sentense_num,
                                  std::string dataTitle)
{
  DIR *dir;
  struct dirent *ent;
  std::vector <std::string> toProcessFilePaths;
  // insertFilesPathInDirIntoVector(pathToDir, toProcessFilePaths);
  if ((dir = opendir(pathToDir.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL){
      std::string pathToSingleFile;
      char end = pathToDir.back();
      if (end == '/' || end == '\\') {
        pathToSingleFile = pathToDir+ent->d_name;
      }else {
        pathToSingleFile = pathToDir+'/'+ent->d_name;
      }
      if(!isDir(pathToSingleFile)){
        if (strcmp(ent->d_name, ".DS_Store")!=0){
          toProcessFilePaths.push_back(pathToSingleFile);
        }
      }
    }
    for (auto path: toProcessFilePaths) {
      formatThenMerge(path, char_count, chapter_num, title_num, paragraph_num, sentense_num, dataTitle);
    }
  }else {
    std::cout << "error processing dir: " << pathToDir << std::endl;
  }
  closedir(dir);

};

/* open and truncate destination of the formatted file and tagged file */
void Formatter::writeRecordHeaderToFile(std::string dataTitle){
  struct stat info;
  if( stat( pathToFormattedDir.c_str(), &info ) != 0 ){
      std::cout << "Creating directory: " << pathToFormattedDir << std::endl;
      makePath(pathToFormattedDir);
  }
  else if( info.st_mode & S_IFDIR ){  // S_ISDIR() doesn't exist on my windows 
      // valid directory do nothing
  }
  else {
    std::cout << pathToFormattedDir << " is not a directory, please retry." << std::endl;
    exit(1);
  }

  std::string formattedDestination = pathToFormattedDir + dataTitle + ".txt";
  std::string indexDestination = pathToFormattedDir + dataTitle + ".tags";
  std::ofstream formattedFile, tagsFile;
  tagsFile.open(indexDestination);
  tagsFile << "";
  tagsFile.close();
  formattedFile.open(formattedDestination);
  formattedFile << "@id:" << dataTitle << std::endl;
  formattedFile << "@title:" << dataTitle << std::endl;
  formattedFile << "@content:";
  formattedFile.close();
};


/**
 * filter-out stop-words and start indexing the text alongside generating formatted
 * file.
 */
void Formatter::formatThenMerge(std::string pathToSingleFile,
                                long &char_count,
                                int &chapter_num,
                                int &title_num,
                                int &paragraph_num,
                                int &sentense_num,
                                std::string dataTitle)
{
  // std::cout << "Current processing file: " << pathToSingleFile << std::endl; 
  std::ifstream chapterFile(pathToSingleFile);
  std::string line, text = "";

  int chineseCount = 0, otherCount = 0;
  while (std::getline(chapterFile, line)){
    for (unsigned int i=0; i< stopWords.size();i++){
      ReplaceStringInPlace(line, stopWords[i], "");
    }
    if (line.length()>2)
      text += (line + '\n');
  }
  chapterFile.close();
  detectLanguageAndUpdateLanguageCount(text.c_str(), chineseCount, otherCount);

  std::string regexEndingPhrasePattern = (chineseCount > otherCount ? "(。)+" : "(\\.|\\?|!)+");
  // NOTE: will coredump if too many files.
  // std::string regexEndingPhrasePattern = (chineseCount > otherCount ? "(。|！|？|\\.)+" : "(\\.|\\?|!)+");

  //Remove duplicated spaces to one .
  std::string::iterator new_end = std::unique(text.begin(), text.end(),
      [](char lhs, char rhs){ return (lhs == rhs) && (lhs == ' '); }
  );
  text.erase(new_end, text.end());

  std::string formattedDestination = pathToFormattedDir + dataTitle + ".txt";
  std::ofstream formattedFile;
  formattedFile.open(formattedDestination, std::ios::in | std::ios::out | std::ios::ate);
  bool titleFlag = true;
  std::deque <std::tuple <std::string, long>> tagQueue;
  chapter_num += 1;
  tagQueue.push_back(getTagTuple("c_"+std::to_string(chapter_num), char_count));
  std::stringstream stext(text);
  while (std::getline(stext, line)){
    if (!titleFlag){
      paragraph_num += 1;
      tagQueue.push_back(getTagTuple("p_"+std::to_string(paragraph_num), char_count));
      lineFormatter(line, regexEndingPhrasePattern, sentense_num, char_count, tagQueue);
      tagQueue.push_back(getTagTuple("p_"+std::to_string(paragraph_num), char_count));
    }else {
      title_num += 1;
      tagQueue.push_back(getTagTuple("t_"+std::to_string(title_num), char_count));
      char_count += line.length();
      tagQueue.push_back(getTagTuple("t_"+std::to_string(title_num), char_count));
      titleFlag = false;
    }
    formattedFile << line;
    line = "";
  }
  tagQueue.push_back(getTagTuple("c_"+std::to_string(chapter_num), char_count));
  formattedFile.close();
  writeTagInfoToFile(tagQueue, dataTitle);
};

/**
 * writeTagInfoToFile - create the tag file with the information stored during file 
 *                      formatting
 * writeTagInfoToFile must be called at the end of formatThenMerge
 */
void Formatter::writeTagInfoToFile(std::deque <std::tuple <std::string, long>> &tagQueue, std::string dataTitle)
{
  std::string tagInfoPath = pathToFormattedDir + dataTitle + ".tags";
  std::ofstream tagFile;
  std::tuple <std::string, long> tag, tag2;
  tagFile.open(tagInfoPath, std::ios::in | std::ios::out | std::ios::ate);

  tag = tagQueue.front();
  tagQueue.pop_front();
  tag2 = tagQueue.back();
  tagQueue.pop_back();
  tagFile << std::get<0>(tag) << "\t" << std::get<1>(tag) << "\t" << std::get<1>(tag2) << "\n";

  if(tagQueue.size() == 0){
    tagFile.close();
    return;
  }

  tag = tagQueue.front();
  tagQueue.pop_front();
  tag2 = tagQueue.front();
  tagQueue.pop_front();
  tagFile << std::get<0>(tag) << "\t" << std::get<1>(tag) << "\t" << std::get<1>(tag2) << "\n";

  if(tagQueue.size() == 0){
    tagFile.close();
    return;
  }

  std::deque <std::tuple <std::string, long>> otherTagQueue;
  for (auto t: tagQueue){
    if (otherTagQueue.size() == 0){
      otherTagQueue.push_back(t);
    }else if (std::get<0>(t) == std::get<0>(otherTagQueue.front())){
      tag = otherTagQueue.front();
      otherTagQueue.pop_front();
      tagFile << std::get<0>(t) << "\t" << std::get<1>(tag) << "\t" << std::get<1>(t) << "\n";
      int currentSize = otherTagQueue.size();
      for (int j=0; j< currentSize/2; j++){
        tag = otherTagQueue.front();
        otherTagQueue.pop_front();
        tag2 = otherTagQueue.front();
        otherTagQueue.pop_front();
        tagFile << std::get<0>(tag) << "\t" << std::get<1>(tag) << "\t" << std::get<1>(tag2) << "\n";
      }
    }else {
      otherTagQueue.push_back(t);
    }
  }
  tagFile.close();
};


/**
 * lineFormatter - insert sentense tags 
 * @line: string of the paragraph
 * @regexEndingPhrasePattern: the punctuation pattern to see as a ending of a phrase
 * @sentense_num: number of the processed sentensed
 * @char_count: count of total character in the record
 * @tagQueue: the deque which stores tags
 *
 * With the given regex pattern, lineFormatter see all the matches of the pattern
 * as a the end of a sentense, thus inserting the `s` tag into the deque.
 */
void Formatter::lineFormatter(std::string &line,
                              std::string &regexEndingPhrasePattern,
                              int &sentense_num,
                              long &char_count,
                              std::deque <std::tuple <std::string, long>> &tagQueue)
{
  std::smatch m;
  std::regex e(regexEndingPhrasePattern.c_str());
  std::string copiedLine = line;
  unsigned int currentP = 0;
  sentense_num += 1;
  tagQueue.push_back(getTagTuple("s_"+std::to_string(sentense_num), char_count + currentP));
  while (regex_search(copiedLine, m, e)) {
    if ((currentP + m.position() + m[0].length()) == line.length()){
      tagQueue.push_back(getTagTuple("s_"+std::to_string(sentense_num), char_count + line.length()));
      sentense_num += 1;
      tagQueue.push_back(getTagTuple("s_"+std::to_string(sentense_num), char_count + line.length()));
    } else {
      tagQueue.push_back(getTagTuple("s_"+std::to_string(sentense_num), char_count + m.position() + currentP));
      sentense_num += 1;
      tagQueue.push_back(getTagTuple("s_"+std::to_string(sentense_num), char_count + m.position() + currentP));
    }
    currentP += (m.position()+m[0].length());
    copiedLine = m.suffix();
  }
  if (currentP != line.length()){
    tagQueue.push_back(getTagTuple("s_"+std::to_string(sentense_num), char_count + line.length()));
  }else {
    sentense_num -= 1;
    tagQueue.pop_back();
  }
  char_count = char_count + line.length();
}

/* Insert the child files of the user provided path only */
void Formatter::insertFilesPathInDirIntoVector(std::string path, std::vector <std::string> &paths)
{
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL){
      std::string pathToSingleFile;
      char end = path.back();
      if (end == '/' || end == '\\') {
        pathToSingleFile = path+ent->d_name;
      }else {
        pathToSingleFile = path+'/'+ent->d_name;
      }
      if(!isDir(pathToSingleFile)){
        if (strcmp(ent->d_name, ".DS_Store")!=0){
          paths.push_back(pathToSingleFile);
        }
      }
    }

  }else {
    std::cout << "error insertFilesPathInDirIntoVector: " << path << std::endl;
  }
  closedir(dir);
}

/* Insert the child directory path of the user provided path only */
void Formatter::insertDirPathInRawDir()
{
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(pathToRawData.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL){
      std::string pathToChildDir;
      char end = pathToRawData.back();
      if (end == '/' || end == '\\') {
        pathToChildDir = pathToRawData+ent->d_name;
      }else {
        pathToChildDir = pathToRawData+'/'+ent->d_name;
      }
      if(isDir(pathToChildDir)){
        if (strcmp(ent->d_name, ".")!=0 && strcmp(ent->d_name, "..")!=0){
          concatFilePaths.push_back(pathToChildDir);
        }
      }
    }
  }else {
    std::cout << "error: insertDirPathInRawDir: " << std::endl;
  }
  closedir(dir);
}
