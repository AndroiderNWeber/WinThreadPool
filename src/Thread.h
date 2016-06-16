/**********************************************************

   *��    ���������߳��� ������IThread
   *��    �ã����ܲ�ִ���û�����
   *ע������ɴ�ָ��������Ͷ�������������ѡһ����Ϊ��������

**********************************************************/

#ifndef _THREAD_H_
#define _THREAD_H_

#include "IThread.h"
#include "ITaskProcess.h"

class Thread : public IThread
{
public:
	//����ָ��������
	Thread(void (*pTaskProcessFunc)(void *), void *pTask = NULL);
	//���ܶ���������
    Thread(ITaskProcess *pTaskProcess, void *pTask = NULL);

protected:
	//ʵ��IThread��Run����
	virtual void Run();

private:
	//���������жϱ�ʶ
	bool m_bIsObjTask;

	//����������Ԫ
	struct ObjTaskItem
	{
		//���������
		ITaskProcess *m_pTaskProcess;

		//��������
		void *m_pTask;

	} m_objTaskItem;

	//ָ��������Ԫ
	struct PtrTaskItem
	{
		//��������
		void (*m_pTaskProcessFunc)(void *);

		//��������
		void *m_pTask; 

	} m_ptrTaskItem;
};

#endif