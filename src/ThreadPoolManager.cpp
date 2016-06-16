#include "ThreadPoolManager.h"

ThreadPoolManager *ThreadPoolManager::ms_pInstance = NULL;

ThreadPoolManager *ThreadPoolManager::GetInstance()
{
	if (NULL == ms_pInstance)
	{
		ms_pInstance = new ThreadPoolManager;
	}

	return ms_pInstance;
}

ThreadPoolManager::ThreadPoolManager()
{
	/*
      * ��������CreateMutex
	  * ���ܣ�  ��������ʼ���������ں˶���
	  * ������  lpMutexAttributes   ��ȫ����ָ��        һ��ΪNULL
	  *         bInitialOwner       �Ƿ�Ϊ��������ӵ��  һ��ΪFALSE
	  *         lpName              �ں˶�������        ��ΪNULL
	  * ����ֵ���ɹ�ʱ���ػ������ں˶����� ʧ��ʱ����NULL
	*/
	m_mutexThreadPool = CreateMutex(NULL, FALSE, NULL);
	assert(NULL != m_mutexThreadPool);

	/*
	  * ��������CreateEvent
	  * ���ܣ�  ��������ʼ���¼��ں˶���
	  * ������  lpEventAttributes   ��ȫ����ָ��        һ��ΪNULL
	  *         bManualReset        TRUE�˹�����        FALSE�Զ�����
	  *         bInitialState       TRUE���ź�״̬      FALSE���ź�״̬
	  *         lpName              �ں˶�������        ��ΪNULL
	  * ����ֵ���ɹ�ʱ�����¼��ں˶����� ʧ��ʱ����NULL
	*/
	m_evtDeleteThreadComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(NULL != m_evtDeleteThreadComplete);

	m_evtUninitPool = CreateEvent(NULL, TRUE, FALSE, NULL);
	assert(NULL != m_evtUninitPool);

	m_evtUninitPoolComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(NULL != m_evtUninitPoolComplete);

	/*
	  * ��������CreateSemaphore
	  * ���ܣ�  ��������ʼ���ź����ں˶���
	  * ������  lpSemaphoreAttributes   ��ȫ����ָ��     һ��ΪNULL
	  *         lInitialCount           ��ʼ��Դ����
	  *         lMaximumCount           ���������Դ����
	  *         lpName                  �ں˶�������     ��ΪNULL
	  * ����ֵ���ɹ�ʱ�����ź����ں˶����� ʧ��ʱ����NULL
	*/
	m_semExecuteTaskProcess = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	assert(NULL != m_semExecuteTaskProcess);

	m_semDeleteThread = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	assert(NULL != m_semDeleteThread);

	//��ȡ������й���������
	m_pTaskQueueManager = TaskQueueManager::GetInstance();

	//���̳߳���� INITIAL_THREAD_COUNT �����Ĺ����߳�
	SetThreadCount(INITIAL_THREAD_COUNT);

	//��ʼ�������̼߳���
	m_zThreadCount = INITIAL_THREAD_COUNT;
}

ThreadPoolManager::~ThreadPoolManager()
{
	//�ȴ������е�����ȫ������
	while (true)
	{
		m_pTaskQueueManager->Lock();
		if (0 != m_pTaskQueueManager->Size())  //��������δ����
		{
			m_pTaskQueueManager->Unlock();

	        //����5�������ִ���ж�
			Sleep(5);
			continue;
		}
		else                                   //������ȫ��������
		{
			m_pTaskQueueManager->Unlock();
			break;
		}
	}

	//����ж���̳߳��¼��ź�
	SetEvent(m_evtUninitPool);

	//�ȴ��̳߳�ж������¼��ź�
	WaitForSingleObject(m_evtUninitPoolComplete, INFINITE);

	//�ͷ��ں˶��� ��ֹ�ں�й¶
	CloseHandle(m_mutexThreadPool);
	m_mutexThreadPool = NULL;

	CloseHandle(m_semExecuteTaskProcess);
	m_semExecuteTaskProcess = NULL;

	CloseHandle(m_semDeleteThread);
	m_semDeleteThread = NULL;

	CloseHandle(m_evtDeleteThreadComplete);
	m_evtDeleteThreadComplete = NULL;

	CloseHandle(m_evtUninitPool);
	m_evtUninitPool = NULL;

	CloseHandle(m_evtUninitPoolComplete);
	m_evtUninitPoolComplete = NULL;
	
	//����������й���������
	delete m_pTaskQueueManager;
}

void ThreadPoolManager::ExecuteTaskProcess()
{
	//�ͷ�1��ִ���������ź����ź�
	ReleaseSemaphore(m_semExecuteTaskProcess, 1, NULL);
}

void ThreadPoolManager::SetThreadCount(const size_t &zThreadCount)
{
	//��֤�̳߳��еĹ����߳�������С�߳���������߳���֮��
	if (MIN_THREAD_COUNT>zThreadCount || MAX_THREAD_COUNT<zThreadCount)
	{
		return;
	}

	//���ü��ٹ����߳�����ʶ
	bool bIsDecreaseThreadCount = false;

	WaitForSingleObject(m_mutexThreadPool, INFINITE);
	//���¹����̼߳���
	m_zThreadCount = zThreadCount;
	//��ǰ�����߳���
	size_t zCurrentThreadCount = m_ThreadPool.size();
	//��ǰ�����߳���С���趨�߳���
	if (zThreadCount > zCurrentThreadCount)
	{
		//���̳߳���� zThreadCountIncrement �����Ĺ����߳�
		size_t zThreadCountIncrement = zThreadCount - zCurrentThreadCount;
		IThread *pWorkerThread = NULL;
		for (size_t i=0; i<zThreadCountIncrement; ++i)
		{
			//���������������߳�
			pWorkerThread = new Thread(ThreadWorkFunc, (void *)this);
			pWorkerThread->Start();
			//���̳߳���ӹ����߳�
			m_ThreadPool.push_back(pWorkerThread);
		}
	}
	//��ǰ�����߳��������趨�߳���
	else if (zThreadCount < zCurrentThreadCount)
	{
		//���ü��ٹ����߳�����ʶ
		bIsDecreaseThreadCount = true;
		//�ͷ� zThreadCountIncrement ������ɾ�������߳��ź����ź�
		size_t zThreadCountIncrement = zCurrentThreadCount - zThreadCount;
		ReleaseSemaphore(m_semDeleteThread, zThreadCountIncrement, NULL);
	}
	ReleaseMutex(m_mutexThreadPool);

	if (true == bIsDecreaseThreadCount)
	{
		//�ȴ� zThreadCountIncrement �����Ĺ����߳�ɾ�����
		WaitForSingleObject(m_evtDeleteThreadComplete, INFINITE);
	}
}

size_t ThreadPoolManager::GetThreadCount() const
{
	WaitForSingleObject(m_mutexThreadPool, INFINITE);
	size_t zThreadCount = m_ThreadPool.size();
	ReleaseMutex(m_mutexThreadPool);

	return zThreadCount;
}

size_t ThreadPoolManager::GetActiveThreadCount() const
{
	return m_zActiveThreadCount;
}

void ThreadPoolManager::ThreadWorkFunc(void *pParam)
{
	if (NULL == pParam)
	{
		return;
	}

	ThreadPoolManager *pThisManager = static_cast<ThreadPoolManager *>(pParam);

	//�źŵȴ�����
	HANDLE arrSingalWait[3] = { pThisManager->m_semExecuteTaskProcess, 
								pThisManager->m_semDeleteThread, 
								pThisManager->m_evtUninitPool };
	bool bHasTask = false;
	bool bIsUninitPool = false;
	DWORD dwSingalValue = 0;
	TaskItem *pTaskItem = NULL;
	while (true)
	{
		//�ȴ�arrSingalWait�е��ź�
		dwSingalValue = WaitForMultipleObjects(3, arrSingalWait, 
	                                           false, INFINITE);
		//�յ�ִ���������ź����ź�
		if (0 == dwSingalValue-WAIT_OBJECT_0)
		{
			//���������ȡ����Pop����˵�����
			pThisManager->m_pTaskQueueManager->Lock();
			bHasTask = !pThisManager->m_pTaskQueueManager->IsEmpty();
			if (true == bHasTask)
			{
				//ȡ�����������˵�����
				pTaskItem = pThisManager->m_pTaskQueueManager->Front();
				//Pop�����������˵�����
				pThisManager->m_pTaskQueueManager->PopFront();
			}
			pThisManager->m_pTaskQueueManager->Unlock();

			//��������ȡ��������
			InterlockedIncrement(&pThisManager->m_zActiveThreadCount);
			if (true==bHasTask && NULL!=pTaskItem)
			{
				//ִ��������
				pTaskItem->m_pTaskProcessFunc(pTaskItem->m_pTask);
				//��������
				delete pTaskItem;
				pTaskItem = NULL;
			}
			InterlockedDecrement(&pThisManager->m_zActiveThreadCount);
		}
		//�յ�ɾ�������߳��ź����ź�
		else if (1 == dwSingalValue-WAIT_OBJECT_0)
		{
			bIsUninitPool = false;
			break;
		}
		//�յ�ж���̳߳��¼��ź�
		else if (2 == dwSingalValue-WAIT_OBJECT_0)
		{
			bIsUninitPool = true;
			break;
		}
	}

	//���̳߳�ɾ���ù����߳�
	WaitForSingleObject(pThisManager->m_mutexThreadPool, INFINITE);
	if (false == pThisManager->m_ThreadPool.empty())
	{
		vector<IThread *>::iterator iter = pThisManager->m_ThreadPool.begin();
		for (; iter!=pThisManager->m_ThreadPool.end(); ++iter)
		{
			if ((*iter)->GetThreadID() == GetCurrentThreadId())
			{
				//�����ù����߳�
				delete static_cast<IThread *>(*iter);
				//���̳߳ز����ù����߳�
				pThisManager->m_ThreadPool.erase(iter);
				break;
			}
		}
		//�ж��Ƿ�ɾ���������߳�
		if (true == bIsUninitPool)
		{
			if (0 == pThisManager->m_ThreadPool.size())
			{
				//�����̳߳�ж������¼��ź�
				SetEvent(pThisManager->m_evtUninitPoolComplete);
			}
		}
		//�ж��Ƿ�ɾ����ָ���������߳�
		else
		{
			if (pThisManager->m_zThreadCount == 
				pThisManager->m_ThreadPool.size())
			{
				//�����߳�����������¼��ź�
				SetEvent(pThisManager->m_evtDeleteThreadComplete);
			}
		}
	}
	ReleaseMutex(pThisManager->m_mutexThreadPool);
}