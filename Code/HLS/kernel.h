#ifndef __TEST_HEAD_H__
#define __TEST_HEAD_H__

#include <hls_video.h>
#include <ap_axi_sdata.h>

#define KERNEL_DIM 3
#define IMG_WIDTH 1024
#define IMG_HEIGHT 768
#define INPUT_IMAGE "screen.bmp"
#define OUTPUT_IMAGE "screenWithAdjust.bmp"
#define GOLD_IMAGE   "goldImage.bmp"

//�����������е�����
typedef ap_axiu<32,1,1,1> 	uint_side_channel;
typedef ap_axis<32,1,1,1> 	int_side_channel;
//����AXI��Stream������
typedef hls::stream<uint_side_channel> 							AXI_STREAM;
//����char���͵ľ������
typedef hls::Window<KERNEL_DIM,KERNEL_DIM,char> 				CHAR_WINDOW;
//����short���͵ľ������
typedef hls::Window<KERNEL_DIM,KERNEL_DIM,short> 				SHORT_WINDOW;
//����LineBuffer������ʱ
typedef hls::LineBuffer<KERNEL_DIM,IMG_WIDTH,unsigned char> 	BUFFER;

//�ο�����
short convolved2d(CHAR_WINDOW * window,char kernel[KERNEL_DIM * KERNEL_DIM]);


//������
short sumWindow(SHORT_WINDOW *window);
//���������е���Сֵ
short minWindow(SHORT_WINDOW *window);
//���������е����ֵ
short maxWindow(SHORT_WINDOW *window);

//���㺯��
void doKernel(AXI_STREAM & inStream,AXI_STREAM & outStream, char kernel[KERNEL_DIM*KERNEL_DIM], int operation);


#endif
