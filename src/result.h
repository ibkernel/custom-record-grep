#ifndef CRGREP_RESULT_H_
#define CRGREP_RESULT_H_

struct result {
	std::string recordTitle;
	int recordScore;
	int recordMatchCount;
};

inline bool sortScore(struct result a, struct result b){
		return (a.recordScore > b.recordScore) ? true : false;
};


class Result {
private:
	std::vector <struct result> searchResult;
	int resultCount;


public:
	Result();
	~Result();

	void insertResult(std::string title, int searchScore, int searchMatchCount);
	void sort(bool order=false);
	void printResult(bool order=false);
	void reset();
};


#endif