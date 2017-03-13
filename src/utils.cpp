#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include "utils.h"



int isDir(const std::string &name) {
	struct stat buffer;
	stat(name.c_str(), &buffer);
	if ( !S_ISDIR(buffer.st_mode) )
	{
		return 0;
	}
	return 1;
}
