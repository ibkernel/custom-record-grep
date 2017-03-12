#ifndef CGREP_BOOK_FORMATTER_H_
#define	CGREP_BOOK_FORMATTER_H_

#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <tuple>


void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);
std::tuple<std::string, int> getTagTuple(std::string tagType, long tagLocation);

class BookFormatter{
private:
	std::string pathToFormattedDir;
	std::string pathToDownloadDir;
	std::string pathToMergedFile;
	std::string pathToDownloadedList;
	std::string pathToStopWords;

	std::vector <std::string> stopWords;
	std::set <std::string> alreadyMerged;
	std::vector <std::string> getNotMergedBookList();


	void mergeWithoutTags();
	void mergeChapters(std::string bookTitle);
	void writeRecordHeaderToFile(std::string bookTitle);
	void formatThenMerge(std::string pathToRawChapter, long &char_count, int &chapter_num, 
				int &title_num, int &paragraph_num, int &sentense_num, std::string bookTitle);
	void writeTagInfoToFile(std::deque <std::tuple <std::string, long>> &tagQueue, std::string bookTitle);
	void lineFormatter(std::string &line,int &sentense_num, long &char_count,
	 std::deque <std::tuple <std::string, long>> &tagQueue);
	std::vector <std::string> getToMergeChaptersPath(std::string bookTitle);

public:
	BookFormatter(std::string pathSource, std::string pathDest, std::string pathToDownloadedSource,
	 							std::string pathToMergedDest);
	BookFormatter(std::string pathSource, std::string pathDest, std::string pathToDownloadedSource,
	 							std::string pathToMergedDest, std::string pathStopWords);
	~BookFormatter(){};

};



#endif