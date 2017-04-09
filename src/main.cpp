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

  TCLAP::MultiArg<std::string> queryArg("q", "query", "Query string", false, "string");
  cmd.add( queryArg );

  TCLAP::ValueArg<std::string> inputPathArg("p", "path", "Path to file or directory", false, "default path","string");
  cmd.add( inputPathArg );

  TCLAP::ValueArg<int> outputSizeArg("o", "output", "Output size", false, -1,"int");
  cmd.add( outputSizeArg );

  TCLAP::SwitchArg orderSwitch("a","ascend","Result in ascending order", cmd, false);
  TCLAP::SwitchArg interactiveSwitch("i","interactive","interactive mode", cmd, false);

  TCLAP::ValueArg<int> editDistanceArg("d","distance","Number of edit distance tolerated",false, 0,"int");
  cmd.add( editDistanceArg );

  TCLAP::MultiArg<std::string> formatDataArg("f", "format", "Path to download dir, formatted dir, downloaded list file, merged file and stop words file.", false,"string" );
  cmd.add( formatDataArg );

  TCLAP::ValueArg<std::string> stopWordPathArg("s", "stopword", "Path to stopword file", false, "no_path","string");
  cmd.add( stopWordPathArg );

  // Parse the argv array.
  cmd.parse( argc, argv );

  // Get the value parsed by each arg. 
  bool isAscending = orderSwitch.getValue();
  bool isInteractive = interactiveSwitch.getValue();
  int distance = editDistanceArg.getValue();
  int outputSize = outputSizeArg.getValue();

  std::string stopWordPath = stopWordPathArg.getValue();
  std::string inputPath = inputPathArg.getValue();
  std::vector <std::string> paths = formatDataArg.getValue();
  std::vector <std::string> queries = queryArg.getValue();


  if (paths.size() == 0){
    Record *records = new Record(inputPath);
    Result searchResult(outputSize);

    cout << "---------Debug info---------" << endl << endl;
    cout << "Record Count: " << records->getRecordCount() << endl;
    cout << "File Count: " << records->getFileCount() << endl;
    // records.dubugPrintAllRecords();
    // cout << endl;
    cout << "----------info End----------" << endl << endl;

    cout << "---------Data loaded---------" << endl;
    if (queries.size() > 0){
      records->searchAndSortWithRank(queries,searchResult, 0, distance);
      searchResult.printResult(isAscending);
      searchResult.reset();
      queries.clear();
    }

    if (isInteractive){
      cout << "---------interactive mode---------" << endl;
      std::vector <std::string> interactiveQuery;
      while(1){
        string searchPatterns;
        cout << "What would you like to search ('q' to exit):" ;
        std::getline(cin, searchPatterns);
        if (searchPatterns == "q" || searchPatterns == "Q")
          break;
        interactiveQuery = parseInteractiveSearchQuery(searchPatterns);
        // NOTE: currently not working with chinese characters.
        std::cout << "-------------tolerated search with distance of " << distance << "----------------" << '\n';
        records->searchAndSortWithRank(interactiveQuery,searchResult, 0, distance); 
        searchResult.printResult(isAscending);
        searchResult.reset();
        interactiveQuery.clear();
      }
    }
    delete records;
  }else {
    if (paths.size() != 2){
      cout << "We need 2 arguments for formatting" << endl;
    }else{
      std::clock_t c_start = std::clock();
      if (stopWordPath == "no_path")
        Formatter formatData(paths[0], paths[1]);
      else
        Formatter formatData(paths[0], paths[1], stopWordPath);
      std::cout << "Format time: " << float( std::clock () - c_start ) /  CLOCKS_PER_SEC << std::endl;
    }
  }

  return 0;

  } catch (TCLAP::ArgException &e)  // catch any exceptions
  { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}