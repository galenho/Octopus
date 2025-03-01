#include "dir_analysis.h"

#ifdef WIN32
void get_dir_info(DirectoryInfo* dir_info, string mode)
{
	_finddata_t file;
	intptr_t HANDLE;
	string Onepath = dir_info->full_path + "/*.*";
	HANDLE = _findfirst(Onepath.c_str(), &file);
	if (HANDLE == -1L)
	{
		assert(false);
	}

	do
	{
		//判断是否有子目录  
		if (file.attrib & _A_SUBDIR)
		{
			//判断是否为"."当前目录，".."上一层目录
			if ((strcmp(file.name, ".") != 0) && (strcmp(file.name, "..") != 0))
			{
				string file_name = file.name;
				if (file_name == "Debug" || file_name == "Release")
				{
					continue;
				}

				DirectoryInfo* info = new DirectoryInfo();
				info->name = file_name;
				info->full_path = dir_info->full_path + "/" + file_name;
				dir_info->dir_list.push_back(info);
				
				get_dir_info(info, mode);
			}
		}
		else
		{
			string name = file.name;
			FileInfo* info = new FileInfo();
			info->name = name;
			info->full_path = dir_info->full_path + "/" + name;

			if (mode == "*.*")
			{
				dir_info->file_list.push_back(info);
			}
			else
			{
				bool is_finded = false;

				string name_ext = "";
				int cursor = name.find_last_of(".");
				if (cursor != string::npos)
				{
					info->name_before = name.substr(0, cursor);
					name_ext = name.substr(cursor + 1, name.length() - (cursor + 1));
					info->name_ext = name_ext;

					vector<string> ext_list;
					split(mode, ";", ext_list);

					for (int i = 0; i < (int)ext_list.size(); i++)
					{
						int pos = ext_list[i].find(".");
						if (pos != string::npos)
						{
							string ext = ext_list[i].substr(pos + 1, ext_list[i].length() - (pos + 1));
							if (name_ext == ext) //筛选
							{
								is_finded = true;
								dir_info->file_list.push_back(info);
								break;
							}
						}
					}
				}

				if (!is_finded)
				{
					delete info;
					info = NULL;
				}
				
			}
		}
	} while (_findnext(HANDLE, &file) == 0);

	_findclose(HANDLE);

	dir_info->CheckHaveCppOrC();
}
#else

void get_dir_info(DirectoryInfo* dir_info, string mode)
{
	DIR* d = NULL;
	struct dirent* dp = NULL; /* readdir函数的返回值就存放在这个结构体中 */
	struct stat st;
	char p[MAX_PATH_LEN] = { 0 };

	string path = dir_info->full_path;
	if (stat(path.c_str(), &st) < 0 || !S_ISDIR(st.st_mode)) 
	{
		printf("invalid path: %s\n", path.c_str());
		return;
	}

	if (!(d = opendir(path.c_str())))
	{
		printf("opendir[%s] error: %m\n", path.c_str());
		return;
	}

	while ((dp = readdir(d)) != NULL)
	{
		/* 把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录 */
		if ((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2)))
			continue;

		snprintf(p, sizeof(p) - 1, "%s/%s", path.c_str(), dp->d_name);
		stat(p, &st);
		if (!S_ISDIR(st.st_mode)) // 文件
		{
			//printf("%s\n", dp->d_name);

			string name = dp->d_name;
			FileInfo* info = new FileInfo();
			info->name = name;
			info->full_path = dir_info->full_path + "/" + name;

			if (mode == "*.*")
			{
				dir_info->file_list.push_back(info);
			}
			else
			{
				bool is_finded = false;

				string name_ext = "";
				int cursor = name.find_last_of(".");
				if (cursor != string::npos)
				{
					info->name_before = name.substr(0, cursor);
					name_ext = name.substr(cursor + 1, name.length() - (cursor + 1));
					info->name_ext = name_ext;

					vector<string> ext_list;
					split(mode, ";", ext_list);

					for (int i = 0; i < (int)ext_list.size(); i++)
					{
						int pos = ext_list[i].find(".");
						if (pos != string::npos)
						{
							string ext = ext_list[i].substr(pos + 1, ext_list[i].length() - (pos + 1));
							if (name_ext == ext) //筛选
							{
								is_finded = true;
								dir_info->file_list.push_back(info);
								break;
							}
						}
					}
				}

				if (!is_finded)
				{
					delete info;
					info = NULL;
				}
			}
		}
		else // 目录
		{
			//printf("%s/\n", dp->d_name);

			string file_name = dp->d_name;
			if (file_name == "Debug" || file_name == "Release")
			{
				continue;
			}

			DirectoryInfo* info = new DirectoryInfo();
			info->name = file_name;
			info->full_path = dir_info->full_path + "/" + file_name;
			dir_info->dir_list.push_back(info);

			get_dir_info(info, mode);
		}
	}

	closedir(d);

	dir_info->CheckHaveCppOrC();
}
#endif

string get_relative_path(string root_path, string dir_path)
{
	string relative_path = "";
	string::size_type pos = dir_path.find(root_path);
	if (pos == string::npos || pos != 0) // 不存在或非法
	{
		assert(false);
		return relative_path;
	}
	else
	{
		int diff_len = dir_path.length() - root_path.length();
		if (diff_len == 0)
		{
			return relative_path;
		}
		else
		{
			return dir_path.substr(root_path.length() + 1, diff_len - 1);
		}
		
	}
}

DirAnalysis::DirAnalysis(string src_path, ProjectInfo& project_info)
{
	src_path_ = src_path;
	project_info_ = project_info;

	dir_info_ = NULL;

	public_header_ = "";
	public_header_ += "################################################################################\n";
	public_header_ += "# Automatically-generated file. Do not edit!\n";
	public_header_ += "################################################################################\n";
	public_header_ += "\n";
}

DirAnalysis::~DirAnalysis()
{
	if (dir_info_)
	{
		delete dir_info_;
		dir_info_ = NULL;
	}
}

bool DirAnalysis::OperateProject()
{
	dir_info_ = new DirectoryInfo();
	dir_info_->name = project_info_.src_dir;
	dir_info_->full_path = src_path_ + "/" + project_info_.src_dir;
	get_dir_info(dir_info_, "*.cpp;*.c");

	BuildObjects(true);
	BuildObjects(false);

	BuildSubDir(true);
	BuildSubDir(false);

	BuildSources(true);
	BuildSources(false);

	BuildMakeFile(true);
	BuildMakeFile(false);
	
	return true;
}

bool DirAnalysis::BuildObjects(bool is_debug)
{
	string content = public_header_;

	content += "LIBS := ";
	
	vector<string> out_list;
	if (is_debug)
	{
		split(project_info_.debug_info.libs, " ", out_list);
	}
	else
	{
		split(project_info_.release_info.libs, " ", out_list);
	}
	
	for (size_t n = 0; n < out_list.size(); n++)
	{
		content += " -l" + out_list[n];
	}

	content += "\n\n";
	content += "USER_OBJS := ";

	string dst_file_path = src_path_ + "/" + project_info_.src_dir + "/";
	if (is_debug)
	{
		dst_file_path  += "Debug";
	}
	else
	{
		dst_file_path += "Release";
	}

	dst_file_path += "/objects.mk";
	
	copy_file_text(content, dst_file_path);

	return true;
}

bool DirAnalysis::BuildSubDir(bool is_debug)
{
	BuildSubDir(is_debug, dir_info_);

	return true;
}

bool DirAnalysis::BuildSubDir(bool is_debug, DirectoryInfo* dir_info)
{
	string content = public_header_;

	content += "# Add inputs and outputs from these tool invocations to the build variables\n";
	
	std::vector<FileInfo*>& file_list = dir_info->file_list;
	if (file_list.size() > 0)
	{
		string root_path = src_path_ + "/" + project_info_.src_dir; //根目录路径
		string relative_path = get_relative_path(root_path, dir_info->full_path); //相对路径

		string relative_path_left = "";
		if (relative_path != "") //不是根目录，即子目录
		{
			relative_path_left = relative_path + "/";
		}

		string relative_path_right = "";
		if (relative_path != "") //不是根目录，即子目录
		{
			relative_path_right = relative_path + "/";
		}

		//-------------------------------------------------------------
		if (dir_info->cpp_file_count > 0)
		{
			content += "CPP_SRCS += \\\n";
			int cpp_file_count = 0;
			for (size_t n = 0; n < file_list.size(); n++)
			{
				if (file_list[n]->name_ext != "cpp")
				{
					continue;
				}

				content += "../" + relative_path_left + file_list[n]->name + " ";

				cpp_file_count++;

				if (cpp_file_count < dir_info->cpp_file_count)
				{
					content += "\\\n";
				}
				else
				{
					content += "\n\n";
				}
			}

			content += "CPP_DEPS += \\\n";
			cpp_file_count = 0;
			for (size_t n = 0; n < file_list.size(); n++)
			{
				if (file_list[n]->name_ext != "cpp")
				{
					continue;
				}

				content += "./" + relative_path_left + file_list[n]->name_before + ".d ";

				cpp_file_count++;

				if (cpp_file_count < dir_info->cpp_file_count)
				{
					content += "\\\n";
				}
				else
				{
					content += "\n\n";
				}
			}
		}
		//-------------------------------------------------------------
		if (dir_info->c_file_count > 0)
		{
			content += "C_SRCS += \\\n";
			int c_file_count = 0;
			for (size_t n = 0; n < file_list.size(); n++)
			{
				if (file_list[n]->name_ext != "c")
				{
					continue;
				}

				content += "../" + relative_path_left + file_list[n]->name + " ";

				c_file_count++;

				if (c_file_count < dir_info->c_file_count)
				{
					content += "\\\n";
				}
				else
				{
					content += "\n\n";
				}
			}

			content += "C_DEPS += \\\n";
			c_file_count = 0;
			for (size_t n = 0; n < file_list.size(); n++)
			{
				if (file_list[n]->name_ext != "c")
				{
					continue;
				}

				content += "./" + relative_path_left + file_list[n]->name_before + ".d ";

				c_file_count++;

				if (c_file_count < dir_info->c_file_count)
				{
					content += "\\\n";
				}
				else
				{
					content += "\n\n";
				}
			}
		}
		//-------------------------------------------------------------
		content += "OBJS += \\\n";

		for (size_t n = 0; n < file_list.size(); n++)
		{
			content += "./" + relative_path_left + file_list[n]->name_before + ".o ";

			if (n < file_list.size() - 1)
			{
				content += "\\\n";
			}
			else
			{
				content += "\n\n";
			}
		}

		//-------------------------------------------------------------
		content += "# Each subdirectory must supply rules for building sources it contributes\n";
		
		if (dir_info->cpp_file_count > 0)
		{
			if (relative_path == "")
			{
				content += "%.o: ../%.cpp\n";
			}
			else
			{
				content += relative_path + "/%.o: ../" + relative_path + "/%.cpp\n";
			}


			content += "\t@echo 'Building file: $<'\n";
			content += "\t@echo 'Invoking: GCC C++ Compiler'\n";

			string include_dirs_content = "";
			vector<string> out_list;
			split(project_info_.include_dirs, " ", out_list);
			for (size_t n = 0; n < out_list.size(); n++)
			{
				include_dirs_content += " -I" + out_list[n];
			}

			if (is_debug)
			{
				content += "\tg++ " + project_info_.debug_info.sub_addition_option + " " + include_dirs_content + " -DDEBUG -O0 -g3";
			}
			else
			{
				content += "\tg++ " + project_info_.release_info.sub_addition_option + " " + include_dirs_content + " -O3";
			}

			content += " -Wall -c -fmessage-length=0 -MMD -MP -MF\"$(@:%.o=%.d)\" -MT\"$(@:%.o=%.d)\" -o\"$@\" \"$<\"\n";
			content += "\t@echo 'Finished building: $<'\n";
			content += "\t@echo ''\n\n";
		}
		
		if (dir_info->c_file_count > 0)
		{
			if (relative_path == "")
			{
				content += "%.o: ../%.c\n";
			}
			else
			{
				content += relative_path + "/%.o: ../" + relative_path + "/%.c\n";
			}


			content += "\t@echo 'Building file: $<'\n";
			content += "\t@echo 'Invoking: GCC C Compiler'\n";

			string include_dirs_content = "";
			vector<string> out_list;
			split(project_info_.include_dirs, " ", out_list);
			for (size_t n = 0; n < out_list.size(); n++)
			{
				include_dirs_content += " -I" + out_list[n];
			}

			if (is_debug)
			{
				content += "\tgcc " + include_dirs_content + " -DDEBUG -O0 -g3";
			}
			else
			{
				content += "\tgcc " + include_dirs_content + " -O3";
			}

			content += " -Wall -c -fmessage-length=0 -MMD -MP -MF\"$(@:%.o=%.d)\" -MT\"$(@:%.o=%.d)\" -o\"$@\" \"$<\"\n";
			content += "\t@echo 'Finished building: $<'\n";
			content += "\t@echo ''\n";
		}

		//-------------------------------------------------------------
		string dst_file_path = src_path_ + "/" + project_info_.src_dir + "/";
		if (is_debug)
		{
			dst_file_path += "Debug";
		}
		else
		{
			dst_file_path += "Release";
		}

		dst_file_path += "/" + relative_path_right + "subdir.mk";

		copy_file_text(content, dst_file_path);
	}
	
	// 遍历子目录
	for (size_t n = 0; n < dir_info->dir_list.size(); n++)
	{
		BuildSubDir(is_debug, dir_info->dir_list[n]);
	}

	return true;
}

void DirAnalysis::BuildMakeFileDir(DirectoryInfo* dir_info, string& content)
{
	vector<FileInfo*>& file_list = dir_info->file_list;
	if (file_list.size() > 0)
	{
		string root_path = src_path_ + "/" + project_info_.src_dir; //根目录路径
		string relative_path = get_relative_path(root_path, dir_info->full_path); //相对路径

		if (relative_path != "") //不是根目录，即子目录
		{
			relative_path = "-include " + relative_path + "/subdir.mk";
		}
		else //根目录
		{
			relative_path = "-include subdir.mk";
		}

		content += relative_path + "\n";
	}

	// 遍历子目录
	for (size_t n = 0; n < dir_info->dir_list.size(); n++)
	{
		BuildMakeFileDir(dir_info->dir_list[n], content);
	}
}

bool DirAnalysis::BuildMakeFile(bool is_debug)
{
	string content = public_header_;

	content += "-include ../makefile.init\n\n";

	content += "RM := rm -rf\n";

	content += "# All of the sources participating in the build are defined here\n";
	content += "-include sources.mk\n";

	string dir_content = "";
	BuildMakeFileDir(dir_info_, dir_content);

	content += dir_content; // 遍历
	content += "-include objects.mk\n\n";

	content += "ifneq ($(MAKECMDGOALS), clean)\n";
	content += "ifneq ($(strip $(C_DEPS)), )\n";
	content += "-include $(C_DEPS)\n";
	content += "endif\n";
	content += "ifneq ($(strip $(CXX_DEPS)), )\n";
	content += "-include $(CXX_DEPS)\n";
	content += "endif\n";
	content += "ifneq ($(strip $(CPP_DEPS)), )\n";
	content += "-include $(CPP_DEPS)\n";
	content += "endif\n";
	content += "ifneq ($(strip $(CC_DEPS)), )\n";
	content += "-include $(CC_DEPS)\n";
	content += "endif\n";
	content += "ifneq ($(strip $(C_UPPER_DEPS)), )\n";
	content += "-include $(C_UPPER_DEPS)\n";
	content += "endif\n";
	content += "ifneq ($(strip $(C++_DEPS)), )\n";
	content += "-include $(C++_DEPS)\n";
	content += "endif\n";
	content += "endif\n\n";

	content += "-include ../makefile.defs\n\n";

	content += "# Add inputs and outputs from these tool invocations to the build variables\n";

	content += "# All Target\n";

	string out_put_name = project_info_.out_put;
	if (is_debug)
	{
		out_put_name += "D";
	}

	if (project_info_.program_type == PROGRAM_TYPE_EXE)
	{
		content += "all: " + out_put_name;
	}
	else if (project_info_.program_type == PROGRAM_TYPE_LIB) //lib
	{
		out_put_name += ".a";

		content += "all: " + out_put_name;
	}
	else // dll
	{
		out_put_name += ".so";

		content += "all: " + out_put_name;
	}

	content += "\n\n";

	content += "# Tool invocations\n";
	content += out_put_name + ": $(OBJS) $(USER_OBJS)\n";
	content += "\t@echo 'Building target: $@'\n";
	
	string lib_dir_content = "";
	if (project_info_.lib_dirs != "")
	{
		lib_dir_content += "-L" + project_info_.lib_dirs;
	}

	string lib_content = "";
	if (project_info_.lib_dirs != "")
	{
		lib_content += "-L" + project_info_.lib_dirs;
	}

	string addition_option_content = "";
	if (is_debug)
	{
		addition_option_content += project_info_.debug_info.make_addition_option;
	}
	else
	{
		addition_option_content += project_info_.release_info.make_addition_option;
	}

	string program_type_content = "";
	if (project_info_.program_type == PROGRAM_TYPE_EXE)
	{
		content += "\t@echo 'Invoking: GCC C++ Linker'\n";
		content += "\tg++ " + addition_option_content + " " + lib_dir_content + " -o\"" + out_put_name + "\" $(OBJS) $(USER_OBJS) $(LIBS)\n";

		program_type_content = "EXECUTABLES";
		
	}
	else if (project_info_.program_type == PROGRAM_TYPE_LIB) //lib
	{
		content += "\t@echo 'Invoking: GCC Archiver'\n";
		content += "\tar -r \"" + out_put_name + "\" $(OBJS) $(USER_OBJS) $(LIBS)\n";

		program_type_content = "ARCHIVES";
	}
	else // dll
	{
		content += "\t@echo 'Invoking: GCC C++ Linker'\n";
		content += "\tg++ " + addition_option_content + " " + lib_dir_content + " -o\"" + out_put_name + "\" $(OBJS) $(USER_OBJS) $(LIBS)\n";

		program_type_content = "EXECUTABLES";
	}

	content += "\t@echo 'Finished building target: $@'\n";
	content += "\t@echo ' '\n\n";

	content += "# Other Targets\n";
	content += "clean:\n";
	content += "\t-$(RM) $(C_DEPS)$(CXX_DEPS)$(CPP_DEPS)$(OBJS)$(CC_DEPS)$(" + program_type_content + ")$(C_UPPER_DEPS)$(C++_DEPS) " + out_put_name + "\n";
	content += "\t-@echo ' '\n";

	content += ".PHONY: all clean dependents\n";
	content += ".SECONDARY:\n\n";

	content += "-include ../makefile.targets\n";


	string dst_file_path = src_path_ + "/" + project_info_.src_dir + "/";
	if (is_debug)
	{
		dst_file_path += "Debug";
	}
	else
	{
		dst_file_path += "Release";
	}

	dst_file_path += "/makefile";

	copy_file_text(content, dst_file_path);

	return true;
}

void DirAnalysis::BuildSourcesDir(DirectoryInfo* dir_info, string& content)
{
	vector<FileInfo*>& file_list = dir_info->file_list;
	if (file_list.size() > 0)
	{
		string root_path = src_path_ + "/" + project_info_.src_dir; //根目录路径
		string relative_path = get_relative_path(root_path, dir_info->full_path); //相对路径

		if (relative_path != "") //不是根目录，即子目录
		{
			relative_path = relative_path + " \\";
		}
		else //根目录
		{
			relative_path = ". \\";
		}

		content += relative_path + "\n";
	}

	// 遍历子目录
	for (size_t n = 0; n < dir_info->dir_list.size(); n++)
	{
		BuildSourcesDir(dir_info->dir_list[n], content);
	}
}

bool DirAnalysis::BuildSources(bool is_debug)
{
	string content = public_header_;

	content += "ASM_SRCS := \n";
	content += "CXX_SRCS := \n";
	content += "S_SRCS := \n";
	content += "OBJ_SRCS := \n";
	content += "C++_SRCS := \n";
	content += "C_SRCS := \n";
	content += "O_SRCS := \n";
	content += "C_UPPER_SRCS := \n";
	content += "CC_SRCS := \n";
	content += "CPP_SRCS := \n";
	content += "C_DEPS := \n";
	content += "CXX_DEPS := \n";
	content += "CPP_DEPS := \n";
	content += "OBJS := \n";
	content += "CC_DEPS := \n";
	content += "C_UPPER_DEPS := \n";
	if (project_info_.program_type == PROGRAM_TYPE_EXE)
	{
		content += "EXECUTABLES := \n";
	}
	else if (project_info_.program_type == PROGRAM_TYPE_LIB) //lib
	{
		content += "ARCHIVES := \n";
	}
	else // dll
	{
		content += "EXECUTABLES := \n";
	}

	
	content += "C++_DEPS := \n\n";

	content += "# Every subdirectory with source files must be described here\n";

	content += "SUBDIRS := \\\n";

	string dir_content = "";
	BuildSourcesDir(dir_info_, dir_content);
	content += dir_content;

	string dst_file_path = src_path_ + "/" + project_info_.src_dir + "/";
	if (is_debug)
	{
		dst_file_path += "Debug";
	}
	else
	{
		dst_file_path += "Release";
	}

	dst_file_path += "/sources.mk";

	copy_file_text(content, dst_file_path);
	return true;
}

