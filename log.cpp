#include"Log.h"
void logger::operator<<(const string &str)
{	//Открываем и дописываем в файл
	fstream fs("log.txt", ios::out | ios::app);
	//Время
	struct tm time_info;
	time_t time_create = time(NULL);
	errno_t err;
	err = localtime_s(&time_info, &time_create);
	if (err)
	{
		fs << "Invalid argument to localtime_s";
	}
	fs << endl << '|' << time_info.tm_hour << ':' << time_info.tm_min << ':' << time_info.tm_sec << '|';
	fs << str;
	fs.close();
}
void logger::operator<<(const int & D)
{
	fstream fs("log.txt", ios::out | ios::app);
	fs << D;
	fs.close();
}
logger::~logger(){}
logger::logger(){}
