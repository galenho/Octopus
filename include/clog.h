/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _CLOG_H
#define _CLOG_H

#include "common.h"
#include "mutex.h"
#include "thread_base.h"
#include "safe_list.h"
#include "event.h"

#ifdef WIN32
#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE TNORMAL | FOREGROUND_INTENSITY
#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#else
#define TRED 1
#define TGREEN 2
#define TYELLOW 3
#define TNORMAL 4
#define TWHITE 5
#define TBLUE 6
#endif

class LogData
{
public:
	LogData()
	{
		op_type = 1;
		data_len = 0;
		data = NULL;

		year = 0;
		month = 0;
		day = 0;
	}

	~LogData()
	{
		if (data)
		{
			delete[] data;
			data = NULL;
		}
	}

public:
	int8  op_type;  // 1:日志数据操作 2:保存操作
	int32 data_len;
	char* data;
	int32 year;
	int32 month;
	int32 day;
};

struct LogFileInfo
{
	std::string full_name;
	int size;
	int num;

	LogFileInfo()
	{
		size = 0;
		num = 0;
	}
};

class CLog : public ThreadBase
{
public:
	enum LogLevel
	{
		LOG_LEVEL_DEBUG = 0,
		LOG_LEVEL_INFO  = 1,
		LOG_LEVEL_WARN  = 2,
		LOG_LEVEL_ERROR = 3
	};

	CLog(int32 log_level = LOG_LEVEL_INFO, int32 buff_size = 1024 * 1024, int32 write_buff_size = 1024);
	virtual ~CLog();

	void init(string file_name, bool is_need_date, int max_log_buff, bool is_need_check = true);
	void set_log_level(int32 log_level);
	int32 get_log_level();

	void WriteLogFile( int32 log_level, const char* ftm, ...);
	void Save();

	void Start();
	void StopWaitExit();

	void WakeUp();

protected:	
	bool Run();

private:
	void Color(unsigned int color);

private:
	int32 log_level_;
	string file_name_;
	string full_name_;

	ofstream fout;

	char* buffer_;
	int32 buff_size_;  //初始化大小
	int32 write_buff_size_; //多大写入文件一次

#ifdef WIN32
	HANDLE stdout_handle, stderr_handle;
#endif

	Mutex mutex;

	SafeList<LogData*> log_list_;

	bool is_thread_exit_; //线程是否退出

	Event event_;

	bool is_need_date_;

	int max_log_buff_;
	int last_log_buff_size_;

	int last_year_;
	int last_month_;
	int last_day_;
	int last_num_;
};

#define LOG_DEBUG(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_DEBUG, ftm, ##__VA_ARGS__)
#define LOG_INFO(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_INFO, ftm, ##__VA_ARGS__)
#define LOG_WARN(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_WARN, ftm, ##__VA_ARGS__)
#define LOG_ERROR(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_ERROR, ftm, ##__VA_ARGS__)

#endif //_CLOG_H

