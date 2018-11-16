#ifndef __WFGUI_SDVIEW_H
#define __WFGUI_SDVIEW_H

#include "ICONVIEW.h"
#include "TREEVIEW.h"
#include "WFGUI_Common.h"


/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//extern int xSize,ySize; 

void 		WFGUI_SDView				(void);
void 		Fill_TreeView				(FILE_TYPE fileType,char* record_file );


void 		Fill_FileList				(char* path,char* record_file,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,FILE_TYPE fileType,int *fileNum);




#endif
