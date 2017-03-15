#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include "load_data.h"

using namespace std;


inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int isDir(const std::string &name) {
	struct stat buffer;
	stat(name.c_str(), &buffer);
	if ( !S_ISDIR(buffer.st_mode) )
	{
		return 0;
	}
	return 1;
}



int loadData(string path, struct record *&searchData) {
	FILE *fptr;
	char *line = NULL;
	char prefix[5];
	size_t len = 0;
	ssize_t read;
	int dataCount = 0, fileCount = 0;
	DIR *dir;
	struct dirent *ent;
	std::vector<string> files;
	struct record *moreData = NULL;
	if (exists(path)){
		if(isDir(path)){
			if ((dir = opendir(path.c_str())) != NULL) {
				if (path[path.length()-1] != '/')
					path = path+"/";
				while ((ent = readdir(dir)) != NULL) {
					string newFilePath = path+ent->d_name;
					if (!isDir(newFilePath)) {
						files.push_back(newFilePath);
						fileCount++;
					}
				}
				closedir(dir);
			}
		}
		else {
			files.push_back(path);
			fileCount++;
		}
	}else {
		cout << "File doesn't exists !" << endl;
		exit(1);
	}

	// NOTE: A better way to handle format inconsistency
	for (int i=0; i<fileCount; i++){
		fptr = fopen(files[i].c_str(), "r");
		while((read = getline(&line, &len, fptr)) != -1){ //char **restrict lineptr
			memcpy( prefix, line, 4);
			prefix[4] = '\0';
			if(strcmp(prefix, "@id:")==0){
					dataCount++;
					moreData = (struct record *) realloc(searchData, dataCount*sizeof(struct record));
					if (moreData != NULL){
						searchData = moreData;
						// searchData[dataCount-1].score = 0;
						searchData[dataCount-1].id = (char *) malloc(read-4);
						strcpy(searchData[dataCount-1].id, (line+4));
						// cout << (line+4);
						// printf("line: %s\n", line);
					}else {
						free(searchData);
						puts ("Error (re)allocating memory");
	       		exit (1);
					}
			}else if (strcmp(prefix, "@tit")==0){
					searchData[dataCount-1].title = (char *) malloc(read-7);
					strcpy(searchData[dataCount-1].title, (line+7));
			}else if (strcmp(prefix, "@con")==0){
					searchData[dataCount-1].content = (char *) malloc(read-9);
					strcpy(searchData[dataCount-1].content, (line+9));
			}else {
					cout << "File " +files[i]+ " did not obeyed input format" << endl;
					break;
			}
		}
		fclose(fptr);
	}

	return dataCount;
}

int loadData2(string path, struct record **searchData) {
	FILE *fptr;
	char *line = NULL;
	char prefix[5];
	size_t len = 0;
	ssize_t read;
	int dataCount = 0, fileCount = 0;
	DIR *dir;
	struct dirent *ent;
	std::vector<string> files;
	struct record *moreData = NULL;

	if (exists(path)){
		if(isDir(path)){
			if ((dir = opendir(path.c_str())) != NULL) {
				if (path[path.length()-1] != '/')
					path = path+"/";
				while ((ent = readdir(dir)) != NULL) {
					string newFilePath = path+ent->d_name;
					if (!isDir(newFilePath)) {
						files.push_back(newFilePath);
						fileCount++;
					}
				}
				closedir(dir);
			}
		}
		else {
			files.push_back(path);
			fileCount++;
		}
	}else {
		cout << "File doesn't exists !" << endl;
		exit(1);
	}

	// NOTE: A better way to handle format inconsistency
	for (int i=0; i<fileCount; i++){
		fptr = fopen(files[i].c_str(), "r");
		while((read = getline(&line, &len, fptr)) != -1){ //char **restrict lineptr
			memcpy( prefix, line, 4);
			prefix[4] = '\0';
			if(strcmp(prefix, "@id:")==0){
					dataCount++;
					moreData = (struct record *) realloc(*searchData, dataCount*sizeof(struct record));
					if (moreData != NULL){
						*searchData = moreData;
						// (*searchData+dataCount-1)->score = 0;
						(*searchData+dataCount-1)->id = (char *) malloc(read-4);
						strcpy((*searchData+dataCount-1)->id, (line+4));
					}else {
						free(*searchData);
						puts ("Error (re)allocating memory");
	       		exit (1);
					}
			}else if (strcmp(prefix, "@tit")==0){
					(*searchData+dataCount-1)->title = (char *) malloc(read-7);
					strcpy((*searchData+dataCount-1)->title, (line+7));
			}else if (strcmp(prefix, "@con")==0){
					(*searchData+dataCount-1)->content = (char *) malloc(read-9);
					strcpy((*searchData+dataCount-1)->content, (line+9));
			}else {
					cout << "File " +files[i]+ " did not obeyed input format" << endl;
					break;
			}
		}
		fclose(fptr);
	}

	return dataCount;
}
