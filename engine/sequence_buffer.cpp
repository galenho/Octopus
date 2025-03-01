#include "sequence_buffer.h"
#include "scheduler.h"

SequenceBuffer::SequenceBuffer()
{
	buffer_ = NULL;
	len_ = 0;
	total_size_ = 0;
}

SequenceBuffer::~SequenceBuffer()
{
	Release();
}

void SequenceBuffer::Allocate(int size )
{
	total_size_ = size;
	buffer_ = new uint8[total_size_];
	memset(buffer_, 0, total_size_);
	len_ = 0;
}

void SequenceBuffer::Release()
{
	if (buffer_)
	{
		delete[] buffer_;
		buffer_ = NULL;
	}

	for (std::list<BuffPair*>::iterator it = delay_buff_list_.begin(); it != delay_buff_list_.end(); it++)
	{
		BuffPair* buff = *it;
		delete buff;
		buff = NULL;
	}
	delay_buff_list_.clear();
}

int SequenceBuffer::GetSpace()
{
	return total_size_ - len_;
}

int SequenceBuffer::GetSize()
{
	return len_;
}

bool SequenceBuffer::Write( const void* data, int bytes )
{
	if (bytes > total_size_)
	{
		PRINTF_ERROR("Write overflow, bytes(%d) > total_size(%d)", bytes, total_size_);
		return false;
	}

	if (delay_buff_list_.size() > 0) //说明队列中是有待发送的消息, 应该插入排到队尾
	{
		BuffPair* delay_buff = new BuffPair();
		delay_buff->data = new uint8[bytes];
		memcpy(delay_buff->data, data, bytes);
		delay_buff->len = bytes;
		delay_buff_list_.push_back(delay_buff);

		FillVector(); //尝试把队列中的缓存加入写缓存区

		return true;
	}
	else
	{
		if (GetSpace() < bytes)
		{
			BuffPair* delay_buff = new BuffPair();
			delay_buff->data = new uint8[bytes];
			memcpy(delay_buff->data, data, bytes);
			delay_buff->len = bytes;
			delay_buff_list_.push_back(delay_buff);
		
			return true;
		}
		else
		{
			memcpy(buffer_ + len_, data, bytes);
			len_ += bytes;

			return true;
		}
	}
}

bool SequenceBuffer::WriteMsg( const void* head_data, int head_len, const void* body_data, int body_len )
{
	int bytes = head_len + body_len;

	if (bytes > total_size_)
	{
		PRINTF_ERROR("Write overflow, bytes(%d) > total_size(%d)", bytes, total_size_);
		return false;
	}

	if (delay_buff_list_.size() > 0) //说明队列中是有待发送的消息, 应该插入排到队尾
	{
		BuffPair* delay_buff = new BuffPair();
		delay_buff->data = new uint8[bytes];
		memcpy(delay_buff->data, head_data, head_len);
		memcpy(delay_buff->data + head_len, body_data, body_len);
		delay_buff->len = bytes;
		delay_buff_list_.push_back(delay_buff);

		FillVector(); //尝试把队列中的缓存加入写缓存区

		return true;
	}
	else
	{
		if (GetSpace() < bytes)
		{
			BuffPair* delay_buff = new BuffPair();
			delay_buff->data = new uint8[bytes];
			memcpy(delay_buff->data, head_data, head_len);
			memcpy(delay_buff->data + head_len, body_data, body_len);
			delay_buff->len = bytes;
			delay_buff_list_.push_back(delay_buff);

			return true;
		}
		else
		{
			memcpy(buffer_ + len_, head_data, head_len);
			len_ += head_len;

			memcpy(buffer_ + len_, body_data, body_len);
			len_ += body_len;

			return true;
		}
	}
}

void SequenceBuffer::Remove(int bytes)
{
	//static int total_times = 0;
	//static int move_times = 0;

	//static int move_byte_len = 0;

	//total_times++;
	if (bytes > 0 && len_ >= bytes)
	{
		if (len_ == bytes) //全部移除
		{
			// 不用管(优化)
		}
		else //向前移
		{
			//move_times++;
			//move_byte_len += (len_ - bytes);

			memmove(buffer_, buffer_ + bytes, len_ - bytes);
		}
		
		len_ -= bytes;

		//PRINTF_DEBUG("%d = %d", bytes, len_);
	}

	//if (total_times % 1000 == 0)
	//{
	//	PRINTF_INFO("total_times = %d, move_times = %d, move_byte_len = %d", total_times, move_times, move_byte_len);
	//}

	FillVector(); //尝试把队列中的缓存加入写缓存区
}

void SequenceBuffer::IncrementWritten(int bytes )
{
	len_ += bytes;
}

void* SequenceBuffer::GetBuffer()
{
	return buffer_ + len_;
}

void* SequenceBuffer::GetBufferStart()
{
	return buffer_;
}

void SequenceBuffer::PrintInfo()
{
	for (int i=0; i < len_; i++)
	{
		uint8 data = *((uint8*)(buffer_ + i)); 
		PRINTF_DEBUG("%2d:%d", i, data);
	}
}

bool SequenceBuffer::FillVector()
{
	bool ret = true;
	while (delay_buff_list_.size() > 0)
	{
		BuffPair *buff_pair = delay_buff_list_.front();
		
		if (GetSpace() < buff_pair->len)
		{
			ret = false;
			break;
		}
		else
		{
			memcpy(buffer_ + len_, buff_pair->data, buff_pair->len);
			len_ += buff_pair->len;

			delay_buff_list_.pop_front();

			delete buff_pair;
			buff_pair = NULL;
		}
	}

	return ret;
}