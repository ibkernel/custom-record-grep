#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>
#include <iterator>
#include "utils.h"
#include <cctype>
#include <string>
#include <errno.h>    // errno, ENOENT, EEXIST

#include "third_party_headers.h"


#if defined(_WIN32)
#include <direct.h>   // _mkdir
#endif

/* count words without spaces */
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

/* replace all `search` pattern in `subject` with `replace` */
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

/* replace first match of to `from` in `str` with `to` */
bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

/* remove final '/' or '\' of the string */
std::string removePrefixPath(const std::string& str)
{
  size_t found = str.find_last_of("/\\");
  return str.substr(found+1);
}

/* split words by spaces */
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

/**
 * parseInteractiveSearchQuery - parse queries on interactive mode only
 * @searchQuery: search string
 * 
 * return the first string quoted by '"' if any or return splitted string
 */
std::vector<std::string> parseInteractiveSearchQuery(std::string &searchQuery)
{
  std::size_t foundFirst = searchQuery.find("\"");

  bool openFlag = false, closingFlag = false;
  if (foundFirst!=std::string::npos){
    openFlag = true;
  }
  std::size_t foundSecond = searchQuery.find("\"", foundFirst+1);
  if (foundSecond!=std::string::npos){
    closingFlag = true;
  }
  if (openFlag && closingFlag){
    std::vector <std::string> parsedQuery = {searchQuery.substr(foundFirst+1, foundSecond-1)};
    return parsedQuery;
  }
  else
    return split(searchQuery, ' ');
}

/**
 * parseSearchQuery - parse queries
 * @searchQuery: search query vector
 * 
 * return tuple<query string, if must have, if must not>
 */
std::vector<std::tuple<std::string, bool, bool>>parseSearchQuery(std::vector <std::string> &searchQueries)
{
  std::vector <std::tuple <std::string, bool, bool>>searchPatterns;
  for (auto q: searchQueries){
    std::size_t foundObligation = q.find("+");
    std::size_t foundNeglate = q.find("-");

    if (foundNeglate == std::string::npos && foundObligation == std::string::npos){
      bool isMustHave = false, isMustNotHave = false;
      switch (q.at(0)){
        case '+':
          q.erase(0,1);
          isMustHave = true;
          searchPatterns.push_back(std::make_tuple(q, isMustHave, isMustNotHave));
          break;
        case '-':
          q.erase(0,1);
          isMustNotHave = true;
          searchPatterns.push_back(std::make_tuple(q, isMustHave, isMustNotHave));
          break;
        default:
          searchPatterns.push_back(std::make_tuple(q, isMustHave, isMustNotHave));
          break;
      }
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


/* used to help crgrep determine the language of the record */
void detectLanguageAndUpdateLanguageCount(const char* src, int &chineseCount, int &otherCount)
{ 
    bool is_plain_text = true;
    bool do_allow_extended_languages = true;
    bool do_pick_summary_language = false;
    bool do_remove_weak_matches = false;
    bool is_reliable;
    // Language plus_one = UNKNOWN_LANGUAGE;
    const char* tld_hint = NULL;
    int encoding_hint = UNKNOWN_ENCODING;
    Language language_hint = UNKNOWN_LANGUAGE;

    double normalized_score3[3];
    Language language3[3];
    int percent3[3];
    int text_bytes;

    Language lang;
    lang = CompactLangDet::DetectLanguage(0,
                                          src, strlen(src),
                                          is_plain_text,
                                          do_allow_extended_languages,
                                          do_pick_summary_language,
                                          do_remove_weak_matches,
                                          tld_hint,
                                          encoding_hint,
                                          language_hint,
                                          language3,
                                          percent3,
                                          normalized_score3,
                                          &text_bytes,
                                          &is_reliable);

    if(strcmp(LanguageName(lang), "ChineseT") == 0)
      chineseCount++;
    else
      otherCount++;
}

/* copy detected language string to char  */
void detectLanguage(const char* src, char *&recordLanguage)
{ 
    bool is_plain_text = true;
    bool do_allow_extended_languages = true;
    bool do_pick_summary_language = false;
    bool do_remove_weak_matches = false;
    bool is_reliable;
    // Language plus_one = UNKNOWN_LANGUAGE;
    const char* tld_hint = NULL;
    int encoding_hint = UNKNOWN_ENCODING;
    Language language_hint = UNKNOWN_LANGUAGE;

    double normalized_score3[3];
    Language language3[3];
    int percent3[3];
    int text_bytes;

    Language lang;
    lang = CompactLangDet::DetectLanguage(0,
                                          src, strlen(src),
                                          is_plain_text,
                                          do_allow_extended_languages,
                                          do_pick_summary_language,
                                          do_remove_weak_matches,
                                          tld_hint,
                                          encoding_hint,
                                          language_hint,
                                          language3,
                                          percent3,
                                          normalized_score3,
                                          &text_bytes,
                                          &is_reliable);

    recordLanguage = (char*) malloc(sizeof(char)*strlen(LanguageName(lang))+1);
    strcpy(recordLanguage, LanguageName(lang));
    //printf("----[ Text (detected: %s) ]----\n", recordLanguage);
}



bool isDirExist(const std::string& path)
{
#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else 
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool makePath(const std::string& path)
{
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
        return true;

    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            size_t pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
#if defined(_WIN32)
        return 0 == _mkdir(path.c_str());
#else 
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        // done!
        return isDirExist(path);

    default:
        return false;
    }
}
