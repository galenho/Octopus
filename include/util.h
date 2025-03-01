/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: 
*/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "common.h"

//----------------------------------------------------------------------------------------
void AddPoint(long p, int nNum);
void DelPoint(long p);
void PrintPointsInfo();

#define SAFE_NEWPOINT(P,T,N) try{ \
	P=NULL; \
	P=new T; \
	AddPoint((long)P, N); \
} catch (...) { \
}

#define SAFE_DELETE(P)       { if(P) { DelPoint((long)P);	delete P;   P=NULL;} }
#define SAFE_DELETE_ARRAY(P) { if(P) { DelPoint((long)P);	delete[] P; P=NULL;} }

//----------------------------------------------------------------------------------------
// ��Сд
void TOLOWER(string& str);

// ���д
void TOUPPER(string& str);

// ������GM����
void StrSplit(const string& src, const char& sep, vector<string>& out_list);

// ����http get���ֶβ�������
void StrSplitLasSpace(const string& src, const char& sep, vector<string>& out_list);

// SQL��ע�뺯��
bool sql_inj(string& content);

unsigned long ip_string2int(const string& str_ip);
string ip_int2string(unsigned int ip_value);

string now_string();
int64 now_second();

string now_date_string();

int64 mk_second(int year, int mon, int day,
				 int hour, int min, int sec);

void  mk_datatime(uint64 seconds,
				  int& year, int& mon, int& day,
				  int& hour, int& min, int& sec,
				  int& wday);

/** 
 *  strΪ�����ַ��� 
 *  formatStr Ϊʱ���Ӧ�ĸ�ʽ, 
 *  ��2012-07-04 15:33:52��Ӧ�ĸ�ʽΪ%d-%d-%d %d:%d:%d 
 */  
uint64 time_from_string(const char* str);

bool FloatEqual(float value0, float value1);

int32 AreaMin(int32 value, int32 min_value);
int32 AreaMax(int32 value, int32 max_value);
int32 AreaMiddle(int32 value, int32 min_value, int32 max_value);

float AreaMinFloat(float value, float min_value);
float AreaMaxFloat(float value, float max_value);
float AreaMiddleFloat(float value, float min_value, float max_value);

// �õ�������
string GetProgressName();

#endif //_UTIL_H_