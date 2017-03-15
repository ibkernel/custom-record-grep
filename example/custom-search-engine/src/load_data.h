#ifndef CRGREP_LOAD_DATA_H_
#define CRGREP_LOAD_DATA_H_



struct record {
	char *id;
	char *title;
	char *content;
};
inline bool exists(const std::string& name);
int isDir(const std::string &name);
int loadData(std::string path, struct record *&searchData);
int loadData2(std::string path, struct record **searchData);

#endif // CRGREP_LOAD_DATA_H_