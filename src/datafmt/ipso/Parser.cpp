
#include <datafmt/ipso/Parser.h>
#include <wsn/sm_ipmt/board.h>
#include <common/terminal.h>

static CoapPacket_t *m_RxCoapPacket;
static CoapPacket_t *m_TxCoapPacket;
static CoapHeader_t m_CoapHeader;
static CoapAction_t m_CoapAction;
static CoapHeader_t m_PrevCoapHeader;
static CoapAction_t m_PrevCoapAction;
static eURICmd  m_PrevRetURICmd = eURICmdUnKnown;
static INT16 m_ObserveFlag = 0;
static INT8 buff[BUFF_SIZE_32];

// -----------------------------------------------------------
// Function Name: Parser_Init
// Description: Paser initialize interanl state and variables
//
// Pararmeters: None
//
// Return:  If successful return eParRetOk (0), else eParRetFail (-1)
//
eParRet Parser_Init(void) // Call Sensor_CheckIsReady to check Sensor is register!
{
    eParRet ParRetCode = eParRetOk;
    
    return ParRetCode;
}

// -----------------------------------------------------------
// Function Name: Parser_Decode
// Description: Decode the IPSO sensor format and set state machine
//
// Pararmeters:
//          1. pPacketRx: Pointer of Rx Coap Packet
//          2. PacketSize: Size of Packet, Must great to zero
//
// Return: URICmd, Ref eURICmd
//
eURICmd Parser_Decode(INT8* pPacketRx, INT32 PacketSize)
{
    INT8  *head, *tail, *cp;
    INT16 Size, PayLoadSize;
    eURICmd  eRetURICmd = eURICmdUnKnown;
	
    // Check receive size of packet
    if(PacketSize>COAP_PACKET_SIZE_MAX)
        return eRetURICmd;	
	
	if(pPacketRx!=NULL)
		m_RxCoapPacket = (CoapPacket_t *) pPacketRx;

    // Check coap version
    if(m_RxCoapPacket->Header.Ver!=1)
    {
        return eRetURICmd;
    }

    memset( &m_CoapHeader, 0, sizeof(CoapHeader_t));
    memset( &m_CoapAction, 0, sizeof(CoapAction_t));
    m_CoapHeader.Code = eCoapCodeMethodNotAllowed;
    m_CoapHeader.Ver = 1;
    m_CoapHeader.Tkl = 0; // In observe use count 0~15, Current not use it
	
	//	Type (T): 2-bit unsigned integer. Indicates if this message is of type
	//	Confirmable (0), Non-confirmable (1), Acknowledgement (2),  Reset (3).
    if(m_RxCoapPacket->Header.Type==eCoapTypeConfirmable)
    {
        m_CoapHeader.Type = eCoapTypeAck;
    }
    else
    {
        m_CoapHeader.Type = eCoapTypeReset;
        return eRetURICmd;
    }

    // Set Message ID
    m_CoapHeader.Msgid = m_RxCoapPacket->Header.Msgid;

    // Payload data pointer and legth
    head = tail = m_RxCoapPacket->Payload;
    PayLoadSize = m_RxCoapPacket->Header.Length;
	
    cp = tail = head;

    switch(m_RxCoapPacket->Header.Code)
    {
        //  Request
        case    eCoapCodeGet:       //  1
        m_CoapHeader.Code = eCoapCodeContent;
        break;
        case    eCoapCodePut:       //  3
        m_CoapHeader.Code = eCoapCodeChage;
        break;
        default:
        m_CoapHeader.Code = eCoapCodeNotImplemented;
        return eRetURICmd;
    }

    //  /sen, /dev, /net
    if((cp = Paser_FindKey(head, (INT8*)"/sen"))!=NULL)
    {
        m_CoapAction.reg.sen = 1;
    }
    else if((cp = Paser_FindKey(head, (INT8*)"/dev"))!=NULL)
    {
        m_CoapAction.reg.dev = 1;
    }
    else  if((cp = Paser_FindKey(head, (INT8*)"/net"))!=NULL)
    {
        m_CoapAction.reg.net = 1;
    }

    head = cp;

    //  /info, /status
    if((cp = Paser_FindKey(head, (INT8*)"/info"))!=NULL)
    {
        m_CoapAction.reg.info = 1;
    }
    else  if((cp = Paser_FindKey(head, (INT8*)"/status"))!=NULL)
    {
        m_CoapAction.reg.status = 1;
    }

    head = cp;

    // action=list,observe,cancel
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"action", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        if(strcmp((char*)buff, "list")==0)
        {
            m_CoapAction.reg.list = 1;
        }
        else if(strcmp((char*)buff, "observe")==0)
        {
            m_CoapAction.reg.observe = 1;
        }
        else if(strcmp((char*)buff, "cancel")==0)
        {
            m_CoapAction.reg.cancel = 1;
        }
		else if(strcmp((char*)buff, "query")==0)
        {
            m_CoapAction.reg.query = 1;
        }
    }

    // field=id, field=health
    // -------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"field", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        if(strcmp((char*)buff, "id")==0)
            m_CoapAction.reg.field = 1;
        else if(strcmp((char*)buff, "health")==0)
            m_CoapAction.reg.health = 1;        
    }

    // id=all or id=x
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"id", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
		 m_CoapAction.reg.id = 1;
        if(strcmp((char*)buff, "all")==0)
        {
            m_CoapAction.reg.all = 1;
        }
        else // id=0,1,2,3,4...7
        {
			if(Size<3)
			{
				m_CoapAction.id = (INT16) strtol((char*)buff,NULL,10);
				Paser_SetSenId(m_CoapAction.id);
			}	
        	/*if(Paser_StrListId(buff)==1) // Not list, only on Id
        	{
        		m_CoapAction.id = strtol(buff,NULL,10);
        	}*/
        }
    }

    // bv (Boolean Value): 1:TRUE, 0:FALSE
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"bv", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.bv = 1;
        if(strtol((char*)buff,NULL,10)==1)
            m_CoapAction.Data.bVal = 1;
        else
            m_CoapAction.Data.bVal = 0;
    }

	// iv (integer): Integer Value
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"iv", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.iv = 1;
        m_CoapAction.Data.iVal = strtol((char*)buff,NULL,10);
    }
	
	// dv (Decimal): 0~100%
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"dv", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.dv = 1;
        m_CoapAction.Data.iVal = strtol((char*)buff,NULL,10);
    }
	
	// fv (float): 0.1234
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"fv", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.fv = 1;
        m_CoapAction.Data.fVal = (float) atof((char*)buff);
    }
	
	// sid=xxxxx
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"sid", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.sid = 1;
        m_CoapAction.sid = strtol((char*)buff,NULL,10);
		if(m_CoapAction.sid<0)
            m_CoapAction.sid = 0;
    }
	
	// rid=xxxxx
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"rid", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.rid = 1;
        m_CoapAction.rid = strtol((char*)buff,NULL,10);
		if(m_CoapAction.rid<0)
            m_CoapAction.rid = 0;
    }
	
    // n=" "
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"n", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        memset(m_CoapAction.name, 0,COAP_ACTION_NAME_MAX);
		if(Size<COAP_ACTION_NAME_MAX)
		{
			m_CoapAction.reg.name = 1;
            if(buff[0]=='"' && buff[Size-1]=='"')
				strncpy((char*)m_CoapAction.name, (char*)&buff[1], Size-2);
			else
				strncpy((char*)m_CoapAction.name, (char*)buff, Size);
		}
		else	
			sprintf((char*)m_CoapAction.name,"name over buffer");
    }

    // software Version sw="1.0.0.1",
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"sw", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.sw = 1;
    }

    // Hardware Version hw="1.0.0.1",
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"hw", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.hw = 1;
    }

    // //dev/info?reset=1
    // ------------------------------------------------------------
    Size = BUFF_SIZE_32;
    cp = Paser_KeyVal(head, (INT8*)"reset", PayLoadSize, buff, &Size);
    if(cp!=NULL && Size>0)
    {
        m_CoapAction.reg.reset = 1;
        if(strtol((char*)buff,NULL,10)==1)
            m_CoapAction.Data.bVal = 1;
        else
            m_CoapAction.Data.bVal = 0;
    }

		// GET
	if(m_RxCoapPacket->Header.Code==eCoapCodeGet)
	{
		// /sen
		if(m_CoapAction.reg.sen==1)
		{
			// /sen/info
			if(m_CoapAction.reg.info==1)
			{
				// /sen/info?action=list
				if(m_CoapAction.reg.list==1) 			//	id=x, id=all and field = id 
				{
					if(m_CoapAction.reg.all==1)  		//	/sen/info?action=list&id=all ( Reserved! Cause to Payload too small )
					{
						eRetURICmd = eURICmdSenInfoListAll;
					}
					else if(m_CoapAction.reg.field==1)	//	/sen/info?action=list&field=id
					{
						eRetURICmd = eURICmdSenInfoListFieldId;
					}
                    else 								//	/sen/info?action=list&id=0,1,2
					{
						eRetURICmd = eURICmdSenInfoList;						
					}		
				}
				else if(m_CoapAction.reg.query==1)
				{
                    if(m_CoapAction.reg.sid==1)         //	/sen/info?action=query&sid=3303
					{
						eRetURICmd = eURICmdQuerySenBySid;
					}
                    else if(m_CoapAction.reg.id==1)		//	/sen/info?action=query&id=0
					{
						eRetURICmd = eURICmdQuerySenById;						
					}
				}	
			}	// /sen/status	    
            else if(m_CoapAction.reg.status==1)
			{
				// /sen/status?action=observe
				if(m_CoapAction.reg.observe==1) 		
				{
                    if(m_CoapAction.reg.all==1)			//	/sen/status?action=observe&id=all
					{
                        eRetURICmd = eURICmdObserve;
					}
					else if(m_CoapAction.reg.name==1)	//	/sen/status?action=observe&n=Room_Tempature
					{
                       // eRetURICmd = eURICmdObserveByName;
					}
					else if(m_CoapAction.reg.id==1)		//	/sen/status?action=observe&id=0,1
					{
                       // eRetURICmd = eURICmdObserveById;
					}					
				} // /sen/status?action=cancel
				else if(m_CoapAction.reg.cancel==1)
				{
					if(m_CoapAction.reg.all==1)			//	/sen/status?action=cancel&id=all
					{
                        eRetURICmd = eURICmdCancel;
					}
					else if(m_CoapAction.reg.name==1)	//	/sen/status?action=cancel&n=Room_Tempature
					{
                        // eRetURICmd = eURICmdCancelByName;
					}
					else if(m_CoapAction.reg.id==1)		//	/sen/status?action=cancel&id=0,1
					{
                        // eRetURICmd = eURICmdCancelById;
					}					
				}				
			}
		} 	// /dev
		else if(m_CoapAction.reg.dev==1)
		{
			// /dev/info
			if(m_CoapAction.reg.info==1)
			{
				if(m_CoapAction.reg.list==1) 			//	/dev/info?action=list
				{
					eRetURICmd = eURICmdDevInfoList;					
				}
			}	// /dev/status
            else if(m_CoapAction.reg.status==1)			//	/dev/status
			{
				eRetURICmd = eURICmdDevStatus;
			}
		}	//	/net
		else if(m_CoapAction.reg.net==1)
		{
			// /net/info
			if(m_CoapAction.reg.info==1)				
			{
				if(m_CoapAction.reg.list==1)			//	/net/info?action=list
				{
					eRetURICmd = eURICmdNetInfoList;
					
				}
			}	// /net/status
            else if(m_CoapAction.reg.status==1)			//	/net/status
			{				
				eRetURICmd = eURICmdNetStatus;
			}		
		}
		
	}	// PUT
    else if(m_RxCoapPacket->Header.Code==eCoapCodePut)
	{
			// /sen
		if(m_CoapAction.reg.sen==1)
		{
				// /sen/info
			if(m_CoapAction.reg.info==1)		//	/sen/info
			{
                if(m_CoapAction.reg.name==1)	//  /sen/info
				{
						
				}				
				else							//	/sen/info
				{
					
				}				
			}	// /sen/status
			else if(m_CoapAction.reg.status==1)	// /sen/status
			{
				if(m_CoapAction.reg.name==1)	//  /sen/status?n=dout&bv=1 
				{
					eRetURICmd = eURICmdSetSenValueByName;
				}				
				else if(m_CoapAction.reg.id==1)	//	/sen/status?id=5&bv=1 
				{
					eRetURICmd = eURICmdSetSenValueById;
				}
			}
			
		}	// /dev
		else if(m_CoapAction.reg.dev==1)
		{
			// /dev/info
			if(m_CoapAction.reg.info==1)			//	/dev/info
			{
                if(m_CoapAction.reg.name==1)			//  /dev/info?n="Demo 1"
				{
					eRetURICmd = eURICmdSetDevName;
				
				}
				else if(m_CoapAction.reg.reset==1)	//	/dev/info?reset=1
				{
					eRetURICmd = eURICmdReset;					
				}
				
			}	// /dev/status
			else if(m_CoapAction.reg.status==1)	// /dev/status
			{
								
			}
		}
	}
	
    return eRetURICmd;
}

// -----------------------------------------------------------
// Function Name: Parser_Encode
// Description: Encode the IPSO sensor format and resend to sensor gateway
// Pararmeters:
//
//          1. URICmd:  URI Command and ref eURICmd
//          2. pPacketTx: Pointer of Tx Packet and Pointer can't be NULL
//          3. PacketSize: Size of Packet, Must great to zero
//
// Return:  Encode Packet Size, -1 means encode fail!
//
INT16 Parser_Encode(eURICmd URICmd,INT8* pPacketTx, INT16 PacketSize)
{   
    INT16 RetPacketSize = -1;
    CoapHeader_t *TxCoapHeader;
    INT8 *cp, *head;
    INT16 i, j, count;
    SenStrcut_tp 	pStructSensor;
	BoardConfig_tp  pBoardCfg;

    if(pPacketTx==NULL || PacketSize<=0)
        return -1;

	// First time accept Observe, then keep old Header and Action	
    if(URICmd==eURICmdObserve && m_PrevRetURICmd!=eURICmdObserve) 
    {
		m_ObserveFlag = 1;
        m_PrevCoapHeader = m_CoapHeader;
        m_PrevCoapAction = m_CoapAction;
	}
	// Only if Cancel to stop send packet, if accept Observe
	if(URICmd==eURICmdCancel && m_ObserveFlag==1)
	{
		m_ObserveFlag = 0;
	}
	else if(URICmd!=eURICmdCancel && m_ObserveFlag==1)
	{
        URICmd = eURICmdObserve;
		m_CoapHeader = m_PrevCoapHeader;
        m_CoapAction = m_PrevCoapAction;
    }
    m_PrevRetURICmd = URICmd;
	
	pBoardCfg = Board_GetCfgPtr();


    // Prepare tx packet, Clean buffer
    memset((void*)pPacketTx, 0, PacketSize);

    // Packet Buffer cast to CoapPacket
    m_TxCoapPacket = (CoapPacket_t *) pPacketTx;

    // Point to CoapPakcet's Header
    TxCoapHeader = (CoapHeader_t *) &m_TxCoapPacket->Header;
    *TxCoapHeader = m_CoapHeader;
    TxCoapHeader->Length = 0;

    // Assembler Payload with pointer cp
    head = cp = m_TxCoapPacket->Payload;
    count = Sensor_GetCount();

    switch(URICmd)
    {
        case    eURICmdOk:
        break;
        case    eURICmdSenInfoListAll:     //  GET /sen/info?action=list
        TxCoapHeader->Code = eCoapCodeContent;
        for(i=0;i<count;i++)
        {
            cp += sprintf((char*)cp,"<%d>;n=\"%s\", mdl=\"%s\", sid=\"%d\"",
             i, Sensor_GetHostName(i), Sensor_GetModelName(i), Sensor_GetSmid(i) );
        }
        break;
		case    eURICmdSenInfoListFieldId: //	GET /sen/info?action=list&field=id
        TxCoapHeader->Code = eCoapCodeContent;
        //	</sen>;0,1,2,3,4,5  =>     17 Bytes
        cp += sprintf((char*)cp,"</sen>;");
        for(i=0;i<count;i++)
        {
            if(i!=0)
                *(cp++)= ',';
            *(cp++)= 0x30+i;
        }
        break;    
        case    eURICmdSenInfoList:        //	GET /sen/info?action=list&id=0,1,2
        TxCoapHeader->Code = eCoapCodeContent;       
        //</0>;n="Room_Tempature", mdl="mp6050", sid="3303"
        for(i=0;i<COAP_ACTION_ID_MAX;i++)
		{
			if(Paser_CheckSenId(i)==TRUE && i<count)
            {
				pStructSensor = Sensor_GetStructPtrById(i);
				cp += sprintf((char*)cp,"</%d>;n=\"%s\", mdl=\"%s\", sid=\"%d\"", 	i,
					pStructSensor->hostname, pStructSensor->modelname, pStructSensor->sid );
			}
		}	
        break;
		case    eURICmdQuerySenBySid:   //  GET /sen/info?action=query&sid=3303
        TxCoapHeader->Code = eCoapCodeContent;
		//</3303>;asm="r", rt="ucum.Cel", type="d", st="ipso", u="Cel"
		if(m_CoapAction.reg.sid==1)
		{
			i = m_CoapAction.sid;
			j = 0;
            if(m_CoapAction.reg.id==1)
            {
                j = m_CoapAction.id;
                pStructSensor = Sensor_GetStructPtrById(j);
                i = pStructSensor->sid;
            }
            cp += sprintf((char*)cp,"</%d>;asm=\"%s\", rt=\"%s\", type=\"%s\", st=\"%s\", u=\"%s\"",
            ((m_CoapAction.reg.id==1)?j:i), Sensor_GetAccessMode(i), Sensor_GetResourceType(i),
            Sensor_GetDataTypeBySid(i), Sensor_GetStandard(i), Sensor_GetUnit(i));
        }
		break;
        case    eURICmdQuerySenById:	//  GET /sen/info?action=query&id=0
		TxCoapHeader->Code = eCoapCodeContent;	
		//</0>;min="-20", max="100", X="x", Y="y", Z="z"
		for(i=0;i<COAP_ACTION_ID_MAX;i++)
		{
			if(Paser_CheckSenId(i)==TRUE && i<count)
			{	
				pStructSensor = Sensor_GetStructPtrById(i);
				//if(pStructSensor->DataType==eDataTypeFloat)
					cp += sprintf((char*)cp,"</%d>;min=\"%.0f\", max=\"%.0f\"", i, pStructSensor->min, pStructSensor->max );
				//else	
				//	cp += sprintf(cp,"</%d>;min=\"%d\", max=\"%d\"", i, (INT32) pStructSensor->min, (INT32) pStructSensor->max );	
				if(pStructSensor->sid==3313 || pStructSensor->sid==3314) //	Accelerometer 3313, Magnetic 3314
				{
					cp += sprintf((char*)cp,", X=\"%s\", Y=\"%s\", Z=\"%s\"", "x", "y", "z" );
				}
			}
		}	
		break;
        //case	eURICmdObserveAll:    //  GET /sen/status?action=observe&id=all
		//case	eURICmdObserveById:   //  GET /sen/status?action=observe&id=0,1
		//case	eURICmdObserveByName: //  /sen/status?action=observe&n=Room_Tempature
		case	eURICmdObserve:
        TxCoapHeader->Code = eCoapCodeContent;
        //	0 36.2,1 26.5,2 9000,3 100,4 0, 5 10 20 30  	all
		//  0 36.2, 1 9000		id
		//	Room_Tempature 36.2	name		
		for(i=0;i<count;i++)
		{
			if(i!=0 && m_CoapAction.reg.all==1) // id=all
				*(cp++)= ',';
	
			if(Paser_CheckSenId(i)==FALSE && m_CoapAction.reg.all==0) // id=x
				continue;
			pStructSensor = Sensor_GetStructPtrById(i);
			if(m_CoapAction.reg.name==1) 		// Observe by name
				cp += sprintf((char*)cp,"%s ", pStructSensor->hostname );
			else	
				cp += sprintf((char*)cp,"%d ", i);	// Observe by id=all

            if(pStructSensor->sid==3313 || pStructSensor->sid==3314)
			{
				cp += sprintf((char*)cp,"%.2f %.2f %.2f", (float) pStructSensor->x, (float) pStructSensor->y, (float) pStructSensor->z );
			}
            else if(pStructSensor->DataType==eDataTypeFloat)
			{
				cp += sprintf((char*)cp,"%.2f", pStructSensor->Data.fVal );
			}
                else if(pStructSensor->DataType==eDataTypeBoolean)
			{
				cp += sprintf((char*)cp,"%d", (UINT32) pStructSensor->Data.bVal );
			}
			else
			{
				cp += sprintf((char*)cp,"%d", pStructSensor->Data.iVal );
			}
		}
		break;
		case    eURICmdCancel:      	//  GET Observe, action=cancel
        TxCoapHeader->Code = eCoapCodeContent;
        // cancel /sen/status?id=all
        //pStructSensor = Sensor_GetStructPtrById(i);
        if(m_CoapAction.reg.all==1)			// Cancel by id=all
		{
            cp += sprintf((char*)cp,"cancel /sen/status?id=all");
		}
		else if(m_CoapAction.reg.id==1)		// Cancel by id=0,1,2,3
        {
            for(i=0;i<=COAP_ACTION_ID_MAX;i++)
            {
                if(Paser_CheckSenId(i)==TRUE && i<count)
                {
                    cp += sprintf((char*)cp,"cancel /sen/status?id=%d",i);
                }
            }
        }
		else if(m_CoapAction.reg.name==1)		// Cancel by name ( Sensor Hostname )
		{
			//pStructSensor = Sensor_GetStructPtrById(i);
			//if(pStructSensor!=NULL)
			//	cp += sprintf(cp,"/sen/status?n=\"%s\"", pStructSensor->hostname );
		}
        break;
		case	eURICmdDevInfoList:		// GET /dev/info?action=list
		TxCoapHeader->Code = eCoapCodeContent;	
		// </dev>;n="Temperature",mdl="WISE1020",sw="1.0.00",reset=0
        cp += sprintf((char*)cp,"</dev>;n=\"%s\",mdl=\"%s\",sw=\"%s\",reset=%d",
            pBoardCfg->hostname, pBoardCfg->modelname, pBoardCfg->sw_version, pBoardCfg->cap.reset );
		break;
        case	eURICmdDevStatus:		//	GET /dev/status?action=list
		TxCoapHeader->Code = eCoapCodeContent;
		break;
		case	eURICmdSetDevName:		//	PUT /dev/info?n="Demo 1"
		// n="Demo1"
        TxCoapHeader->Code = eCoapCodeChage; 				
		if(m_CoapAction.reg.dev==1 && m_CoapAction.reg.name==1)
		{
			/*len = strlen(m_CoapAction.name);
			if(len>0 && len<BOARD_NAME_MAX)
			{
                memset(pBoardCfg->hostname, 0,BOARD_NAME_MAX);
				strncpy(pBoardCfg->hostname, m_CoapAction.name, len);
                cp += sprintf(cp,"n=\"%s\"",pBoardCfg->hostname );
			}*/

			cp += sprintf((char*)cp,"n=\"%s\"",pBoardCfg->hostname );
		}	
		break;	
        case    eURICmdSetSenValueById: //  PUT /sen/status?id=5&bv=1											
        TxCoapHeader->Code = eCoapCodeChage;
        //	Payload id=5, bv=1
		for(i=0;i<COAP_ACTION_ID_MAX;i++)
		{
			if(Paser_CheckSenId(i)==TRUE && i<count)
			{			
				pStructSensor = Sensor_GetStructPtrById(i);
				if(pStructSensor!=NULL && pStructSensor->sid==3201) //		Digital Output 3201
				{
					cp += sprintf((char*)cp,"id=%d, ",i);
					if(m_CoapAction.reg.bv==1 )
						cp += sprintf((char*)cp,"bv=%d", m_CoapAction.Data.bVal );
					else if(m_CoapAction.reg.iv==1 )
						cp += sprintf((char*)cp,"iv=%d", m_CoapAction.Data.iVal );
					else if(m_CoapAction.reg.dv==1 )
						cp += sprintf((char*)cp,"dv=%d", m_CoapAction.Data.iVal );
					else if(m_CoapAction.reg.fv==1 )
						cp += sprintf((char*)cp,"fv=%f", m_CoapAction.Data.fVal );
				}
				else
				{
					cp += sprintf((char*)cp,"Device not is GPO!");
				}
			}
		}	
        break;
        case    eURICmdSetSenValueByName:      //	PUT	/sen/status?n=dout&bv=1
        TxCoapHeader->Code = eCoapCodeChage;
		//	Payload n="dout", bv=1
		pStructSensor = Sensor_GetStructPtrByHostname( m_CoapAction.name );
		if(pStructSensor!=NULL && pStructSensor->sid==3201) //		Digital Output 3201)
		{
			cp += sprintf((char*)cp,"n=\"%s\", ", pStructSensor->hostname );
			if(m_CoapAction.reg.bv==1) 
				cp += sprintf((char*)cp,"bv=%d", m_CoapAction.Data.bVal );
			else if(m_CoapAction.reg.iv==1 )
				cp += sprintf((char*)cp,"iv=%d", m_CoapAction.Data.iVal );
			else if(m_CoapAction.reg.dv==1 )
				cp += sprintf((char*)cp,"dv=%d", m_CoapAction.Data.iVal );
			else if(m_CoapAction.reg.fv==1 )
				cp += sprintf((char*)cp,"fv=%f", m_CoapAction.Data.fVal );
		}
		else
		{	
			cp += sprintf((char*)cp,"Device not is GPO!");
		}
        break;
        case	eURICmdNetInfoList:				//	GET /net/info?action=list
		TxCoapHeader->Code = eCoapCodeContent;
		// </net>;n="dust", sw="1.0.0.1"		
        cp += sprintf((char*)cp,"</net>;n=\"%s\", sw=\"%s\"", pBoardCfg->netdev_name, pBoardCfg->sw_version );

		break;
        case	eURICmdNetStatus:				//	Get /net/status
		TxCoapHeader->Code = eCoapCodeContent;
		
		
		break;
		case	eURICmdReset:
		TxCoapHeader->Code = eCoapCodeChage;	//	PUT /dev/info?reset=1
		//	reset=1		
        if(pBoardCfg->cap.reset==1)
            cp += sprintf((char*)cp,"reset=1");
        else
            cp += sprintf((char*)cp,"Not support reset");
		break;
        case    eURICmdUnKnown:     			//  UnKnown
        TxCoapHeader->Code = eCoapCodeContent;
		cp += sprintf((char*)cp,"Bad Code!");
        default:
        break;
    }

    if(cp==head)
        RetPacketSize = -1;
    else
        RetPacketSize = cp-head;


    return RetPacketSize + sizeof(CoapHeader_t);
}

// -----------------------------------------------------------
// Function Name: Paser_FindKey
// Description: Find Key and return last position of char which not processed
// Pararmeters:
//          1. Cptr: Pointer of payload buffer
//          2. Key: Key
//
// Return: If successful return last position of char which not processed, else return NULL
//
INT8* Paser_FindKey(INT8 *Cptr,INT8 *Key)
{
    INT16 Size = strlen((char*)Key);

    if(strncmp((char*)Cptr, (char*)Key, Size)==0)
        return Cptr+Size;
    else
        return NULL;
}

// -----------------------------------------------------------
// Function Name: Paser_KeyVal
// Description: Find Key and return the Value ( Key=Value )
// Pararmeters:
//          1. Cptr: Pointer of payload buffer
//          2. Key: Key be Searched
//          3. MaxSize: The max size of buffer will be searched
//          4. Val: Pointer to buffer of Value
//          5. Size: Size of buffer of Value
//
// Return: If successful return last position of char which not processed, else return NULL
//
INT8* Paser_KeyVal(INT8 *Cptr, INT8*  Key, INT16 MaxSize, INT8 *Buff, INT16 *Size)
{
    INT8 *cp;
    INT8 *first;
    INT8 *last;
    INT8 *mid;
    INT16 i, len;
    first = mid = last = NULL;
    cp = Cptr;
    len = strlen((char*)Key);
    memset(Buff,0,*Size);

    // /sen/info?action=list&field=id
    // Search Key
    for(i=0;i<MaxSize;i++)
    {
        if((cp==NULL || cp[i]=='\0') && first==NULL && last==NULL)
            break;

        if((cp[i]=='&'|| cp[i]=='?') && first==NULL) // &=0x26,?=0x
        {
            first=&cp[i];
            i++;
            if(strncmp((char*)&cp[i], (char*)Key, len)==0) // Match Key
            {
                if(cp[i+len]=='=')
                {
					first = &cp[i];  // Key start address
                    if((len+i+1)<MaxSize)
                    {
                        i = len+i+1;   // Value start adress
                        mid = &cp[i];
                    }
                }
            }
            else
                first = NULL;
        }

        if((cp[i]=='&'|| cp[i]=='\0') && last==NULL) // &=0x26,?=0x
        {
            last = &cp[i];
        }
    }

    // Get Value
    if((last-mid)>0 && (last-mid)<*Size && mid!=NULL && last!=NULL)
    {        
        cp = (INT8*)strncpy((char*)Buff, (char*)mid, last-mid );
        *Size = strlen((char*)Buff);
    }
    else if(mid!=NULL && last==NULL)
    {
        cp = (INT8*)strcpy((char*)Buff, (char*)mid );
    }
    else
    {
        *Size = -1;
        cp = NULL;
    }
   return cp;
}

// -----------------------------------------------------------
// Function Name: Paser_StrListId
// Description:	Parse sensor list IDs and set ID
//
// Pararmeters:
//          1. SensorList: Sensor ID lists with string
//
// Return:  If successful return number of Sensor IDs, else return -1
//
INT16 Paser_StrListId(const INT8 *SensorList)
{
    INT16 id = 0;
    INT16 count = 0;
	char *delim = (char*)",";
	INT8 number[3];
	char *p, *str;
	
	if(SensorList==NULL)
		return -1;
	
	str = (char *) SensorList;
	p = strtok(str,delim);
	while (p!=NULL)
	{
	  memset(number,0,3);
	  sprintf ((char*)number,"%s\n",p);
	  p = strtok (NULL, delim);
	  id = (INT16) strtol((char*)number,NULL,10);
	  if(id>=0 && id<COAP_ACTION_ID_MAX)
	  {
		Paser_SetSenId( id );
		count++;
	  }
	}      

	return count;
}

// -----------------------------------------------------------
// Function Name: Paser_SetSenId
// Description: Set ID number as be actived Sensor ID ( Max 16 sensor devices, 0~15 )
//
// Pararmeters:
//          1.  SensorId: ID to be active Sensor ID
//
// Return: None
//
void Paser_SetSenId(INT16 SensorId)
{
    switch(SensorId)
    {
        case 0: m_CoapAction.sen.id0 = 1; break;
        case 1: m_CoapAction.sen.id1 = 1; break;
        case 2: m_CoapAction.sen.id2 = 1; break;
        case 3: m_CoapAction.sen.id3 = 1; break;
        case 4: m_CoapAction.sen.id4 = 1; break;
        case 5: m_CoapAction.sen.id5 = 1; break;
        case 6: m_CoapAction.sen.id6 = 1; break;
        case 7: m_CoapAction.sen.id7 = 1; break;
		case 8: m_CoapAction.sen.id8 = 1; break;
        case 9: m_CoapAction.sen.id9 = 1; break;
        case 10: m_CoapAction.sen.id10 = 1; break;
        case 11: m_CoapAction.sen.id11 = 1; break;
        case 12: m_CoapAction.sen.id12 = 1; break;
        case 13: m_CoapAction.sen.id13 = 1; break;
        case 14: m_CoapAction.sen.id14 = 1; break;
        case 15: m_CoapAction.sen.id15 = 1; break;	
    }
}

// -----------------------------------------------------------
// Function Name: Paser_SetSenIdByString
// Description: Paseing Set Sensor Id string and set id bitmask
// Pararmeters:
//          1. SenIdString: Sensod Id String, For exapmle., id=0,1,2,4,5
//
// Return: None
//
void Paser_SetSenIdByString(INT8 *SenIdString)
{
    
}

// -----------------------------------------------------------
// Function Name: Paser_CheckSenId
// Description: Check Sensor ID is actived by selected SensorId
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return: If this Sensor Id is active return TRUE, else return FALSE
//
BOOL Paser_CheckSenId(INT16 SensorId)
{
    BOOL RetVal = FALSE;

    switch(SensorId)
    {
        case 0: if(m_CoapAction.sen.id0==1) RetVal= TRUE ; break;
		case 1: if(m_CoapAction.sen.id1==1) RetVal= TRUE ; break;
        case 2: if(m_CoapAction.sen.id2==1) RetVal= TRUE ; break;
        case 3: if(m_CoapAction.sen.id3==1) RetVal= TRUE ; break;
        case 4: if(m_CoapAction.sen.id4==1) RetVal= TRUE ; break;
        case 5: if(m_CoapAction.sen.id5==1) RetVal= TRUE ; break;
        case 6: if(m_CoapAction.sen.id6==1) RetVal= TRUE ; break;
        case 7: if(m_CoapAction.sen.id7==1) RetVal= TRUE ; break;
        case 8: if(m_CoapAction.sen.id8==1) RetVal= TRUE ; break;
		case 9: if(m_CoapAction.sen.id9==1) RetVal= TRUE ; break;
        case 10: if(m_CoapAction.sen.id10==1) RetVal= TRUE ; break;
        case 11: if(m_CoapAction.sen.id11==1) RetVal= TRUE ; break;
        case 12: if(m_CoapAction.sen.id12==1) RetVal= TRUE ; break;
        case 13: if(m_CoapAction.sen.id13==1) RetVal= TRUE ; break;
        case 14: if(m_CoapAction.sen.id14==1) RetVal= TRUE ; break;
        case 15: if(m_CoapAction.sen.id15==1) RetVal= TRUE ; break;
    }

    return RetVal;
}

// -----------------------------------------------------------
// Function Name: Parsr_itoa 
// Description: If OS no itoa function use it
// Pararmeters:
//          1. i: digit
//
// Return: return 0~9
//
#define INT_DIGITS 19		/* enough for 64 bit integer */

INT8 *Parsr_itoa(INT32 i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static INT8 buf[INT_DIGITS + 2];
  memset(buf, 0, INT_DIGITS + 2 );
  char *p = (char*)(buf + INT_DIGITS + 1);	/* points to terminating '\0' */
  if (i >= 0)
  {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return (INT8 *)p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return (INT8 *)p;
}

// -----------------------------------------------------------
// Function Name: Paser_GetCoapActionPtr
// Description: Get pointer of CoapAction Structure
// Pararmeters: None
//
// Return: reuturn pointer of CoapAction Structure
//
CoapAction_t *Paser_GetCoapActionPtr(void)
{
    
	return &m_CoapAction;
}

#if defined(TEST_PARSER)
// -----------------------------------------------------------
// Function Name: Parser_Test
// Description: Self test with MCU Side
// Pararmeters:
//			1. PacketBuff: Pointer of Coap Packet
//			2. Legth: Buffer Legth
//			3. TestCase: Select Test Case
//			4. SensorId: Sensor Id 
//
// Return: reuturn length of Coap Packet
//
UINT8 Parser_Test(UINT8* PacketBuff, UINT8 Length, INT16 TestCase, INT16 SensorId, INT16 Sid)
{
	UINT8 Size = 0;
	char *p;
	CoapPacket_t *pTxCoapPacket;

	if(PacketBuff==NULL || Length<0)
		return 0;

	memset(PacketBuff, 0,  Length);
	pTxCoapPacket = (CoapPacket_t *) PacketBuff;
	
#if 1
	pTxCoapPacket->Header.Type = eCoapTypeConfirmable;
#else
	pTxCoapPacket->Header.Type =  eCoapTypeReset;
#endif
	
	pTxCoapPacket->Header.Ver = 1;
	pTxCoapPacket->Header.Tkl = 0;
	pTxCoapPacket->Header.Msgid = 0x5a5a;

    p = (char *)pTxCoapPacket->Payload;
	switch(TestCase)
	{
		case 1:		//	0. Get Node's info
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		sprintf(p,"%s","/dev/info?action=list");
		break;
		case 2:		//	0.1 Get Net's info
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		sprintf(p,"%s","/net/info?action=list");
		break;
		case 3:		//	1.1 Get Sensor ID List	
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		sprintf(p,"%s","/sen/info?action=list&field=id");
		break;
		case 4:		//	1.2 Get Node's Detail Infomation
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		sprintf(p,"/sen/info?action=list&id=%d", SensorId);
		break;
		case 5:		//	1.3 Query Sensor Properties Information
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		if(Sid==0)
			sprintf(p,"/sen/info?action=query&sid=%d&id=%d", Sensor_GetSmid(SensorId), SensorId );// </3303>;asm="r", rt="ucum.Cel", type="d", st="ipso", u="Cel"
		else
			sprintf(p,"/sen/info?action=query&sid=%d", Sid);
		break;
		case 6:		//	1.4 Query Sensor Model Information
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		sprintf(p,"/sen/info?action=query&id=%d", SensorId);	//	</0>;min="-20", max="100", X="x", Y="y", Z="z" 
		break;
		case 7:		//	2.1 Get all sensors' value by Observe
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		sprintf(p,"/sen/status?action=observe&id=all"); // 0 36.2,1 26.5,2 9000,3 100,4 0, 5 10 20 30
		break;
		case 8:		//	2.1 Canel all sensors' value by Observe
		pTxCoapPacket->Header.Code = eCoapCodeGet;
		sprintf(p,"/sen/status?action=cancel&id=all"); 	//	cancel /sen/status?id=all
		break;
		case 9:		//	3.1 Set sensor value by id
		pTxCoapPacket->Header.Code = eCoapCodePut;
		sprintf(p,"/sen/status?id=%d&bv=%d", SensorId, ((Sid==0)?0:1) ); 	//	id=3, bv=1
		break;
		case 10:	//	3.2 Set sensor value by name
		pTxCoapPacket->Header.Code = eCoapCodePut;
		sprintf(p,"/sen/status?n=DigitalOutput&bv=%d", ((SensorId==0)?0:1) ); 	//	n="DigitalOutput", bv=1
		break;
		case 11:	//	4.1 Set Dev Name	
		pTxCoapPacket->Header.Code = eCoapCodePut;
		sprintf(p,"/dev/info?n=\"Demo 1\""); 	//	 n="Demo1"
		break;
		case 12:	//	4.2 Reset	
		pTxCoapPacket->Header.Code = eCoapCodePut;
		sprintf(p,"/dev/info?reset=1"); 	//	 reset=1 // if reset=0 means no support reset function
		break;
		default:	
		pTxCoapPacket->Header.Code = eCoapCodeChage;
		break;
	}

	Size = strlen(p);
	pTxCoapPacket->Header.Length = Size;
	
	return Size + sizeof(CoapHeader_t);
}
#endif
