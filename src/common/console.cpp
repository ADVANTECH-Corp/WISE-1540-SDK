
//*****************************************************************************
//                 Included files
//*****************************************************************************
#include "common/console.h"

//*****************************************************************************
//                 Global variables
//*****************************************************************************
Serial g_tConsole(SERIAL_TX, SERIAL_RX);
Serial *g_pConsole = &g_tConsole;
static TCmdSet *g_apCmdSet[CONSOLE_MAX_CMD_SET_NUM];


//*****************************************************************************
//                 Global functions
//*****************************************************************************
int Console_iInit(void)
{
	g_tConsole.baud(CONSOLE_BAUDRATE);
	g_tConsole.format();
	CONSOLE_CLRSCR;

	memset((void*)g_apCmdSet , 0 ,CONSOLE_MAX_CMD_SET_NUM);

	return eConsole_Ok;
}

int Console_iUnInit(void)
{
	return eConsole_Ok;
}

int Console_iRegCmdSet(TCmdSet *_ptCmdSet)
{
	int i;

	for(i=0; i < CONSOLE_MAX_CMD_SET_NUM; i++) {
		if(g_apCmdSet[i] == NULL) {
			g_apCmdSet[i] = _ptCmdSet;
			return eConsole_Ok;
		}
	}

	return eConsole_Err_BufIsFull;
}

int Console_iRunCmd(char *_strCmd)
{
	int i, iRet;
	
	//
	// Looking for commands and running
	//
	for(i=0; i < CONSOLE_MAX_CMD_SET_NUM; i++) {
		if(g_apCmdSet[i]->pfRunCmd != NULL) {
			iRet = g_apCmdSet[i]->pfRunCmd(_strCmd);
			if(iRet == eConsole_Ok || iRet == eConsole_Err_Failed)
				return iRet;
		}
	}

	return eConsole_Err_UnKnowCmd;
}

char Console_cGetChar(void)
{
	return g_tConsole.getc();
}

int Console_iPutChar(char _cChar)
{
	g_tConsole.putc(_cChar);

	return 0;
}

int Console_iGetString(char *_pcBuf, unsigned int _uiBufLen)
{
	char cChar;
    int iLen = 0;
    
    //
    // Receive a character and echo it to console
    //
    cChar = Console_cGetChar();
    if(cChar != '\b') {
    	Console_iPutChar(cChar);
    }
    iLen = 0;
	
    //
    // Check the end of Command
    //
    while((cChar != '\r') && (cChar !='\n') )
    {
        //
        // Check buffer if overflow
        //
        if(iLen >= (int)_uiBufLen)
        {
            return -1;
        }
        
        //
        // Copying Data from UART into a buffer
        //
        if(cChar != '\b')
        { 
            *(_pcBuf + iLen) = cChar;
            iLen++;
        }
        else
        {
            //
            // Deleting last character when you hit backspace 
            //
            if(iLen)
            {
                iLen--;
            }
        }

		//
    	// Receive a character and echo it to console
    	//
    	while(1) {
    		cChar = Console_cGetChar();
			if(iLen < (int)(_uiBufLen - 1) || cChar == '\b')
				break;
    	}

		if(cChar != '\b') {
        	Console_iPutChar(cChar);
        }
        else {
			if(iLen) {
        		Console_iPutChar(cChar);
        		Console_iPutChar(' ');
        		Console_iPutChar(cChar);
			}
        }
    }

    *(_pcBuf + iLen) = '\0';

    Console_iPutChar('\n');
	Console_iPutChar('\r');

    return iLen;
}

