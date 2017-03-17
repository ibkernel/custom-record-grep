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
#include "result.h"
#include "utils.h"
#include "formatter.h"

using namespace std;

int main(int argc, char** argv){

	try {  

	TCLAP::CmdLine cmd("Command description message", ' ', "0.1");

	TCLAP::ValueArg<std::string> queryArg("q","query", "Query string", false, "default query","string");
	cmd.add( queryArg );

	TCLAP::ValueArg<std::string> inputPathArg("p", "path", "Path to file or directory", false, "default path","string");
	cmd.add( inputPathArg );

	TCLAP::SwitchArg orderSwitch("a","ascend","Result in ascending order", cmd, false);
	TCLAP::SwitchArg interactiveSwitch("i","interactive","interactive mode", cmd, false);

	TCLAP::ValueArg<int> editDistanceArg("d","distance","Number of edit distance tolerated",false, 0,"int");
	cmd.add( editDistanceArg );

	TCLAP::ValueArg<std::string> scorePathArg("s","score","Path to custom score description file",false,"default","string");
	cmd.add( scorePathArg );

	TCLAP::MultiArg<std::string> formatDataArg("f", "format", "Path to download dir, formatted dir, downloaded list file, merged file and stop words file.", false,"string" );
	cmd.add( formatDataArg );
	// Parse the argv array.
	cmd.parse( argc, argv );

	// Get the value parsed by each arg. 
	bool isAscending = orderSwitch.getValue();
	bool isInteractive = interactiveSwitch.getValue();
	int distance = editDistanceArg.getValue();
	std::string scorePath = scorePathArg.getValue();
	std::string query = queryArg.getValue();
	std::string inputPath = inputPathArg.getValue();
	std::vector <std::string> paths = formatDataArg.getValue();


	std::cout << "My score path is: " << scorePath << std::endl;

	if (paths.size() == 0){
		Record records(inputPath);
		Result searchResult;

		cout << "---------Debug info---------" << endl;
		cout << "Record Count: " << records.getRecordCount() << endl;
		cout << "File Count: " << records.getFileCount() << endl;
		//records.dubugPrintAllRecords();
		cout << "----------info End----------" << endl;


		cout << "---------Data loaded---------" << endl;
		if (query != "default query"){
			cout << "Searching for: " << query << endl;
			records.searchAndSortWithRank(query,searchResult, 0, distance);
			searchResult.printResult(isAscending);
			searchResult.reset();
		}

		if (isInteractive){
			cout << "---------interactive mode---------" << endl;
			while(1){
				string searchPatterns;
				cout << "What would you like to search ('q' to exit):" ;
				std::getline(cin, searchPatterns);
				if (searchPatterns == "q" || searchPatterns == "Q")
					break;
				// NOTE: currently not working with chinese characters.
				std::cout << "-------------Fuzzy search with distance of " << distance << "----------------" << '\n';
				records.searchAndSortWithRank(searchPatterns,searchResult, 0, distance); //fuzzy search with 1 edit distance tolerance
				searchResult.printResult(isAscending);
				searchResult.reset();
			}
		}

	}else {
		if (paths.size() < 2){
			cout << "We need at least 2 arguments for formatting" << endl;
		}else{
			// TODO: check validity of paths.
			std::clock_t c_start = std::clock();
			if (paths.size() == 2)
				Formatter formatData(paths[0], paths[1]);
			else if (paths.size() == 3)
				Formatter formatData(paths[0], paths[1], paths[2]);
			// if (paths.size() == 4)
			// 	BookFormatter formatBook(paths[0], paths[1], paths[2], paths[3]);
			// else if(paths.size() == 5)
			// 	BookFormatter formatBook(paths[0], paths[1], paths[2], paths[3], paths[4]);
			std::cout << "Format time: " << float( std::clock () - c_start ) /  CLOCKS_PER_SEC << std::endl;
		}
	}

	return 0;

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}