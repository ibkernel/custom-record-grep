#ifndef CRGREP_RESULT_H_
#define CRGREP_RESULT_H_

struct result {
	std::string recordTitle;
	double recordScore;
	int recordMatchCount;
	bool isValid;
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

	int getResultCount();
	std::string getResultTitle(int i );
	double getResultScore(int i);

	void insertResult(std::string title, int searchScore, int searchMatchCount, bool isComplianceToMustAndMustNotHave);
	void sort(bool order=false);
	void printResult(bool order=false);
	void reset();
};


#endif