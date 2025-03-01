/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "utility.h"

int32_t CreateDirectory(const std::string& directoryPath)
{
	uint32_t dirPathLen = directoryPath.length();
	if (dirPathLen > MAX_PATH_LEN)
	{
		return -1;
	}
	char tmpDirPath[MAX_PATH_LEN] = { 0 };
	for (uint32_t i = 0; i < dirPathLen; ++i)
	{
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (ACCESS(tmpDirPath, 0) != 0)
			{
				int32_t ret = MKDIR(tmpDirPath);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	}
	return 0;
}

bool readFileIntoString(const char* filename, string& out_data)
{
	char ch;
	FILE *fd = fopen(filename, "rb");
	
	if (!fd)
	{
		return false;
	}

	while((ch=fgetc(fd))!=EOF)
	{
		out_data += ch;
	}

	fclose(fd);

	return true;
}

void copy_file_text(string& src_data, string& dst_file_path)
{
	vector<string> out_list;
	split(dst_file_path, "/", out_list);
	if (out_list.size() >= 2)
	{
		string folder = "";
		for (size_t i =0; i< out_list.size()-1; i++)
		{
			if (i > 0)
			{
				folder += "/";
			}
			folder += out_list[i];
		}
		
		if (folder != "")
		{
			CreateDirectory(folder + "/");
		}
	}

	string dst_data;
	bool dst_ret = readFileIntoString(dst_file_path.c_str(), dst_data);
	if (dst_ret && src_data == dst_data) //相同的文本，不需要再写文件，减少编译
	{
		return;
	}
	else
	{
		FILE *fd = fopen(dst_file_path.c_str(), "wb");
		assert (fd);
		fputs(src_data.c_str(), fd);
		fclose(fd);
	}
}

string Replace( const string& orignStr, const string& oldStr, const string& newStr )
{
	size_t pos = 0;
	string tempStr = orignStr;
	string::size_type newStrLen = newStr.length();
	string::size_type oldStrLen = oldStr.length();
	while(true)
	{
		pos = tempStr.find(oldStr, pos);

		if (pos == string::npos) 
			break;

		tempStr.replace(pos, oldStrLen, newStr);        
		pos += newStrLen;
	}

	return tempStr;
}

void split(string& str, const string& delim, vector<string>& out_list) 
{  
	size_t delim_size = delim.length();
	size_t find_cursor = 0;
	size_t pos = 0;
	while (find_cursor < str.length() && (pos = str.find(delim, find_cursor)) != string::npos)
	{
		string left_data = str.substr(find_cursor, pos - find_cursor);
		out_list.push_back(left_data);

		find_cursor = pos + delim_size;
	}

	if (out_list.size() > 0)
	{
		if (find_cursor >= str.length())
		{
			out_list.push_back("");
		}
		else
		{
			out_list.push_back(str.substr(find_cursor, str.length() - find_cursor));
		}
	}
	else
	{
		if (str.length() > 0)
		{
			out_list.push_back(str);
		}
	}
}

string RT()
{
	string str = "\r\n";
	return str;
}