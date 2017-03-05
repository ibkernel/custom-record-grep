#ifndef CRGREP_LOAD_H_
#define CRGREP_LOAD_H_

struct record {
	char *id;
	char *title;
	char *content;
};
void print();

inline bool exists(const std::string& name);
int isDir(const std::string &name);
int loadData(std::string path, struct record *&data);
int loadData2(std::string path, struct record **data);

#endif