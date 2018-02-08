#ifndef H264_ANLYZER_H
#define H264_ANLYZER_H

int H264_nalu_analyzer(unsigned char* data, unsigned int len, unsigned char *nal_type, int *offset, int *nal_len, int* end_bit);
int H264_find_start(unsigned char* data, unsigned int len, int *offset);
int H264_base64_sps(unsigned char* data);
int H264_base64_pps(unsigned char* data);

int H264NaluAnalyzer(unsigned char* data, unsigned int len, unsigned char *nal_type, int *offset, int *nal_len);
int H264FindStartBit(unsigned char* data, unsigned int len, int *offset);

#endif