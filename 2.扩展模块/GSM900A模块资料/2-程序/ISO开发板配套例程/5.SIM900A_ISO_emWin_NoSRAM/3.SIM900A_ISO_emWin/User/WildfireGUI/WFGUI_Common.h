#ifndef __WFGUI_COMMON_H
#define __WFGUI_COMMON_H

#include "WFGUI_LinkedList.h"

#include "ICONVIEW.h"
#include "bsp_usart1.h"
#include "bsp_date.h"
#include "ff.h"

/* �û��������Ϣ */
#define MY_MESSAGE_RTC 				(WM_USER+0)
#define MY_MESSAGE_SNAPSHOT 	(WM_USER+1)
#define MY_MESSAGE_CTRLCHANGE	(WM_USER+2)

/* GUI ID */
#define GUI_ID_FRAMEWIN0 	(GUI_ID_USER+1)	
#define GUI_ID_FRAMEWIN1 	(GUI_ID_USER+2)	
#define GUI_ID_FRAMEWIN2 	(GUI_ID_USER+3)	
#define GUI_ID_FRAMEWIN3 	(GUI_ID_USER+4)	
#define GUI_ID_FRAMEWIN4 	(GUI_ID_USER+5)	
#define GUI_ID_FRAMEWIN5 	(GUI_ID_USER+6)	
#define GUI_ID_FRAMEWIN6 	(GUI_ID_USER+7)	
#define GUI_ID_FRAMEWIN7 	(GUI_ID_USER+8)	

#define GUI_ID_LISTWHEEL4 (GUI_ID_USER + 9)
#define GUI_ID_LISTWHEEL5 (GUI_ID_USER + 10)
#define GUI_ID_LISTWHEEL6 (GUI_ID_USER + 11)
#define GUI_ID_FRAMEWIN8 	(GUI_ID_USER+12)

#define GUI_ID_WINDOW0 	  (GUI_ID_USER+13)

#define GUI_ID_FRAMEWIN9 	(GUI_ID_USER+14)
#define GUI_ID_FRAMEWIN10 (GUI_ID_USER+15)

/* icon��ҳ�� */
#define VIRTUAL_WIN_NUM					(2)

/* ÿҳ��ͼ����*/
#define ICON_PAGE_NUM						(20)



#define FILE_NAME_LEN 	150			//�ļ������ȣ������⵽�ļ�������50 ��������ļ� 
#define PATH_LEN				100
#define MUSIC_NAME_LEN 	24			//LCD��ʾ���ļ�������󳤶�
#define _DF1S	0x81


#define FILE_LIST_PATH 			"0:WF_OS/Record/filelist.txt"					//�ļ���¼�б��ļ���Ŀ¼
#define IMAGE_LIST_PATH 		"0:WF_OS/Record/imagelist.txt"					//ͼƬ�ļ���¼�б��ļ���Ŀ¼
#define TEXT_LIST_PATH 			"0:WF_OS/Record/textlist.txt"					//ͼƬ�ļ���¼�б��ļ���Ŀ¼


#define XBF_FONT_PATH				"0:/WF_OS/FONT/weiruanex2.xbf"				//�ⲿ�����ļ��Ĵ洢·��


typedef struct WIN_PARA{			//����ʹ�õ����û���������������ڻص�������ʹ��
  int xSizeLCD, ySizeLCD;			//LCD��Ļ�ߴ�
  int xPosWin, yPosWin;				//���ڵ���ʼλ��
	int xSizeWin, ySizeWin;			//���ڳߴ�
	int xSizeWinVirtual;				//���ⴰ�ڳߴ�	
	
	unsigned char ctr_index;		//��¼icorn���ڵ�����
	unsigned char isDesktop;		//��¼���洰���Ƿ�����ǰ  0�����洰�ڲ�����ǰ 1�����洰������ǰ
	
	WM_HWIN hWinCtrl;						//���ƴ��ڵľ�����ײ���
	WM_HWIN hWinMain;						//����ʾ���ľ�����в���
	WM_HWIN hWinStatus;					//״̬���ڵľ����������
	WM_HWIN hWinTime;
	
	WM_HWIN hApp[20];						//��¼�������е�App���
	
}WIN_PARA;

typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
  const char       * pExplanation;
} BITMAP_ITEM;


typedef struct
{
	FIL hFontFile;
	GUI_FONT     XFont;
	GUI_XBF_DATA XBF_Data; 	

}SD_FONT;


typedef struct
{
	struct list_head listNode;
	WM_HWIN hAPP;
	
}HANDLE_LIST;

typedef enum FILE_TYPE {ALLFILE,IMAGEFILE,TEXTFILE} FILE_TYPE;					//�ļ�����


extern WIN_PARA WinPara;			//�û�ʹ�õĴ��ڶ�������
extern SD_FONT SDFont;

/*ʱ��ṹ��*/
extern struct rtc_time systmtime;

/* ��Щ�����ļ��洢�� WFGUI_Icon.c �ļ�*/

/***ͼ��**/
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Logo;				//Ұ��Logo��δ����״̬
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_LogoPr;			//Ұ��Logo������״̬

extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Floder;			//�ļ�������
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Clock;				//ʱ��
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Message;			//����
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Phone;				//�绰
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Note;				//�ʼ�
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Calculator;	//������
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Camera;			//����ͷ
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Picture;			//ͼƬ�����
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Map;					//��ͼ
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Temperature;	//�¶ȼ�
extern GUI_CONST_STORAGE GUI_BITMAP bmWFGUI_UserApp;  //�û�ͼ��




/* ���к��� */
//extern void 			App_Insert			(WM_HWIN NewApp);
//extern WM_HWIN 		App_Delete			(WM_HWIN NewApp);
//extern WM_HWIN 		App_GetTopWin		(void);

/* hAPP������ */
extern HANDLE_LIST* 	hAPPLinkedList_NewNode		(void);
extern void 					hAPPLinkedList_Init				(void);
extern void						hAPPLinkedList_Del				(HANDLE_LIST *node);
extern void 					hAPPLinkedList_AddTail		(HANDLE_LIST *node);
extern HANDLE_LIST* 	hAPPLinkedList_GetAppNode	(WM_HWIN hAPP);
extern HANDLE_LIST* 	hAPPLinkedList_GetAppTop	(void);





#endif


