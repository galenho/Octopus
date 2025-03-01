#include "clog.h"
#include "guard.h"
#include "util.h"

CLog::CLog( int32 log_level /*= LOG_LEVEL_INFO*/, int32 buff_size /*= 1024 * 1024*/ , int32 write_buff_size /*= 1024*/ )
{
	log_level_ = log_level;

#ifdef WIN32
	stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
	stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	ASSERT(buff_size > 20);
	ASSERT(write_buff_size < buff_size);

	buff_size_ = buff_size;
	write_buff_size_ = write_buff_size;

	buffer_ = new char[buff_size_];
	memset(buffer_, 0, buff_size_);

	is_need_date_ = false;
	max_log_buff_ = 0;

	last_year_ = 0;
	last_month_ = 0;
	last_day_ = 0;

	last_num_ = 1;
	last_log_buff_size_ = 0;

	is_running_ = false;
	is_thread_exit_ = false;

	set_name("log thread");
}

CLog::~CLog()
{
	if (buffer_)
	{
		delete[] buffer_;
		buffer_ = NULL;
	}

	LogData* log_data = NULL;
	while (log_list_.pop(log_data))
	{
		delete log_data;
		log_data = NULL;
	}
}

void CLog::init( string file_name, bool is_need_date, int max_log_buff, bool is_need_check /*= true*/)
{
	file_name_ = file_name;
	is_need_date_ = is_need_date;
	max_log_buff_ = max_log_buff;

	int year, month, day, hour, min, sec, wday = 0;
	mk_datatime(now_second(), year, month, day, hour, min, sec, wday);

	last_year_ = year;
	last_month_ = month;
	last_day_ = day;

	if (is_need_date)
	{
		if (max_log_buff > 0)
		{
			if (is_need_check)
			{
				// 看文件是否存在, 如果存在的话, 需要得到文件的大小, 看是否大于设定的最大尺寸
				std::vector<LogFileInfo> log_file_list;
				int num = last_num_;
				while (true)
				{
					std::string full_name = file_name + "_" + to_string(year) + "_" + to_string(month) + "_" + to_string(day) + "_" + to_string(num) + ".txt";
					FILE* fp = fopen(full_name.c_str(), "rb");
					if (fp)
					{
						fseek(fp, 0, SEEK_END);
						LogFileInfo info;
						info.full_name = full_name;
						info.size = ftell(fp);
						info.num = num;
						log_file_list.push_back(info);
						fclose(fp);

						num++;
					}
					else
					{
						break;
					}
				}

				if (log_file_list.size() > 0) //找到最后的文件
				{
					LogFileInfo& info = log_file_list[log_file_list.size() - 1];
					full_name_ = info.full_name;
					last_num_ = info.num;
					last_log_buff_size_ = info.size;
				}
				else //文件不存在
				{
					full_name_ = file_name + "_" + to_string(year) + "_" + to_string(month) + "_" + to_string(day) + "_" + to_string(num) + ".txt";;
					last_log_buff_size_ = 0;
				}
			}
			else // 不需要检查
			{
				full_name_ = file_name + "_" + to_string(year) + "_" + to_string(month) + "_" + to_string(day) + "_" + to_string(last_num_) + ".txt";;
				last_log_buff_size_ = 0;
			}
		}
		else
		{
			full_name_ = file_name + "_" + to_string(year) + "_" + to_string(month) + "_" + to_string(day) + ".txt";
			last_log_buff_size_ = 0;
		}
	}
	else
	{
		full_name_ = file_name + ".txt";
		last_log_buff_size_ = 0;
	}
}

void CLog::set_log_level( int32 log_level )
{
	Guard guard(mutex);

	log_level_ = log_level;
}

int32 CLog::get_log_level()
{
	return log_level_;
}

void CLog::Color( unsigned int color )
{
#ifndef WIN32
	static const char* colorstrings[TBLUE + 1] =
	{
		"", "\033[22;31m", "\033[22;32m", "\033[01;33m",
		"\033[0m", "\033[01;37m", "\033[1;34m",
	};
	fputs(colorstrings[color], stdout);
#else
	SetConsoleTextAttribute(stdout_handle, (WORD)color);
#endif
}

void CLog::Start()
{
	CreateDir(full_name_);
	fout.open(full_name_.c_str(), ios::app);//打开文件

	Activate();
}

bool CLog::Run()
{
	LogData* log_data = NULL;
	int32 buff_len = 0;
	bool is_save_soon = false;

	while(is_running_)
	{
		while (log_list_.pop(log_data))
		{
			if (log_data->op_type == 1)
			{
				bool is_new_file = false;
				if (is_need_date_) //按日期
				{
					if (last_year_ != log_data->year || last_month_ != log_data->month || last_day_ != log_data->day) //不是同一天
					{
						is_new_file = true;
						last_num_ = 1;
					}
					else
					{
						if (max_log_buff_ > 0) //按日期 + 分块
						{
							if (last_log_buff_size_ + log_data->data_len > max_log_buff_)
							{
								is_new_file = true;
								last_num_++;
							}
						}
					}
				}

				if (is_new_file)
				{
					if (buff_len > 0)
					{
						fout << flush;	//回写缓冲区（回写缓冲区会在保持文件打开的情况下保存文件）
					}
					fout.close();//关闭

					init(file_name_, is_need_date_, max_log_buff_, false);
					fout.open(full_name_.c_str(), ios::app);//打开文件
				}

				//--------------------------------------------------------------------------------------
				if (buff_len + log_data->data_len > write_buff_size_)
				{
					fout<<flush;	//回写缓冲区（回写缓冲区会在保持文件打开的情况下保存文件）
					buff_len = 0;
				}

				fout << log_data->data; //写入文件
				buff_len += log_data->data_len;

				last_log_buff_size_ += log_data->data_len;
			}
			else if (log_data->op_type == 2)
			{
				is_save_soon = true;
			}
			else if (log_data->op_type == 3)
			{
				is_save_soon = true;
				is_running_ = false;
			}

			delete log_data;
			log_data = NULL;
		}

		if (is_save_soon)
		{
			// 最后剩余的日志
			if (buff_len > 0)
			{
				fout<<flush;	//回写缓冲区（回写缓冲区会在保持文件打开的情况下保存文件）
			}
			is_save_soon = false;
		}

		if (is_running_)
		{
			event_.Wait();
		}
	}

	// 最后剩余的日志
	if (buff_len > 0)
	{
		fout<<flush;	//回写缓冲区（回写缓冲区会在保持文件打开的情况下保存文件）
	}
	fout.close();//关闭

	is_thread_exit_ = true;

	return true;
}

void CLog::StopWaitExit()
{
	LogData* log_data = new LogData();
	log_data->op_type = 3;
	log_list_.push_back(log_data);

	// 唤醒线程
	WakeUp();

	while (!is_thread_exit_) //直到线程退出
	{
		SleepMs(200);
	}
}

void CLog::WriteLogFile( int32 log_level, const char* fmt, ... )
{
	Guard guard(mutex);

	if (log_level < log_level_)
		return;

	int length = 0;

	time_t timep;
	time (&timep);
	tm *localTime = localtime(&timep);

	int year = localTime->tm_year + 1900;
	int month = localTime->tm_mon + 1;
	int day = localTime->tm_mday;

	// (1)写入日期头
	sprintf(buffer_, "%04u-%02u-%02u %02u:%02u:%02u ", 
		year, month, day,
		localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

	length += 20;

	// (2)写入日志体
	va_list ap;
	va_start(ap, fmt);
	int len = vsnprintf (buffer_ + length, buff_size_ - length - 2, fmt, ap);
	va_end(ap);

	if (len <= 0) //日志太长了
	{
		printf("error: log is to long\n");
		return;
	}

	length += len;

	// (3)写入"\n"
	buffer_[length] = '\n';
	length++;

	switch (log_level)
	{
	case LOG_LEVEL_DEBUG:
		Color(TNORMAL);
		break;
	case LOG_LEVEL_INFO:
		Color(TBLUE);
		break;
	case LOG_LEVEL_WARN:
		Color(TYELLOW);
		break;
	case LOG_LEVEL_ERROR:
		Color(TRED);
		break;
	default:
		break;
	}

	//---------------------------------------------------------------------
	// 1. 输出到屏幕
	//---------------------------------------------------------------------
	buffer_[length] = '\0';
	length++;
	printf("%s", buffer_);

	Color(TNORMAL); //恢复一下color

	//---------------------------------------------------------------------
	// 2. 输出到文件
	//---------------------------------------------------------------------
	LogData* log_data = new LogData();
	log_data->op_type = 1;
	log_data->data_len = length;
	log_data->data = new char[length];
	memcpy (log_data->data, buffer_, length);
	log_data->year = year;
	log_data->month = month;
	log_data->day = day;
	int size = log_list_.push_back(log_data);
	if (size == 1)
	{
		WakeUp();
	}
}

void CLog::WakeUp()
{
	event_.Notify();
}

void CLog::Save()
{
	LogData* log_data = new LogData();
	log_data->op_type = 2;
	log_list_.push_back(log_data);

	// 唤醒线程
	WakeUp();
}
