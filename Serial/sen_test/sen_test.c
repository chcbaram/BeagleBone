//----------------------------------------------------------------------------------------------
//            TITLE       : ���ܼ� ��� ������ ���� ���α׷�
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

	//-----  �ʱ�ȭ
	//
	Main_Init();

	printf("Irda Test Pgm..\n");	
	printf("Irda Speed : 2400 BPS\n");	
	printf("Q key : Exit Pgm..\n");	
	

	while(1)
	{

		//usleep( 500*1000 );
                
        
		/*			
		// �����Ͱ� ������쿡
		if( UART_Q_VAILD( UART_Q_0 ) )
		{
                                                                                                 
			Uart_Q_Pop( UART_Q_0, &Uart_Data );
			
			Uart_Print("ACK : ");
			Uart_Putch( Uart_Data );  // �ø���� ���� ���
			Uart_Print("\n");
			
			
			if( Uart_Data == 'q' ) break;
		}
		*/
		
		
		Uart_Data = Uart_Getch();
		Uart_Print("ACK : ");
		Uart_Putch( Uart_Data );  // �ø���� ���� ���
		Uart_Print("\n");		
			
		// Ű���� ó��
		//if( Key_Proc() ) break;

	}


	//----- ���� ó��
	//	
	Uart_ClosePort();
	//KeyBrd_Close();

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
                                                                                                 
        //KeyBrd_Init();
        Uart_Handle_Ptr = Uart_Open( COME_EX1, BAUD_115200 );
        //Uart_Sig_Init( Uart_Handle_Ptr );
	//Uart_Sig_Func_Init( Uart_Handle_Ptr, Sen_Receive_Handle );
	//Uart_Sig_Func_Init( Uart_Handle_Ptr, Uart_Rxd_Func );
}




//----------------------------------------------------------------------------------------------
//            TITLE   : Key_Proc
//
//            WORK    : Ű�� ���ȴ��� �˻��ؼ� ó���Ѵ�
//                      q �� ������ ���α׷� �����ϰ�
//                      �׷��� ������ �ø���� ���� ������ �����Ѵ�
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
                                                                                                 
		// DOS ���� ȣȯ�� ���ؼ�
		// ���������� ����Ű�� 0x0d �� 13�̴�
		if( Key_Data == 10 ) Key_Data = 13;
                                                                                                 
		//KeyBrd_Putch( Key_Data );
		Uart_Putch( Key_Data );		
                                                                                                 
	}

	return 0;
                                                                                                 
}








