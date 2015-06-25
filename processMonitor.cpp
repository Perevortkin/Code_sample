#include "processMonitor.h"
using namespace std;
vector<HANDLE> Num;//Количество дескрипторов созданных процессов
int MonitorProcess::NumofCreatedProcesses = 0;//Количество созданных процессов
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
	//Закрываем все открытые дескрипторы
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
		Num.push_back(pi.hProcess);//Помещаем дескриптор в контейнер
		CloseHandle(pi.hThread);   //Закрываем поток
			processState = on;     //Включаем условие перезагрузки
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
		processState = off; //Выключаем условие перезагрузки
		DWORD d;
		HANDLE h = Num[NumofCreatedProcesses - 1];
		int out = GetExitCodeProcess(h, &d);
		TerminateProcess(h, out);
		//Ждем закрытия обьекта, так как функция TerminateProcess возвращает значение еще до закрытия процесса
		WaitForSingleObject(h, INFINITE);
		CloseHandle(h);//Закрываем дескриптор процесса
		Num.pop_back();
		cout << "Closing process";
		log << "Process closed";
		NumofCreatedProcesses -= 1;
		processState = on; //Включаем обратно
	}
}
void MonitorProcess::Process_info()
{
	if (NumofCreatedProcesses > 0)
	{
		//получаем HANDLE процесса (открываем процесс)
		HANDLE hCur = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION,
			true, pi.dwProcessId);
		DWORD i, code, sh, sh1, id;
		int k;
		char buf[100];
		char name[MAX_PATH];
		cout << "\nInfo about current process\n";
		log << "\nInfo about current process\n";
		//Текущий каталог
		i = GetCurrentDirectory(100, buf);
		cout << "Current Directory  ->" << buf;
		log << "Current Directory  ->";
		log << buf;
		//Командная строка
		GetModuleFileNameEx(hCur, NULL, name, MAX_PATH);
		cout << "\nModule filename is : " << name;
		log << "\nModule filename is : ";
		log << name;
		//Id процесса
		id = GetProcessId(pi.hProcess);
		cout << "\nProcess id     ->" << id;
		log << "\nProcess id     ->";
		log << id;
		//Код статуса завершения
		k = GetExitCodeProcess(hCur, &code);
		cout << "\nExitCode Process   ->" << code;
		log << "\nExitCode Process   ->";
		log << code;
		//Параметры перезагрузки
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
	//Создаем новый поток для мониторинга состояния процесса
	unsigned int dwThreadID;
	 _beginthreadex(NULL, 0,DoWork, this, 0, &dwThreadID);
}
void MonitorProcess::Monitor_already_running_process()
{
	cout << "\nEnter the id of process that you want to monitor:";
	DWORD processID, aProcesses[1024], cbNeeded, cProcesses;
	HANDLE processHandle;
	cin >> processID;
	//Получаем список процессов
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		cout << "\nCan`t take the process list";
	}
	// Вычислим сколько id процессов получено
	cProcesses = cbNeeded / sizeof(DWORD);
	//Проверим введенный id
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
		//Получаем дескриптор процесса по id
		processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
		char name[MAX_PATH];
		//Командная строка
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
		CloseHandle(Num[NumofCreatedProcesses - 1]);//Закрываем дескриптор закрытого процесса
		Num.pop_back(); //удаляем дескриптор закрытого процесса
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
	fu(); //Вызываем Callback
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