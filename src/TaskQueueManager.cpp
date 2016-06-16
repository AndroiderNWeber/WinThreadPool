#include "TaskQueueManager.h"

TaskQueueManager *TaskQueueManager::ms_pInstance = NULL;

TaskQueueManager *TaskQueueManager::GetInstance()
{
	if (NULL == ms_pInstance)
	{
		ms_pInstance = new TaskQueueManager;
	}

	return ms_pInstance;
}

TaskQueueManager::TaskQueueManager()
{
	//��������ʼ��������л������ں˶���
	m_mutexTaskQueue = CreateMutex(NULL, FALSE, NULL);
}

TaskQueueManager::~TaskQueueManager()
{
	//ȷ����������е�����ȫ������
	if (false == m_TaskQueue.empty())
	{
		deque<TaskItem *>::iterator iter = m_TaskQueue.begin();
		for (; iter != m_TaskQueue.end(); ++iter)
		{
			if (NULL != *iter)
			{
				delete static_cast<TaskItem *>(*iter);
			}
		}
	}

	CloseHandle(m_mutexTaskQueue);
	m_mutexTaskQueue = NULL;
}

void TaskQueueManager::Lock()
{
	//ռ��������л�����
	WaitForSingleObject(m_mutexTaskQueue, INFINITE);
}

void TaskQueueManager::Unlock()
{
	//�ͷ�������л�����
	ReleaseMutex(m_mutexTaskQueue);
}

void TaskQueueManager::PushBack(void (*pTaskProcessFunc)(void *), void *pTask)
{
	m_TaskQueue.push_back(new TaskItem(pTaskProcessFunc, pTask));
}

void TaskQueueManager::PushBack(ITaskProcess *pTaskProcess, void *pTask)
{
	ObjTaskItem *pObjTaskItem = new ObjTaskItem(pTaskProcess, pTask);
	m_TaskQueue.push_back(new TaskItem(TaskConvertFunc, pObjTaskItem));
}

TaskItem *TaskQueueManager::Front()
{
	if (false == m_TaskQueue.empty())
	{
		return static_cast<TaskItem *>(m_TaskQueue.front());
	}
	else
	{
		return NULL;
	}
}

void TaskQueueManager::PopFront()
{
	if (false == m_TaskQueue.empty())
	{
		m_TaskQueue.pop_front();
	}
}

size_t TaskQueueManager::Size()
{
	return m_TaskQueue.size();
}

bool TaskQueueManager::IsEmpty()
{
	return m_TaskQueue.empty();
}

void TaskQueueManager::TaskConvertFunc(void *pParam)
{
	if (NULL == pParam)
	{
		return;
	}

	//������������Ԫת��Ϊָ��������Ԫ
	ObjTaskItem *pObjTaskItem = static_cast<ObjTaskItem *>(pParam);

	//ִ��������
	pObjTaskItem->m_pTaskProcess->TaskPorcess(pObjTaskItem->m_pTask);

	//��������
	delete pObjTaskItem;
}