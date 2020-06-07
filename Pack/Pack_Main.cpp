#include "Class_Define.h"

//��ȡ�ļ���С 
int File_Management::File_Size(const char* File){
	FILE* fp = fopen(File, "rb");
	unsigned int Current_ReadPos = ftell(fp);					//��ȡ��ǰ��ȡ�ļ���λ�� ���б���
	int FileSize;
	fseek(fp, 0, SEEK_END);
	FileSize = ftell(fp);										//��ȡ�ļ��Ĵ�С
	fseek(fp, Current_ReadPos, SEEK_SET );						//�ָ��ļ�ԭ����ȡ��λ��
	fclose(fp);
	return FileSize;
}

//��ȡ�ļ�·�� 
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

//File_Procession���캯��
File_Procession::File_Procession(){
	PackItem = new Package[MAX_SIZE];
}

File_Procession::~File_Procession(){
	delete[] PackItem;
}

//����ļ���Ϣ 
int File_Procession::Pack_Info(const char* File, const char* Dest, int* File_Cnt){
	FILE* fpPack = fopen(Dest, "wb");							//д����ļ�
	if(!fpPack){
		printf("��Ŀ���ļ�ʱ���ļ���ʧ��!(Pack_Info)\n");
		return False;
	}
	if(-1 == Rec_Route(File, PreRoute, &FileInfo, Handle)){		//�����ļ�·�� 
		printf("����·��ʧ��!(Pack_Info)\n");
		return False;
	}
	do{
		if(FileInfo.attrib & _A_SUBDIR)
			continue;
		strcat(strcpy(Route, PreRoute), FileInfo.name);			//����·��
		strcpy(PackItem[*File_Cnt].FileRoute, Route);			//�ļ�·��
		strcpy(PackItem[*File_Cnt].FileName, FileInfo.name);	//�ļ���
		PackItem[*File_Cnt].FileSize = File_Size(Route);		//�ļ�����
		PackItem[*File_Cnt].FileTime = FileInfo.time_create; 	//�ļ�����ʱ�� 
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

//����ļ����� 
int File_Procession::Pack_Cont(const char* File, int* File_Cnt, FILE* fpPack){
	fseek(fpPack, 0, SEEK_END);									//��λ���ļ�ĩβ
	if(-1 == Rec_Route(File, PreRoute, &FileInfo, Handle)){		//�����ļ�·��
		printf("����·��ʧ��!(Pack_Cont)");
		return False;
	}
	int Cnt = 0; 
	do{
		strcat(strcpy(Route, PreRoute), FileInfo.name);			//����·��
		FILE* frPack = fopen(Route, "rb");
		int ch;
		while( (ch = fgetc(frPack)) != EOF)						//���ֽڴ��� 
			fputc(ch, fpPack);
		++Cnt;
		fclose(frPack);
	}while(!_findnext(Handle, &FileInfo) && Cnt < *File_Cnt);
	fwrite(File_Cnt, sizeof(int), 1, fpPack); 
	fclose(fpPack);
	return True;
}

/*---------------------------*/

//File_ListInfo���캯��
File_ListInfo::File_ListInfo(){
	PackItem = new Package[MAX_SIZE];
}

File_ListInfo::~File_ListInfo(){
	delete[] PackItem;
}

//�ļ�����ʱ�� 
char* File_ListInfo::sec2date(time_t t, const char* fmt){
  static char str[128];
  strftime(str, sizeof(str), fmt, localtime(&t));
  return str;
}

//�г������ļ���Ϣ 
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

//����г������ļ���Ϣ
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
		printf("���� %d ����\n", format);
		return False;
	}
	fclose(fp);
	return True;
}

/*---------------------------*/

//File_Unpack���캯��

File_Unpack::File_Unpack(){
	PackItem = new Package[MAX_SIZE];
}

File_Unpack::~File_Unpack(){
	delete[] PackItem;
}

//���
int File_Unpack::UnPack(const char* DePack, const char* Default, int Cnt){
	FILE* fuPack = fopen(DePack, "rb");
	if(!fuPack){
		printf("����ļ���ʧ��!(UnPack)\n");
		return False;
	}
	if(!Default){
		printf("��ѹ·������!\n");
		return False;
	}
	for(int i = 0; i < Cnt; ++i){
		fseek(fuPack, _MAX_FNAME, SEEK_CUR);
		fread(PackItem[i].FileName, _MAX_FNAME, 1, fuPack);
		fseek(fuPack, sizeof(__time64_t), SEEK_CUR);
		fread(&PackItem[i].FileSize, sizeof(PackItem[i].FileSize), 1, fuPack);
		fseek(fuPack, 4, SEEK_CUR);
	}
	int Cnted = 0;												//�Ѷ��ļ���
	char* Unpack = new char[_MAX_FNAME + 1];					//��Ž�ѹ·��
	fseek(fuPack, 0, SEEK_SET);									//�ļ�ָ�븴λ 
	fseek(fuPack, Cnt * sizeof(Package), SEEK_CUR);			//ָ��������ʼλ��
	while(Cnted < Cnt){
		strcat(strcpy(Unpack, Default), PackItem[Cnted].FileName);				//������ѹ·�� "E:\my\***"
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

//��񻯽��
int File_Unpack::UnPack(const char* DePack, const char* Default, int Cnt, int format){
	FILE* fuPack = fopen(DePack, "rb");
	if(!fuPack){
		printf("����ļ���ʧ��!(UnPack)\n");
		return False;
	}
	if(!Default){
		printf("��ѹ·������!\n");
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
		char* Unpack = new char[_MAX_FNAME + 1];					//��Ž�ѹ·��
		fseek(fuPack, 0, SEEK_SET);									//�ļ�ָ�븴λ 
		fseek(fuPack, Cnt * sizeof(Package), SEEK_CUR);				//ָ��������ʼλ��
		for(int i = 0; i < format - 1; ++i)
			fseek(fuPack, PackItem[i].FileSize, SEEK_CUR);
		strcat(strcpy(Unpack, Default), PackItem[format - 1].FileName);				//������ѹ·�� "E:\my\***"
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
		printf("���� %d ����!\n", format);
		return False;
	}
	fclose(fuPack);
	return True;
}

/*---------------------------*/

//File_Run���캯��

 
//��� 
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
		printf("���ʧ��!\n");
	else
		printf("����ɹ�!\n");

	printf("�ļ���: %d\n", File_Cnt);
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
			printf("�г���Ϣʧ��!\n");
	}
	else{
		if(-1 == F_L.List_Info(argv2, Cnt, format))
			printf("�г���Ϣʧ��!\n");
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
			printf("���ʧ��!\n");
		else
			printf("����ɹ�!\n");
	}
	else{
		if(-1 == F_U.UnPack(argv2, argv3, Cnt, format))
			printf("���ʧ��!\n");
		else
			printf("����ɹ�!\n");
	}
	fclose(fcnt);
}

/*---------------------------*/

//File_Judge���캯�� 
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
