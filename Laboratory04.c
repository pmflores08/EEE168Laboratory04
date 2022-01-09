/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "xparameters.h"
#include "xil_io.h"

#include "xil_types.h"
#include "xuartlite.h"

#define BUFFER_SIZE 100
XUartLite UartLiteInst;

int main()
{
    init_platform();

    int Status;
    Status = XUartLite_Initialize(&UartLiteInst, XPAR_AXI_UARTLITE_0_DEVICE_ID);
    if (Status != XST_SUCCESS){
    	xil_printf("UART initialization failed\n");
    }
    Status = XUartLite_SelfTest(&UartLiteInst);
    if (Status != XST_SUCCESS){
    	xil_printf("UART self-test failed\n");
    }

    u8 RxBuffer[BUFFER_SIZE];
    int rxCount, rxBytes;

    while(1){
    	rxCount = 0;
    	RxBuffer[0] = 0x00;

    	while(1){
    		rxBytes = XUartLite_Recv(&UartLiteInst, &RxBuffer[rxCount], 1);
    		if ((rxBytes != 0) && (RxBuffer[rxCount] == 0x0D)){
    			break;
    		}
    		rxCount += rxBytes;
    	}

    	u8 seed = RxBuffer[0];
    	u8 length = RxBuffer[1];
    	Xil_Out32(XPAR_CUSTOMCRC_0_S00_AXI_BASEADDR, 0x00000000);

    	u32 control_and_status_register = 0;
    	control_and_status_register = (control_and_status_register << 24) + (length << 16) + (seed << 8) + 1;
    	xil_printf("Control and Status Register: 0x%08X\n\r",control_and_status_register);

    	u32 data_x_register[7] = {0,0,0,0,0,0,0};
    	int DataRegister = 0;
    	int isFilled = 0;

    	for (int i=0;i<rxCount-2;i++){
    		data_x_register[DataRegister] += (RxBuffer[i+2] << (24-(8*isFilled)));
    		isFilled+=1;
    		if(isFilled == 4){
    			DataRegister+=1;
    			isFilled=0;
    		}
    	}

    	for (int i=0;i<=DataRegister;i++){
    		Xil_Out32(XPAR_CUSTOMCRC_0_S00_AXI_BASEADDR + 8 + (4*i), data_x_register[i]);Xil_Out32(XPAR_CUSTOMCRC_0_S00_AXI_BASEADDR + 8 + (4*i), data_x_register[i]);
    		if (data_x_register[i] == 0){
    			continue;
    		}
    		else{
    			xil_printf("Data%2d",i);
    			xil_printf(" Register: 0x%02X\n\r",Xil_In32(XPAR_CUSTOMCRC_0_S00_AXI_BASEADDR + 8 + (4*i)));
    		}
    	}

    	Xil_Out32(XPAR_CUSTOMCRC_0_S00_AXI_BASEADDR, control_and_status_register);
    	xil_printf("CRC Register: 0x%02X\n\r", Xil_In32(XPAR_CUSTOMCRC_0_S00_AXI_BASEADDR + 4));
    }
    cleanup_platform();
    return 0;
}
