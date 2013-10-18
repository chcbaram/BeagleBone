//----------------------------------------------------------------------------------------------
//            TITLE       : ������ ���̺귯��
//            WORK        :
//            DATE        : 2004. 11. 29
//            FILE        : Thread_Lib.c
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------


//----- ������� ����
//
#ifndef   LIB_INCLUDE
#define  THREAD_LIB_LOCAL  
#include "Thread_Lib.h"
#endif





                                                                                                 
//----------------------------------------------------------------------------------------------
//            TITLE   : Thread_Init
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
void Thread_Init( void )
{
	U8 i;
	
	for( i=0; i<THREAD_MAX; i++ )
	{
		Thread_Obj[i].Thread_Num  = 0;
		Thread_Obj[i].Thread_Use  = 0;
		Thread_Obj[i].Thread_Time = 0;
		Thread_Obj[i].Thread_Mode = 0;
	}
}






//----------------------------------------------------------------------------------------------
//            TITLE   : Thread_Init
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
void Thread_Create( U16 ID, void *(*Thread_Func)(void *), void *Arg_Etc  )
{
	if( Thread_Obj[ID].Thread_Use == THREAD_USE ) return;
	
	Thread_Obj[ID].Thread_Num  = ID;
	Thread_Obj[ID].Thread_Use  = THREAD_USE;	
	Thread_Obj[ID].Thread_Func = Thread_Func; 

}



//----------------------------------------------------------------------------------------------
//            TITLE   : Thread_Start
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
void Thread_Start( U16 ID )
{
	pthread_create( &Thread_Obj[ID].Thread_ID, NULL, Thread_Obj[ID].Thread_Func, NULL );
}




//----------------------------------------------------------------------------------------------
//            TITLE   : Thread_Stop
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
void Thread_Stop( U16 ID )
{
	pthread_join( Thread_Obj[ID].Thread_ID, NULL );
}





//----------------------------------------------------------------------------------------------
//            TITLE   : Thread_Stop
//
//            WORK    : 
//
//            DATE    : 2004. 11. 29
//----------------------------------------------------------------------------------------------
void Thread_Stop_All( void )
{
	U8 i;
	
	for( i=0; i<THREAD_MAX; i++ )
	{
		if( Thread_Obj[i].Thread_Use == THREAD_USE )
		{
			Thread_Stop( i );
		}
	}
}




