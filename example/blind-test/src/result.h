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
public:
  Result();
  Result(int size);
  ~Result();

  int getResultCount() const;
  std::string getResultTitle(int i ) const;
  double getResultScore(int i) const;
  int getOutputSize() const;

  void insertResult(std::string title,
                    int searchScore,
                    int searchMatchCount,
                    bool isComplianceToMustAndMustNotHave);
  void sort(bool order=false);
  void printResult(bool order=false);
  void reset();

private:
  std::vector <struct result> searchResult;
  int resultCount;
  int outputSize;
};


#endif