/**********************************************************
  
   *��    ����������й�������
   *��    �ã�Ϊ�̳߳��ṩ��������Լ�����Ԫ
   *ע����������������ʱ��ʹ��Lock()��Unlock()������֤�̰߳�ȫ

**********************************************************/

#ifndef _TASKQUEUE_H_
#define _TASKQUEUE_H_

#include <Windows.h>
#include <deque>
#include "ITaskProcess.h"

using namespace std;

/** ����Ԫ **/
struct TaskItem
{
	TaskItem(void (*pTaskProcessFunc)(void *), void *pTask = NULL)
	{
		m_pTaskProcessFunc = pTaskProcessFunc;
		m_pTask = pTask;
	}

	//��������
	void (*m_pTaskProcessFunc)(void *);
	
	//��������
	void *m_pTask;
};

/** ������й����� **/
class TaskQueueManager
{
public:
	//����������й�������Ψһ����
	static TaskQueueManager *GetInstance();

	~TaskQueueManager();

	//��ס�������
	void Lock();

	//�����������
	void Unlock();

	//������������ָ��������
	void PushBack(void (*pTaskProcessFunc)(void *), void *pTask = NULL);
	//�����������Ӷ���������
	void PushBack(ITaskProcess *pTaskProcess, void *pTask = NULL);

	//ȡ�����������˵�����
	TaskItem *Front();

	//Pop�����������˵�����
	void PopFront();

	//��ȡ������д�С
	size_t Size();

	//��������Ƿ�Ϊ��
	bool IsEmpty();

private:
	TaskQueueManager();

	//ת�����������������Ԫ����
	static void TaskConvertFunc(void *pParam);

private:
	//������й���������
	static TaskQueueManager *ms_pInstance;

	//������м��以����
	deque<TaskItem *> m_TaskQueue;
	HANDLE m_mutexTaskQueue;

	//����������Ԫ
	struct ObjTaskItem
	{
		ObjTaskItem(ITaskProcess *pTaskProcess, void *pTask = NULL)
		{
			m_pTaskProcess = pTaskProcess;
			m_pTask = pTask;
		}

		//���������
		ITaskProcess *m_pTaskProcess;

		//��������
		void *m_pTask;
	};
};

#endif