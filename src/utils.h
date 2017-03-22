#ifndef CRGREP_UTILS_H_
#define CRGREP_UTILS_H_


bool replace(std::string& str, const std::string& from, const std::string& to);


void ReplaceStringInPlace(std::string& subject,
                          const std::string& search,
                          const std::string& replace);

template<typename Out>
void split(const std::string &s, char delim, Out result);
std::vector<std::string> split(const std::string &s, char delim);
std::string removePrefixPath(const std::string& str);

std::vector<std::tuple<std::string, bool, bool>> parseSearchQuery(std::vector <std::string> &s);
std::vector<std::string> parseInteractiveSearchQuery(std::string &searchQuery);


inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int isDir(const std::string &name);


int countWords(const char* str);

#endif