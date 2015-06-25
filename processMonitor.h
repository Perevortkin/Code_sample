#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <Psapi.h>
#include"Log.h"
#include<process.h>
enum state{ off, on };
class MonitorProcess
{
public:
	MonitorProcess(LPCTSTR AppName, LPTSTR ComLine, LPSECURITY_ATTRIBUTES ProcAt = NULL, LPSECURITY_ATTRIBUTES ThreadAt = NULL,
		BOOL InherH = FALSE, DWORD CreaF = 0, LPVOID Envo = NULL, LPCTSTR Directory = NULL);
	~MonitorProcess();
	void Monitor_already_running_process();//Ищет по id запущенный процесс и сообщает информацию 
	void Create_process();//Создает процесс
	void Process_info();//Сообщает информацию о созданном процессе
	void Close_process();//Закрывает процесс
	void Is_process_injob();//Проверяет, не закрылся ли процесс
	void Restart_process(void(*)());//Запускает методы Close_process и Create_process
	void ManuallyStop(void(*)());//Запускает метод Close_process
	static unsigned WINAPI DoWork(void*);//Ждет, пока созданный	 процесс не закроется сам
	void Launch_process(void(*)()); //Запускает методы Create_process и Is_process_injob
	static int NumofCreatedProcesses;
private:
	logger log;
	state processState;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//Параметры функции CreateProcess
	LPCTSTR lpApplicationName;                 // имя исполняемого модуля
	LPTSTR lpCommandLine;                      // Командная строка
	LPSECURITY_ATTRIBUTES lpProcessAttributes; // Указатель на структуру SECURITY_ATTRIBUTES
	LPSECURITY_ATTRIBUTES lpThreadAttributes;  // Указатель на структуру SECURITY_ATTRIBUTES
	BOOL bInheritHandles;                      // Флаг наследования текущего процесса
	DWORD dwCreationFlags;                     // Флаги способов создания процесса
	LPVOID lpEnvironment;                      // Указатель на блок среды
	LPCTSTR lpCurrentDirectory;                // Текущий диск или каталог
};
