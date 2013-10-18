//-----------------------------------------------------------------------------------------------
//            TITLE       : Key ó�� �Լ� ���� 
//            WORK        :
//            DATE        : 2003. 2. 27
//            FILE        : Key_Lib.c
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------







//----------------------------------------------------------------------------------------------
//            TITLE   : init_keyboard
//
//            WORK    : Űó�� �ʱ�ȭ
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
void KeyBrd_Init(void)
{
	//
	//------------ �͹̳��� �Ӽ��� �о�´�
	//
        tcgetattr(0,&initial_settings);
	
	//
	//------------ �͹̳��� Canonical Mode, Non Canonical Mode �ΰ��� �ִ�
	//             ���⼭�� Non Canonical Mode �� ���
        new_settings             =  initial_settings;

	//
	//------------ c_lflag <- Local Mode �̴�
	//
        new_settings.c_lflag    &= ~ICANON;        // NON Canonical Mode �� �ٲ۴�
        new_settings.c_lflag    &= ~ECHO;          // ECHO �� ����
        new_settings.c_lflag    &= ~ISIG;          // Ư��ĳ���Ϳ� ���� �˻��ϴ����� ��������

	//
	//------------ read ����� �о�� ���� ����... ����
	//
        new_settings.c_cc[VMIN]  =  1;             // �͹̳ο��� ���� ���� ������ 1byte
        new_settings.c_cc[VTIME] =  0;             // Time out = 0 ��°��� Time Out Ÿ�̸Ӱ� ���� ����
	
        tcsetattr(0, TCSANOW, &new_settings);      // ���ο� ������ ����
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Close
//
//            WORK    : �͹̳� ����
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
void KeyBrd_Close( void )
{
	//
	//------------ �ʹ̳��� ���� ���·� �����Ѵ�
	//
        tcsetattr(0,TCSANOW, &initial_settings);
}






//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Hit
//
//            WORK    : Ű�� ���ȴ��� �˻�
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
int KeyBrd_Hit(void)
{
        char ch;
        int nread;

        if(peek_character != -1)
                return 1;

	new_settings.c_cc[VMIN] = 0;           // VMIN = 0 �̸� ��ٸ��� �ʰ� �ٷ� 1����Ʈ �����´�
        tcsetattr(0, TCSANOW, &new_settings);
        nread = read(0,&ch,1);                 // �̰��� �ٽ�.. ��ٸ��� �ʰ� �ٷ� ������
	                                       // �����Ͱ� �ֳ� ���Ŀ� ���� return �� �ٸ�		
        new_settings.c_cc[VMIN] = 1;           // �ٽ� �������...
        tcsetattr(0,TCSANOW, &new_settings);

	if(nread == 1)
        {
		peek_character = ch;
		return 1;
	}
	
	return 0;
}




//----------------------------------------------------------------------------------------------
//            TITLE   : readch
//
//            WORK    : getch �� �Ȱ��� ������ �Ѵ�
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
int KeyBrd_Getch_Buf( void )
{
        char ch;

	if(peek_character != -1)
        {
		ch = peek_character;
		peek_character = -1;
		return ch;
	}
	
	read(0,&ch,1);

#ifdef  DOS_MODE
	// ����Ű�� 0x0d �� ��ȯ�Ѵ�
	if( ch == 10 ) 	ch = 13;
#endif
	return ch;
}




//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Getch
//
//            WORK    : Ű���� �����ش�
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
char KeyBrd_Getch(void)
{
	char ch;

	while( !KeyBrd_Hit() );

	ch = KeyBrd_Getch_Buf();

#ifdef  DOS_MODE
	// ����Ű�� 0x0d �� ��ȯ�Ѵ�
	if( ch == 10 ) 	ch = 13;
#endif
	return ch;
				
}





                                                                                                 
//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Putch
//
//            WORK    : Ű���� �����ش�
//
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
void KeyBrd_Putch( char KeyData )
{
	write(0, &KeyData, 1 );                                                                                    
}
                                                                                                 
                                                                                                 

