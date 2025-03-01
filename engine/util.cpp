/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "timer.h"
#include "scheduler.h"

using namespace std;

hash_map<long, int> g_mapPoints;

void AddPoint(long p, int nNum)
{
	g_mapPoints.insert(pair<long, int>(p, nNum));
}

void DelPoint(long p)
{
	hash_map<long, int>::iterator itr = g_mapPoints.find(p);

	if(itr != g_mapPoints.end())
	{
		g_mapPoints.erase(itr);
	}
}

void PrintPointsInfo()
{
	if (g_mapPoints.size() > 0)
	{
		PRINTF_ERROR("memory leak, have %d object pointers", g_mapPoints.size());
		hash_map<long, int>::iterator itr = g_mapPoints.begin();
		while(itr != g_mapPoints.end())
		{
			PRINTF_ERROR("address: %d number:(%d)", *itr, (int)itr->second);
			itr++;
		}
	}
	else
	{
		PRINTF_INFO("the memory is beautiful.");
	}

	Scheduler::get_instance()->get_logger()->Save();
}

void TOLOWER(string& str)
{
	for (size_t i = 0; i < str.length(); ++i)
		str[i] = (char)tolower(str[i]);
};

void TOUPPER(string& str)
{
	for (size_t i = 0; i < str.length(); ++i)
		str[i] = (char)toupper(str[i]);
};

void StrSplit(const string& src, const char& sep, vector<string>& out_list)
{
	if (src.length() == 0)
	{
		return;
	}

	uint32 cursor = 0;
	for (uint32 n=0; n < src.length(); n++)
	{
		if (src[n] == sep)
		{
			uint32 len = n - cursor;
			if (len > 0)
			{
				out_list.push_back(src.substr(cursor, len)); //压入前面部分
				cursor = n + 1;
			}
		}
	}

	if (cursor < src.length())  //最后一个分割符后边没有字符, 就不算这个
	{
		out_list.push_back(src.substr(cursor, src.length() - cursor));
	}
}

void StrSplitLasSpace(const string& src, const char& sep, vector<string>& out_list)
{
	if (src.length() == 0)
	{
		return;
	}

	uint32 cursor = 0;
	for (uint32 n=0; n < src.length(); n++)
	{
		if (src[n] == sep)
		{
			uint32 len = n - cursor;
			if (len > 0)
			{
				out_list.push_back(src.substr(cursor, len)); //压入前面部分
				cursor = n + 1;
			}
		}
	}

	if (cursor == src.length())  //最后一个分割符后边没有字符, 就不算这个
	{
		out_list.push_back("");
	}
}

bool sql_inj(string& content)
{
	string content_low = content;
	transform(content_low.begin(), content_low.end(), content_low.begin(), ::tolower);

	string inj_str = "'|!|{|}|[|]|(|)|/|\\|;|-|+|,|*|%|@|or|and|exec|insert|select|delete|update|drop|union|count|chr|mid|master|truncate|char|declare"; 

	vector<string> inj_collects;
	StrSplit(inj_str, '|', inj_collects);
	for (uint32 n=0; n < inj_collects.size(); n++)
	{
		string::size_type pos = content_low.find(inj_collects[n]);
		if(pos == string::npos ) // 不存在
		{
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}


void MakeIntString(char* buf, int num)
{
	if (num < 10)
	{
		buf[0] = '0';
		//itoa(num, &buf[1], 10);
		sprintf(&buf[1], "%u", num);
	}
	else
	{
		//itoa(num,buf,10);
		sprintf(buf, "%u", num);
	}
}

void MakeIntStringNoZero(char* buf, int num)
{
	//itoa(num,buf,10);
	sprintf(buf, "%u", num);
}


unsigned long ip_string2int(const string& str_ip)
{
	//IP转化为数值
	//没有格式检查
	//返回值就是结果
	int a[4];
	string ip = str_ip;
	string strTemp;
	size_t pos;
	size_t i = 3;

	do
	{
		pos = ip.find(".");
		if (pos != string::npos)
		{
			strTemp = ip.substr(0, pos);
			a[i] = atoi(strTemp.c_str());
			i--;
			ip.erase(0, pos + 1);
		}
		else
		{
			strTemp = ip;
			a[i] = atoi(strTemp.c_str());
			break;
		}
	}
	while (1);

	return (a[3] << 24) + (a[2] << 16) + (a[1] << 8) + a[0];
}

string ip_int2string(unsigned int ip_value)
{
	//数值转化为IP
	stringstream str;
	str << ((ip_value & 0xff000000) >> 24) << "."
		<< ((ip_value & 0x00ff0000) >> 16) << "."
		<< ((ip_value & 0x0000ff00) >> 8) << "."
		<< (ip_value & 0x000000ff);
	
	return str.str();
}

string now_string()
{
	time_t timep;      
	time (&timep);    
	tm g_localTime = *(localtime(&timep));

	char buffer[256];
	sprintf(buffer, "%04u-%02u-%02u %02u:%02u:%02u", 
		g_localTime.tm_year + 1900, g_localTime.tm_mon + 1, g_localTime.tm_mday, 
		g_localTime.tm_hour, g_localTime.tm_min, g_localTime.tm_sec);

	string str;
	str.append(buffer);
	return str;
}

string now_date_string()
{
	time_t timep;      
	time (&timep);    
	tm g_localTime = *(localtime(&timep));

	char buffer[256];
	sprintf(buffer, "%04u-%02u-%02u", 
		g_localTime.tm_year + 1900, g_localTime.tm_mon + 1, g_localTime.tm_mday);

	string str;
	str.append(buffer);
	return str;
}

int64 now_second()
{
	time_t timep;      
	time (&timep);
	return timep;
}

int64 mk_second(int year, int mon, int day,
			     int hour, int min, int sec)
{
	struct tm time_fields;
	memset(&time_fields, 0, sizeof(time_fields));

	time_fields.tm_year = year - 1900;
	time_fields.tm_mon = mon - 1;
	time_fields.tm_mday = day;

	time_fields.tm_hour = hour;
	time_fields.tm_min = min;
	time_fields.tm_sec = sec;

	time_t seconds = mktime(&time_fields);
	return seconds;
}

void  mk_datatime(uint64 seconds,
				   int& year, int& mon, int& day,
				   int& hour, int& min, int& sec,
				   int& wday)
{
	tm *localTime = localtime((const time_t *)(&seconds));
	year = localTime->tm_year + 1900;
	mon = localTime->tm_mon + 1;
	day = localTime->tm_mday;

	hour = localTime->tm_hour;
	min = localTime->tm_min;
	sec = localTime->tm_sec;

	// 转化一下
	if (localTime->tm_wday == 0)
	{
		wday = 7;
	}
}

uint64 time_from_string(const char* str)  
{  
  struct tm tm1;  
  memset(&tm1, 0, sizeof(tm1));
  int year,mon,mday,hour,min,sec = 0;  
  if( -1 == sscanf(str,"%d-%d-%d %d:%d:%d",&year,&mon,&mday,&hour,&min,&sec))
	  return -1;

  tm1.tm_year=year - 1900;  
  tm1.tm_mon=mon - 1;  
  tm1.tm_mday=mday;  
  tm1.tm_hour=hour;  
  tm1.tm_min=min;  
  tm1.tm_sec=sec;  
  return mktime(&tm1);
}  

bool FloatEqual(float value0, float value1)
{
#define DELTA 0.000001
	return fabsf(value0 - value1) <= DELTA;
}
//-----------------------------------------------------------------------------
int32 AreaMiddle(int32 value, int32 min_value, int32 max_value)
{
	if (value < min_value)
	{
		return min_value;
	}
	else if (value > max_value)
	{
		return max_value;
	}
	else
	{
		return value;
	}
}
int32 AreaMin(int32 value, int32 min_value)
{
	if (value < min_value)
	{
		return min_value;
	}
	else
	{
		return value;
	}
}
int32 AreaMax(int32 value, int32 max_value)
{
	if (value > max_value)
	{
		return max_value;
	}
	else
	{
		return value;
	}
}

//-----------------------------------------------------------------------------
float AreaMinFloat(float value, float min_value)
{
	if (value < min_value)
	{
		return min_value;
	}
	else
	{
		return value;
	}
}
float AreaMaxFloat(float value, float max_value)
{
	if (value > max_value)
	{
		return max_value;
	}
	else
	{
		return value;
	}
}
float AreaMiddleFloat(float value, float min_value, float max_value)
{
	if (value < min_value)
	{
		return min_value;
	}
	else if (value > max_value)
	{
		return max_value;
	}
	else
	{
		return value;
	}
}

//-----------------------------------------------------------------------------
#define  PROGRESS_NAME_LEN 200
string GetProgressName()
{
	string progress_name = "";

#ifdef WIN32
	//获取进程名称
	//d:\Projects\Debug\Test.exe
	char szFileFullPath[PROGRESS_NAME_LEN] = { 0 };
	::GetModuleFileNameA(NULL, szFileFullPath, PROGRESS_NAME_LEN);//获取文件路径
	int length = (int)(strlen(szFileFullPath));
	for (int i = length - 1; i >= 0; i--)//从路径后面开始找\，即倒着找右斜杠
	{
		if (szFileFullPath[i] == '\\')//找到第一个\，就可以马上获取进程名称了
		{
			i++;
			for (int j = 0; i <= length; j++)//结束符\0不能少,即i=length
			{
				progress_name += szFileFullPath[i++];
			}
			break;
		}
	}
#else
	char strProcessPath[PROGRESS_NAME_LEN] = { 0 };
	if (readlink("/proc/self/exe", strProcessPath, PROGRESS_NAME_LEN) <= 0)
	{
		return progress_name;
	}

	char* strProcessName = strrchr(strProcessPath, '/');
	if (strProcessName)
	{
		strProcessName++;  //去掉‘/’
		progress_name = strProcessName;
	}
#endif

	return progress_name;
}