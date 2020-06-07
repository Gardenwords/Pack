#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#define False  	   -1
#define True		1
#define MAX_SIZE 	1000
#define _MAX_FNAME 	256

struct Package{
	char 			FileRoute[_MAX_FNAME];	//文件路径
	char 			FileName[_MAX_FNAME];	//文件名
	__time64_t		FileTime;				//文件创建时间 
	int 			FileSize;				//文件长度
};

class File_Management{
public:
	int File_Size(const char* File);
	int Rec_Route(const char* File, char* PreRoute, _finddata_t* FileInfo, long &Handle);
};

class File_Procession:public File_Management{
protected:
	int 				File_Cnt;
	long 				Handle;
	struct 	_finddata_t FileInfo;
	char 				PreRoute[_MAX_FNAME];
	char 				Route[_MAX_FNAME];
	struct Package* 	PackItem;
public:
	File_Procession();
	~File_Procession();
	int Pack_Info(const char* File, const char* Dest, int* File_Cnt);
	int Pack_Cont(const char* File, int* File_Cnt, FILE* fpPack);
};

class File_ListInfo{
private:
	Package* PackItem;
public:
	File_ListInfo();
	~File_ListInfo();
	char* sec2date(time_t t, const char* fmt = "%Y/%m/%d %H:%M:%S");
	int List_Info(const char* File, int Cnt);
	int List_Info(const char* File, int Cnt, int format);
};

class File_Unpack{
private:
	Package* PackItem;
public:
	File_Unpack();
	~File_Unpack();
	int UnPack(const char* DePack, const char* Default, int Cnt);
	int UnPack(const char* DePack, const char* Default, int Cnt, int format);
};

class File_Run{
public:
	int Format(const char* sign, int* format);
	void Run_Pack(char* argv1, char* argv2, int File_Cnt);
	void Run_List(char* argv1, char* argv2, int Cnt);
	void Run_Unpack(char* argv1, char* argv2, char* argv3, int Cnt);
};

class File_Judge{
private:
	int 	File_Cnt;
	int 	Cnt;
	char* 	argv1;
	char* 	argv2;
	char 	sign[_MAX_FNAME];
public:
	File_Judge(char* argv1, char* argv2);
	~File_Judge();
	void Sign_Process();
	void Judge_Usage(char* argv3);
};



