/**********************************************************
  
   *��    ���������������
   *��    �ã��ṩ�̵߳�������ӿ�
   *ע�������

**********************************************************/

#ifndef _ITASKPROCESS_H_
#define _ITASKPROCESS_H_

class ITaskProcess
{
public:
	virtual ~ITaskProcess(){}

	//������ӿ�
	virtual void TaskPorcess(void *pTask) = 0;
};

#endif