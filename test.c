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

#define BUFFER_SIZE 10
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
		xil_printf("Enter string: ");
		
		/* Initialize RxBuffer */
		rxCount = 0;
		RxBuffer[0] = 0x00;
		
		while(1){
			rxBytes = XUartLite_Recv(&UartLiteInst, &RxBuffer[rxCount], 1);
			if ((rxBytes != 0) && (RxBuffer[rxCount] == 0x0D)){
				break;
			}
			rxCount += rxBytes;
		}
		
		xil_printf("Data: 0x");
		for (rxBytes=0; rxBytes<rxCount; rxBytes++){
			xil_printf("%02X", RxBuffer[rxBytes]);
			XUartLite_Recv(&UartLiteInst, &RxBuffer[rxBytes], 1)
		}
		xil_printf("\n")
	}
	
    cleanup_platform();
    return 0;
}
