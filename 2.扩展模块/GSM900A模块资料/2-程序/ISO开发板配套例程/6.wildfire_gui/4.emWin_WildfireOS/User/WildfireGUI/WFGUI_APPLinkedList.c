/**
  ******************************************************************************
  * @file    WFGUI_APPLinkedList.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   应用句柄链表操作函数
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */


#include "WFGUI_Common.h"
#include "WFGUI_LinkedList.h"

/* 主要是在Linux链表操作的基础上添加了malloc分配数据 和free处理数据释放 */

static  HANDLE_LIST hAppHead;				//链表头部

/**
  * @brief  hAPPLinkedlist_NewNode为创建一个新的hAPP结点，分配空间
  * @param  none
  * @retval 指向新结点的指针
  */
 HANDLE_LIST* hAPPLinkedList_NewNode(void)
{
	HANDLE_LIST *app = (HANDLE_LIST *)malloc(sizeof(HANDLE_LIST));
	
	if (app == NULL)
	 {
		 DEBUG("\r\n hAPP linkedList malloc error\r\n ");
		return NULL;
	 }
	 
	 return app;
	 
}

/**
  * @brief  hAPPLinkedlist_Init初始化hAPP链表
  * @param  none
  * @retval none
  */
 void hAPPLinkedList_Init(void)
{
	
	INIT_LIST_HEAD(&hAppHead.listNode );

}

/**
  * @brief  hAPPLinkedList_Del删除链表中的结点，并释放malloc空间
  * @param  none
  * @retval none
  */
void hAPPLinkedList_Del(HANDLE_LIST *node)
{
	/* TBD node在释放时没有被设置为NULL */
	if (node == NULL || (node->listNode.next == NULL && node->listNode.prev == NULL))
	 {
		 DEBUG("\r\n hAPP linkedList free error\r\n ");
		return ;
	 }
	 
	 /* 删除在链表里的记录 */
	 list_del(&node->listNode);
	 
	 /* 释放结点的malloc空间 */
	 free(node);
	 	 
}


/**
  * @brief hAPPLinkedList_AddTail增加结点到链表
  * @param  none
  * @retval none
  */
void hAPPLinkedList_AddTail(HANDLE_LIST *node)
{
		list_add_tail(&node->listNode,&hAppHead.listNode);
}


/**
  * @brief  hAPPLinkedList_GetAppNode根据APP句柄获取链表结点
  * @param  none
  * @retval app结点句柄，没有的话返回NULL
  */
HANDLE_LIST* hAPPLinkedList_GetAppNode(WM_HWIN hAPP)
{
	struct list_head *pos;	
	HANDLE_LIST *node;
	
	list_for_each(pos,&hAppHead.listNode)
	{
		/* TBD 这里有个warning，不知道影响大不大*/		
		node = list_entry(pos,HANDLE_LIST,listNode);
		
		DEBUG("\r\n node.app =%ld \r\n ",node->hAPP);
		
		/* 返回与输入app句柄相同的结点 */
		if(node->hAPP == hAPP)
		{
				return node;
		}
		
	}
	
	/* 找不到与输入app句柄相同的结点 */
	if(pos == &hAppHead.listNode)
		return NULL;
	
	return NULL;
	
}


/**
  * @brief  hAPPLinkedList_GetAppTop获取显示在最上层的窗口(即链表最后一个结点)
  * @param  none
  * @retval app结点句柄，没有的话返回NULL
  */
HANDLE_LIST* hAPPLinkedList_GetAppTop(void)
{	
	HANDLE_LIST *node;
	
	if(!list_empty(&hAppHead.listNode))					//若链表非空
	{

		/* hAppHead.listNode.prev 头结点的前一个结点，即尾部结点*/
		node = list_entry(hAppHead.listNode.prev,HANDLE_LIST,listNode);
		
		DEBUG("\r\n node.app =%ld \r\n ",node->hAPP);

		return node;

	}		
	else
		return NULL;
	
}









