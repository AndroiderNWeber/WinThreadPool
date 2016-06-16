/**********************************************************

   *��    �����̳߳ع�����
   *��    �ã�ʵ�ֶԹ����̵߳�ͳһ����
   *ע�����1.��������ͨ������ģʽʵ��
              2.���̳߳ؿɿ����ʹ��
              3.��ʼ����С��������߳����ֱ�Ϊ512��1��2048

**********************************************************/

#ifndef _THREADPOOLMANAGER_H_
#define _THREADPOOLMANAGER_H_

#include <vector>
#include <assert.h>
#include <iostream>
#include "TaskQueueManager.h"
#include "Thread.h"

using namespace std;

class ThreadPoolManager
{
public:
	//�����̳߳ع�������Ψһ����
	static ThreadPoolManager *GetInstance();

	~ThreadPoolManager();

	//ִ��������
	void ExecuteTaskProcess();

	//�����̳߳��еĹ����߳���
	void SetThreadCount(const size_t &zThreadCount);

	//��ȡ�̳߳��еĹ����߳���
	size_t GetThreadCount() const;

	//��ȡ�̳߳��еĻ�Ծ�����߳���
	size_t GetActiveThreadCount() const;

private:
	ThreadPoolManager();

	//�̹߳�������
	static void ThreadWorkFunc(void *pParam);

private:
	//�̳߳ع���������
	static ThreadPoolManager *ms_pInstance;

	//������е���
	TaskQueueManager *m_pTaskQueueManager;

	//�̳߳ؼ��以����
	vector<IThread *> m_ThreadPool;
	HANDLE m_mutexThreadPool;

	//�̳߳��еĹ����߳���
	size_t m_zThreadCount;

	//�̳߳��еĻ�Ծ�����߳���
	size_t m_zActiveThreadCount;

	//ִ���������ź����ź�
	HANDLE m_semExecuteTaskProcess;

	//ɾ�������߳��ź����ź�
	HANDLE m_semDeleteThread;

	//�����߳�ɾ������¼��ź�
	HANDLE m_evtDeleteThreadComplete;

	//ж���̳߳��¼��ź�
	HANDLE m_evtUninitPool;

	//�̳߳�ж������¼��ź�
	HANDLE m_evtUninitPoolComplete;

	//�̳߳صĳ�ʼ�����߳���
	static const int INITIAL_THREAD_COUNT = 512;

	//�̳߳ص���С�����߳���
	static const int MIN_THREAD_COUNT = 1;

	//�̳߳ص�������߳���
	static const int MAX_THREAD_COUNT = 2048;
};

#endif