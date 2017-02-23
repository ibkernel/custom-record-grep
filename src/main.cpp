#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include <tclap/CmdLine.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>

using namespace std;

struct record {
	char *id;
	char *title;
	char *content;
};


struct record *data = (struct record*) malloc(sizeof(struct record));
struct record *moreData = NULL;

inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int isDir(const std::string &name) {
	struct stat buffer;
	stat(name.c_str(), &buffer);
	if ( !S_ISDIR(buffer.st_mode) )
	{
		return 0;
	}
	return 1;
}



void loadData(string path) {
	FILE *fptr;
	char *line = NULL;
	char prefix[5];
	size_t len = 0;
	ssize_t read;
	int dataCount = 0, fileCount = 0;
	DIR *dir;
	struct dirent *ent;
	std::vector<string> files;

	if (exists(path)){
		if(isDir(path)){
			cout << "it is a dir" << endl;
			if ((dir = opendir(path.c_str())) != NULL) {
				// TODO: Check path correctness (slashes)
				while ((ent = readdir(dir)) != NULL) {
					string newFilePath = path+"/"+ent->d_name;
					if (!isDir(newFilePath)) {
						files.push_back(newFilePath);
						fileCount++;
					}
				}
				closedir(dir);
			}
		}
		else {
			files.push_back(path);
			fileCount++;
		}
	}else {
		cout << "File doesn't exists !" << endl;
		exit(1);
	}
	for (int i=0; i<fileCount; i++){
		cout << files[i] << endl;
	}

	// TODO: Check file format if obeyed
	for (int i=0; i<fileCount; i++){
		fptr = fopen(files[i].c_str(), "r");
		while((read = getline(&line, &len, fptr)) != -1){ //char **restrict lineptr
			memcpy( prefix, line, 4);
			prefix[4] = '\0';
			if(strcmp(prefix, "@id:")==0){
					dataCount++;
					moreData = (struct record *) realloc(data, dataCount*sizeof(struct record));
					if (moreData != NULL){
						data = moreData;
						data[dataCount-1].id = (char *) malloc(read-4);
						strcpy(data[dataCount-1].id, (line+4));
						// cout << (line+4);
						// printf("line: %s\n", line);
					}else {
						free(data);
						puts ("Error (re)allocating memory");
	       		exit (1);
					}
			}else if (strcmp(prefix, "@tit")==0){
					data[dataCount-1].title = (char *) malloc(read-7);
					strcpy(data[dataCount-1].title, (line+7));
			}else if (strcmp(prefix, "@con")==0){
					data[dataCount-1].content = (char *) malloc(read-9);
					strcpy(data[dataCount-1].content, (line+9));
			}else {
					cout << "special things happened 0..0" <<endl;
			}
		}
		fclose(fptr);
	}

	// for(int i=0; i <dataCount; i++) {
	// 	cout << data[i].id;
	// 	cout << data[i].title;
	// }

	return;
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


	// Define a switch and add it to the command line.
	// A switch arg is a boolean argument and only defines a flag that
	// indicates true or false.  In this example the SwitchArg adds itself
	// to the CmdLine object as part of the constructor.  This eliminates
	// the need to call the cmd.add() method.  All args have support in
	// their constructors to add themselves directly to the CmdLine object.
	// It doesn't matter which idiom you choose, they accomplish the same thing.

	// TCLAP::SwitchArg reverseSwitch("r","reverse","Print name backwards", cmd, false);

	// Parse the argv array.
	cmd.parse( argc, argv );

	// Get the value parsed by each arg. 
	std::string scorePath = scorePathArg.getValue();
	std::string query = queryArg.getValue();
	std::string inputPath = inputPathArg.getValue();
	// bool reverseName = reverseSwitch.getValue();


	std::cout << "My score path is: " << scorePath << std::endl;
	std::cout << "Your query is:" << query << std::endl;

	loadData(inputPath);
	free(data);

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}