/*--------------------------------------------------------------------
 *                        ------ Flash ------
 *
 * Description	: Access to read/write information from internal flash 
 *                on STM32L443RC.
 *
 * Company		: Advantech Co., Ltd.
 * Department	: Embedded Core Group
 * Author		: Will Chen
 * Create Date	: 2017/02/17
 *--------------------------------------------------------------------*/

//*****************************************************************************
//                 Included files
//*****************************************************************************
#include "common/terminal.h"
#include "mcu/stm32l443rc/flash.h"
#include <string.h>



//*****************************************************************************
//                 Global variables
//*****************************************************************************
static uint32_t FirstPage = 0, NbOfPages = 0, BankNumber = 0;
static uint32_t Address = 0, PAGEError = 0;
static __IO uint32_t data32 = 0;

// Variable used for Erase procedure
static FLASH_EraseInitTypeDef EraseInitStruct;



//*****************************************************************************
//                 Local functions prototype
//*****************************************************************************
static uint32_t GetPage(uint32_t Address);
static uint32_t GetBank(uint32_t Address);



//*****************************************************************************
//                 Local functions
//*****************************************************************************
int Flash_iTest(void)
{
#ifdef FLASH_TEST
    int iRet;
    unsigned int i, j;
    uint8_t u8Pattern;
    uint32_t u32Data, u32Pattern, u32Cnt;
    uint8_t au8TestBuf[8];

    for(i=0; i < 6; i++) 
    {
        switch(i)
        {   
            case 0:
                u32Pattern = 0x00000000;
                break;
            case 1:
                u32Pattern = 0xffffffff;
                break;
            case 2:
                u32Pattern = 0x55555555;
                break;
            case 3:
                u32Pattern = 0x5a5a5a5a;
                break;
            case 4:
                u32Pattern = 0xaaaaaaaa; 
                break;
            default:
                u32Pattern = 0xa5a5a5a5;             
                break;                
        }

        //
        // Erase page
        //
        iRet = Flash_iEraseInfo();
        if(iRet != eIF_OK)
            return iRet;
            
        //
        // Program flsah with pattern
        //
        DBG_print("Test pattern: 0x%x\r\n", u32Pattern);
        u8Pattern = (uint8_t)(u32Pattern >> 24);
        memset(au8TestBuf, u8Pattern, sizeof(au8TestBuf));
        u32Cnt = INFO_MAX_SIZE >> 3;
        for(j=0; j < u32Cnt; j++) {
            iRet = Flash_iProgramInfo(au8TestBuf, j << 3, 8);
            if(iRet != eIF_OK)
                return iRet;
        }
        
        //
        // Verification with programmed data
        //
        u32Cnt = INFO_MAX_SIZE >> 2;
        for(j=0; j < u32Cnt; j++) {
            iRet = Flash_iReadInfo((uint8_t*)&u32Data, j << 2, 4);
            if(iRet != eIF_OK)
                return iRet;
            if(u32Data != u32Pattern) {
                DBG_print("Test failed: data[0x%04x]:0x%08x, pattern:0x%08x \r\n", 
                            j << 2, u32Data, u32Pattern);
                return eIF_Err_TstCmpFail;
            }
        }
    }
#endif /* FLASH_TEST */

    return eIF_OK;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
    uint32_t page = 0;
    
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      /* Bank 1 */
      page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
      /* Bank 2 */
      page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }
    
    return page;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t Addr)
{
    return FLASH_BANK_1;
}

static int Flash_iCheckBoundary(unsigned int _uiOffset, unsigned int _uiSize)
{
    unsigned int uiTemp;
    
    uiTemp = (_uiOffset + _uiSize - 1) / INFO_MAX_SIZE;
    _uiOffset /= INFO_MAX_SIZE;
    
    if(_uiOffset != uiTemp)
        return eIF_Err_Boundary;
    else
        return eIF_OK;   
}

static int Flash_iCheckAlignment(unsigned int _uiOffset, int _iMode)
{
    int iRet;
    
    if(_iMode == WRITE_MODE)
        iRet = (int)(_uiOffset & 0x7);   
    else
        iRet = (int)(_uiOffset & 0x3);
    
    if(iRet)
        return eIF_Err_Alignment;
    else
        return eIF_OK;     
}

/*
static int Flash_iCheckEmpty(unsigned int _uiOffset, unsigned int _uiSize)
{
    return 0;   
}
*/

int Flash_iEraseInfo(void)
{
    int iRet;
    
	/* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
	
	/* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
    /* Get the 1st page to erase */
    FirstPage = GetPage(FLASH_USER_START_ADDR);
    /* Get the number of pages to erase from 1st page */
    NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
    /* Get the bank */
    BankNumber = GetBank(FLASH_USER_START_ADDR);
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks       = BankNumber;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
       you have to make sure that these data are rewritten before they are accessed during code
       execution. If this cannot be done safely, it is recommended to flush the caches by setting the
       DCRST and ICRST bits in the FLASH_CR register. */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        /*
          Error occurred while page erase.
          User can add here some code to deal with this error.
          PAGEError will contain the faulty page and then to know the code error on this page,
          user can call function 'HAL_FLASH_GetError()'
        */
        iRet =  eIF_Err_EraseFail;
    }
    else {
        iRet = eIF_OK;   
    }
    
    HAL_FLASH_Lock();
    
    return iRet;  
}

int Flash_iErasePage(uint32_t _u32StartPage, uint32_t _u32SEndPage)
{
    int iRet;
    
	/* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
	
	/* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
    /* Get the 1st page to erase */
    FirstPage = GetPage(_u32StartPage);
    /* Get the number of pages to erase from 1st page */
    NbOfPages = GetPage(_u32SEndPage + FLASH_PAGE_SIZE - 1) - FirstPage + 1;
    /* Get the bank */
    BankNumber = GetBank(_u32StartPage);
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks       = BankNumber;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
       you have to make sure that these data are rewritten before they are accessed during code
       execution. If this cannot be done safely, it is recommended to flush the caches by setting the
       DCRST and ICRST bits in the FLASH_CR register. */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        /*
          Error occurred while page erase.
          User can add here some code to deal with this error.
          PAGEError will contain the faulty page and then to know the code error on this page,
          user can call function 'HAL_FLASH_GetError()'
        */
        iRet =  eIF_Err_EraseFail;
    }
    else {
        iRet = eIF_OK;   
    }
    
    HAL_FLASH_Lock();
    
    return iRet;  
}

int Flash_iReadInfo(uint8_t* _pu8Des, unsigned int _uiOffset, unsigned int _uiSize)
{   
    int iRet;
    unsigned int i;
    uint32_t *p32Des;
    
    //
    // Check argument
    //
    if(_pu8Des == NULL) 
        return eIF_Err_Arg;
    else
        p32Des = (uint32_t*)_pu8Des;
    
    iRet = Flash_iCheckAlignment(_uiOffset, READ_MODE);
    if(iRet != eIF_OK) return iRet;
          
    iRet = Flash_iCheckBoundary(_uiOffset, _uiSize);
    if(iRet != eIF_OK) return iRet;
    
    Address = FLASH_USER_START_ADDR + _uiOffset;
    for(i=0, _uiSize = (_uiSize + 3) >> 2, iRet = eIF_OK; i < _uiSize; i++)
    {
        *(p32Des + i) = *(__IO uint32_t *)Address;
        Address = Address + 4;   
    }
    
    return eIF_OK;
}

int Flash_iProgramInfo(uint8_t* _pu8Src, unsigned int _uiOffset, unsigned int _uiSize)
{
    int iRet;
    unsigned int i;
    uint64_t *p64Src;
     
    //
    // Check argument
    //
    if(_pu8Src == NULL) 
        return eIF_Err_Arg;
    else
        p64Src = (uint64_t*)_pu8Src;
    
    iRet = Flash_iCheckAlignment(_uiOffset, WRITE_MODE);
    if(iRet != eIF_OK) return iRet;
          
    iRet = Flash_iCheckBoundary(_uiOffset, _uiSize);
    if(iRet != eIF_OK) return iRet; 
    
	/* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
	
	/* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    /* Program the user Flash area word by word */
    Address = FLASH_USER_START_ADDR + _uiOffset;
    
    for(i=0, _uiSize = (_uiSize + 7) >> 3, iRet = eIF_OK; i < _uiSize; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, *(p64Src + i)) == HAL_OK)
        {
          Address = Address + 8;
        }
        else
        {
          /* Error occurred while writing data in Flash memory.
             User can add here some code to deal with this error */
          iRet = eIF_Err_ProgramFail;
          break;
        }      
    }

    /* Lock the Flash to disable the flash control register access (recommended
        to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    
    return iRet;
}

