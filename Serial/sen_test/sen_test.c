//----------------------------------------------------------------------------------------------
//            TITLE       : 적외선 통신 실험을 위한 프로그램
//            WORK        :
//            DATE        : 2004. 2. 12
//            FILE        : Sen_Lib.h
//            Modified By : - IRSL -
//----------------------------------------------------------------------------------------------




#include <stdio.h>


#include "../Main_Lib/Define.h"

#include "../Main_Lib/KeyBrd_Lib.h"
#include "../Main_Lib/Uart_Lib.h"
#include "../Main_Lib/Uart_Sig_Lib.h"
                                                                                                 
                                                                                                 
#include "../Main_Lib/KeyBrd_Lib.c"
#include "../Main_Lib/Uart_Lib.c"
#include "../Main_Lib/Uart_Sig_Lib.c"




int Key_Proc( void );
void Main_Init( void );



void Uart_Rxd_Func( char Data )
{
	static First_Check = 0;
	
	
	if( First_Check )
	{
		printf("%x\n", Data );
	}
	
	First_Check = 1;
}


int main( void )
{
	char i;
	char Key_Data;
	char Uart_Data;	

	//-----  초기화
	//
	Main_Init();

	printf("Irda Test Pgm..\n");	
	printf("Irda Speed : 2400 BPS\n");	
	printf("Q key : Exit Pgm..\n");	




	while(1)
	{

		//usleep( 500*1000 );
                
        
		/*			
		// 데이터가 왔을경우에
		if( UART_Q_VAILD( UART_Q_0 ) )
		{
                                                                                                 
			Uart_Q_Pop( UART_Q_0, &Uart_Data );
			
			Uart_Print("ACK : ");
			Uart_Putch( Uart_Data );  // 시리얼로 문자 출력
			Uart_Print("\n");
			
			
			if( Uart_Data == 'q' ) break;
		}
		*/
		
		
		//Uart_Data = Uart_Getch();
		//Uart_Print("ACK : ");
		//Uart_Putch( Uart_Data );  // 시리얼로 문자 출력
		//Uart_Print("\n");		
			
		// 키보드 처리
		//if( Key_Proc() ) break;
		
		if( KeyBrd_Hit() )
		{
			Key_Data = KeyBrd_Getch();
			printf("Key Pressed %c\r\n", Key_Data);
			
			if( Key_Data == 'q' ) break;
		}

	}


	//----- 종료 처리
	//	
	//Uart_ClosePort();
	KeyBrd_Close();

	return 0;
}




//----------------------------------------------------------------------------------------------
//            TITLE   : Main_Init
//
//            WORK    :
//
//            DATE    : 2003. 6. 4
//----------------------------------------------------------------------------------------------
void Main_Init( void )
{
	int  Uart_Handle_Ptr;
                                                                                                 
	KeyBrd_Init();
	//Uart_Handle_Ptr = Uart_Open( COME_EX1, BAUD_115200 );
	//Uart_Sig_Init( Uart_Handle_Ptr );
	//Uart_Sig_Func_Init( Uart_Handle_Ptr, Sen_Receive_Handle );
	//Uart_Sig_Func_Init( Uart_Handle_Ptr, Uart_Rxd_Func );
}




//----------------------------------------------------------------------------------------------
//            TITLE   : Key_Proc
//
//            WORK    : 키가 눌렸는지 검사해서 처리한다
//                      q 를 누르면 프로그램 종료하고
//                      그렇지 않으면 시리얼로 문자 데이터 전송한다
//
//            DATE    : 2003. 6. 4
//----------------------------------------------------------------------------------------------
int Key_Proc( void )
{

	char Key_Data;

	if( KeyBrd_Hit() )
	{
		//Key_Data = KeyBrd_Getch();
                                                                                                 
		if( Key_Data == 'q' ) return 1;
                                                                                                 
		// DOS 모드와 호환을 위해서
		// 도스에서는 엔터키가 0x0d 즉 13이다
		if( Key_Data == 10 ) Key_Data = 13;
                                                                                                 
		//KeyBrd_Putch( Key_Data );
		//Uart_Putch( Key_Data );		
                                                                                                 
	}

	return 0;
                                                                                                 
}








