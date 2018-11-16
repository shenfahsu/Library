/**
  ******************************************************************************
  * @file    WFGUI_Keypad.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ���뷨����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <stddef.h>
#include "DIALOG.h"
#include <string.h>
#include "t9py.h"
#include "ff.h"

#include "WFGUI_Common.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define COLOR_BACK0            0xFF3333
#define COLOR_BACK1            0x550000
#define COLOR_BORDER           0x444444
#define COLOR_KEYPAD0          0xAAAAAA
#define COLOR_KEYPAD1          0x555555
#define BUTTON_COLOR0          0xEEEEEE
#define BUTTON_COLOR1          0xCCCCCC
#define BUTTON_COLOR2          0xCCCCCC
#define BUTTON_COLOR3          0xAAAAAA
#define BUTTON_SKINFLEX_RADIUS 4
#define ID_BUTTON              (GUI_ID_USER + 0)
#define APP_INIT_LOWERCASE     (WM_USER + 0)

#define ID_TEXT					(GUI_ID_USER+40)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;
  GUI_COLOR	   bkColor;
  void (* pfDraw)(WM_HWIN hWin);
} BUTTON_DATA;




/*********************************************************************
*
*       Static data using function pointers
*
**********************************************************************
*/

#define BUTTON_XSIZE	45		//��
#define BUTTON_YSIZE	24		//��	
#define PITCH			    3			//�С��о�	
#define GUI_KEY_COLOR		GUI_WHITE	//��ͨ����ɫ
#define GUI_FUN_COLOR		0xffffffc2	//���ܰ�����ɫ


GUI_ConstString ListBoxTest[] = {",",".","?","!","...",":",";","~",NULL};

static const BUTTON_DATA _aButtonData[] = {
	
	/*	��һ�Ű��� */
  { PITCH*2+(BUTTON_XSIZE+PITCH)	,PITCH+(BUTTON_YSIZE+PITCH)			,  BUTTON_XSIZE		, BUTTON_YSIZE, "1"   ,GUI_KEY_COLOR },
  { PITCH*2+(BUTTON_XSIZE+PITCH)*2	,PITCH+(BUTTON_YSIZE+PITCH)			,  BUTTON_XSIZE		, BUTTON_YSIZE, "2ABC",GUI_KEY_COLOR },
  { PITCH*2+(BUTTON_XSIZE+PITCH)*3	,PITCH+(BUTTON_YSIZE+PITCH)			,  BUTTON_XSIZE		, BUTTON_YSIZE, "3DEF",GUI_KEY_COLOR },
  { PITCH*2+(BUTTON_XSIZE+PITCH)*4	,PITCH+(BUTTON_YSIZE+PITCH)			,  BUTTON_XSIZE-10	, BUTTON_YSIZE, "<--" ,GUI_FUN_COLOR },

  /*	�ڶ��Ű��� */
  { PITCH*2+(BUTTON_XSIZE+PITCH)	,	PITCH+(BUTTON_YSIZE+PITCH)*2	,  BUTTON_XSIZE		, BUTTON_YSIZE, "4GHI"	,GUI_KEY_COLOR},
  { PITCH*2+(BUTTON_XSIZE+PITCH)*2	,	PITCH+(BUTTON_YSIZE+PITCH)*2	,  BUTTON_XSIZE		, BUTTON_YSIZE, "5JKL"	,GUI_KEY_COLOR},
  { PITCH*2+(BUTTON_XSIZE+PITCH)*3	,	PITCH+(BUTTON_YSIZE+PITCH)*2	,  BUTTON_XSIZE		, BUTTON_YSIZE, "6MNO"	,GUI_KEY_COLOR},
  { PITCH*2+(BUTTON_XSIZE+PITCH)*4	,   PITCH+(BUTTON_YSIZE+PITCH)*2	,  BUTTON_XSIZE-10	, BUTTON_YSIZE, "CA"  ,GUI_FUN_COLOR},

  /* �����Ű���	*/
  { PITCH*2+(BUTTON_XSIZE+PITCH)	,	PITCH+(BUTTON_YSIZE+PITCH)*3	,  BUTTON_XSIZE		, BUTTON_YSIZE, "7PQRS"	,GUI_KEY_COLOR },
  { PITCH*2+(BUTTON_XSIZE+PITCH)*2	,	PITCH+(BUTTON_YSIZE+PITCH)*3	,  BUTTON_XSIZE		, BUTTON_YSIZE, "8TUV"	,GUI_KEY_COLOR},
  { PITCH*2+(BUTTON_XSIZE+PITCH)*3	,	PITCH+(BUTTON_YSIZE+PITCH)*3	,  BUTTON_XSIZE		, BUTTON_YSIZE, "9WXYZ"	,GUI_KEY_COLOR},
  { PITCH*2+(BUTTON_XSIZE+PITCH)*4	,   PITCH+(BUTTON_YSIZE+PITCH)*3	,  BUTTON_XSIZE-10	, BUTTON_YSIZE, "0"  ,GUI_FUN_COLOR},

    /* �����Ű���	*/
  { PITCH*2+(BUTTON_XSIZE+PITCH)		,	PITCH+(BUTTON_YSIZE+PITCH)*4	,  BUTTON_XSIZE-10		, BUTTON_YSIZE, "123"	,GUI_FUN_COLOR},
  { PITCH*2+(BUTTON_XSIZE+PITCH)*2-10	,	PITCH+(BUTTON_YSIZE+PITCH)*4	,  BUTTON_XSIZE+20		, BUTTON_YSIZE, "   "	,GUI_KEY_COLOR },
  { PITCH*2+(BUTTON_XSIZE+PITCH)*3+10	,	PITCH+(BUTTON_YSIZE+PITCH)*4	,  BUTTON_XSIZE-10		, BUTTON_YSIZE, "C/E"	,GUI_FUN_COLOR },
  { PITCH*2+(BUTTON_XSIZE+PITCH)*4		,   PITCH+(BUTTON_YSIZE+PITCH)*4	,  BUTTON_XSIZE-10		, BUTTON_YSIZE, "<-|"  ,GUI_FUN_COLOR},
  { PITCH*2								,   PITCH+(BUTTON_YSIZE+PITCH)*4	,  BUTTON_XSIZE			, BUTTON_YSIZE, ",."  ,GUI_FUN_COLOR},

};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

extern void clrindex(void);
/*********************************************************************
*
*       _DrawSkinFlex_BUTTON
*
* Function description
*   Skinning routine for keypad buttons
*/
static int _DrawSkinFlex_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  static GUI_MEMDEV_Handle ahMemEdges[4];
  GUI_MEMDEV_Handle        hMem;
  WM_HWIN                  hWin;
  unsigned                 i;
  int                      xPos;
  int                      yPos;
  int                      xSize;
  int                      ySize;
  int                      x;
  int                      y;
  int                      Id;
  int                      IsPressed;
  void (* pfDraw)(WM_HWIN hWin);

  x = 0;
  y = 0;
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_CREATE:
    if (ahMemEdges[0] == 0) {
      //
      // Create small helper window which never becomes visible
      //
      hWin = WM_CreateWindowAsChild(0, 0, 32, 32, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, NULL, 0);
      WM_SelectWindow(hWin);
      //
      // Create memory devices for the edges of the buttons
      //
      for (i = 0; i < GUI_COUNTOF(ahMemEdges); i++) {
        switch (i) {
        case 0:
          x = 0;
          y = 0;
          break;
        case 1:
          x = -16;
          y = 0;
          break;
        case 2:
          x = -16;
          y = -16;
          break;
        case 3:
          x = 0;
          y = -16;
          break;
        }
        //
        // Create magnified device 
        //
        hMem = GUI_MEMDEV_CreateFixed(0, 0, 16, 16, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
        GUI_MEMDEV_Select(hMem);
        GUI_SetBkColor(GUI_TRANSPARENT);
        GUI_Clear();
        GUI_SetColor(COLOR_BORDER);
        GUI_DrawRoundedFrame(x, y, x + 31, y + 31, 16, 4);
        //
        // Shrink to required size
        //
        ahMemEdges[i] = GUI_MEMDEV_CreateFixed(0, 0, 4, 4, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
        GUI_MEMDEV_Select(ahMemEdges[i]);
        GUI_SetBkColor(GUI_TRANSPARENT);
        GUI_Clear();
        GUI_MEMDEV_RotateHQ(hMem, ahMemEdges[i], -6, -6, 0, 250);
        GUI_MEMDEV_Delete(hMem);
        GUI_MEMDEV_Select(0);
      }
      WM_DeleteWindow(hWin);
    }
    break;
  case WIDGET_ITEM_DRAW_TEXT:
    //
    // Use private or default function for drawing bitmap or text
    //
    hWin = pDrawItemInfo->hWin;
    Id   = WM_GetId(hWin);
    i    = Id - ID_BUTTON;
    pfDraw = _aButtonData[i].pfDraw;
    if (pfDraw) {
      pfDraw(hWin);
    } else {
      BUTTON_DrawSkinFlex(pDrawItemInfo);
    }
    break;
  case WIDGET_ITEM_DRAW_BACKGROUND:
    //
    // Draw background of button
    //
    IsPressed = BUTTON_IsPressed(pDrawItemInfo->hWin);
    xPos      = WM_GetWindowOrgX(pDrawItemInfo->hWin);
    yPos      = WM_GetWindowOrgY(pDrawItemInfo->hWin);
    xSize     = WM_GetWindowSizeX(pDrawItemInfo->hWin);
    ySize     = WM_GetWindowSizeY(pDrawItemInfo->hWin);
    if (IsPressed) {
      GUI_DrawGradientRoundedV(0, 0, xSize - 1, ySize - 1, 4, BUTTON_COLOR2, GUI_BLUE);//BUTTON_COLOR3
    } else {
      GUI_DrawGradientRoundedV(0, 0, xSize - 1, ySize - 1, 4, BUTTON_COLOR0, BUTTON_COLOR1);
    }
    GUI_SetColor(COLOR_BORDER);
    GUI_DrawHLine(        0, 4, xSize - 5);
    GUI_DrawHLine(ySize - 1, 4, xSize - 5);
    GUI_DrawVLine(        0, 4, ySize - 5);
    GUI_DrawVLine(xSize - 1, 4, ySize - 5);
    GUI_MEMDEV_WriteAt(ahMemEdges[0], xPos +  0, yPos +  0);
    GUI_MEMDEV_WriteAt(ahMemEdges[1], xPos + xSize - 4, yPos +  0);
    GUI_MEMDEV_WriteAt(ahMemEdges[2], xPos + xSize - 4, yPos + 20);
    GUI_MEMDEV_WriteAt(ahMemEdges[3], xPos +  0, yPos + 20);
    break;
  default:
    //
    // Use the default skinning routine for processing all other commands
    //
    return BUTTON_DrawSkinFlex(pDrawItemInfo);
  }
  return 0;
}

unsigned int key_msg;

extern void T9pyTask(unsigned int key);



struct t9PY_index
{
    const uint32_t t9PY;
    const uint8_t *PY;
    const uint8_t *PY_mb;
};

struct t9py_struct
{
	uint8_t  pysrf;       //���뷨ѡ��
	uint8_t  key_len;
	uint8_t  firstno;     //����ĵ�һ�����ְ���
	uint8_t  mblen;       //������ĳ���
	uint8_t  mbtimes;     //����л��Ĵ���
	uint32_t t9py;		//���ְ������,��ϳ�16���ƣ����簴��246��Ϊ0x246
	const struct t9PY_index  *t9PY_addr;		//ָ������ַ
};

extern struct  t9py_struct t9pyfun;  



extern void sim900a_gbk2ucs2(char * ucs2,char * gbk);




WM_HWIN hMulti;

static void _cbWordWin(WM_MESSAGE * pMsg)
 {
	 WM_HWIN	hText;
	 WM_HWIN hEdit;
	 int        NCode;
	 int        Id;
	 char		i;
	 static	int IdSel = 0;
	 int	PYSel = 0;
	 char	WordSel[4] = {0} ; 

	 WM_MOTION_INFO * pInfo;
	 static char move_flag = 0;

	  switch (pMsg->MsgId) {
		
		case WM_CREATE:
			for(i=0;i<110;i++)
				{			
					hText = TEXT_CreateEx(PITCH+i*18,PITCH,18,20,pMsg->hWin,WM_CF_SHOW,TEXT_CF_HCENTER|TEXT_CF_VCENTER,ID_TEXT+i,"");
				}
			break;
#if 1	
				

		case WM_NOTIFY_PARENT:							//֪ͨ������
			 
			Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
			NCode = pMsg->Data.v;                 // ֪ͨ����
			 
		switch(NCode){
			case WM_NOTIFICATION_CLICKED:
				if(Id>=ID_TEXT && Id<ID_TEXT+110)
					{
						PYSel = Id - ID_TEXT;

						if(IdSel == 0 && (move_flag != 1)&& (strlen((const char *)t9pyfun.t9PY_addr->PY_mb)>PYSel*3))
						{								
							hText    = WM_GetDialogItem(pMsg->hWin,Id);	
							TEXT_SetBkColor(hText,GUI_DARKGRAY);
							IdSel = Id;//��¼����ֹ�ظ�����
						}
					}
			
				break;

			case WM_NOTIFICATION_RELEASED:
				move_flag = 0;
				if(Id>=ID_TEXT && Id<ID_TEXT+110)
					{	
						if(t9pyfun.pysrf == T9PY)
						{
							if(IdSel != 0 /* && move_flag != 1*/)	//��ѡ�����ַ�
							{
								
								hText    = WM_GetDialogItem(pMsg->hWin,IdSel);
								TEXT_SetBkColor(hText,GUI_TRANSPARENT);
							
								PYSel = IdSel - ID_TEXT;						
						
								WordSel[0] =  t9pyfun.t9PY_addr->PY_mb[PYSel*3] ;	//ָ��ѡ�е��ֵ����   UTF8��	
								WordSel[1] =  t9pyfun.t9PY_addr->PY_mb[PYSel*3+1] ;							
								WordSel[2] =  t9pyfun.t9PY_addr->PY_mb[PYSel*3+2] ;
								
								/* ��ȡ��ǰ���ھ�� */
								hEdit = WM_GetFocussedWindow();
								{
									WM_CALLBACK * pcb = WM_GetCallback(hEdit);
														
									if(pcb == MULTIEDIT_Callback)
									{								
										MULTIEDIT_AddText(hEdit,WordSel);
									}
								}
									
								clrindex();

								IdSel = 0;									
							}
						}

					}			
				break;


			default:
				break;
		}//NCODE switch

		break;
		
#if 1
		case WM_MOVE:	
			if(IdSel != 0)
			{
			hText    = WM_GetDialogItem(pMsg->hWin,IdSel);
			TEXT_SetBkColor(hText,GUI_TRANSPARENT);
			}
	

			IdSel = 0;								//���ѡ����־
			move_flag = 1;						//��־�ƶ��˴���

		break;

		case WM_MOUSEOVER_END:
			move_flag = 0;						//�ƶ�����
			break;
		#endif
#if 1
		case WM_MOTION:             //�����motion��ֱ�ӻ���break����ִֹ��text
			pInfo = (WM_MOTION_INFO *)pMsg->Data.p;
			 if (pInfo) {
				  switch (pInfo->Cmd) {
					  case WM_MOTION_INIT:
						pInfo->SnapX = 1;		//���ض���
						pInfo->SnapY = 0;
						pInfo->Period = 1000;
						break;
					  }
			 }
			  
			break;

#endif
			 

		case WM_PAINT:
			GUI_DrawGradientV(0, 0,WM_GetWindowSizeX(pMsg->hWin), WM_GetWindowSizeY(pMsg->hWin), COLOR_KEYPAD0, COLOR_KEYPAD1);

		break;
		
#endif
		default:
			break;
  }
 
 
 }

#define BUTTON	0
#define LISTBOX 1

 void PY_WordDisplay(WM_HWIN hParent,char object)
 {
	WM_HWIN	 hText;
	WM_HWIN	 hListBox;
	unsigned char *test;
	unsigned int  i;	
	char dis_temp[4]={1,1,1,'\0'};                      //utf8 3���ֽڱ�ʾһ�����֣�'\0'Ϊ������

	char ListBoxNum;

	{
		#ifndef UTF8_CODE
			char unicode_buf[400];
			char utfcode_buf[400];
			/* ƴ�������GBK�룬Ҫת�� */
			//��ʾ		
			test = 	t9pyfun.t9PY_addr->PY_mb;

			sim900a_gbk2ucs2(unicode_buf,test);
	
			GUI_UC_ConvertUC2UTF8((unsigned short *)unicode_buf,strlen(unicode_buf),utfcode_buf,50);

			for(i=0;i<110;i++)
			{
				if(dis_temp[0] != '\0')		//�ַ�����û����
				{
					*dis_temp= *(utfcode_buf+i*3);
					*(dis_temp+1) = *(utfcode_buf+i*3+1);
					*(dis_temp+2) = *(utfcode_buf+i*3+2);

					hText    = WM_GetDialogItem(hParent,ID_TEXT+i);
					TEXT_SetText(hText,dis_temp);

				}
				else//�Ѷ���text����Ϊ0
				{
					hText    = WM_GetDialogItem(hParent,ID_TEXT+i);
					TEXT_SetText(hText,"");
					}			
			}

		#else
		   /* ƴ�������UTF8�룬����Ҫת�� */
			//��ʾ
			test = 	(unsigned char *)t9pyfun.t9PY_addr->PY_mb;

			for(i=0;i<110;i++)
			{
				if(dis_temp[0] != '\0')		//�ַ�����û����
				{
					*dis_temp= *(test+i*3);
					*(dis_temp+1) = *(test+i*3+1);
					*(dis_temp+2) = *(test+i*3+2);

					hText    = WM_GetDialogItem(hParent,ID_TEXT+i);
					TEXT_SetText(hText,dis_temp);

				}
				else//�Ѷ���text����Ϊ0
				{
					hText    = WM_GetDialogItem(hParent,ID_TEXT+i);
					TEXT_SetText(hText,"");
				}
			}
		#endif

			if(object == BUTTON )
			{
				hListBox = WM_GetDialogItem(hParent,GUI_ID_LISTBOX0);
				/* ��ʾƴ��������� */
				ListBoxNum = LISTBOX_GetNumItems(hListBox);	    //��ȡԭlistbox����
				for(i=0;(i<ListBoxNum  || i< t9pyfun.mblen);i++)
				{
					if(i<t9pyfun.mblen)						                //��û��ʾ��ƴ������
					{
						if(ListBoxNum>t9pyfun.mblen)		                                              //���ԭlistbox���ȴ���ƴ��������
							LISTBOX_SetString(hListBox,(const char *)(t9pyfun.t9PY_addr+i)->PY,i);		  //�޸�ԭ��listbox��Ŀ������
						else								                                                          //���ԭlistbox���Ȳ���
							LISTBOX_InsertString(hListBox,(const char *)(t9pyfun.t9PY_addr+i)->PY,i);		//�����µ���Ŀ	
					}
					else	//ɾ���������Ŀ
						LISTBOX_DeleteItem(hListBox,t9pyfun.mblen);//�Ѷ������ɾ��
				}		
			}

	  }
 
 }
 
 const unsigned char PY_mb_abcdef     []={0xe5,0x95,0x8a,0x00};

/*********************************************************************
*
*       _cbKeyPad
*
* Function description
*   Callback routine of keypad window
*/
static void _cbKeyPad(WM_MESSAGE * pMsg) {
  WM_HWIN    hWin;
  WM_HWIN    hButton;
  WM_HWIN	 hListBox;
	WM_HWIN hEdit;
	
  static WM_HWIN   hTimer;
  char       c;
  int        Id;
  int        NCode;
  int        xSize;
  int        ySize;
  unsigned int  i;
  char ListBoxSel;
  static char PreListBoxSel = 0;
  char	WordSel[4] = {0} ; 
  static char TimerFlag=0;
  static char PreC=0;
  static char SamePress=0;
  


  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
	

  case WM_CREATE:
    //
    // Create the keyboard buttons
    //
    for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) {
      hButton = BUTTON_CreateEx(_aButtonData[i].xPos, _aButtonData[i].yPos, _aButtonData[i].xSize, _aButtonData[i].ySize, 
                                hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
      BUTTON_SetText(hButton, _aButtonData[i].acLabel);

      BUTTON_SetBkColor(hButton,BUTTON_CI_UNPRESSED,_aButtonData[i].bkColor);
      BUTTON_SetFocussable(hButton, 0);
    }


	hListBox = LISTBOX_CreateEx(PITCH,PITCH+(BUTTON_YSIZE+PITCH),50,70,pMsg->hWin,WM_CF_SHOW,0,GUI_ID_LISTBOX0,NULL);//ListBoxTest

	LISTBOX_SetAutoScrollV(hListBox,1);

		

  WM_CreateWindowAsChild( 2,
                          4,
                          240*9,
                          24,
                          pMsg->hWin, WM_CF_SHOW |WM_CF_MEMDEV| WM_CF_MOTION_X, _cbWordWin, NULL
                          );


	
    break;

  case WM_PAINT:
    //
    // Draw background
    //
    xSize = WM_GetWindowSizeX(hWin);
    ySize = WM_GetWindowSizeY(hWin);
    GUI_SetColor(COLOR_BORDER);
    GUI_DrawRect(0, 0, xSize - 1, ySize - 1);
    GUI_DrawGradientV(1, 1, xSize - 2, ySize - 2, COLOR_KEYPAD0, COLOR_KEYPAD1);
    break;

  case WM_TIMER:
	Id = WM_GetTimerId(pMsg->Data.v);	
 	TimerFlag = 1;
	{
	WordSel[0] = t9pyfun.t9PY_addr->PY_mb[SamePress-1];
		
	hEdit =	WM_GetFocussedWindow();
		
			{
					WM_CALLBACK * pcb = WM_GetCallback(hEdit);
										
					if(pcb == MULTIEDIT_Callback)
					{								
						MULTIEDIT_AddText(hEdit,WordSel);
					}
			}

	SamePress = 0;
	TimerFlag = 0;
	}

    WM_DeleteTimer(Id);

	break;

  case WM_NOTIFY_PARENT:
    //
    // Send key message to currently focused window
    //
    Id    = WM_GetId(pMsg->hWinSrc);
	NCode = pMsg->Data.v;

    switch (NCode) {
    case WM_NOTIFICATION_RELEASED:	

		if(Id == ID_BUTTON+12)			//�л���������״̬
		{
			if(t9pyfun.pysrf != T9SZ)	//�л����뷨״̬
				t9pyfun.pysrf = T9SZ;
			else
				t9pyfun.pysrf = T9PY;
		}
		else if(Id == ID_BUTTON+14)		//�л���Ӣ������״̬
		{

			if(t9pyfun.pysrf != T9DX)	//�л����뷨״̬
				t9pyfun.pysrf = T9DX;
			else
				t9pyfun.pysrf = T9PY;
		
		}
		else if(Id == ID_BUTTON+7)		//�л����뷨״̬
		{
			if( t9pyfun.pysrf != T9PY)	//������������ֻ�ƴ������״̬����Ӣ��״̬�£�
			{			
				if(t9pyfun.pysrf == T9DX)	//�л����뷨״̬
					t9pyfun.pysrf = T9XX;
				else
					t9pyfun.pysrf = T9DX;
			}

		}
		else if(Id == ID_BUTTON+3)
		{

		  GUI_SendKeyMsg(GUI_KEY_DELETE,1);
		  GUI_SendKeyMsg(GUI_KEY_DELETE, 0);
		}
		else
		{
		hListBox = WM_GetDialogItem(pMsg->hWin,GUI_ID_LISTBOX0);

		if(Id >=ID_BUTTON  && Id<(ID_BUTTON+GUI_COUNTOF(_aButtonData) ) )	//����
		{
		 if (_aButtonData[Id - ID_BUTTON].acLabel) {	//ȷ�����ĸ�����
		
			 c = _aButtonData[Id - ID_BUTTON].acLabel[0];
		
			//����	
			T9pyTask(c);

			if(t9pyfun.pysrf == T9DX || t9pyfun.pysrf == T9XX && TimerFlag != 1)
			 {
				 {
					 if(c == PreC || SamePress == 0 && c!='1' )
				
					 TimerFlag = 0;
					 WM_DeleteTimer(hTimer);	//		ɾ��ԭ����timer
					 hTimer = WM_CreateTimer(pMsg->hWin,0,1200,0);
					 PreC = c;

					 if(SamePress >= strlen((const char *)t9pyfun.t9PY_addr->PY))
						SamePress = 1;
					 else
						SamePress++;
					
					 return;
				 
				 }
			 }

			if(t9pyfun.pysrf == T9PY && c != '1')
			{

				/* ��ʾ��ѡƴ�� */
				PY_WordDisplay(pMsg->hWin,BUTTON);

				LISTBOX_SetSel(hListBox,0);		//��λѡ��Ϊ0
				PreListBoxSel = 0;				//��λ��һ�εļ�¼
			}		
			else if(t9pyfun.pysrf == T9SZ)
			{
				//PYSel = Id - ID_BUTTON;
				WordSel[0] = t9pyfun.t9PY_addr->PY_mb[0];
				
				hEdit =	WM_GetFocussedWindow();
				
			{
					WM_CALLBACK * pcb = WM_GetCallback(hEdit);
										
					if(pcb == MULTIEDIT_Callback)
					{								
						MULTIEDIT_AddText(hEdit,WordSel);
					}
				}
			}

		 }		

     }
	if(Id == GUI_ID_LISTBOX0)
	{
		if(t9pyfun.pysrf == T9PY)
		{
			// TBD ����ѡ����ʲôƴ��			
			ListBoxSel = LISTBOX_GetSel(hListBox);			

			t9pyfun.t9PY_addr = t9pyfun.t9PY_addr + ListBoxSel - PreListBoxSel;		//�޸�ָ������ PreListBoxSel��һ��ѡ������ȥ��һ�ε�ƫ��

			/* ���� */
			PreListBoxSel = ListBoxSel;

			
			//��ʾ
			PY_WordDisplay(pMsg->hWin,LISTBOX);
		}
		else if(t9pyfun.pysrf == T9SZ)
		{

		
		}		

		}
	 }

      break;
    }
	break;


  default:
	  break;
  }
}



/*********************************************************************
*
*       MainTask
*/

void KeypadInit(WM_HWIN hFrame) {
 
	
		/* ʹ�ܴ洢����ʹ�� */	
	 WM_SetCreateFlags(WM_CF_MEMDEV);

#if 0									//0:/WF_OS/FONT/HuaWen17.xbf
     res = f_open(&hFile,"0:/WF_OS/FONT/weiruanex2.xbf",FA_READ|FA_OPEN_EXISTING);
    if (res == FR_OK) {
			
     /* �ɹ����ֿ��ļ� */
    GUI_XBF_CreateFont(	&Font,             					// ָ�򴴽��õ��������ݽṹ�洢λ��
												&XBF_Data,         					// ָ��XBF_DATA���ݽṹ
												GUI_XBF_TYPE_PROP_AA2_EXT, 	// Ҫ��������������
												_cbGetData,        					// ��ȡ�ֿ��ļ��Ļص�����
												&hFile);            				// ָ���ֿ��ļ����ļ�����
			
		/* ����GUI���� */
    GUI_SetFont(&Font);
			
		/* �����ı����� */	
	  TEXT_SetDefaultFont(&Font);
			
		/* ����GUI������� */
	  GUI_UC_SetEncodeUTF8();
	}


#endif 


#if 1
	BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON);	//ʹ�ô洢���������
#else
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
#endif
	
	/* ���������Ϣ */
	clrindex();

  /* �������� */ 
  WM_CreateWindowAsChild(0, 110, 240, 140,hFrame , WM_CF_SHOW | WM_CF_STAYONTOP, _cbKeyPad, 0);

}

/*************************** End of file ****************************/
