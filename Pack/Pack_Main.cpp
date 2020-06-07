#include "Class_Define.h"

//获取文件大小 
int File_Management::File_Size(const char* File){
	FILE* fp = fopen(File, "rb");
	unsigned int Current_ReadPos = ftell(fp);					//获取当前读取文件的位置 进行保存
	int FileSize;
	fseek(fp, 0, SEEK_END);
	FileSize = ftell(fp);										//获取文件的大小
	fseek(fp, Current_ReadPos, SEEK_SET );						//恢复文件原来读取的位置
	fclose(fp);
	return FileSize;
}

//获取文件路径 
int File_Management::Rec_Route(const char* File, char* PreRoute, _finddata_t* FileInfo, long &Handle){
	if(NULL == strchr(File, '*')){
		int len = strlen(File);
		char* Search = new char[len + 1]; 
		strcpy(PreRoute, File); 
		strcat(strcpy(Search, File), "*.*");
		Handle = _findfirst(Search, FileInfo);
	}else{
		int start;
		for(start = 0; File[start] != '*'; ++start)
			PreRoute[start] = File[start];

		PreRoute[start] = '\0';
		Handle = _findfirst(File, FileInfo);
	}
	if(-1 == Handle)
		return False;
	
	return True;
}

/*---------------------------*/

//File_Procession构造函数
File_Procession::File_Procession(){
	PackItem = new Package[MAX_SIZE];
}

File_Procession::~File_Procession(){
	delete[] PackItem;
}

//打包文件信息 
int File_Procession::Pack_Info(const char* File, const char* Dest, int* File_Cnt){
	FILE* fpPack = fopen(Dest, "wb");							//写入的文件
	if(!fpPack){
		printf("打开目标文件时，文件打开失败!(Pack_Info)\n");
		return False;
	}
	if(-1 == Rec_Route(File, PreRoute, &FileInfo, Handle)){		//查找文件路径 
		printf("查找路径失败!(Pack_Info)\n");
		return False;
	}
	do{
		if(FileInfo.attrib & _A_SUBDIR)
			continue;
		strcat(strcpy(Route, PreRoute), FileInfo.name);			//完整路径
		strcpy(PackItem[*File_Cnt].FileRoute, Route);			//文件路径
		strcpy(PackItem[*File_Cnt].FileName, FileInfo.name);	//文件名
		PackItem[*File_Cnt].FileSize = File_Size(Route);		//文件长度
		PackItem[*File_Cnt].FileTime = FileInfo.time_create; 	//文件创建时间 
		fwrite(&PackItem[*File_Cnt], sizeof(Package), 1, fpPack);
		(*File_Cnt)++;
		if(*File_Cnt == MAX_SIZE)
			break;
	}while(!_findnext(Handle, &FileInfo));
	if(-1 == Pack_Cont(File, File_Cnt, fpPack))
		return False;
	_findclose(Handle);
	fclose(fpPack);
	return True;
}

//打包文件内容 
int File_Procession::Pack_Cont(const char* File, int* File_Cnt, FILE* fpPack){
	fseek(fpPack, 0, SEEK_END);									//定位至文件末尾
	if(-1 == Rec_Route(File, PreRoute, &FileInfo, Handle)){		//查找文件路径
		printf("查找路径失败!(Pack_Cont)");
		return False;
	}
	int Cnt = 0; 
	do{
		strcat(strcpy(Route, PreRoute), FileInfo.name);			//完整路径
		FILE* frPack = fopen(Route, "rb");
		int ch;
		while( (ch = fgetc(frPack)) != EOF)						//按字节存入 
			fputc(ch, fpPack);
		++Cnt;
		fclose(frPack);
	}while(!_findnext(Handle, &FileInfo) && Cnt < *File_Cnt);
	fwrite(File_Cnt, sizeof(int), 1, fpPack); 
	fclose(fpPack);
	return True;
}

/*---------------------------*/

//File_ListInfo构造函数
File_ListInfo::File_ListInfo(){
	PackItem = new Package[MAX_SIZE];
}

File_ListInfo::~File_ListInfo(){
	delete[] PackItem;
}

//文件创建时间 
char* File_ListInfo::sec2date(time_t t, const char* fmt){
  static char str[128];
  strftime(str, sizeof(str), fmt, localtime(&t));
  return str;
}

//列出所有文件信息 
int File_ListInfo::List_Info(const char* File, int Cnt){
	FILE* fp = fopen(File, "rb");
	if(!fp)
		return False;
	fseek(fp, 0, SEEK_SET);
	for(int i = 0; i < Cnt; ++i){
		fseek(fp, _MAX_FNAME, SEEK_CUR);
		fread(PackItem[i].FileName, _MAX_FNAME, 1, fp);
		fread(&PackItem[i].FileTime, sizeof(__time64_t), 1, fp);
		fread(&PackItem[i].FileSize, sizeof(int), 1, fp);
		fseek(fp, 4, SEEK_CUR);
	}
	for(int i = 0; i < Cnt; ++i)
		printf("%-20s\t%10dK\t%s\n", PackItem[i].FileName, PackItem[i].FileSize / 1024 + 1, sec2date(PackItem[i].FileTime));
	fclose(fp);
	return True;
}

//规格化列出所有文件信息
int File_ListInfo::List_Info(const char* File, int Cnt, int format){
	FILE* fp = fopen(File, "rb");
	if(!fp)
		return False;
	if(format <= Cnt){
		fseek(fp, 0, SEEK_SET);
		for(int i = 0; i < Cnt; ++i){
			fseek(fp, _MAX_FNAME, SEEK_CUR);
			fread(PackItem[i].FileName, _MAX_FNAME, 1, fp);
			fread(&PackItem[i].FileTime, sizeof(__time64_t), 1, fp);
			fread(&PackItem[i].FileSize, sizeof(int), 1, fp);
			fseek(fp, 4, SEEK_CUR);
		}
		for(int i = 0; i < Cnt; ++i){
			printf("%-20s\t%10dK\t%s\n", PackItem[i].FileName, PackItem[i].FileSize / 1024 + 1, sec2date(PackItem[i].FileTime));
			if((i + 1) % format == 0){
				putchar('\n');
				system("pause");
				putchar('\n');
			}
		}
	}
	else{
		printf("参数 %d 错误\n", format);
		return False;
	}
	fclose(fp);
	return True;
}

/*---------------------------*/

//File_Unpack构造函数

File_Unpack::File_Unpack(){
	PackItem = new Package[MAX_SIZE];
}

File_Unpack::~File_Unpack(){
	delete[] PackItem;
}

//解包
int File_Unpack::UnPack(const char* DePack, const char* Default, int Cnt){
	FILE* fuPack = fopen(DePack, "rb");
	if(!fuPack){
		printf("解包文件打开失败!(UnPack)\n");
		return False;
	}
	if(!Default){
		printf("解压路径错误!\n");
		return False;
	}
	for(int i = 0; i < Cnt; ++i){
		fseek(fuPack, _MAX_FNAME, SEEK_CUR);
		fread(PackItem[i].FileName, _MAX_FNAME, 1, fuPack);
		fseek(fuPack, sizeof(__time64_t), SEEK_CUR);
		fread(&PackItem[i].FileSize, sizeof(PackItem[i].FileSize), 1, fuPack);
		fseek(fuPack, 4, SEEK_CUR);
	}
	int Cnted = 0;												//已读文件数
	char* Unpack = new char[_MAX_FNAME + 1];					//存放解压路径
	fseek(fuPack, 0, SEEK_SET);									//文件指针复位 
	fseek(fuPack, Cnt * sizeof(Package), SEEK_CUR);			//指向内容起始位置
	while(Cnted < Cnt){
		strcat(strcpy(Unpack, Default), PackItem[Cnted].FileName);				//完整解压路径 "E:\my\***"
		printf("%s\n", PackItem[Cnted].FileName);
		FILE* fwPack = fopen(Unpack, "wb");
		int ch;
		for(int i = 0; i < PackItem[Cnted].FileSize; ++i){
			ch = fgetc(fuPack);
			fputc(ch, fwPack);
		}
		++Cnted;
		
		fclose(fwPack);
	}
	free(Unpack);
	fclose(fuPack);
	return True;
}

//规格化解包
int File_Unpack::UnPack(const char* DePack, const char* Default, int Cnt, int format){
	FILE* fuPack = fopen(DePack, "rb");
	if(!fuPack){
		printf("解包文件打开失败!(UnPack)\n");
		return False;
	}
	if(!Default){
		printf("解压路径错误!\n");
		return False;
	}
	if(format <= Cnt){
		for(int i = 0; i < format; ++i){
			fseek(fuPack, _MAX_FNAME, SEEK_CUR);
			fread(PackItem[i].FileName, _MAX_FNAME, 1, fuPack);
			fseek(fuPack, sizeof(__time64_t), SEEK_CUR);
			fread(&PackItem[i].FileSize, sizeof(PackItem[i].FileSize), 1, fuPack);
			fseek(fuPack, 4, SEEK_CUR);
		}
		char* Unpack = new char[_MAX_FNAME + 1];					//存放解压路径
		fseek(fuPack, 0, SEEK_SET);									//文件指针复位 
		fseek(fuPack, Cnt * sizeof(Package), SEEK_CUR);				//指向内容起始位置
		for(int i = 0; i < format - 1; ++i)
			fseek(fuPack, PackItem[i].FileSize, SEEK_CUR);
		strcat(strcpy(Unpack, Default), PackItem[format - 1].FileName);				//完整解压路径 "E:\my\***"
		printf("%s\n", PackItem[format - 1].FileName);
		FILE* fwPack = fopen(Unpack, "wb");
		int ch;
		for(int i = 0; i < PackItem[format - 1].FileSize; ++i){
			ch = fgetc(fuPack);
			fputc(ch, fwPack);
		}
		fclose(fwPack);
		free(Unpack);
	}
	else{
		printf("参数 %d 错误!\n", format);
		return False;
	}
	fclose(fuPack);
	return True;
}

/*---------------------------*/

//File_Run构造函数

 
//规格化 
int File_Run::Format(const char* sign, int* format){
	sscanf(sign + 2, "%d", format);
	if(0 == format)
		return -1;
	else
		return 1;
}

void File_Run::Run_Pack(char* argv1, char* argv2, int File_Cnt){
	File_Procession F_P;
	if(-1 == F_P.Pack_Info(argv1, argv2, &File_Cnt))
		printf("打包失败!\n");
	else
		printf("打包成功!\n");

	printf("文件数: %d\n", File_Cnt);
}

void File_Run::Run_List(char* argv1, char* argv2, int Cnt){
	FILE* fcnt = fopen(argv2, "rb");
	if(!fcnt)
		return;
	fseek(fcnt, -4, SEEK_END);
	fread(&Cnt, sizeof(int), 1, fcnt);
	int format;
	Format(argv1, &format);
	File_ListInfo F_L;
	if(!format){
		if(-1 == F_L.List_Info(argv2, Cnt))
			printf("列出信息失败!\n");
	}
	else{
		if(-1 == F_L.List_Info(argv2, Cnt, format))
			printf("列出信息失败!\n");
	}
	fclose(fcnt);
}

void File_Run::Run_Unpack(char* argv1, char* argv2, char* argv3, int Cnt){
	FILE* fcnt = fopen(argv2, "rb");
	if(!fcnt)
		return;
	fseek(fcnt, -4, SEEK_END);
	fread(&Cnt, sizeof(int), 1, fcnt);
	int format;
	Format(argv1, &format);
	File_Unpack F_U; 
	if(!format){
		if(-1 == F_U.UnPack(argv2, argv3, Cnt))
			printf("解包失败!\n");
		else
			printf("解包成功!\n");
	}
	else{
		if(-1 == F_U.UnPack(argv2, argv3, Cnt, format))
			printf("解包失败!\n");
		else
			printf("解包成功!\n");
	}
	fclose(fcnt);
}

/*---------------------------*/

//File_Judge构造函数 
File_Judge::File_Judge(char* argv1, char* argv2){
	this->File_Cnt = 0;
	this->Cnt = 0;
	this->argv1 = new char[_MAX_FNAME];
	this->argv2 = new char[_MAX_FNAME];
	strcpy(this->argv1, argv1);
	strcpy(this->argv2, argv2);	
}

File_Judge::~File_Judge(){
	delete[] argv1;
	delete[] argv2;
}

void File_Judge::Judge_Usage(char* argv3){
	File_Run F_R;
	if( !(strcmp(sign, "-u") == 0) && !(strcmp(sign, "-l") == 0) )		/*E:\Pack>pack E:\tmp\ E:\Out\xy.z		*/
		F_R.Run_Pack(argv1, argv2, File_Cnt);
	else																/*E:\Pack>pack -l E:\Out\xy.z			*/
		if(!(strcmp(sign, "-u") == 0))
			F_R.Run_List(argv1, argv2, Cnt);
		else															/*E:\Pack>pack -u E:\Out\xy.z E:\my\	*/
			F_R.Run_Unpack(argv1, argv2, argv3, Cnt);
}

void File_Judge::Sign_Process(){
	strncpy(sign, argv1, 2);
	sign[2] = '\0';
}

int main(int argc, char* argv[]){
	File_Judge F_J(argv[1], argv[2]);
	F_J.Sign_Process();
	F_J.Judge_Usage(argv[3]);

	return 0;
}
