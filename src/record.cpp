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

#include "./cld/encodings/compact_lang_det/compact_lang_det.h"
#include "./cld/encodings/compact_lang_det/ext_lang_enc.h"
#include "./cld/encodings/compact_lang_det/unittest_data.h"
#include "./cld/encodings/proto/encodings.pb.h"


using namespace std;

Record::~Record(){};
Record::Record(std::string path){
	inputPath = path;
	fileCount = 0;
	dataCount = 0;
	buildRecord();
};

//TODO: caseInsensitive not implemented yet
char * Record::searchFactory(char *text, char *recordLanguage, std::string pattern, bool caseInsensitive, unsigned int editDistance) {
	if (strcmp(text, "FORMAT_ERROR")==0)
		return NULL;
	else if ((strcmp(recordLanguage,"ChineseT")==0 )|| editDistance == 0)
		return strstr(text, pattern.c_str());
	else
		return fuzzySearch(text, pattern.c_str(), editDistance);
}

void Record::searchId(char *id, char *recordLanguage, std::string pattern, int &searchScore, int &searchMatchCount, 
											bool caseInsensitive, unsigned int editDistance){
	if((searchFactory(id, recordLanguage, pattern.c_str(), caseInsensitive, editDistance))>0){
				searchScore += idWeight;
				searchMatchCount++;
	}
}

void Record::searchTitle(char *title, char *recordLanguage, std::string pattern, int &searchScore, int &searchMatchCount,
												bool caseInsensitive, unsigned int editDistance){
	if((searchFactory(title, recordLanguage, pattern.c_str(), caseInsensitive, editDistance))!=NULL){
		searchScore += titleWeight;
		searchMatchCount++;
	}
}

void Record::searchContent(char *content, char *recordLanguage, std::vector <std::string> &searchPatterns, int recordIndex, int &searchScore, int &searchMatchCount,
													bool caseInsensitive, unsigned int editDistance){
	char *text, *found;
	int foundLocation;
	double tmpScore;
	// NOTE: array of size searchPatterns.size();
	std::vector <std::tuple <int, int, int>> foundTuple;
	std::vector <std::vector <std::tuple <int,int,int>>> patternLocationTuples;

	for (auto searchPattern: searchPatterns){
		text = content;
		found = NULL;
		while((found = searchFactory(text, recordLanguage, searchPattern.c_str(), caseInsensitive, editDistance))!=NULL){
			foundLocation = found - content;
			if (!rank[recordIndex].isDefaultRanking())
				foundTuple.push_back(rank[recordIndex].getRankTreeTuple(foundLocation));
			text = found + searchPattern.length();
			searchMatchCount++;
		}
		patternLocationTuples.push_back(foundTuple);
		foundTuple.clear();
	}
	if (rank[recordIndex].isDefaultRanking())
		tmpScore= searchMatchCount * contentWeight;
	else
		tmpScore= rank[recordIndex].getAdvancedRankingScore(patternLocationTuples);

	// TODO: Adjust tmpScore according to the % of the pattern in the content

	// NOT ACCURATE AT ALL, it will ignore little match count
	//tmpScore = double(titleWeight * searchMatchCount / data[recordIndex].approxCharactersCount);
	//std::cout << searchMatchCount << std::endl;
	//std::cout << data[recordIndex].approxCharactersCount << std::endl;
	searchScore += tmpScore;
}

void Record::searchAndSortWithRank(std::string pattern, Result &searchResult, bool caseInsensitive, unsigned int editDistance ){
	int searchScore, searchMatchCount;
	std::vector <std::string> searchPatterns = parseSearchQuery(pattern);

	for (int i=0; i < dataCount; i++){
			searchScore = searchMatchCount = 0;
			for (auto searchPattern: searchPatterns){
				searchId(data[i].id, data[i].language, searchPattern, searchScore, searchMatchCount, caseInsensitive, editDistance);
				searchTitle(data[i].title, data[i].language, searchPattern, searchScore, searchMatchCount, caseInsensitive, editDistance);
			}
			searchContent(data[i].content, data[i].language, searchPatterns, i, searchScore, searchMatchCount, caseInsensitive, editDistance);

			if (searchMatchCount > 0 && searchScore > 0){
					std::string bookTitle(data[i].title);
					searchResult.insertResult(bookTitle, searchScore, searchMatchCount);
			}
	}

}

void Record::buildRecord(){
	checkPathAndSetFileVectors();
	readFileThenSetRecordAndRank();
};


// TODO: remove last character:newline
void Record::readFileThenSetRecordAndRank(){
	FILE *fptr;
	char *line = NULL;
	char prefix[5];
	size_t len = 0, read;
	data = (struct record*) malloc(sizeof(struct record));
	bool isNewRecord = true;

	for (int i=0; i<fileCount; i++){
		fptr = fopen(rawfiles[i].c_str(), "r");
		int dataCountForCurrentFile = 0;
		while((read = getline(&line, &len, fptr)) != -1){
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
}

void Record::handlePrefixCases(int &dataCountForCurrentFile, size_t &read, char *&line, bool &isNewRecord) {
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


int Record::getRecordCharactersCount(size_t lineCharCount, int prefixCount, char *& line, char *language) {
	if (strcmp(language, "ChineseT") == 0)
		return (lineCharCount - prefixCount)/3; // assume all chinese characters
	else
		return  countWords(line); // will count the prefix as a redundant word
}

void Record::incrementLocalFileDataCountAndDataCount(int &currentFileDataCount) {
	dataCount += 1;
	currentFileDataCount += 1;
}


void Record::handleMalformedCases(std::string malformType, int &dataCountForCurrentFile, bool &isNewRecord){
	incrementLocalFileDataCountAndDataCount(dataCountForCurrentFile);
	createAndAssignDefaultStructData();
	std::cout << malformType << std::endl;
	isNewRecord = true;	
}

// TODO: malloc error handling
void Record::createMemoryThenInsert(char *&target, char *&source, int offset,  size_t &size) {
	target= (char *) malloc(size-offset); // +1
	if (target == NULL){
		//throw std::bad_alloc("Memory not enough");
	}
	if (size == offset) { // empty source
		target[0] = '\0';
	}else {
		strcpy(target, (source+offset));
		target[size-offset-1] = '\0';
	}
}

int Record::setPrefixAndReturnOffset(std::string &prefix, bool &isPrefixToolong,char *&line){
	int offset=1;
	while(line[offset]!=':'){
		prefix += line[offset];
		offset++;
		if (offset == 30) {
			isPrefixToolong = true;
			break;
		}
	}
	offset += 1; // skip ':'
	return offset;
}

// TODO: malloc error handling
void Record::createAndAssignDefaultStructData(){
	struct record *moreData = NULL;
	moreData = (struct record *) realloc(data, dataCount*sizeof(struct record));
	if (moreData == NULL){
		//throw std::bad_alloc("Memory not enough");
	}
	else {
		data = moreData;
		data[dataCount-1].id = strdup("FORMAT_ERROR");
		data[dataCount-1].title = strdup("FORMAT_ERROR");
		data[dataCount-1].content = strdup("FORMAT_ERROR");
		data[dataCount-1].language = strdup("FORMAT_ERROR");
	}
}

void Record::insertAllRanksForCurrentFile(std::string &tagPath, int dataCountForCurrentFile) {
	// NOTE: Rule: if there is more than one record in the file
	// The index-file (.tags) must be assigned to the first record

	Ranking newRank(tagPath);
	rank.push_back(newRank);
	for (int i=0; i < dataCountForCurrentFile-1; i++) {
		Ranking newRank("NO-INDEX_FILE");
		rank.push_back(newRank);
	}
}

void Record::detectLanguage(const char* src, char *&recordLanguage){ 
		bool is_plain_text = true;
		bool do_allow_extended_languages = true;
		bool do_pick_summary_language = false;
		bool do_remove_weak_matches = false;
		bool is_reliable;
		Language plus_one = UNKNOWN_LANGUAGE;
		const char* tld_hint = NULL;
		int encoding_hint = UNKNOWN_ENCODING;
		Language language_hint = UNKNOWN_LANGUAGE;

		double normalized_score3[3];
		Language language3[3];
		int percent3[3];
		int text_bytes;

		Language lang;
		lang = CompactLangDet::DetectLanguage(0,
		                                      src, strlen(src),
		                                      is_plain_text,
		                                      do_allow_extended_languages,
		                                      do_pick_summary_language,
		                                      do_remove_weak_matches,
		                                      tld_hint,
		                                      encoding_hint,
		                                      language_hint,
		                                      language3,
		                                      percent3,
		                                      normalized_score3,
		                                      &text_bytes,
		                                      &is_reliable);

		recordLanguage = (char*) malloc(sizeof(char)*strlen(LanguageName(lang)));
		strcpy(recordLanguage, LanguageName(lang));
		//printf("----[ Text (detected: %s) ]----\n", recordLanguage);
}

void Record::checkPathAndSetFileVectors(){
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

int Record::getFileCount(){
	return fileCount;
}

int Record::getRecordCount() {
	return dataCount;
};

