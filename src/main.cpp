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
#include "load_data.h"
#include "search.h"
#include "load_data.h"
#include "book_formatter.h"
using namespace std;


bool sortScore(struct resultFormat a, struct resultFormat b){
	return (a.score > b.score) ? true : false;
}

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

	//Record records(inputPath); 
	if (paths.size() == 0){
		Record records("../data/formattedData/一念永恒.txt");
		//Record records(inputPath); 
		while(1){
			string searchPattern;
			cout << "Search something, I shall return it's location if there are any, of course:) :" ;
			cin >> searchPattern;
			records.search(searchPattern);
		}
	}else {
		if (paths.size() < 4){
			cout << "We need at least 4 arguments for formatting" << endl;
		}else{
			// TODO: check validity of paths.
			if (paths.size() == 4)
				BookFormatter formatBook(paths[0], paths[1], paths[2], paths[3]);
			else if(paths.size() == 5)
				BookFormatter formatBook(paths[0], paths[1], paths[2], paths[3], paths[4]);
			// for (int i=0;i<paths.size();i++)
			// 	cout << paths[i] << endl;
		}
	}
	// records.printRecord();
	//Record *records = new Record(inputPath);
	// records->printRecord();

	// vector<struct resultFormat> result;
	// int resultCount = search(query, data, dataCount, result);
	// sort(result.begin(), result.end(), sortScore);

	/* Print all data */
	// for(int i=0; i <dataCount; i++) {
	// 	cout << data[i].id;
	// 	cout << data[i].title;
	// 	// cout << data[i].content;
	// }


	/* Print query result */
	// for (int i=0; i< resultCount; i++){
	// 		cout << data[result[i].id].id;
	// 		cout << data[result[i].id].title;
	// 		cout << result[i].score << endl;
	// }

	// while(1){
	// 	string searchPattern;
	// 	cout << "Search something, I shall return it's location if there are any, of course:) :" ;
	// 	cin >> searchPattern;
	// 	records.search(searchPattern);
	// }

	return 0;

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}