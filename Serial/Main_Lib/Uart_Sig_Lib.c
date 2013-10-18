//----------------------------------------------------------------------------------------------
//            TITLE       : Uart ���� ���� �Լ� ���
//            WORK        :
//            DATE        : 2003. 7. 4
//            FILE        : Uart_Q_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------



                                                                                                 


                                                                                                 
//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Sig_Init
//
//            WORK    : �ø��� �ñ׳� �ʱ�ȭ
//
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------
void Uart_Sig_Init( int Handle_Ptr )
{
	U8 i;

	//----- �ø�����Ʈ �ñ׳� �ڵ鷯 ���� 
	//
	Sig_Action.sa_handler  = Uart_Q_Sig_Handler;
	//Sig_Action.sa_mask     = 0;
	Sig_Action.sa_flags    = 0;
	Sig_Action.sa_restorer = NULL; 
	sigaction( SIGIO, &Sig_Action, NULL );

	// SIGIO �ñ׳��� ������ �ֵ��� �Ѵ�
	fcntl( Handle_Ptr, F_SETOWN, getpid() );  
	fcntl( Handle_Ptr, F_SETFL, FASYNC );  // ���� ��ũ���͸� �񵿱����

}	




//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Sig_Func_Init
//
//            WORK    : �ø��� �ñ׳� �ʱ�ȭ
//
//            DATE    : 2004. 2. 7
//----------------------------------------------------------------------------------------------
void Uart_Sig_Func_Init( int Handle_Ptr, void (*Func_Ptr)(char Data) )
{
	U8 i;

	//----- �ø�����Ʈ �ñ׳� �ڵ鷯 ���� 
	//
	Sig_Action.sa_handler  = Uart_Q_Sig_Func_Handler;
	//Sig_Action.sa_mask     = 0;
	Sig_Action.sa_flags    = 0;
	Sig_Action.sa_restorer = NULL; 
	sigaction( SIGIO, &Sig_Action, NULL );

	// SIGIO �ñ׳��� ������ �ֵ��� �Ѵ�
	fcntl( Handle_Ptr, F_SETOWN, getpid() );  
	fcntl( Handle_Ptr, F_SETFL, FASYNC );  // ���� ��ũ���͸� �񵿱����

	Sig_Func_Ptr = Func_Ptr;
}	





                                                                                                 
//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Q_Sig_Handler
//
//            WORK    : �ø��� ��Ʈ �ڵ鷯 �Լ� 
//
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------
void Uart_Q_Sig_Handler( int Status )
{
	int Receive_Size;
	int i;

	
	Receive_Size = read( Uart_Handle, Uart_Buf, 255 );

	if( Receive_Size > 0 )   // �����͸� �޾Ҵٸ�?
	{
		for( i=0; i<Receive_Size; i++)
		{
			Uart_Q_Push( UART_Q_0, &Uart_Buf[i] );
		}
	}
}





//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Q_Sig_Func_Handler
//
//            WORK    : �ø��� ��Ʈ �ڵ鷯 �Լ� ����� ���� �Լ� ����
//
//            DATE    : 2004. 2. 7
//----------------------------------------------------------------------------------------------
void Uart_Q_Sig_Func_Handler( int Status )
{
	int Receive_Size;
	int i;

	
	Receive_Size = read( Uart_Handle, Uart_Buf, 255 );

	if( Receive_Size > 0 )   // �����͸� �޾Ҵٸ�?
	{
		for( i=0; i<Receive_Size; i++)
		{
			Uart_Q_Push( UART_Q_0, &Uart_Buf[i] );
			
			(*Sig_Func_Ptr)( Uart_Buf[i] );
		}
	}
}


