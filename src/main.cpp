#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <tclap/CmdLine.h>
#include "record.h"
#include "utils.h"
#include "book_formatter.h"

using namespace std;


// bool sortScore(struct resultFormat a, struct resultFormat b){
// 	return (a.score > b.score) ? true : false;
// }

int main(int argc, char** argv){

	// Wrap everything in a try block.  Do this every time, 
	// because exceptions will be thrown for problems.
	try {  

	TCLAP::CmdLine cmd("Command description message", ' ', "0.1");

	TCLAP::UnlabeledValueArg<std::string> queryArg("query", "Query string", true, "0","string");
	cmd.add( queryArg );

	TCLAP::UnlabeledValueArg<std::string> inputPathArg("pathToLoad", "Path to file or directory", true, "0","string");
	cmd.add( inputPathArg );

	TCLAP::ValueArg<std::string> scorePathArg("s","score","Path to custom score description file",false,"default","string");
	cmd.add( scorePathArg );

	TCLAP::MultiArg<std::string> formatDataArg("f", "format", "Path to download dir, formatted dir, downloaded list file, merged file and stop words file.", false,"string" );
	cmd.add( formatDataArg );
	// Parse the argv array.
	cmd.parse( argc, argv );

	// Get the value parsed by each arg. 
	std::string scorePath = scorePathArg.getValue();
	std::string query = queryArg.getValue();
	std::string inputPath = inputPathArg.getValue();
	std::vector <std::string> paths = formatDataArg.getValue();


	std::cout << "My score path is: " << scorePath << std::endl;
	std::cout << "Your query is:" << query << std::endl;

	if (paths.size() == 0){
		Record records(inputPath);
		std::vector <std::tuple <std::string, int>> result;

		while(1){
			string searchPatterns;
			cout << "What would you like to search ('q' to exit):" ;
			std::getline(cin, searchPatterns);
			if (searchPatterns == "q" || searchPatterns == "Q")
				break;
			// NOTE: currently not working with chinese characters.
			//records.searchAndSortWithRank(searchPatterns, 0, 1); //fuzzy search with 1 edit distance tolerance
			result = records.searchAndSortWithRank(searchPatterns);
			for (auto x:result){
				std::cout << "Book :" << std::get<0>(x);
				std::cout << "Rank score:" << std::get<1>(x) << std::endl;
			}
			result.clear();
			std::cout << "-----------------------------" << '\n';
		}
	}else {
		if (paths.size() < 4){
			cout << "We need at least 4 arguments for formatting" << endl;
		}else{
			// TODO: check validity of paths.
			std::clock_t c_start = std::clock();
			if (paths.size() == 4)
				BookFormatter formatBook(paths[0], paths[1], paths[2], paths[3]);
			else if(paths.size() == 5)
				BookFormatter formatBook(paths[0], paths[1], paths[2], paths[3], paths[4]);
			std::cout << float( std::clock () - c_start ) /  CLOCKS_PER_SEC << std::endl;
		}
	}

	return 0;

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}