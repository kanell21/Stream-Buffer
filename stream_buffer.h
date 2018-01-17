#ifndef STREAM_BUFFER_H
#define STREM_BUFFER_H

#include <cmath>
#include <iostream>
#include <vector>
#include <queue>
//#include "globals.h"
using namespace std;
//typedef unsigned int  ADDRINT;

template <class BufferType>
class StreamBuffer
{
	public:
			std::queue<BufferType> fifo_buffer;
		unsigned int buffer_size;
		StreamBuffer(unsigned int buff_size);
//                static void splitAddr(BufferType addr,BufferType &tag,unsigned int cache_line);
};	

template <class BufferType>
class StreamBufferModule
{
	private:
		vector< StreamBuffer<BufferType> > sbuff; //Stream Buffers Vector - LRU policy
		unsigned int sbuff_num; // # of Stream buffers
		unsigned int sbuff_size; // Stream buffer size
		unsigned int cache_line;

	public:
		bool access(BufferType addr);
		void replace_LRU(BufferType addr);
		unsigned int get_sbuffnum();
		unsigned int get_sbuffsize();
		void createNewStream(BufferType addr,typename vector< StreamBuffer <BufferType> >::iterator bf);
		StreamBufferModule(unsigned int num,unsigned int depth,unsigned int cache_line);
};



template <class BufferType>
StreamBuffer<BufferType>::StreamBuffer(unsigned int buff_size)
{
	buffer_size = buff_size;
}	

static void splitAddr(long unsigned int address,long unsigned int &tag,unsigned int cache_line)
{
	tag = address >> int(log2(cache_line));
}



template <class BufferType>
unsigned int StreamBufferModule<BufferType>::get_sbuffnum(){
	return sbuff_num;
}

template <class BufferType>
unsigned int StreamBufferModule<BufferType>::get_sbuffsize(){
	return sbuff_size;
}



template <class BufferType>
bool StreamBufferModule<BufferType>::access(BufferType addr){
	BufferType prefetch_tag;
	BufferType tag;
	splitAddr(addr,tag,cache_line); // find the tag of the cache line
	unsigned int not_exists=0;
	for(typename vector< StreamBuffer<BufferType> >::iterator  it = sbuff.begin() ; it != sbuff.end(); ++it){
		if(!(*it).fifo_buffer.empty()){
			if((*it).fifo_buffer.front() == addr  ){ 
				prefetch_tag=(*it).fifo_buffer.front();
				(*it).fifo_buffer.pop();
				prefetch_tag+=(*it).buffer_size;
				(*it).fifo_buffer.push(prefetch_tag);
						return true;
			}
			else {
				not_exists+=1;
			}
				
		}
		else {
			createNewStream(tag,it);
			return false;
		}
			
	}
	 if ( not_exists == sbuff_num ){  
		 replace_LRU(tag); 
		 return false;
	 }
	 return false;
}
template <class BufferType>
void StreamBufferModule<BufferType>::replace_LRU(BufferType addr){
	sbuff.pop_back(); // If stream not empty 
	sbuff.insert(sbuff.begin(),1,StreamBuffer<BufferType>(sbuff_size));
	for(unsigned int i=1 ; i<= sbuff_size ;i++)
		sbuff.front().fifo_buffer.push(addr+i); // Prefetch the stream from L2 ( Needs improvement)
		return;
}
//Replace least recently used to allocate new stream - LRU
template <class BufferType>
void StreamBufferModule<BufferType>::createNewStream(BufferType addr,typename vector< StreamBuffer<BufferType> >::iterator bf){
	sbuff.erase(bf);
	sbuff.insert(sbuff.begin(),1,StreamBuffer<BufferType>(sbuff_size));
	for(unsigned int i=1 ; i<= sbuff_size ;i++){
		sbuff.front().fifo_buffer.push(addr+i);
//		cout <<  "Pushing addr" << (addr+i) ;
	}
	return;
		
}//On L1 cache miss and Stream Buffer Miss create a new stream
template <class BufferType>
StreamBufferModule<BufferType>::StreamBufferModule(unsigned int sbuff_n,unsigned int sbuff_s,unsigned int line_size){ // Create a specific number of stream_buffers
		sbuff_num=sbuff_n;
		sbuff_size=sbuff_s;
		cache_line=line_size;
		for (unsigned int i=0; i<sbuff_num;i++){
			sbuff.push_back(StreamBuffer<BufferType>(sbuff_size));
		}
}

#endif
