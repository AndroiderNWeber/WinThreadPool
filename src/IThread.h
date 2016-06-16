/**********************************************************

   *��    �����̻߳���
   *��    �ã��ṩ�̵߳Ļ�������
   *ע����������������ǰ���ͷ�Run()�������������Դ

**********************************************************/

#ifndef _ITHREAD_H_
#define _ITHREAD_H_

#include <windows.h>

class IThread
{
public:
	IThread();
	virtual ~IThread();

	//�����̵߳�Ψһ����
    bool Start();

	//��ȡ�߳�ID
	DWORD GetThreadID() const;

	//��ȡ�߳�HANDLE
	HANDLE GetThreadHandle() const;

protected:
	//������Ҫʵ�ָ÷��������ҵ����
	virtual void Run() = 0;

private:
	//�̺߳���
	static DWORD WINAPI ThreadFunc(LPVOID lpParam);

private:
	//�߳�ID
	DWORD m_dwThreadID;

	//�߳̾��
	HANDLE m_hThreadHandle;
};

#endif