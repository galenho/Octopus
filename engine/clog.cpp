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
				// ���ļ��Ƿ����, ������ڵĻ�, ��Ҫ�õ��ļ��Ĵ�С, ���Ƿ�����趨�����ߴ�
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

				if (log_file_list.size() > 0) //�ҵ������ļ�
				{
					LogFileInfo& info = log_file_list[log_file_list.size() - 1];
					full_name_ = info.full_name;
					last_num_ = info.num;
					last_log_buff_size_ = info.size;
				}
				else //�ļ�������
				{
					full_name_ = file_name + "_" + to_string(year) + "_" + to_string(month) + "_" + to_string(day) + "_" + to_string(num) + ".txt";;
					last_log_buff_size_ = 0;
				}
			}
			else // ����Ҫ���
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
	fout.open(full_name_.c_str(), ios::app);//���ļ�

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
				if (is_need_date_) //������
				{
					if (last_year_ != log_data->year || last_month_ != log_data->month || last_day_ != log_data->day) //����ͬһ��
					{
						is_new_file = true;
						last_num_ = 1;
					}
					else
					{
						if (max_log_buff_ > 0) //������ + �ֿ�
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
						fout << flush;	//��д����������д���������ڱ����ļ��򿪵�����±����ļ���
					}
					fout.close();//�ر�

					init(file_name_, is_need_date_, max_log_buff_, false);
					fout.open(full_name_.c_str(), ios::app);//���ļ�
				}

				//--------------------------------------------------------------------------------------
				if (buff_len + log_data->data_len > write_buff_size_)
				{
					fout<<flush;	//��д����������д���������ڱ����ļ��򿪵�����±����ļ���
					buff_len = 0;
				}

				fout << log_data->data; //д���ļ�
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
			// ���ʣ�����־
			if (buff_len > 0)
			{
				fout<<flush;	//��д����������д���������ڱ����ļ��򿪵�����±����ļ���
			}
			is_save_soon = false;
		}

		if (is_running_)
		{
			event_.Wait();
		}
	}

	// ���ʣ�����־
	if (buff_len > 0)
	{
		fout<<flush;	//��д����������д���������ڱ����ļ��򿪵�����±����ļ���
	}
	fout.close();//�ر�

	is_thread_exit_ = true;

	return true;
}

void CLog::StopWaitExit()
{
	LogData* log_data = new LogData();
	log_data->op_type = 3;
	log_list_.push_back(log_data);

	// �����߳�
	WakeUp();

	while (!is_thread_exit_) //ֱ���߳��˳�
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

	// (1)д������ͷ
	sprintf(buffer_, "%04u-%02u-%02u %02u:%02u:%02u ", 
		year, month, day,
		localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

	length += 20;

	// (2)д����־��
	va_list ap;
	va_start(ap, fmt);
	int len = vsnprintf (buffer_ + length, buff_size_ - length - 2, fmt, ap);
	va_end(ap);

	if (len <= 0) //��־̫����
	{
		printf("error: log is to long\n");
		return;
	}

	length += len;

	// (3)д��"\n"
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
	// 1. �������Ļ
	//---------------------------------------------------------------------
	buffer_[length] = '\0';
	length++;
	printf("%s", buffer_);

	Color(TNORMAL); //�ָ�һ��color

	//---------------------------------------------------------------------
	// 2. ������ļ�
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

	// �����߳�
	WakeUp();
}
