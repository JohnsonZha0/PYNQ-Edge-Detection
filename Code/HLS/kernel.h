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

//定义数据流中的数据
typedef ap_axiu<32,1,1,1> 	uint_side_channel;
typedef ap_axis<32,1,1,1> 	int_side_channel;
//定义AXI—Stream数据流
typedef hls::stream<uint_side_channel> 							AXI_STREAM;
//定义char类型的卷积窗口
typedef hls::Window<KERNEL_DIM,KERNEL_DIM,char> 				CHAR_WINDOW;
//定义short类型的卷积窗口
typedef hls::Window<KERNEL_DIM,KERNEL_DIM,short> 				SHORT_WINDOW;
//定义LineBuffer用于延时
typedef hls::LineBuffer<KERNEL_DIM,IMG_WIDTH,unsigned char> 	BUFFER;

//参考函数
short convolved2d(CHAR_WINDOW * window,char kernel[KERNEL_DIM * KERNEL_DIM]);


//求卷积和
short sumWindow(SHORT_WINDOW *window);
//求卷积窗口中的最小值
short minWindow(SHORT_WINDOW *window);
//求卷积窗口中的最大值
short maxWindow(SHORT_WINDOW *window);

//顶层函数
void doKernel(AXI_STREAM & inStream,AXI_STREAM & outStream, char kernel[KERNEL_DIM*KERNEL_DIM], int operation);


#endif
