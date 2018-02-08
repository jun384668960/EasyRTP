#include <stdio.h>
#include "H264Analyzer.h"
#include "fnc_log.h"
#include "base64.h"

unsigned char prevPtr1 = '\0'; 
unsigned char prevPtr2 = '\0'; 
unsigned char prevPtr3 = '\0'; 

unsigned char* sps = NULL;
unsigned int sps_len = NULL;
unsigned char* pps = NULL;
unsigned int pps_len = NULL;

//存在把0x00000001拆分的BUG
int H264_nalu_analyzer(unsigned char* data, unsigned int len, unsigned char *nal_type, int *offset, int *nal_len, int* end_bit)
{
	int ret = -1;
	int offset2 = 0;
	ret = H264_find_start(data, len, offset);
	if(ret == 0)//find it
	{
		//skip head data witch unknow nalu type 
		if(*offset >= 4)
		{
			fnc_log(FNC_LOG_DEBUG,"H264_find_start find it *offset:%d\n",*offset);
			*nal_len = *offset - 3;
			*offset = 0;

			//end bit
			*end_bit = 0;
			return -1;
		}

		//find end 
		if(len - 4 == 0)
		{
			*nal_len = 0;
			*end_bit = 1;
			return -2;
		}
		
		*nal_type = data[4] & 0x1F;
			
		ret = H264_find_start(data + 4, len - 4, &offset2);
		if(ret == 0) // find it 
		{
			fnc_log(FNC_LOG_DEBUG,"H264_find_start find it *offset:%d offset2:%d\n",*offset,offset2);
			*nal_len = offset2 - 3;// 4 is 0x00000001 lenght

			if(*nal_type == 7)//sps
			{
				if(sps != NULL)
				{
					free(sps);
					sps == NULL;
				}
				sps = (unsigned char*)malloc(sizeof(unsigned char) * (*nal_len));
				memcpy(sps, data + *offset + 1, *nal_len);
				sps_len = *nal_len;
			}
			else if(*nal_type == 8) //pps
			{
				if(pps != NULL)
				{
					free(pps);
					pps == NULL;
				}
				pps = (unsigned char*)malloc(sizeof(unsigned char) * (*nal_len));
				memcpy(pps, data + *offset + 1, *nal_len);
				pps_len = *nal_len;
			}
			//*offset = offset2;
			//exit(1);
		}
		else
		{
			fnc_log(FNC_LOG_DEBUG,"H264_find_start unfind it *offset:%d offset2:%d\n",*offset,offset2);
			*nal_len = len - 4;
			//*offset = len;
		}

		//end bit
		*end_bit = 1;
		
		*offset = *offset + 1;
		return 0;
	}
	else	//whole nalu data
	{
		*end_bit = 0;
		*nal_len = len;
		*offset = 0;
		return -1;
	}
}

int H264_find_start(unsigned char* data, unsigned int len, int *offset)
{
	int i = 0;
	
//	prevPtr1 = '\0'; 
//	prevPtr2 = '\0'; 
//	prevPtr3 = '\0'; 
	
	unsigned char* ptr = data;
	while( i <= len)
	{
		if(prevPtr1 == 0x00 && prevPtr2 == 0x00 && prevPtr3 == 0x00 && ptr[i] == 0x01)
		{
			*offset = i;
			prevPtr1 = prevPtr2;
			prevPtr2 = prevPtr3;
			prevPtr3 = *(ptr + i);
			
			return 0;
		}

		prevPtr1 = prevPtr2;
		prevPtr2 = prevPtr3;
		prevPtr3 = *(ptr + i);

		i++;
	}

	return -1;
}

int H264_base64_sps(unsigned char* data)
{
	if(data == NULL || sps == NULL)
	{
		return -1;
	}
	base64_encode(sps,data,sps_len);

	return 0;
}

int H264_base64_pps(unsigned char* data)
{
	if(data == NULL || pps == NULL)
	{
		return -1;
	}
	base64_encode(pps,data,pps_len);

	return 0;
}

int H264NaluAnalyzer(unsigned char* data, unsigned int len, unsigned char *nal_type, int *offset, int *nal_len)
{
	//find start bit
	int ret = -1;
	int offset2 = 0;

	ret = H264FindStartBit(data, len, offset);
	if(ret == 0)
	{
		if((*offset) > 0)
		{
			*nal_type = 255;
			*nal_len = (*offset);
			
			if(*nal_len == 103)
			{
				fnc_log(FNC_LOG_DEBUG, "*offset:%d len:%d\n",*offset, len);
			}
			
			return 0;
		}
		
		if(((*offset) + 4) < len)
		{
			*nal_type = data[(*offset) + 4] & 0x1F;
			//fnc_log(FNC_LOG_DEBUG, "H264FindStartBit *offset:%d *nal_type:%d\n", *offset,*nal_type);
		}
		else
		{
			*nal_len = 4;
			return 0;
		}
		//return 0;
	}
	else
	{
		*nal_len = len;
		*offset = -1;
		if(*nal_len == 103)
		{
			fnc_log(FNC_LOG_DEBUG, "0 *offset:%d len:%d\n",*offset, len);
		}
		return 0;
	}
	
	//find next start bit
	ret = H264FindStartBit(data+((*offset) + 4), len-((*offset) + 4), &offset2);
	if(ret == 0)
	{
		//fnc_log(FNC_LOG_DEBUG, "H264FindStartBit offset2:%d\n", offset2);
		*nal_len = offset2 + 4;
		if(*nal_len == 103 && *nal_type == 9)
		{
			fnc_log(FNC_LOG_DEBUG, "1 *offset:%d offset2:%d len:%d len-((*offset) + 4):%d\n",*offset,offset2, len,len-((*offset) + 4));
		}
		return 0;
	}
	else
	{
		*nal_len = len;
		if(*nal_len == 103 && *nal_type == 9)
		{
			fnc_log(FNC_LOG_DEBUG, "2 *offset:%d len:%d\n",*offset, len);
		}
		return 0;
	}
}

int H264FindStartBit(unsigned char* data, unsigned int len, int *offset)
{
	int i = 0;
	unsigned char* ptr = data;
	//fnc_log(FNC_LOG_DEBUG, "len:%d\n",len);
	if(len < 4)
	{
		return -1;
	}
	
	while( i < len - 4 )
	{
		//unsigned int test4Bytes = (ptr[i+0]<<24)|(ptr[i+1]<<16)|(ptr[i+2]<<8)|ptr[i+3];

		//if(test4Bytes == 0x00000001)//find it
		if(ptr[i+0] == 0x00 && ptr[i+1] == 0x00 && ptr[i+2] == 0x00 && ptr[i+3] == 0x01)
		{
			*offset = i;
			//fnc_log(FNC_LOG_DEBUG, "find it test4Bytes:%x *offset:%d\n",test4Bytes, *offset);
			return 0;
		}

		i++;
	}
	*offset = -1;
	
	return -1;
}