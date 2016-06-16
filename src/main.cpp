#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "TaskQueueManager.h"
#include "ThreadPoolManager.h"

//�����̵߳���������
void TaskProcessFunc(void *);

int main()
{
	//��ʼ�ڴ�״̬��¼
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);

	//��ȡ������й���������
	TaskQueueManager *pTaskQueueManager = TaskQueueManager::GetInstance();

	//��ȡ�̳߳ع���������
	ThreadPoolManager *pThreadPoolManager = ThreadPoolManager::GetInstance();

	//����������������ִ��������
	for (int i=0; i<1024; ++i)
	{
		//����������������
		pTaskQueueManager->Lock();
		pTaskQueueManager->PushBack(TaskProcessFunc, NULL);
		pTaskQueueManager->Unlock();

		//ִ��������
		pThreadPoolManager->ExecuteTaskProcess();
	}

	getchar();

	pThreadPoolManager->SetThreadCount(1024);
	cout << "Current count of worker threads in thread-pool is: " 
		 <<  pThreadPoolManager->GetThreadCount() << endl;

	//�����̳߳ع�����
	delete pThreadPoolManager;

	//�����ڴ�״̬��¼
	_CrtMemCheckpoint(&s2);
	//�ڴ�й¶���
	if (0 != _CrtMemDifference(&s3, &s1, &s2))
	{
		_CrtMemDumpStatistics(&s3);
	}

	return 0;
}

void TaskProcessFunc(void *)
{
	cout << "Worker thread: " << GetCurrentThreadId() 
		 << " is running"     << endl;

	//ģ���㷨����
	Sleep(50);
}