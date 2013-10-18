//----------------------------------------------------------------------------------------------
//            TITLE       : Uart ���� ���� �Լ� ���
//            WORK        :
//            DATE        : 2003. 7. 4
//            FILE        : Uart_Sig_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------
                                                                                                 
                                                                                                 
#ifndef  _UART_SIG_LIB_H
#define  _UART_SIG_LIB_H


#include "Uart_Q_Lib.c"



//----------------- �ñ׳� ���� 
//

#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>



struct sigaction Sig_Action;    // �ñ׳� �׼� ����

// �ø��� �����Ͱ� ������ �����ϰ��� �ϴ� �Լ� ������
//
void ( *Sig_Func_Ptr )( char Data );


void Uart_Sig_Init( int Handle_Ptr );
void Uart_Sig_Func_Init( int Handle_Ptr, void (*Func_Ptr)(char Data) );
void Uart_Sig_Func_Init( int Handle_Ptr, void (*Func)(char Data) );
void Uart_Q_Sig_Func_Handler( int Status );



void Uart_Q_Sig_Handler( int Status );






#endif

