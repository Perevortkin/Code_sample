#include "processMonitor.h"
using namespace std;
vector<HANDLE> Num;//���������� ������������ ��������� ���������
int MonitorProcess::NumofCreatedProcesses = 0;//���������� ��������� ���������
MonitorProcess::MonitorProcess(LPCTSTR AppName, LPTSTR ComLine,
	LPSECURITY_ATTRIBUTES ProcAt, LPSECURITY_ATTRIBUTES ThreadAt,
	BOOL InherH, DWORD CreaF, LPVOID Envo, LPCTSTR Directory)
{
	lpApplicationName = AppName;
	lpCommandLine = ComLine;
	lpProcessAttributes = ProcAt;
	lpThreadAttributes = ThreadAt;
	bInheritHandles = InherH;
	dwCreationFlags = CreaF;
	lpEnvironment = Envo;
	lpCurrentDirectory = Directory;
}
MonitorProcess::~MonitorProcess()
{
	//��������� ��� �������� �����������
	if (NumofCreatedProcesses > 0)
	{
		for (int i=0; i < NumofCreatedProcesses; i++)
		{
			CloseHandle(Num[i]);
		}
	}
	_endthreadex(0);
}
void MonitorProcess::Create_process()
{
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (CreateProcess(lpApplicationName, lpCommandLine,
		lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
		&si, &pi))
	{
		NumofCreatedProcesses += 1;
		Num.push_back(pi.hProcess);//�������� ���������� � ���������
		CloseHandle(pi.hThread);   //��������� �����
			processState = on;     //�������� ������� ������������
	}
	else
	{
		log << "Error, can`t create process";
		cout << "Error, can`t create process";
		processState = off;
	}
}
void MonitorProcess::Close_process()
{
	if (NumofCreatedProcesses > 0)
	{
		processState = off; //��������� ������� ������������
		DWORD d;
		HANDLE h = Num[NumofCreatedProcesses - 1];
		int out = GetExitCodeProcess(h, &d);
		TerminateProcess(h, out);
		//���� �������� �������, ��� ��� ������� TerminateProcess ���������� �������� ��� �� �������� ��������
		WaitForSingleObject(h, INFINITE);
		CloseHandle(h);//��������� ���������� ��������
		Num.pop_back();
		cout << "Closing process";
		log << "Process closed";
		NumofCreatedProcesses -= 1;
		processState = on; //�������� �������
	}
}
void MonitorProcess::Process_info()
{
	if (NumofCreatedProcesses > 0)
	{
		//�������� HANDLE �������� (��������� �������)
		HANDLE hCur = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION,
			true, pi.dwProcessId);
		DWORD i, code, sh, sh1, id;
		int k;
		char buf[100];
		char name[MAX_PATH];
		cout << "\nInfo about current process\n";
		log << "\nInfo about current process\n";
		//������� �������
		i = GetCurrentDirectory(100, buf);
		cout << "Current Directory  ->" << buf;
		log << "Current Directory  ->";
		log << buf;
		//��������� ������
		GetModuleFileNameEx(hCur, NULL, name, MAX_PATH);
		cout << "\nModule filename is : " << name;
		log << "\nModule filename is : ";
		log << name;
		//Id ��������
		id = GetProcessId(pi.hProcess);
		cout << "\nProcess id     ->" << id;
		log << "\nProcess id     ->";
		log << id;
		//��� ������� ����������
		k = GetExitCodeProcess(hCur, &code);
		cout << "\nExitCode Process   ->" << code;
		log << "\nExitCode Process   ->";
		log << code;
		//��������� ������������
		k = GetProcessShutdownParameters(&sh, &sh1);
		if (k == 1)
			i = GetProcessVersion(GetCurrentProcessId());
		cout << "\nProcess Version    ->" << i;
		log << "\nProcess Version    ->";
		log << i;
	}
 }
void MonitorProcess::Is_process_injob()
{
	//������� ����� ����� ��� ����������� ��������� ��������
	unsigned int dwThreadID;
	 _beginthreadex(NULL, 0,DoWork, this, 0, &dwThreadID);
}
void MonitorProcess::Monitor_already_running_process()
{
	cout << "\nEnter the id of process that you want to monitor:";
	DWORD processID, aProcesses[1024], cbNeeded, cProcesses;
	HANDLE processHandle;
	cin >> processID;
	//�������� ������ ���������
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		cout << "\nCan`t take the process list";
	}
	// �������� ������� id ��������� ��������
	cProcesses = cbNeeded / sizeof(DWORD);
	//�������� ��������� id
	enum process{no,yes};
	process isProcess = no;
	for (DWORD i = 0; i < cProcesses; i++)
	{
		if (processID == aProcesses[i])
		{
			isProcess = yes;
		}
	}
	if (isProcess == yes)
	{
		//�������� ���������� �������� �� id
		processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
		char name[MAX_PATH];
		//��������� ������
		if (GetModuleFileNameEx(processHandle,NULL,name,MAX_PATH))
		{
			cout << "\nModule filename is : " << name;
		}
		else 
		{
			cout << "\nCan`t get filename";
		}
	}
	else 
	{
		cout << "\nThere is no current process running";
	}
}
unsigned MonitorProcess::DoWork(void* caller)
{
	MonitorProcess* self = (MonitorProcess*)caller;
	WaitForSingleObject(Num[NumofCreatedProcesses-1], INFINITE);
	if (self->processState == on & NumofCreatedProcesses>0)
	{
		CloseHandle(Num[NumofCreatedProcesses - 1]);//��������� ���������� ��������� ��������
		Num.pop_back(); //������� ���������� ��������� ��������
		NumofCreatedProcesses -=1;
		self->Create_process();
		self->Is_process_injob();
	}
	return 0;
}
void MonitorProcess::Launch_process(void(*fu)())
{
	Create_process();
	Is_process_injob();
	fu(); //�������� Callback
}
void MonitorProcess::Restart_process(void(*fu)())
{
	Close_process();
	Create_process();
	fu();
}
void MonitorProcess::ManuallyStop(void(*fu)())
{
	Close_process();
	fu();
}