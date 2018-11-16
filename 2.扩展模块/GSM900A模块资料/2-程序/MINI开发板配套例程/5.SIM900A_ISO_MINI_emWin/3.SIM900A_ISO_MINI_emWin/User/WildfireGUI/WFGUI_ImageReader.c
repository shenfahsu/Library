/**
  ******************************************************************************
  * @file    WFGUI_ImageReader.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ͼƬ�Ķ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"

#include "bsp_usart1.h"
#include "WFGUI_Common.h"
#include "WFGUI_ImageReader.h"
#include "WFGUI_SDView.h"


/* ֱ�Ӷ����char���ͣ��п��ܵ�ַ�����룬����DMA������� */
//char _acBuffer[1024*4];

/* �����4�ֽڶ��룬��ֹDMA������� */
__align(4) char _acBuffer[1024*4] ;		

/*Ҳ������������,��������ͣ��Ӷ��������Զ����䵽4�ֽڶ���Ŀռ�*/
//int _acIntBuffer[1024];


static WM_CALLBACK*     _pcbOldImageWin = NULL;
static  int 						ImageNum				=	0;

static void Image_Display(int sel_num,WM_HWIN hParent);



/**
  * @brief  _GetData���ڶ�ȡ��洢��ͼ��ĺ��� ������bmp jpg gif�ļ�
  * @param   p           - Pointer to application defined data.
	*					 NumBytesReq - Number of bytes requested.	
	*  				 ppData      - Pointer to data pointer. This pointer should be set to
	*                 				a valid location.
	*  				 StartOfFile - If this flag is 1, the data pointer should be set to the
	*                 				beginning of the data stream.

  * @retval ��ȡ�����ֽ���
  */	
	
static int _GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  UINT NumBytesRead;
  FIL * phFile;
	FRESULT fres;		

  phFile = (FIL *)p;
	
	
  /*
  * Check buffer size
  */
  if (NumBytesReq > sizeof(_acBuffer)) {
    NumBytesReq = sizeof(_acBuffer);
  }

  /*
  * Set file pointer to the required position
  */ 
	fres =  f_lseek (phFile, Off);
    
    if(fres != FR_OK)
      return 0;

	/*
  * Read data into buffer
  */	
	#if 0	/* TBD ֱ�Ӷ�ȡ���ݣ���32��24λͼƬʱ������ */
		fres =  f_read(phFile, _acBuffer, NumBytesReq, &NumBytesRead);
	#else
		/* ��֪Ϊ�Σ���ȡ24��32λͼ��ʱ��
		*	ÿ�����󳬹�512�ֽڵ�ʱ�������ֿ����ζ�ȡû���⣬
		*	�����ǵײ�������,��������������ļ�ϵͳ��û���� ������Ҳ�ǵ�ַ�ռ���������*/
	 if(NumBytesReq>512)		
	 {	 
		 unsigned char  i;
		 unsigned int		read_count = 0;
		 unsigned char  count = NumBytesReq/512;		//����ж��ٸ����ݿ�
		 unsigned int   remain = NumBytesReq%512;		//����һ����ֽ���Ŀ
		 
		 for(i=0;i<count;i++)
		 {
			 /* ��ȡ512�ֽ� */
			fres = f_read(phFile, _acBuffer + i*512, 512, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 if(remain)                                 // ��ȡʣ���ֽ�
		 {
			fres = f_read(phFile, _acBuffer + i*512, remain, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 
		NumBytesRead =  read_count;
	 }
	 else	/* С��512�ֽڵ���� */
		fres =  f_read(phFile, _acBuffer, NumBytesReq, &NumBytesRead); 

 #endif

  /*
  * Set data pointer to the beginning of the buffer
  */
  *ppData = (const U8 *)_acBuffer;
	
  /*
  * Return number of available bytes
  */
  return NumBytesRead;
}


/**
  * @brief  _GetPngData���ڶ�ȡ��洢��ͼ��ĺ��� ������gng�ļ�
  * @param  
	*		
  * @retval ��
  */
static int _GetPNGData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  UINT NumBytesRead;
  FIL * phFile;
	FRESULT fres;
  U8 * pData;

  pData  = (U8 *)*ppData;
  phFile = (FIL *)p;
  //
  // Set file pointer to the required position
  //
  fres = f_lseek(phFile, Off);
  
    if(fres != FR_OK)
      return 0;
  //
  // Read data into buffer

	#if 0	/* TBD ֱ�Ӷ�ȡ���ݣ���32��24λͼƬʱ������ */
		fres =  f_read(phFile, pData, NumBytesReq, &NumBytesRead);
	#else
		/* ��֪Ϊ�Σ���ȡ24��32λͼ��ʱ��
		*	ÿ�����󳬹�512�ֽڵ�ʱ�������ֿ����ζ�ȡû���⣬
		*	�����ǵײ�������,��������������ļ�ϵͳ��û���� ������Ҳ�ǵ�ַ�ռ���������*/
	 if(NumBytesReq>512)		
	 {	 
		 unsigned char  i;
		 unsigned int		read_count = 0;
		 unsigned char  count = NumBytesReq/512;		//����ж��ٸ����ݿ�
		 unsigned int   remain = NumBytesReq%512;		//����һ����ֽ���Ŀ
		 
		 for(i=0;i<count;i++)
		 {
			 /* ��ȡ512�ֽ� */
			fres = f_read(phFile, pData + i*512, 512, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 if(remain)                                 //��ȡʣ���ֽ� 
		 {
			fres = f_read(phFile, pData + i*512, remain, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 
		NumBytesRead =  read_count;
	 }
	 else	/* С��512�ֽڵ���� */
		fres =  f_read(phFile, pData, NumBytesReq, &NumBytesRead); 

 #endif
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}



/**
  * @brief  _cbImageWin framewin��ܵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbImageWin(WM_MESSAGE * pMsg)
{
	HANDLE_LIST *appNode;
	
		switch (pMsg->MsgId) {			//��Ϣ����

#if 1			//��֣�����Ҫ�������
		case WM_NOTIFY_PARENT:		//֪ͨ		
			break;//֪ͨ����break
#endif	
			
			case WM_DELETE:
				
				/* ��ȡapp�����Ӧ�������� */
				appNode = hAPPLinkedList_GetAppNode(pMsg->hWin);
				if(appNode != NULL)
				{
					/* ɾ��app���������ļ�¼ */	
					hAPPLinkedList_Del(appNode);
				
					/* ������Ϣ֪ͨctrl����*/		
					WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	
				}
			
				break;
		
			default:		
      WM_DefaultProc(pMsg);
			
		}//MsgId
	
	if (_pcbOldImageWin) {
			(*_pcbOldImageWin)(pMsg);	//���þɵĻص�����������δ�������
		}	
	
}



/**
  * @brief  GUI_JPEG_GetXSizeEx JPEG��ʽͼƬ�Ļ�ȡX��С����
  * @param  
	*		
  * @retval ��
  */
static int GUI_JPEG_GetXSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p)
{
	GUI_JPEG_INFO JPEGInfo;
	
  GUI_JPEG_GetInfoEx(_GetData, p, &JPEGInfo);  /* ��ȡJPEGͼƬ����Ϣ */
	
	return JPEGInfo.XSize;                       //����XSize 
}

/**
  * @brief  GUI_JPEG_GetYSizeEx JPEG��ʽͼƬ�Ļ�ȡY��С����
  * @param  
	*		
  * @retval ��
  */
static int GUI_JPEG_GetYSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p)
{
	GUI_JPEG_INFO JPEGInfo;
	
  GUI_JPEG_GetInfoEx(_GetData, p, &JPEGInfo); /* ��ȡJPEGͼƬ����Ϣ */                   
	
	return JPEGInfo.YSize;                      //����YSize
}


/**
  * @brief  GUI_PNG_DrawScaledEx ��������ʾPNGͼƬ������PNGû�а�������ʾͼƬ�ĺ�����������ֱ����ʾ��
  * @param  
	*		
  * @retval ��
  */
static int GUI_PNG_DrawScaledEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom)
{
	return	GUI_PNG_DrawEx( pfGetData,  p, 0, 0);  

}

/**
  * @brief  GUI_GIF_ActiveDrawEx ֱ����ʾGIF��̬ͼƬ
  * @param  
	*		
  * @retval ��
  */
static int GUI_GIF_ActiveDrawEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0)
{
	int j;
	GUI_GIF_INFO       GifInfo   = {0}; /* Info structure of GIF file */
  GUI_GIF_IMAGE_INFO ImageInfo = {0}; /* Info structure of one particular GIF image of the GIF file */

	
	GUI_GIF_GetInfoEx(pfGetData, p, &GifInfo);                          /* ��ȡGIFͼ����Ϣ */

	for (j = 0; j < GifInfo.NumImages; j++) 
			{
					GUI_GIF_DrawSubEx(pfGetData,p, x0, y0,j);                   /* ��ʾ��ͼ�� */
					GUI_GIF_GetImageInfoEx(pfGetData, p, &ImageInfo, j);        /* ��ȡ��ͼ����Ϣ */
					GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* ����ͼ����Ϣ��ʱһ��ʱ�� */
				}		

  return 0;      
}


/**
  * @brief  GUI_GIF_ActiveDrawScaledEx ��������ʾGIF��̬ͼƬ
  * @param  
	*		
  * @retval ��
  */
static int GUI_GIF_ActiveDrawScaledEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom)
{
	int j;
	GUI_GIF_INFO       GifInfo   = {0}; /* Info structure of GIF file */
  GUI_GIF_IMAGE_INFO ImageInfo = {0}; /* Info structure of one particular GIF image of the GIF file */

	
	GUI_GIF_GetInfoEx(pfGetData, p, &GifInfo);                            /* ��ȡGIFͼ����Ϣ */

	for (j = 0; j < GifInfo.NumImages; j++) 
			{
					GUI_GIF_DrawSubScaledEx(pfGetData,p, x0, y0,j,Num,Denom);     /* ��������ʾ��ͼ�� */
					GUI_GIF_GetImageInfoEx(pfGetData, p, &ImageInfo, j);          /* ��ȡ��ͼ����Ϣ */
					GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);      /* ����ͼ����Ϣ��ʱһ��ʱ�� */
				}	

  return 0;      
        

}


/**
  * @brief  ImageReader_Raw �ڸ����ĸ�������ʾͼƬ
  * @param  file_name ͼƬ�ļ��� ��hParent �����ھ��
	*		
  * @retval ��
  */
static void  ImageReader_Raw(char *file_name ,WM_HWIN hParent)
{
	FIL hFile ;
	FRESULT fres;
	
	IMAGE_FUN ImageFun;
	
  int XSize, YSize, XPos, YPos,XSizeWin,YSizeWin;

	/* �����ػ洰�ڣ�����ͼƬ���ڴ����ػ�ʱ��ˢ�� */
	WM_PaintWindowAndDescs(hParent);

	/* ѡ���䴰�����ڻ��Ʋ��� */ 
	WM_SelectWindow(hParent);
	
  /* ��ȡ�����ڴ�С */
	XSizeWin = WM_GetWindowSizeX(hParent);
	YSizeWin = WM_GetWindowSizeY(hParent);
	
  /* ˢ�¸����� */
	GUI_ClearRect(0, 0,XSizeWin, YSizeWin);	
	
  /* �����ļ�����ͼƬ����ָ����и�ֵ */
	if(strstr(file_name,".bmp")||strstr(file_name,".BMP"))
	{
	
				ImageFun.GetXSizeEx 	= GUI_BMP_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_BMP_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_BMP_DrawEx;
				ImageFun.DrawScaledEx = GUI_BMP_DrawScaledEx;
				ImageFun.GetDataEx		= _GetData;
		
	}
	else if(strstr(file_name,".jpg")||strstr(file_name,".JPG"))
	{
		
				ImageFun.GetXSizeEx 	= GUI_JPEG_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_JPEG_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_JPEG_DrawEx;
				ImageFun.DrawScaledEx = GUI_JPEG_DrawScaledEx;
				ImageFun.GetDataEx		= _GetData;
		
	}
	else if(strstr(file_name,".gif")||strstr(file_name,".GIF"))
	{
				ImageFun.GetXSizeEx 	= GUI_GIF_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_GIF_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_GIF_ActiveDrawEx;
				ImageFun.DrawScaledEx = GUI_GIF_ActiveDrawScaledEx;
				ImageFun.GetDataEx		= _GetData;
	
	}
	else if(strstr(file_name,".png")||strstr(file_name,".PNG"))
	{
			
				ImageFun.GetXSizeEx 	= GUI_PNG_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_PNG_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_PNG_DrawEx;
				ImageFun.DrawScaledEx = GUI_PNG_DrawScaledEx;								//PNGû��scaled������
				ImageFun.GetDataEx		= _GetPNGData;
	}
	else 
		return; //��֧�ֵĸ�ʽ����������	
	
  /* ��ͼƬ�ļ� */
	fres = f_open(&hFile,file_name,  FA_READ|FA_OPEN_EXISTING| FA_OPEN_ALWAYS );
    if(fres != FR_OK)
       return ; 
  
	DEBUG("\r\n open extsting fres = %d",fres); 	
	
  /* ��ȡͼƬ��С */
	XSize = ImageFun.GetXSizeEx(ImageFun.GetDataEx, &hFile);
  YSize = ImageFun.GetYSizeEx(ImageFun.GetDataEx, &hFile);
	
	DEBUG("\r\nXSize = %d ,YSize= %d ",XSize,YSize); 	

#if DRAW_SCALE		//���ձ�������ͼƬ��ʹͼƬ����������Ļ��ʾ
	if((XSize > XSizeWin) || (YSize > YSizeWin))	
	{		
		/* ���ݴ��ڰ���������ͼƬ */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* ��������ʾͼƬ */
			//while (WM_IsWindow(hParent)) 
			{
				ImageFun.DrawScaledEx(ImageFun.GetDataEx,&hFile,0,YPos,XSizeWin,XSize);
				GUI_Delay(10);	
				}	
			}
		else
		{
			
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* ��������ʾͼƬ */
			//while (WM_IsWindow(hParent)) 
			{
				ImageFun.DrawScaledEx(ImageFun.GetDataEx,&hFile,XPos,0,YSizeWin,YSize);	
				GUI_Delay(10);
			}		
		}			
	}
	else
	{
		/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* ��ԭ������ʾͼƬ*/
		//while (WM_IsWindow(hParent) ) 
		{
			ImageFun.DrawEx(ImageFun.GetDataEx, &hFile, XPos, YPos);
			GUI_Delay(10);
		}			
	}
	
#else
	
			/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* ��ԭ������ʾͼƬ*/
		//ֱ����ʾ���ͻ�����
		//while (WM_IsWindow(hParent)) 
		{		
			ImageFun.DrawEx(ImageFun.GetDataEx, &hFile, XPos, YPos); 
			GUI_Delay(10);	
		}			

#endif	
	
  f_close(&hFile);
	
}


/**
  * @brief  ImageReader_FrameWin ʹ��framewin��ʾͼƬ
  * @param  file_name ͼƬ�ļ���
	*		
  * @retval ��
  */
void ImageReader_FrameWin( char * file_name) {
	
	WM_HWIN hFrameC;
	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();

	
	hFrame->hAPP = FRAMEWIN_CreateEx(	0,
															0,
															WinPara.xSizeWin,
															WinPara.ySizeWin,
															WinPara.hWinMain,
															WM_CF_SHOW,
															FRAMEWIN_CF_ACTIVE/*|FRAMEWIN_CF_MOVEABLE*/,
															GUI_ID_FRAMEWIN2,
															"Image Reader",
															0);
	
		/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);


	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hFrame);
	
  /* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	
	
	_pcbOldImageWin = WM_SetCallback(hFrame->hAPP, _cbImageWin );	//��ȡ�ɵĻص�����ָ��
	
	FRAMEWIN_SetText(hFrame->hAPP,file_name);
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	
	/* �ڿͻ�������ʾͼƬ */
	ImageReader_Raw(file_name,hFrameC);

}





/**
  * @brief  _cbImageAPPWin,ͼƬAPP�ص�����
  * @param  none
  * @retval none
  */
static void _cbImageAPPWin(WM_MESSAGE * pMsg)
{
	WM_HWIN   hWin;
	WM_HWIN		hButton;
	WM_HWIN 	hText ;
	HANDLE_LIST *appNode;

	
	int        NCode;
  int        Id;	
	
	static int ImageIndex = 0;
	
	hWin = pMsg->hWin;	

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:	
			
      /* �������� */
			hButton = BUTTON_CreateEx(WinPara.xSizeWin-40,WinPara.ySizeWin-20,20, 20, 
																		pMsg->hWin, WM_CF_SHOW|WM_CF_STAYONTOP,0, GUI_ID_BUTTON0);
		
			BUTTON_SetText(hButton, "<");
		
		
			hButton = BUTTON_CreateEx(WinPara.xSizeWin-20,WinPara.ySizeWin-20,20, 20, 
																		pMsg->hWin, WM_CF_SHOW|WM_CF_STAYONTOP,0, GUI_ID_BUTTON1);
		
			BUTTON_SetText(hButton, ">");
		
			/* ��ʾ�ļ�·�� */
			hText = TEXT_CreateEx(0,WinPara.ySizeWin-20,200,20,hWin,WM_CF_SHOW|WM_CF_HASTRANS,TEXT_CF_HCENTER|TEXT_CF_VCENTER,GUI_ID_TEXT0,NULL);
	
			TEXT_SetFont(hText,&GUI_Font16_ASCII);
			TEXT_SetTextColor(hText,GUI_WHITE);			
		
			break;			
		
		case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
				NCode = pMsg->Data.v;                 // ֪ͨ����
			 
			 if(NCode == WM_NOTIFICATION_RELEASED)
				 {
				 
					 if(Id == GUI_ID_BUTTON0)           //��һ��ͼƬ��ť
					 {					 
						 if(ImageIndex != 0)
						{					
                /* ��ʾ��һ��ͼƬ */
							 Image_Display(--ImageIndex,hWin);
						 }
            else
            {
              /* ��ʾԭ����ͼƬ */
              Image_Display(ImageIndex,hWin);

            }
						 					
					 } 
					 else if(Id == GUI_ID_BUTTON1)      //��һ��ͼƬ��ť
					 {						 
							if(ImageIndex<ImageNum )
							{
								/* ��ʾ��һ��ͼƬ */
								Image_Display(++ImageIndex,hWin);
							}
              else
              {
                /* ��ʾԭ����ͼƬ */
								Image_Display(ImageIndex,hWin);
  
              }
						}
						 
				 }
			
			
			
		break;
			
		
		
		case WM_DELETE:			
				
			/* ��ȡapp�����Ӧ�������� */
			appNode = hAPPLinkedList_GetAppNode(pMsg->hWin);
			if(appNode != NULL)
			{
				/* ɾ��app���������ļ�¼ */	
				hAPPLinkedList_Del(appNode);
			
				/* ������Ϣ֪ͨctrl����*/		
				WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	
			}
			break;
		
		case WM_PAINT:
			
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin),WM_GetWindowSizeY(pMsg->hWin));
		
			break;
		
		default:
			break;
		
	}
	
	
}



/**
  * @brief  Image_Display ��ʾͼƬ
	*					
  * @param  none
  * @retval none
  */
static void Image_Display(int sel_num ,WM_HWIN hParent)
{
		WM_HWIN hText;
	
		char* file_name;
		FIL hFile;
		FRESULT fres;							//���ؽ��		
		unsigned int rw_num;			//�Ѷ�����д���ֽ���

		//���ļ�
		
		file_name 	= (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //Ϊ�洢Ŀ¼����ָ�����ռ�
		
		fres = f_open (&hFile, IMAGE_LIST_PATH, FA_READ ); 		        //�򿪴��������ļ�
      
      if(fres != FR_OK)
        return ;
  
		fres = f_lseek (&hFile, sel_num*FILE_NAME_LEN);				        //��������ֵ���ҽ�Ҫ���ļ���·��
		fres = f_read(&hFile, file_name, FILE_NAME_LEN, &rw_num);
		fres = f_close (&hFile);	
		
		/* ��ʾͼƬ·�� */
		hText = WM_GetDialogItem(hParent,GUI_ID_TEXT0);	
		TEXT_SetText(hText,file_name);

		DEBUG("\r\n filename = %s\r\n",file_name);
	

		/* ��ʾͼƬ������ */
		ImageReader_Raw(file_name,hParent);	
	
		free(file_name);								                               //�ͷ�malloc�ռ�		
		file_name = NULL;

}




/**
  * @brief  WFGUI_ImageReader ͼƬ�����������
	*					
  * @param  none
  * @retval none
  */
void WFGUI_ImageReader(void)
{
	
	//WM_HWIN hImage;	
	HANDLE_LIST *hImage = hAPPLinkedList_NewNode();

	
	/* ����ͼƬ��������� */
	hImage->hAPP = WM_CreateWindowAsChild(0,
																	0,
																	WinPara.xSizeWin,
																	WinPara.ySizeWin ,
																	WinPara.hWinMain ,
																	WM_CF_SHOW , 
																	_cbImageAPPWin,
																	0);	
	
	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hImage);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	

	/* ɨ��ͼƬ�ļ� */
	Fill_FileList("0:",IMAGE_LIST_PATH,NULL,NULL,IMAGEFILE,&ImageNum);
	GUI_Delay(10);	

	/* ��ʾ��һ��ͼ�� */
	Image_Display(0,hImage->hAPP);


}




