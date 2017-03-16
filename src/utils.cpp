#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include "utils.h"


bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

std::string removePrefixPath(const std::string& str) {
	size_t found = str.find_last_of("/\\");
	return str.substr(found+1);
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if(!item.empty())
			*(result++) = item;
	}
}

// TODO: Must have (+) and Must Not have (-)
std::vector<std::string> parseSearchQuery(std::string &searchQuery) {
	std::size_t foundFirst = searchQuery.find("\"");

	bool openFlag = false, closingFlag = false;
	if (foundFirst!=std::string::npos){
		//std::cout << "First Needle found at " << foundFirst << "\n";
		openFlag = true;
	}
	std::size_t foundSecond = searchQuery.find("\"", foundFirst+1);
	if (foundSecond!=std::string::npos){
		//std::cout << "Second Needle found at " << foundSecond << "\n";
		closingFlag = true;
	}
	if (openFlag && closingFlag){
		//std::cout << "This is an exact query :" << searchQuery.substr(foundFirst+1, foundSecond-1) <<"\n";
		std::vector <std::string> parsedQuery = {searchQuery.substr(foundFirst+1, foundSecond-1)};
		return parsedQuery;
	}
	else
		return split(searchQuery, ' ');
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
