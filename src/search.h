#ifndef CRGREP_SEARCH_H_
#define CRGREP_SEARCH_H_

struct resultFormat {
	int id;
	int score;
};

int search(std::string pattern, struct record *&data, int dataCount, std::vector<struct resultFormat> &result);

#endif
