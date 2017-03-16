#ifndef CRGREP_UTILS_H_
#define CRGREP_UTILS_H_


bool replace(std::string& str, const std::string& from, const std::string& to);
std::string removePrefixPath(const std::string& str);

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);
template<typename Out>
void split(const std::string &s, char delim, Out result);
std::vector<std::string> split(const std::string &s, char delim);

std::vector<std::string> parseSearchQuery(std::string &s);

inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int isDir(const std::string &name);



#endif