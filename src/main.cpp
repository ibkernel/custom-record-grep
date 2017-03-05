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


using namespace std;


bool sortScore(struct resultFormat a, struct resultFormat b){
	return (a.score > b.score) ? true : false;
}

int main(int argc, char** argv)
{

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

	// Parse the argv array.
	cmd.parse( argc, argv );

	//struct record *data = (struct record *)malloc(sizeof(struct record));
	struct record *data = NULL;

	// Get the value parsed by each arg. 
	std::string scorePath = scorePathArg.getValue();
	std::string query = queryArg.getValue();
	std::string inputPath = inputPathArg.getValue();
	// bool reverseName = reverseSwitch.getValue();


	std::cout << "My score path is: " << scorePath << std::endl;
	std::cout << "Your query is:" << query << std::endl;
	int dataCount = loadData(inputPath, data);
	//int dataCount = loadData2(inputPath, &data);

	vector<struct resultFormat> result;
	int resultCount = search(query, data, dataCount, result);
	sort(result.begin(), result.end(), sortScore);

	/* Print all data */
	// for(int i=0; i <dataCount; i++) {
	// 	cout << data[i].id;
	// 	cout << data[i].title;
	// 	// cout << data[i].content;
	// }


	/* Print query result */
	for (int i=0; i< resultCount; i++){
			cout << data[result[i].id].id;
			cout << data[result[i].id].title;
			cout << result[i].score << endl;
	}

	free(data);
	return 0;

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}