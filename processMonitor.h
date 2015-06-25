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
	void Monitor_already_running_process();//���� �� id ���������� ������� � �������� ���������� 
	void Create_process();//������� �������
	void Process_info();//�������� ���������� � ��������� ��������
	void Close_process();//��������� �������
	void Is_process_injob();//���������, �� �������� �� �������
	void Restart_process(void(*)());//��������� ������ Close_process � Create_process
	void ManuallyStop(void(*)());//��������� ����� Close_process
	static unsigned WINAPI DoWork(void*);//����, ���� ���������	 ������� �� ��������� ���
	void Launch_process(void(*)()); //��������� ������ Create_process � Is_process_injob
	static int NumofCreatedProcesses;
private:
	logger log;
	state processState;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//��������� ������� CreateProcess
	LPCTSTR lpApplicationName;                 // ��� ������������ ������
	LPTSTR lpCommandLine;                      // ��������� ������
	LPSECURITY_ATTRIBUTES lpProcessAttributes; // ��������� �� ��������� SECURITY_ATTRIBUTES
	LPSECURITY_ATTRIBUTES lpThreadAttributes;  // ��������� �� ��������� SECURITY_ATTRIBUTES
	BOOL bInheritHandles;                      // ���� ������������ �������� ��������
	DWORD dwCreationFlags;                     // ����� �������� �������� ��������
	LPVOID lpEnvironment;                      // ��������� �� ���� �����
	LPCTSTR lpCurrentDirectory;                // ������� ���� ��� �������
};
