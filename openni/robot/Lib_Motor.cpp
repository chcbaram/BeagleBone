#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>




#include "./Main_Lib/Define.h"

#include "./Main_Lib/Uart_Lib.h"
#include "./Main_Lib/Uart_Sig_Lib.h"
#include "./Main_Lib/Thread_Lib.h"                                                                                          
                                                                                                 
#include "./Main_Lib/Uart_Lib.c"
#include "./Main_Lib/Uart_Sig_Lib.c"



int Lib_Motor_Debug = 0;

int Lib_Motor_PwmLeft  = 0;
int Lib_Motor_PwmRight = 0;



void *Lib_Motor(void *Arg)
{
	THREAD_OBJ *pArg = (THREAD_OBJ *)Arg;

	printf("Enter Motor Thread \n");

	while(pArg->Thread_Stop == FALSE)
	{
		if( Lib_Motor_Debug == 1 )
		{
			printf("Pwm %d %d\n", Lib_Motor_PwmLeft, Lib_Motor_PwmRight );	
		}
		usleep(100*1000);
	}

	printf("Exit Motor Thread \n");
}




