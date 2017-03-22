#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include "utils.h"
#include <cctype>

int countWords(const char* str)
{
   if (str == NULL)
      return 0;  

   bool inSpaces = true;
   int numWords = 0;
   while (*str != '\0')
   {
      if (std::isspace(*str))
      {
         inSpaces = true;
      }
      else if (inSpaces)
      {
         numWords++;
         inSpaces = false;
      }

      ++str;
   }

   return numWords;
}

// replace all
void ReplaceStringInPlace(std::string& subject,
                          const std::string& search,
                          const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
};

bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

std::string removePrefixPath(const std::string& str)
{
	size_t found = str.find_last_of("/\\");
	return str.substr(found+1);
}


std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

template<typename Out>
void split(const std::string &s, char delim, Out result)
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if(!item.empty())
			*(result++) = item;
	}
}

int isDir(const std::string &name)
{
	struct stat buffer;
	stat(name.c_str(), &buffer);
	if ( !S_ISDIR(buffer.st_mode) )
	{
		return 0;
	}
	return 1;
}

// TODO: separte "" exact query and others
std::vector<std::string> parseInteractiveSearchQuery(std::string &searchQuery)
{
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

std::vector<std::tuple<std::string, bool, bool>>parseSearchQuery(std::vector <std::string> &searchQueries)
{
	std::vector <std::tuple <std::string, bool, bool>>searchPatterns;
	for (auto q: searchQueries){
		std::size_t foundObligation = q.find("+");
		std::size_t foundNeglate = q.find("-");

		if (foundNeglate == std::string::npos && foundObligation == std::string::npos){
			std::cout << "Casual exact match: " << q << std::endl;
			searchPatterns.push_back(q);
		}else {
			for (auto slicedQuery : split(q, ' ')){
				// Handle Special queries : +, -
				bool isMustHave = false, isMustNotHave = false;
				switch (slicedQuery.at(0)){
					case '+':
						slicedQuery.erase(0,1);
						isMustHave = true;
						searchPatterns.push_back(std::make_tuple(slicedQuery, isMustHave, isMustNotHave));
						break;
					case '-':
						slicedQuery.erase(0,1);
						isMustNotHave = true;
						searchPatterns.push_back(std::make_tuple(slicedQuery, isMustHave, isMustNotHave));
						break;
					default:
						searchPatterns.push_back(std::make_tuple(slicedQuery, isMustHave, isMustNotHave));
						break;
				}
			}
		}
	}
	return searchPatterns;
}
