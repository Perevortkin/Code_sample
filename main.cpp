#include "processMonitor.h"
using namespace std;
logger callback_log;
void OnProcStart();
void OnProcRestart();
void OnProcManuallyStopped();
int main()
{
	MonitorProcess process("C:\\Windows\\System32\\NOTEPAD.EXE", "-o F:\\1.txt");
	cout << "\nEnter:\n1 create process\n2 to reboot process\n3 to close process\n"
		<< "4 to info\n5 to see the process state\n6 to out the choise\n";
	char ch;
	cin >> ch;
	while (ch != ('6')){
		switch (ch)
		{
		case  '1': process.Launch_process(OnProcStart); break;
		case  '2': process.Restart_process(OnProcRestart); break;
		case  '3':process.ManuallyStop(OnProcManuallyStopped); break;
		case  '4': process.Process_info(); break;
		case  '5': process.Monitor_already_running_process();
		default:cout << "\nInvalid choise, please enter:\n3 to close process\n"
			<< "2 to reboot process\n4 to info\nb to see the process state\n6 to out the choise\n";
		}
		cout << "\nEnter:\n1 create process\n2 to reboot process\n3 to close process\n"
			 <<"4 to info\n5 to see the process state\n6 to out the choise\n";
		cin >> ch;
	}
	return 0;
}
void OnProcStart()
{
	callback_log << "Process created";
}
void OnProcManuallyStopped()
{
	callback_log << "Process stopped";
}
void OnProcRestart()
{
	callback_log << "Process restarted";
}