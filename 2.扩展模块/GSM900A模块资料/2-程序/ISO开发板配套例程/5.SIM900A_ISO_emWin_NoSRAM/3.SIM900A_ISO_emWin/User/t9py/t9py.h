#ifndef  __t9py_h
#define  __t9py_h

#include "stm32f10x.h"


#define  UTF8_CODE  1

#define   T9BD        0x00	  //�������
#define   T9DX        0x01	  //��дӢ��
#define   T9XX        0x02	  //СдӢ��
#define   T9PY        0x03	  //T9ƴ������
#define   T9SZ        0x04	  //T9��������

#define   T9NULL      0x00    //��ѯ���Ϊ��



#define   SRF_X       0x00    // ��ʾ���뷨��X����
#define   SRF_Y       0X00    // ��ʾ���뷨��Y����
#define   STR_X       0X00    // ���������׵�ַX
#define   STR_Y       16	  // ���������׵�ַY
#define   T9PY_X      0		  // ƴ����ʾ�׵�ַX
#define   T9PY_Y      223	  // ƴ����ʾ�׵�ַY
#define   INDEX_X     70	  // ������ʾ�׵�ַX
#define   INDEX_Y     223	  // ������ʾ�׵�ַY




extern  struct t9py_struct t9pyfun;
extern const struct t9PY_index  t9PY_sz[];

extern void clrindex(void);

extern  void py_index_sub(void);            //���뷨��������

extern  void dispsf(void);                  //��ʾ���������

extern  void T9pyTask(unsigned int key);



#endif    //end of __t9py_h

