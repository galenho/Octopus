/*
** Octopus
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     
*/

#ifndef _SEQUENCE_BUFFER_H_
#define _SEQUENCE_BUFFER_H_

#include "common.h"

struct BuffPair
{
	uint8 *data;
	int32 len;

	BuffPair()
	{
		len = 0;
		data = NULL;
	}

	~BuffPair()
	{
		if (data)
		{
			delete[] data;
			data = NULL;
		}
	}
};

class SequenceBuffer
{
public:
	SequenceBuffer();
	~SequenceBuffer();

	void Allocate(int size);
	void Release();

	bool Write(const void* data, int bytes);
	bool WriteMsg( const void* head_data, int head_len, const void* body_data, int body_len );
	void Remove(int bytes);
	
	void IncrementWritten(int bytes);

	void* GetBuffer();
	void* GetBufferStart();

	int GetSpace();
	int GetSize();

	void PrintInfo();

	bool FillVector();

private:
	uint8* buffer_;
	int len_;

	int total_size_;

	std::list<BuffPair*> delay_buff_list_;
};



#endif		//_SEQUENCE_BUFFER_H_

