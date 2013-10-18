//----------------------------------------------------------------------------------------------
//            TITLE       : Serial ��� ó�� �Լ� ���� 
//            WORK        :
//            DATE        : 2003. 3. 7
//            FILE        : Uart_Lib.c
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------






// MACRO Uart_Open(PortNum,BaudData) �� ���� �Ǿ��ִ�.

//----------------------------------------------------------------------------------------------
//            TITLE   : Ser_OpenPortCanonical_Mode
//
//            WORK    : �ø��� ��Ʈ �ʱ�ȭ
//            
//            DATE    : 2003. 3. 7
//----------------------------------------------------------------------------------------------
void Uart_OpenPortCanonical_Mode( int Uart_PortNum, int Uart_BaudData  )
{

	Uart_Handle = open( Uart_PortStr[ Uart_PortNum ], O_RDWR | O_NOCTTY );
	if( Uart_Handle < 0 )
	{
		perror( Uart_PortStr[ Uart_PortNum ] );
		exit( -1 );
	}


	// �����Ѵ� ���� �ø�����Ʈ ������
	tcgetattr( Uart_Handle, &Term_OldIo );
	// Write zeros to a byte string
	bzero( &Term_NewIo, sizeof( Term_NewIo ) );

	// CS8   : 8N1 (8bit, No Parity, 1 Stopbit
	// CREAD : ���� ����
	Term_NewIo.c_cflag = Uart_Baud[ Uart_BaudData ] | CS8 | CLOCAL | CREAD;	
	
	// IGNPAR : Parity ������ �ִ� ���� ����Ʈ�� �����Ѵ�
	// ICRNL  : CR ���ڸ� NL ���ڷ� ��ȯ ó���Ѵ�. ( �̼����� ���ϸ� �ٸ� ��ǻ�ʹ� 
	//          CR ���ڸ� �� ���� ���Ṯ�ڷ� �ν����� ���� �� �ִ�
	Term_NewIo.c_iflag = IGNPAR ;
	
	Term_NewIo.c_oflag = 0;

	// ICANON : Canonical �Է��� �����ϰ� �Ѵ�
	Term_NewIo.c_lflag = ICANON;

	Term_NewIo.c_cc[ VINTR    ] = 0;
	Term_NewIo.c_cc[ VQUIT    ] = 0;
	Term_NewIo.c_cc[ VERASE   ] = 0;
	Term_NewIo.c_cc[ VKILL    ] = 0;
	Term_NewIo.c_cc[ VEOF     ] = 4;
	Term_NewIo.c_cc[ VTIME    ] = 0;
	Term_NewIo.c_cc[ VMIN     ] = 1;
	Term_NewIo.c_cc[ VSWTC    ] = 0;
	Term_NewIo.c_cc[ VSTART   ] = 0;
	Term_NewIo.c_cc[ VSTOP    ] = 0;
	Term_NewIo.c_cc[ VSUSP    ] = 0;
	Term_NewIo.c_cc[ VEOL     ] = 0;
	Term_NewIo.c_cc[ VREPRINT ] = 0;
	Term_NewIo.c_cc[ VDISCARD ] = 0;
	Term_NewIo.c_cc[ VWERASE  ] = 0;
	Term_NewIo.c_cc[ VLNEXT   ] = 0;
	Term_NewIo.c_cc[ VEOL2    ] = 0;
	
	// tcflush  ( Uart_Handle, TCIFLUSH );
	
	// TCSANOW : change occurs immediately.
	tcsetattr( Uart_Handle, TCSANOW, &Term_NewIo );
	
	Uart_ModeFlag = CANON_MODE;
}





//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_OpenPortNonCanonical_Mode
//
//            WORK    : �ø��� ��Ʈ �ʱ�ȭ
//            
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------------
int Uart_OpenPortNonCanonical_Mode( int Uart_PortNum, int Uart_BaudData )
{


        Uart_Handle = open( Uart_PortStr[ Uart_PortNum ], O_RDWR | O_NOCTTY | O_NONBLOCK );
	if( Uart_Handle < 0 )
	{
                perror( Uart_PortStr[ Uart_PortNum ] );
                exit( -1 );
	}

	// �����Ѵ� ���� �ø�����Ʈ ������
	tcgetattr( Uart_Handle, &Term_OldIo );
	// Write zeros to a byte string
	bzero( &Term_NewIo, sizeof( Term_NewIo ) );
	// CS8   : 8N1 (8bit, No Parity, 1 Stopbit
	// CREAD : ���� ����
	Term_NewIo.c_cflag = Uart_Baud[ Uart_BaudData ] | CS8 | CLOCAL | CREAD;
	// IGNPAR : Parity ������ �ִ� ���� ����Ʈ�� �����Ѵ�
	// ICRNL  : CR ���ڸ� NL ���ڷ� ��ȯ ó���Ѵ�. ( �̼����� ���ϸ� �ٸ� ��ǻ�ʹ� 
	//          CR ���ڸ� �� ���� ���Ṯ�ڷ� �ν����� ���� �� �ִ�
	Term_NewIo.c_iflag = IGNPAR ;
	
	Term_NewIo.c_oflag = 0;

	Term_NewIo.c_cc[ VTIME ]   = 0;
	Term_NewIo.c_cc[ VMIN  ]   = 0;

	tcflush  ( Uart_Handle, TCIFLUSH );
	tcsetattr( Uart_Handle, TCSANOW, &Term_NewIo );
	
	// Flush
	//
	//ioctl( Uart_Handle, TCFLSH, 0 );

	Uart_ModeFlag = NONE_CANON_MODE;

	return 	Uart_Handle;
}





//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_ClosePort
//
//            WORK    : �ø��� ��Ʈ �ݴ´� 
//            
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------
void Uart_ClosePort( void )
{
	tcsetattr( Uart_Handle, TCSANOW, &Term_OldIo );	
}






//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Getch
//
//            WORK    : ���� 1���� �޴´� 
//            
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------------
int Uart_Getch( void )
{
	char Uart_GetData;
	
	switch( Uart_ModeFlag )
	{
		case CANON_MODE:
			
			/* ������ 1����Ʈ �����´� */			
			read( Uart_Handle, Uart_Buf, 1 );

			Uart_GetData = Uart_Buf[0];
					
			break;
			
		case NONE_CANON_MODE:
		
			Uart_GetData = read( Uart_Handle, Uart_Buf, 1 );

			if( Uart_GetData < 1) return 1000;

			Uart_GetData = Uart_Buf[0];
			break;
	}

	return Uart_GetData;
}




	
//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Putch
//
//            WORK    : ���� 1���� ������ 
//            
//            DATE    : 2003. 3. 7
//----------------------------------------------------------------------------------------------
void Uart_Putch( char Uart_PutData )
{
	
	
	switch( Uart_ModeFlag )
	{
		case CANON_MODE:
		case NONE_CANON_MODE:
			
			/* ������ 1����Ʈ ������ */			
			write( Uart_Handle, &Uart_PutData, 1 );
			break;
	}

}



//----------------------------------------------------------------------------
//
//          TITLE : Uart_Print
//
//          WORK  : �ø���� ���ڿ��� ������.
//
//----------------------------------------------------------------------------
void Uart_Print(char *UartPrintData)
{
	while( *UartPrintData != '\0') 
	{
		Uart_Putch(*UartPrintData);
               
		if( *UartPrintData == 0x0A )  // LF �� CR�� ���Ĵ�
			Uart_Putch(0x0D);	
               		
		UartPrintData++;
	}        
}







//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Gets
//
//            WORK    : ���� 1���� �޴´� 
//            
//            DATE    : 2003. 3. 7
//----------------------------------------------------------------------------------------------
char *Uart_Gets( void )
{
	
	switch( Uart_ModeFlag )
	{
		// ǥ�� ���
		case CANON_MODE:
			
			/* �����Ϳ��� �����´� */
			Buf_ReadSize = read( Uart_Handle, Uart_Buf, 255 );
			Uart_Buf[ Buf_ReadSize ] = 0;			
			
			return Uart_Buf;
			
			break;
		// ��ǥ�� ���
		case NONE_CANON_MODE:
			
			Buf_ReadSize = read( Uart_Handle, Uart_Buf, 255);
			
			//if( Buf_ReadSize < 1 ) return Ser_Buf;
			
			Term_NewIo.c_cc[ VMIN  ]   = Buf_ReadSize-1;
			tcsetattr( Uart_Handle, TCSANOW, &Term_NewIo );
			
			/* ������ 1����Ʈ �����´� */
			Buf_ReadSize = read( Uart_Handle, Uart_Buf, 255 );
			
			Uart_Buf[ Buf_ReadSize ] = 0;
						
			Term_NewIo.c_cc[ VMIN  ]   = 0;
			tcsetattr( Uart_Handle, TCSANOW, &Term_NewIo );
			
			return Uart_Buf;
			
			break;
	}

	return 0;	
}
		
