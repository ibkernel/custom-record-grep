#ifndef CRGREP_Formatter_H_
#define CRGREP_Formatter_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <ctime>

#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <tuple>
#include "utils.h"


void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);
std::tuple<std::string, int> getTagTuple(std::string tagType, long tagLocation);


// one file -> one record
// one dir -> merge every file in that dir into one record

class Formatter{
private:
	std::string pathToFormattedDir;
	std::string pathToRawData;
	std::string pathToStopWords;

	std::vector <std::string> stopWords;
	std::vector <std::string> singleFilePaths;
	std::vector <std::string> concatFilePaths;

	void processFile(bool concatFlag=false) {
		if (!concatFlag){
			for (auto path: singleFilePaths) {
				int chapter_num = 0, title_num = 0, paragraph_num = 0, sentense_num =0;
				long char_count = 0;
				std::string dataTitle = removePrefixPath(path);
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

	}

	void processConcatFile(std::string pathToDir, long &char_count, int &chapter_num, int &title_num, int &paragraph_num, int &sentense_num, std::string dataTitle){
		std::vector <std::string> toProcessFilePaths;
		insertFilesPathInDirIntoVector(pathToDir, toProcessFilePaths);
		for (auto path: toProcessFilePaths) {
			//std::cout << path << std::endl;
			formatThenMerge(path, char_count, chapter_num, title_num, paragraph_num, sentense_num, dataTitle);
		}
		toProcessFilePaths.clear();
	}

	void writeRecordHeaderToFile(std::string dataTitle){
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

	void formatThenMerge(std::string pathToSingleFile, long &char_count, int &chapter_num, int &title_num, int &paragraph_num, int &sentense_num, std::string dataTitle){
		std::cout << "Current processing file: " << pathToSingleFile << std::endl; 
		std::ifstream chapterFile(pathToSingleFile);
		std::string line, text = "";

		while (std::getline(chapterFile, line)){
			for (int i=0; i< stopWords.size();i++){
				ReplaceStringInPlace(line, stopWords[i], "");
			}
			if (line.length()>2)
				text += (line + '\n');
		}
		//TODO: ??????
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
		int lineCOunt = 0;
		while (std::getline(stext, line)){
			if (!titleFlag){
				//line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
				paragraph_num += 1;
				tagQueue.push_back(getTagTuple("p_"+std::to_string(paragraph_num), char_count));
				lineFormatter(line, sentense_num, char_count, tagQueue);
				tagQueue.push_back(getTagTuple("p_"+std::to_string(paragraph_num), char_count));
			}else {
				//line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
				title_num += 1;
				tagQueue.push_back(getTagTuple("t_"+std::to_string(title_num), char_count));
				//std::cout << "Title:" << line << std::endl;
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


	void writeTagInfoToFile(std::deque <std::tuple <std::string, long>> &tagQueue, std::string dataTitle){
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


		int queue_count = 0;
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

	void lineFormatter(std::string &line,int &sentense_num, long &char_count,
	 std::deque <std::tuple <std::string, long>> &tagQueue){
		std::smatch m;
		//std::regex e("(。|！|？|!|\\?)+");
		std::regex e("(。)+");
		std::string copiedLine = line;
		int currentP = 0;
		sentense_num += 1;
		bool lastLineFlag = false;
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

	void insertFilesPathInDirIntoVector(std::string path, std::vector <std::string> &paths){
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(path.c_str())) != NULL) {
			while ((ent = readdir(dir)) != NULL){
				std::string pathToSingleFile;
				char end = path.back();
				if (end == '/' || end == '\\') {
					pathToSingleFile = path+ent->d_name;
					// std::cout << "True: " << path << std::endl;
				}else {
					pathToSingleFile = path+'/'+ent->d_name;
					// std::cout << "False: " << path << std::endl;
				}
				if(!isDir(pathToSingleFile)){
					//std::cout << "files path: " << pathToSingleFile << std::endl;
					if (strcmp(ent->d_name, ".DS_Store")!=0)
						paths.push_back(pathToSingleFile);
				}
			}

		}else {
			std::cout << "error: getAllFilesPathInDir" << std::endl;
		}
	
	}

	void updateDirPathInRawDir() {
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(pathToRawData.c_str())) != NULL) {
			while ((ent = readdir(dir)) != NULL){
				std::string pathToChildDir;
				char end = pathToRawData.back();
				if (end == '/' || end == '\\') {
					pathToChildDir = pathToRawData+ent->d_name;
					// std::cout << "True: " << pathToRawData << std::endl;
				}else {
					pathToChildDir = pathToRawData+'/'+ent->d_name;
					// std::cout << "False: " << pathToRawData << std::endl;
				}
				if(isDir(pathToChildDir)){
					// 	std::cout << "dir path: " << pathToChildDir << std::endl;
					if (strcmp(ent->d_name, ".")!=0 && strcmp(ent->d_name, "..")!=0){
						concatFilePaths.push_back(pathToChildDir);
						//std::cout << "dir path: " << pathToChildDir << std::endl;
					}
				}
			}
		}else {
			std::cout << "error: updateDirPathInRawDir" << std::endl;
		}
	}

public:
	Formatter(std::string pathSource, std::string pathDest){
		pathToFormattedDir = pathDest;
		pathToRawData = pathSource;
		pathToStopWords = "none";
		bool isConcatFile = true;
		if (isDir(pathSource)){
			insertFilesPathInDirIntoVector(pathToRawData, singleFilePaths);
			processFile();
			updateDirPathInRawDir();
			processFile(isConcatFile);
		}else {
			singleFilePaths.push_back(pathSource);
			processFile();
		}
		

	};

	Formatter(std::string pathSource, std::string pathDest, std::string pathStopWords){
		pathToFormattedDir = pathDest;
		pathToRawData = pathSource;
		pathToStopWords = pathStopWords;
		std::ifstream stopWordsFile(pathToStopWords);
		std::string words;
		std::cout << "Your stop words:" << std::endl;
		bool isConcatFile = true;

		while (std::getline(stopWordsFile, words)){
			stopWords.push_back(words);
		}
		if (isDir(pathSource)){
			insertFilesPathInDirIntoVector(pathToRawData, singleFilePaths);
			processFile();
			updateDirPathInRawDir();
			processFile(isConcatFile);
		}else {
			singleFilePaths.push_back(pathSource);
			processFile();
		}
	};

	~Formatter(){};

};



#endif