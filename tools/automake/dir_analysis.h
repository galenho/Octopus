#pragma once

#include "utility.h"

enum ProgramType
{
	PROGRAM_TYPE_EXE = 0,
	PROGRAM_TYPE_LIB = 1,
	PROGRAM_TYPE_DLL = 2
};

struct RunInfo
{
	string libs;
	string sub_addition_option;
	string make_addition_option;
};

struct ProjectInfo
{
	string src_dir;
	string out_put;
	int program_type;

	string include_dirs;
	string lib_dirs;

	RunInfo debug_info;
	RunInfo release_info;
};

struct CppConfig
{
	string src_path;
	string thread_count;
	std::vector<ProjectInfo> project_list;
};

class FileInfo
{
public:
	FileInfo() {}
	~FileInfo() {}

public:
	string name;			// abc.cpp
	string name_before;		// abc
	string name_ext;		// cpp
	string full_path;
};

class DirectoryInfo
{
public:
	DirectoryInfo() 
	{
		cpp_file_count = 0;
		c_file_count = 0;
	}

	~DirectoryInfo()
	{
		for (size_t n=0; n< file_list.size(); n++)
		{
			delete file_list[n];
		}

		for (size_t n = 0; n < dir_list.size(); n++)
		{
			delete dir_list[n];
		}
	}

	void CheckHaveCppOrC()
	{
		for (size_t n = 0; n < file_list.size(); n++)
		{
			if (file_list[n]->name_ext == "cpp")
			{
				cpp_file_count++;
				continue;
			}

			if (file_list[n]->name_ext == "c")
			{
				c_file_count++;
				continue;
			}
		}
	}

public:
	string name;
	string full_path;	  //ÍêÕûÂ·¾¶
	std::vector<FileInfo*> file_list;
	std::vector<DirectoryInfo*> dir_list;
	
	int  cpp_file_count;
	int  c_file_count;
};

class DirAnalysis
{
public:
	DirAnalysis(string src_path, ProjectInfo& project_info);
	~DirAnalysis();

	bool OperateProject();

private:
	bool BuildObjects(bool is_debug);

	bool BuildSubDir(bool is_debug);
	bool BuildSubDir(bool is_debug, DirectoryInfo* dir_info);

	void BuildMakeFileDir(DirectoryInfo* dir_info, string& content);
	bool BuildMakeFile(bool is_debug);
	
	void BuildSourcesDir(DirectoryInfo* dir_info, string& content);
	bool BuildSources(bool is_debug);

private:
	string src_path_;
	ProjectInfo project_info_;

	DirectoryInfo* dir_info_;

	string public_header_;
};
