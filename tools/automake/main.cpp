// test_cpp.cpp : 定义控制台应用程序的入口点。
//

#include "utility.h"
#include "tinyxml/tinyxml.h"
#include "dir_analysis.h"

using namespace std;

CppConfig cpp_config;

//------------------------------------------------------------------------------------------------------
bool LoadXML()
{
	TiXmlDocument* doc = new TiXmlDocument;

	if (!doc->LoadFile("automake.xml"))
	{
		assert(false);

		delete doc;
		doc = NULL;
		return false;
	}

	TiXmlElement* root_emt = doc->RootElement();
	if (root_emt != NULL)
	{
		TiXmlElement* projects_emt = root_emt->FirstChildElement("Projects");
		assert(projects_emt);

#ifdef WIN32
		cpp_config.src_path = projects_emt->Attribute("src_path_win");
#else
		cpp_config.src_path = projects_emt->Attribute("src_path_linux");
#endif // DEBUG
		
		cpp_config.thread_count = projects_emt->Attribute("thread_count");

		TiXmlElement* project_emt = projects_emt->FirstChildElement();
		while (project_emt) //项目
		{
			ProjectInfo project_info;
			project_info.src_dir = project_emt->Attribute("src");
			project_info.out_put = project_emt->Attribute("OutPut");
			project_info.program_type = atoi(project_emt->Attribute("ProgramType"));

			TiXmlElement* include_dir_emt = project_emt->FirstChildElement("IncludeDir");
			project_info.include_dirs = include_dir_emt->GetText() == NULL ? "" : include_dir_emt->GetText();

			TiXmlElement* lib_dir_emt = project_emt->FirstChildElement("LibDir");
			project_info.lib_dirs = lib_dir_emt->GetText() == NULL ? "" : lib_dir_emt->GetText();

			TiXmlElement* debug_emt = project_emt->FirstChildElement("Debug");
			project_info.debug_info.libs = debug_emt->Attribute("Lib");
			project_info.debug_info.sub_addition_option = debug_emt->Attribute("SubAdditionOption");
			project_info.debug_info.make_addition_option = debug_emt->Attribute("MakeAdditionOption");

			TiXmlElement* release_emt = project_emt->FirstChildElement("Release");
			project_info.release_info.libs = release_emt->Attribute("Lib");
			project_info.release_info.sub_addition_option = release_emt->Attribute("SubAdditionOption");
			project_info.release_info.make_addition_option = release_emt->Attribute("MakeAdditionOption");
			
			// 插入项目
			cpp_config.project_list.push_back(project_info);

			// 下一个项目
			project_emt = project_emt->NextSiblingElement();
		}
	}

	// 清理资源
	delete doc;
	doc = NULL;

	return true;
}

void BuildProjectScript()
{
	for (int n = 0; n < (int)cpp_config.project_list.size(); n++)
	{
		DirAnalysis dir(cpp_config.src_path, cpp_config.project_list[n]);
		dir.OperateProject();
	}
}

void BuildCleanScript(bool is_debug)
{
	string content = "#!/bin/sh\n\n";

	for (int n = 0; n < (int)cpp_config.project_list.size(); n++)
	{
		if (n == 0)
		{
			content += "cd ..\n";
		}
		else
		{
			content += "cd ../..\n";
		}

		content += "cd " + cpp_config.project_list[n].src_dir + "/";
		if (is_debug)
		{
			content += "Debug\n";
		}
		else
		{
			content += "Release\n";
		}

		content += "make clean\n\n";
	}

	
	content += "cd ../../bin\n";
	content += "rm";
	for (int n = 0; n < (int)cpp_config.project_list.size(); n++)
	{
		//-----------------------------------------------------------------------------------
		string out_put_name = cpp_config.project_list[n].out_put;
		if (is_debug)
		{
			out_put_name += "D";
		}

		if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_LIB)
		{
			out_put_name += ".a";
		}
		else if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_DLL)
		{
			out_put_name += ".so";
		}
		else
		{

		}
		//-----------------------------------------------------------------------------------

		if (cpp_config.project_list[n].program_type == 1)
		{
			continue;
		}

		content += " " + out_put_name;
	}

	content += "\n";
	content += "cd ../lib\n";
	content += "rm";
	for (int n = 0; n < (int)cpp_config.project_list.size(); n++)
	{
		//-----------------------------------------------------------------------------------
		string out_put_name = cpp_config.project_list[n].out_put;
		if (is_debug)
		{
			out_put_name += "D";
		}

		if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_LIB)
		{
			out_put_name += ".a";
		}
		else if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_DLL)
		{
			out_put_name += ".so";
		}
		else
		{

		}
		//-----------------------------------------------------------------------------------
		if (cpp_config.project_list[n].program_type != 1)
		{
			continue;
		}

		content += " " + out_put_name;
	}

	string dst_file_path = "";
	if (is_debug)
	{
		dst_file_path = "cleanDebug.sh";
	}
	else
	{
		dst_file_path = "cleanRelease.sh";
	}
	copy_file_text(content, dst_file_path);
}

void BuildMakeScript(bool is_debug)
{
	string content = "#!/bin/sh\n\n";
	content += "./automake\n";

	for (int n = 0; n < (int)cpp_config.project_list.size(); n++)
	{
		content += "\n";
		if (n == 0)
		{
			content += "cd ..\n";
		}
		else
		{
			content += "cd ../..\n";
		}

		content += "cd " + cpp_config.project_list[n].src_dir + "/";
		if (is_debug)
		{
			content += "Debug\n";
		}
		else
		{
			content += "Release\n";
		}

		//------------------------------------------------------------------------------------------
		string out_put_name = cpp_config.project_list[n].out_put;
		if (is_debug)
		{
			out_put_name += "D";
		}

		if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_LIB)
		{
			out_put_name += ".a";
		}
		else if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_DLL)
		{
			out_put_name += ".so";
		}
		else
		{

		}

		string log_name = cpp_config.project_list[n].src_dir + "_build.log";
		//------------------------------------------------------------------------------------------

		content += "rm -rf " + log_name + "\n";
		content += "rm -rf " + out_put_name + "\n";

		content += "make all -j" + cpp_config.thread_count + " 2>&1 | tee " + log_name + "\n";

		if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_LIB)
		{
			content += "cp " + out_put_name + " ../../lib/linux64/" + out_put_name + "\n";
		}
		else if (cpp_config.project_list[n].program_type == PROGRAM_TYPE_DLL)
		{
			content += "cp " + out_put_name + " ../../bin/" + out_put_name + "\n";
		}
		else
		{
			content += "cp " + out_put_name + " ../../bin/" + out_put_name + "\n";
		}
	}

	if (is_debug)
	{
		content += "\necho make Debug ok!\n";
	}
	else
	{
		content += "\necho make Release ok!\n";
	}


	string dst_file_path = "";
	if (is_debug)
	{
		dst_file_path = "makeDebug.sh";
	}
	else
	{
		dst_file_path = "makeRelease.sh";
	}
	copy_file_text(content, dst_file_path);
}

int main(int argc, char* argv[])
{
	if (argc != 1 && argc != 2)
	{
		return -1;
	}

	bool ret = LoadXML();
	assert(ret);

	BuildProjectScript();

	if (argc == 2)
	{
		string arg = argv[1];
		if (arg == "script")
		{
			BuildCleanScript(true);
			BuildCleanScript(false);

			BuildMakeScript(true);
			BuildMakeScript(false);
		}
	}
	
	printf("operater is successfull!\n");

    return 0;
}

