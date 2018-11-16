/**
  ******************************************************************************
  * @file    WFGUI_APPLinkedList.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   Ӧ�þ�������������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */


#include "WFGUI_Common.h"
#include "WFGUI_LinkedList.h"

/* ��Ҫ����Linux��������Ļ����������malloc�������� ��free���������ͷ� */

static  HANDLE_LIST hAppHead;				//����ͷ��

/**
  * @brief  hAPPLinkedlist_NewNodeΪ����һ���µ�hAPP��㣬����ռ�
  * @param  none
  * @retval ָ���½���ָ��
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
  * @brief  hAPPLinkedlist_Init��ʼ��hAPP����
  * @param  none
  * @retval none
  */
 void hAPPLinkedList_Init(void)
{
	
	INIT_LIST_HEAD(&hAppHead.listNode );

}

/**
  * @brief  hAPPLinkedList_Delɾ�������еĽ�㣬���ͷ�malloc�ռ�
  * @param  none
  * @retval none
  */
void hAPPLinkedList_Del(HANDLE_LIST *node)
{
	/* TBD node���ͷ�ʱû�б�����ΪNULL */
	if (node == NULL || (node->listNode.next == NULL && node->listNode.prev == NULL))
	 {
		 DEBUG("\r\n hAPP linkedList free error\r\n ");
		return ;
	 }
	 
	 /* ɾ����������ļ�¼ */
	 list_del(&node->listNode);
	 
	 /* �ͷŽ���malloc�ռ� */
	 free(node);
	 	 
}


/**
  * @brief hAPPLinkedList_AddTail���ӽ�㵽����
  * @param  none
  * @retval none
  */
void hAPPLinkedList_AddTail(HANDLE_LIST *node)
{
		list_add_tail(&node->listNode,&hAppHead.listNode);
}


/**
  * @brief  hAPPLinkedList_GetAppNode����APP�����ȡ������
  * @param  none
  * @retval app�������û�еĻ�����NULL
  */
HANDLE_LIST* hAPPLinkedList_GetAppNode(WM_HWIN hAPP)
{
	struct list_head *pos;	
	HANDLE_LIST *node;
	
	list_for_each(pos,&hAppHead.listNode)
	{
		/* TBD �����и�warning����֪��Ӱ��󲻴�*/		
		node = list_entry(pos,HANDLE_LIST,listNode);
		
		DEBUG("\r\n node.app =%ld \r\n ",node->hAPP);
		
		/* ����������app�����ͬ�Ľ�� */
		if(node->hAPP == hAPP)
		{
				return node;
		}
		
	}
	
	/* �Ҳ���������app�����ͬ�Ľ�� */
	if(pos == &hAppHead.listNode)
		return NULL;
	
	return NULL;
	
}


/**
  * @brief  hAPPLinkedList_GetAppTop��ȡ��ʾ�����ϲ�Ĵ���(���������һ�����)
  * @param  none
  * @retval app�������û�еĻ�����NULL
  */
HANDLE_LIST* hAPPLinkedList_GetAppTop(void)
{	
	HANDLE_LIST *node;
	
	if(!list_empty(&hAppHead.listNode))					//������ǿ�
	{

		/* hAppHead.listNode.prev ͷ����ǰһ����㣬��β�����*/
		node = list_entry(hAppHead.listNode.prev,HANDLE_LIST,listNode);
		
		DEBUG("\r\n node.app =%ld \r\n ",node->hAPP);

		return node;

	}		
	else
		return NULL;
	
}









