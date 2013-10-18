//----------------------------------------------------------------------------
//    ���α׷��� : Uart ���� ���� 
//
//    ������     : Hanchol Cho
//
//    ��  ¥     : 2003.7.4
//
//    MPU        : 
//    ���ϸ�     : Uart_Q_Lib.c
//----------------------------------------------------------------------------


#define UART_Q_0    0



#define UART_BUFFER_OBJ_MAX 2

#define UART_BUFFER_MAX     50                //  ���� �ִ� ����
#define UART_DATA_PTR_TYPE  S8                //  ������ ���� ���� 



#define UART_Q_EMPTY        1
#define UART_Q_FULL         0
#define UART_POP_SUCCESS    2

#define UART_Q_SIZE(x)       Uart_Q_Size[x]       // ������ ����� �����ش�.
#define UART_Q_VAILD(x)      Uart_Q_Size[x]       // ĵ�� �����Ͱ� �ִ��� �˻�      

             
UART_DATA_PTR_TYPE    Uart_Q_Buffer[ UART_BUFFER_OBJ_MAX ][ UART_BUFFER_MAX+1 ];  // ������ ������ ����


S16  Uart_Q_Size[ UART_BUFFER_OBJ_MAX ];                       // ������ ������ ����  
U16  Uart_Q_Start[ UART_BUFFER_OBJ_MAX ];                      // ������ ���� ������
U16  Uart_Q_End[ UART_BUFFER_OBJ_MAX ];                        // ������ �� ������





//----------------------------------------------------------------------------
//
//          TITLE : Uart_Q_Init
//
//          WORK  : ĵ�� Q ���۸� �ʱ�ȭ �Ѵ�
//
//----------------------------------------------------------------------------
void Uart_Q_Init(void)
{
	U8 i;
	
	for( i=0; i < UART_BUFFER_OBJ_MAX; i++)
	{
        	Uart_Q_Size[i]  = 0;
        	Uart_Q_Start[i] = 0;    // ť�� ������ �ʱ�ȭ
        	Uart_Q_End[i]   = 0;    // ť�� ���� �ʱ�ȭ
      	}
}        





//----------------------------------------------------------------------------
//
//          TITLE : Uart_Q_Add
//
//          WORK  : ĵ�� Q ���ۿ� ������ ����
//
//                  0 : ���� 
//----------------------------------------------------------------------------
S8 Uart_Q_Push(U8 Buf_Index, UART_DATA_PTR_TYPE  *PushData )
{
        
        // �������� �� �����ʹ� �׻� ����ִ� ������ ����Ű�� �ִ�.
        //  |      |
        //  |      | <- End ����Ʈ
        //  | Start |
        // �����͸� End �����Ϳ� �������� �����͸� ������Ų��.


	// ������ ���� ����� �ʱ�ȭ
	//
        if( Uart_Q_End[Buf_Index] == Uart_Q_Start[Buf_Index]) Uart_Q_Size[Buf_Index] = 0;
                
                
	// ť ���ۿ� ������ ����
	//                
        Uart_Q_Buffer[Buf_Index][ Uart_Q_End[Buf_Index] ] = *PushData;
        
        
        
        if( ++Uart_Q_End[Buf_Index] >= UART_BUFFER_MAX )    // Ű ������ ���ڸ��� ����
        {                                      // �����͸� ó������ ������
                if( Uart_Q_Start[Buf_Index] )        // ���������Ͱ� ó���� ���� ������
                {
                        Uart_Q_End[Buf_Index] = 0;   // �������͸� ó������ �ű��.
                }                        
                else                     // ���������Ͱ� ó���� ������
                {                        // ���������͸� �������� �Űܾ� �Ѵ�.
                        Uart_Q_Start[Buf_Index]++;  
                        Uart_Q_End[Buf_Index]  = 0;
                }                                                
        }       
        else if( Uart_Q_End[Buf_Index] == Uart_Q_Start[Buf_Index] )      // �������Ͷ� ���������Ͷ� ������
        {                                        // ���������͸� �������� Ȩ�ܾ��Ѵ�.
                if( ++Uart_Q_Start[Buf_Index] >= UART_BUFFER_MAX )
                {
                        Uart_Q_Start[Buf_Index] = 0;          // �ű�����ϴ� ���������Ͱ�
                                                                // ���������� ó������ �ű��.
                }                        
        }
        
        // �� ������ ���� ��ġ�� �ʵ��� ����
        //
        if( ++Uart_Q_Size[Buf_Index] > UART_BUFFER_MAX)
        { 
		Uart_Q_Size[Buf_Index] = UART_BUFFER_MAX;
	}

	return 0; 
}                                                  





//----------------------------------------------------------------------------
//
//          TITLE : Uart_Q_Pop
//
//          WORK  : ĵ�� Q ���ۿ� ������ �����´�.
//
//                  
//----------------------------------------------------------------------------

S8 Uart_Q_Pop(U8 Buf_Index, UART_DATA_PTR_TYPE *Motion_Data_Ptr )
{
        U16 Pop_Index;

	Pop_Index = Uart_Q_Start[Buf_Index];    // ������ ������ �ε��� ����
	
        if( Uart_Q_End[Buf_Index] == Uart_Q_Start[Buf_Index])
        {
        	Uart_Q_Size[Buf_Index] = 0;
        	return UART_Q_EMPTY; // ���۰� ���������
	}	
	
	// ť�� ���� �ɷȴٸ� �ٽ� ÷���� ������.
	//                                
        if( ++Uart_Q_Start[Buf_Index] >= UART_BUFFER_MAX)
                Uart_Q_Start[Buf_Index] = 0;

	
	// ������� �Ѱ��� �����͸� �������Ƿ� ���δ�
	//
        Uart_Q_Size[Buf_Index]--;
        
        // ��� �÷ο� ����
        //
        if( Uart_Q_Size[Buf_Index] < 0 ) Uart_Q_Size[Buf_Index] = 0;
        
        // ť ������ ������ ���������ϴ� ���ϴ� �����μ��� ī���Ѵ�
        //
        *Motion_Data_Ptr =  Uart_Q_Buffer[Buf_Index][ Pop_Index ];
        
        
        
        return UART_POP_SUCCESS;     // �ε� ����
}



