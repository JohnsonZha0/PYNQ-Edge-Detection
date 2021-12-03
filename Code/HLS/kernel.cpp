#include "kernel.h"

void doKernel(AXI_STREAM & inStream,AXI_STREAM & outStream, char kernel[KERNEL_DIM*KERNEL_DIM], int operation)
{
#pragma HLS ARRAY_PARTITION COMPLETE variable=kernel
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=operation bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=kernel bundle=CTRL_BUS
#pragma HLS INTERFACE axis register both port=outStream
#pragma HLS INTERFACE axis register both port=inStream

	BUFFER lineBuf;
	SHORT_WINDOW window;

	//用来标记行和列的索引值
	int pixConvolved = 0;

	//计算延时，填充line_buffer
	int waitPixel = IMG_WIDTH + 1;
	int cntWait = 0;
	int sentPixel = 0;
	unsigned char pix;

	uint_side_channel dataOutChannel;
	uint_side_channel currentChannel;


	for(int i = 0; i < (IMG_HEIGHT); i++ )
	{
		for(int j = 0; j < (IMG_WIDTH); j++)
		{
#pragma HLS PIPELINE
			currentChannel = inStream.read();
			unsigned char R = currentChannel.data & 0xFF;
			unsigned char G = (currentChannel.data >> 8) & 0xFF;
			unsigned char B = (currentChannel.data >> 16) & 0xFF;
			pix = (R*76 + G*150 + B*30) >> 8;
			lineBuf.shift_pixels_up(j);
			lineBuf.insert_bottom_row(pix,j);
			//put data on the window, and multiply by kernel
			for(int idxWinRow = 0; idxWinRow < KERNEL_DIM; idxWinRow++)
			{
				for(int idxWinCol=0; idxWinCol < KERNEL_DIM; idxWinCol++)
				{
					//idxWinCol and pixConvolved will slide the window
					short val = (short)lineBuf.getval(idxWinRow, idxWinCol + pixConvolved);
					//采样窗口中的数据与Kernel(卷积算子)相乘
					val = (short)kernel[(idxWinRow * KERNEL_DIM) + idxWinCol] * val;
					window.insert(val,idxWinRow,idxWinCol);
				}
			}
			short valOut=0;
			if((i >= KERNEL_DIM-1) && (j >= KERNEL_DIM -1))
			{
				switch(operation)
				{
				case 0:
				{
					valOut = sumWindow(&window);
					if(valOut < 0 ){
						valOut = 0;
					}
					break;
				}

				case 1:
				{
					valOut = minWindow(&window);
					break;
				}

				case 2:
				{
					valOut = maxWindow(&window);
					break;
				}
				default:;
				}

				pixConvolved++;
				dataOutChannel.data = (valOut << 16) | (valOut << 8) | valOut;
				dataOutChannel.dest = currentChannel.dest;
				dataOutChannel.id = currentChannel.id;
				dataOutChannel.keep = currentChannel.keep;
				dataOutChannel.strb = currentChannel.strb;
				dataOutChannel.user = currentChannel.user;
				dataOutChannel.last = currentChannel.last;
				outStream.write(dataOutChannel);
				sentPixel++;

			}
			else
			{
				dataOutChannel.data = 0;
				dataOutChannel.dest = currentChannel.dest;
				dataOutChannel.id = currentChannel.id;
				dataOutChannel.keep = currentChannel.keep;
				dataOutChannel.strb = currentChannel.strb;
				dataOutChannel.user = currentChannel.user;
				dataOutChannel.last = currentChannel.last;
				outStream.write(dataOutChannel);
				sentPixel++;
			}
		}
		pixConvolved = 0;
	}
}

//求卷积和
short sumWindow(SHORT_WINDOW *window){
	short sum=0;
	for(int idxRow = 0; idxRow < KERNEL_DIM; idxRow++){
		for(int idxCol = 0;idxCol < KERNEL_DIM;idxCol++){
			sum += (short)window->getval(idxRow, idxCol);
		}
	}
	return sum;

}
//求卷积窗口中的最小值
short minWindow(SHORT_WINDOW *window){
	unsigned char minVal = 255;
	for(int idxRow = 0; idxRow < KERNEL_DIM; idxRow++){
		for(int idxCol = 0; idxCol < KERNEL_DIM; idxCol++){
			unsigned char valInWindow;
			valInWindow = window->getval(idxRow, idxCol);
			if(valInWindow < minVal){
				minVal = valInWindow;
			}
		}
	}
	return minVal;
}
//求卷积窗口中的最大值
short maxWindow(SHORT_WINDOW *window){
	unsigned char maxVal = 0;
	for(int idxRow=0;idxRow<KERNEL_DIM;idxRow++){
		for(int idxCol=0;idxCol<KERNEL_DIM;idxCol++){
			unsigned char valInWindow;
			valInWindow = window->getval(idxRow, idxCol);
			if(valInWindow > maxVal){
				maxVal = valInWindow;
			}
		}
	}
	return maxVal;
}


