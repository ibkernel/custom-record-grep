#ifndef CRGREP_UTILS_H_
#define CRGREP_UTILS_H_


inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int isDir(const std::string &name);



#endif