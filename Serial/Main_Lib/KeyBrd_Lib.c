//-----------------------------------------------------------------------------------------------
//            TITLE       : Key 처리 함수 모음 
//            WORK        :
//            DATE        : 2003. 2. 27
//            FILE        : Key_Lib.c
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------




 
struct termios orig_termios;




void KeyBrd_Close( void );




//----------------------------------------------------------------------------------------------
//            TITLE   : init_keyboard
//
//            WORK    : 키처리 초기화
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
void KeyBrd_Init(void)
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(KeyBrd_Close);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
    
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Close
//
//            WORK    : 터미널 복구
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
void KeyBrd_Close( void )
{
    tcsetattr(0, TCSANOW, &orig_termios);
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Hit
//
//            WORK    : 키가 눌렸는지 검사
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
int KeyBrd_Hit(void)
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KeyBrd_Getch
//
//            WORK    : 키값을 돌려준다
//            
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
char KeyBrd_Getch(void)
{
    int r;
    unsigned char c;
    
    if ((r = read(0, &c, sizeof(c))) < 0) 
    {
        return r;
    } else {
        return c;
    }		
}


                                                                                        
                                                                                                 
                                                                                                 

