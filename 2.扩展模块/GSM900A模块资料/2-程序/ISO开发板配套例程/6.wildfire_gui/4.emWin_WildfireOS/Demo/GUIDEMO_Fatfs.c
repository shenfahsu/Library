#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart1.h"	
#include "ff.h"
#include "GUI.h"
#include "DIALOG.h"

FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[4096]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "Welcome to use Wildfire iso stm32 Development Board today is a good day";



void Fatfs_MainTask(void)
{

	  /* ������ʾ�ļ����ݵĴ��ڴ�С */ 
		GUI_RECT file_content={5,120,235,200};

		/* ����Ļˢ�ϱ�����ɫ */
		GUI_ClearRect(0,0,240,320);	
	  
		/* ����������ɫ */
		GUI_SetFont(GUI_FONT_8X16);

		GUI_DispStringAt ("f_mount...",5,20);
	
		/* Register work area for each volume (Always succeeds regardless of disk status) */
		f_mount(0,&fs);	

		/* function disk_initialize() has been called in f_open */
		
	  GUI_DispStringAt ("f_open :newfile.txt ...",5,40);
	
		/* Create new file on the drive 0 */
		res = f_open(&fnew, "0:newfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
		 
		if ( res == FR_OK )
		{
			res = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
			f_close(&fnew);      
		}

		GUI_DispStringAt ("f_read :newfile.txt ...",5,60);
		
		res = f_open(&fnew, "0:newfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
		res = f_read(&fnew, buffer, sizeof(buffer), &br); 

		printf("\r\n %s ", buffer);
		
		GUI_DispStringAt ("file content:",5,100);
		
		/* ����ǰ����ɫ��������ɫ��*/
		GUI_SetColor(GUI_RED);
		
		/* �������� */
		GUI_SetFont(GUI_FONT_COMIC18B_ASCII);
		
		/* ��ʾ�ı�����Ļ�� */
		GUI_DispStringInRectWrap((const char *)buffer,&file_content,GUI_TA_LEFT,GUI_WRAPMODE_WORD) ;
		
		/* Close open files */
		f_close(&fnew);	                                      
		 
    /* Unregister work area prior to discard it */
    f_mount(0, NULL);
		
		//while(1);
}

	  

