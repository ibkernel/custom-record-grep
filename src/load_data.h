#ifndef CRGREP_LOAD_H_
#define CRGREP_LOAD_H_
#include <fstream>
struct record {
	char *id;
	char *title;
	char *content;
};

class WeightedTree {
private:
	WeightedTree **chapterNodes;
	int **left;
	int weight;

public:
	WeightedTree();
	~WeightedTree();
	void insert(char tag, int left, int right){
		switch (tag){
			case 'c':
				std::cout << "You got c: " << left << "," << right << std::endl;
				break;
			case 't':
				std::cout << "You got t: " << left << "," << right << std::endl;
				break;
			case 'p':
				std::cout << "You got p: " << left << "," << right << std::endl;
				break;
			case 's':
				std::cout << "You got s: " << left << "," << right << std::endl;
				break;
			default:
				std::cout << "error tag" << std::endl;
				break;
		}
	};
	int search(int foundLocation); // return score
};




class Ranking {
private:
	WeightedTree	*root;
	std::string pathToTagFile;

public:

	Ranking(std::string tagFilePath){
		pathToTagFile = tagFilePath;
		root = NULL;
	};
	~Ranking(){};

	void buildRank() {
		// std::cout << pathToTagFile << std::endl;
		std::ifstream infile(pathToTagFile);
		std::string tagName;
		int left, right;
		while(infile >> tagName >> left >> right){
			// std::cout << tagName <<"\t"<<left<<"\t" << right << std::endl;
			root->insert(tagName.at(0), left, right);
		}
	}

};

void print();

inline bool exists(const std::string& name);
int isDir(const std::string &name);
int loadData(std::string path, struct record *&data);
int loadData2(std::string path, struct record **data);

void createWeightTree(std::string path, struct weightTree *&rankingData);

#endif