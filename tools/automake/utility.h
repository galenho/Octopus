/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: 
*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef WIN32
#include <io.h>
#include <direct.h> 
#else
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>


#define MAX_PATH_LEN 256

#ifdef WIN32
#define ACCESS(fileName,accessMode) _access(fileName,accessMode)
#define MKDIR(path) _mkdir(path)
#else
#define ACCESS(fileName,accessMode) access(fileName,accessMode)
#define MKDIR(path) mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

using namespace std;

// 创建目录
int32_t CreateDirectory(const std::string& directoryPath);

// 从文件读入到string里
bool readFileIntoString(const char* filename, string& out_data);

// 是否是同样的文件内容(一个文本，一个路径)
void copy_file_text(string& src_data, string& dst_file_path);

// 替换文本
string Replace( const string& orignStr, const string& oldStr, const string& newStr );

// 分割字符
void split(string& str, const string& delim, vector<string>& out_list);

// 换行
string RT();

#endif //_UTILITY_H_