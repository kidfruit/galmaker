#include<iostream>
#include<windows.h>
#include<stdio.h>
//#include"pack.h"
const DWORD BUF_4M =65536;//64K Buffer 0x100000;//1MB Buffer
const DWORD BUF_8M =BUF_4M*2;
const DWORD BUF_16M =BUF_8M*2;
const DWORD BUF_32M =BUF_16M*2;
const DWORD BUF_64M =BUF_32M*2;

DWORD BUFSIZE=BUF_4M;
#define FILEID	"Kylinx File Packer"

using namespace std;

struct STPackerInfo
{
	DWORD dwOffsite;
	DWORD dwSize;
};
//卸包
BOOL UnPack(char*szPackFileName,char*szDestDir)
{

	if(!szPackFileName || !szDestDir)
		return false;

	::CreateDirectory(szDestDir,NULL);
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Begin UnPack File: "<<szPackFileName<<"..."<<endl;

	WIN32_FIND_DATA fd;
	char szDestName[4096];
	wsprintf(szDestName,"%s\\*.*",szDestDir);
	HANDLE hff=::FindFirstFile(szDestName,&fd);
	if(INVALID_HANDLE_VALUE==hff)
		return false;
	FindClose(hff);

	FILE*fpPack=fopen(szPackFileName,"rb");
	if(!fpPack)
		return false;
	
	char strFileID[128];
	memset(strFileID,0,128);
	fread(strFileID,strlen(FILEID),1,fpPack);
	if(strcmp(strFileID,FILEID)!=0)
	{

		cout<<"Not A KPack File!"<<endl;

		fclose(fpPack);
		return false;
	}

	char szName[4096];
	int nLen;
	DWORD dwSize=0;
	char*pFileBuf=new char[BUFSIZE];
	if(!pFileBuf)
		return false;
	
	while(1)
	{
		memset(szName,0,4096);
		if(!fread(&nLen,sizeof(int),1,fpPack))
			break;
		fread(szName,nLen,1,fpPack);
		fread(&dwSize,sizeof(DWORD),1,fpPack);

		wsprintf(szDestName,"%s\\%s",szDestDir,szName);
		SetFileAttributes(szDestName,FILE_ATTRIBUTE_NORMAL);	//确保文件属性为可写

		char szCreateDirName[4096];
		memset(szCreateDirName,0,4096);
		int nLength=strlen(szDestName);
		nLength--;
		while(nLength>=0 && szDestName[nLength]!='\\')
		{
			nLength--;
		}
		strncpy(szCreateDirName,szDestName,nLength);
		::CreateDirectory(szCreateDirName,NULL);
		cout<<"----------------------------------------------------------------------------"<<endl;
		cout<<"Unpacking File "<<szDestName<<"..."<<endl;
		FILE*fp=fopen(szDestName,"wb");
		if(!fp)
		{
			cout<<"Unpacking File "<<szDestName<<" Failed!"<<endl;
			delete [] pFileBuf;
			return false;
		}
		for(;(int)dwSize>0;)
		{
			DWORD dwBufSize=dwSize >BUFSIZE ? BUFSIZE :dwSize;
			::fread(pFileBuf,dwBufSize,1,fpPack);
			::fwrite(pFileBuf,dwBufSize,1,fp);
			dwSize-=BUFSIZE;
		}
		
		fclose(fp);
		cout<<"Unpacking "<<szDestName<<" OK!"<<endl;
	}
	delete [] pFileBuf;
	fclose(fpPack);

	cout<<"UnPack File: "<<szPackFileName<<" OK!"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	return true;
}
BOOL PackFile(FILE*fp,char*szFileName)
{
	
	HANDLE hf=NULL;
	DWORD dwFileSize=0;
	char*pFileBuf=new char[BUFSIZE];
	if(!pFileBuf)
		return false;
	hf=::CreateFile(szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,SECURITY_ANONYMOUS);
	if(INVALID_HANDLE_VALUE!=hf)
	{
		cout<<"Packing File: "<<szFileName<<"..."<<endl;
		int szLen=strlen(szFileName);
		fwrite(&szLen,sizeof(int),1,fp);
		fwrite(szFileName,szLen,1,fp);
		dwFileSize=::GetFileSize(hf,0);
		fwrite(&dwFileSize,sizeof(DWORD),1,fp);
		OVERLAPPED ovlped;
		ZeroMemory(&ovlped,sizeof(ovlped));
		for(;(int)dwFileSize>0;)
		{
			DWORD dwBufSize=dwFileSize>BUFSIZE ? BUFSIZE :dwFileSize;
			DWORD dwSizeRead;
			::ReadFile(hf,pFileBuf,dwBufSize,&dwSizeRead,&ovlped);
			::fwrite(pFileBuf,dwBufSize,1,fp);
			ovlped.Offset+=dwBufSize;
			dwFileSize-=BUFSIZE;
		}
		CloseHandle(hf);
		cout<<"Packing File: "<<szFileName<<" OK!"<<endl;
		cout<<"----------------------------------------------------------------------------"<<endl;
	}
	delete [] pFileBuf;

	return true;
}

BOOL ListPackedFile(char*szPackName)
{
	FILE*fpPack=fopen(szPackName,"rb");
	if(!fpPack)
		return false;
	char str[1024];
	memset(str,0,1024);
	fread(str,strlen(FILEID),1,fpPack);
	if(strcmp(str,FILEID)!=0)
	{
		fclose(fpPack);
		cout<<"File: "<<szPackName<<" Is Not A kPack File!"<<endl;
		exit(0);
		return false;
	}
	DWORD dwOffsite=strlen(FILEID);
	char szName[4096];
	int nLen;
	DWORD dwSize=0;
	while(1)
	{
		memset(szName,0,4096);
		if(!fread(&nLen,sizeof(int),1,fpPack))
			break;
		fread(szName,nLen,1,fpPack);
		fread(&dwSize,sizeof(DWORD),1,fpPack);
		dwOffsite+=sizeof(int)+sizeof(DWORD)+nLen;
		cout<<"File:"<<szName<<" Size="<<dwSize<<" Offsite="<<dwOffsite<<endl;
		fseek(fpPack,dwSize,SEEK_CUR);
		dwOffsite+=dwSize;
	}
	fclose(fpPack);
	return true;
}
BOOL ListPackedFile(FILE*fpPack)
{
	rewind(fpPack);
	fseek(fpPack,strlen(FILEID),SEEK_SET);
	DWORD dwOffsite=strlen(FILEID);
	char szName[4096];
	int nLen;
	DWORD dwSize=0;
	while(1)
	{
		memset(szName,0,4096);
		if(!fread(&nLen,sizeof(int),1,fpPack))
			break;
		fread(szName,nLen,1,fpPack);
		fread(&dwSize,sizeof(DWORD),1,fpPack);
		dwOffsite+=sizeof(int)+sizeof(DWORD)+nLen;
		cout<<"File:"<<szName<<" Size="<<dwSize<<" Offsite="<<dwOffsite<<endl;
		fseek(fpPack,dwSize,SEEK_CUR);
		dwOffsite+=dwSize;
	}
	return true;
}
BOOL ExtractFileFromPack(FILE*fpPack,char*szFile,STPackerInfo*pPackInfo)
{
	if(!szFile)
	{
		cout<<"Bad Param!"<<endl;
		return false;
	}
	rewind(fpPack);
	char str[1024];
	memset(str,0,1024);
	fread(str,strlen(FILEID),1,fpPack);
	if(strcmp(str,FILEID)!=0)
	{
		fclose(fpPack);
		cout<<"File Is Not A kPack File!"<<endl;
		exit(0);
		return false;
	}
	pPackInfo->dwOffsite=strlen(FILEID);
	char szName[4096];
	int nLen;
	DWORD dwSize=0;

	char *pFileBuf=new char[BUFSIZE];
	if(!pFileBuf)
	{
		cout<<"Out Of Memory!"<<endl;
		return false;
	}
	while(1)
	{
		memset(szName,0,4096);
		if(!fread(&nLen,sizeof(int),1,fpPack))
			break;
		fread(szName,nLen,1,fpPack);
		fread(&dwSize,sizeof(DWORD),1,fpPack);
		pPackInfo->dwOffsite+=sizeof(int)+nLen+sizeof(DWORD);
		if(strcmp(strlwr(szName),strlwr(szFile))==0)
		{
			pPackInfo->dwSize=dwSize;
			SetFileAttributes(szFile,FILE_ATTRIBUTE_NORMAL);	//确保文件属性为可写
			FILE*fp=fopen(szFile,"wb");
			if(!fp)
			{
				cout<<"Create File "<<szFile<<" Failed!"<<endl;
				delete [] pFileBuf;
				return false;
			}
			for(;(int)dwSize>0;)
			{
				DWORD dwBufSize=dwSize>BUFSIZE ? BUFSIZE :dwSize;
				::fread(pFileBuf,dwBufSize,1,fpPack);
				::fwrite(pFileBuf,dwBufSize,1,fp);
				dwSize-=BUFSIZE;
			}
			fclose(fp);
			delete [] pFileBuf;
			return true;
		}	
		pPackInfo->dwOffsite+=dwSize;
		fseek(fpPack,dwSize,SEEK_CUR);
	}
	delete [] pFileBuf;
	return false;
}
BOOL GetFileFromPack(FILE*fpPack,char*szFile,STPackerInfo*pPackInfo)
{
	if(!szFile || !pPackInfo)
		return false;
	rewind(fpPack);

	char str[1024];
	memset(str,0,1024);
	fread(str,strlen(FILEID),1,fpPack);
	if(strcmp(str,FILEID)!=0)
	{
		fclose(fpPack);
		cout<<"File Is Not A kPack File!"<<endl;
		exit(0);
		return false;
	}
	pPackInfo->dwOffsite=strlen(FILEID);
	char szName[4096];
	int nLen;
	DWORD dwSize=0;
	while(1)
	{
		memset(szName,0,4096);
		if(!fread(&nLen,sizeof(int),1,fpPack))
			break;
		fread(szName,nLen,1,fpPack);
		fread(&dwSize,sizeof(DWORD),1,fpPack);
		pPackInfo->dwOffsite+=sizeof(int)+nLen+sizeof(DWORD);
		if(strcmp(strlwr(szName),strlwr(szFile))==0)
		{
			pPackInfo->dwSize=dwSize;
			return true;
		}	
		pPackInfo->dwOffsite+=dwSize;
		fseek(fpPack,dwSize,SEEK_CUR);
	}
	return false;
}
void ErrorParam(char*exe)
{
	cout<<"Add File To Pack File: "<<exe<<" -a [Dir] PackFileName [/1/2/3/4/5]"<<endl;
	cout<<"List All File In Pack: "<<exe<<" -l PackFileName [/1/2/3/4/5]"<<endl;
	cout<<"Find File In Pack: "<<exe<<" -f filename PackFileName [/1/2/3/4/5]"<<endl;
	cout<<"UnPack File From Pack: "<<exe<<" -u PackFileName DestDir [/1/2/3/4/5]"<<endl;
	cout<<"Extract A File From Pack: "<<exe<<" -e filename PackFileName [/1/2/3/4/5]"<<endl;
	exit(0);
}
int Pack(FILE*fpPack,char*dir,char*szExeFile)
{
	char szDir[1024];
	if(!dir || strcmp(dir,"")==0)
	{
		strcpy(szDir,"*.*");
	}
	else
		wsprintf(szDir,"%s\\*.*",dir);
	WIN32_FIND_DATA fd;
	HANDLE hff=::FindFirstFile(szDir,&fd);
	if(INVALID_HANDLE_VALUE!=hff)
	{
		if(strcmp(fd.cFileName,".")==0 || strcmp(fd.cFileName,"..")==0 || strcmp(szExeFile,strlwr(fd.cFileName))==0)
		{
		}
		else
		{
			FILE*fp=NULL;
			if(szDir[0]=='*')
			{
				fp=fopen(fd.cFileName,"r");
				if(fp)
				{
					fclose(fp);
					PackFile(fpPack,fd.cFileName);
				}
				else
				{
					Pack(fpPack,fd.cFileName,szExeFile);
				}
			}
			else
			{
				char str[1024];
				wsprintf(str,"%s\\%s",dir,fd.cFileName);
				fp=fopen(str,"r");
				if(fp)
				{
					fclose(fp);
					PackFile(fpPack,str);
				}
				else
				{
					Pack(fpPack,str,szExeFile);
				}
			}
			
		}
		while(::FindNextFile(hff,&fd))
		{
			if(strcmp(fd.cFileName,".")==0 || strcmp(fd.cFileName,"..")==0 || strcmp(szExeFile,strlwr(fd.cFileName))==0)
			{
			}
			else
			{
				FILE*fp=NULL;
				if(szDir[0]=='*')
				{
					fp=fopen(fd.cFileName,"r");
					if(fp)
					{
						fclose(fp);
						PackFile(fpPack,fd.cFileName);
					}
					else
					{
						Pack(fpPack,fd.cFileName,szExeFile);
					}
				}
				else
				{
					char str[1024];
					wsprintf(str,"%s\\%s",dir,fd.cFileName);
					fp=fopen(str,"r");
					if(fp)
					{
						fclose(fp);
						PackFile(fpPack,str);
					}
					else
					{
						Pack(fpPack,str,szExeFile);

					}
				}
			}
		}
	}
	return 1;
}
int main(int argc,char**argv)
{
	BUFSIZE=BUF_4M;
	cout<<"KPACK 1.0 2003/8/14,Author:Kylinx"<<endl;
	if(argc==5)
	{
		if(strcmp(argv[4],"/1")==0)
		{
			argc=4;
			BUFSIZE=BUF_4M;
		}
		else if(strcmp(argv[4],"/2")==0)
		{
			argc=4;
			BUFSIZE=BUF_8M;
		}
		else if(strcmp(argv[4],"/3")==0)
		{
			argc=4;
			BUFSIZE=BUF_16M;
		}
		else if(strcmp(argv[4],"/4")==0)
		{
			argc=4;
			BUFSIZE=BUF_32M;
		}
		else if(strcmp(argv[4],"/5")==0)
		{
			BUFSIZE=BUF_64M;
			argc=4;
		}
	}
	else if(argc==4)
	{
		if(strcmp(argv[3],"/1")==0)
		{
			argc=3;
			BUFSIZE=BUF_4M;
		}
		else if(strcmp(argv[3],"/2")==0)
		{
			argc=3;
			BUFSIZE=BUF_8M;
		}
		else if(strcmp(argv[3],"/3")==0)
		{
			argc=3;
			BUFSIZE=BUF_16M;
		}
		else if(strcmp(argv[3],"/4")==0)
		{
			argc=3;
			BUFSIZE=BUF_32M;
		}
		else if(strcmp(argv[3],"/5")==0)
		{
			BUFSIZE=BUF_64M;
			argc=3;
		}
	}
	cout<<"Using Buffer Size: "<<BUFSIZE<<" Bytes."<<endl;
	if(argc==3)
	{
		if(strcmp(strlwr(argv[1]),"-a")==0)
		{
			FILE*fp=fopen(argv[2],"wb");
			if(!fp)
			{
				cout<<"Error: Can't Create Pack File "<<argv[2]<<endl;
				return 0;
			}
			fwrite(FILEID,strlen(FILEID),1,fp);
			
			char szExeFile[1024];
			memset(szExeFile,0,1024);
			int nLength=strlen(argv[0]);
			BOOL bRet;
			if(nLength>4)
			{
				if( (argv[0][nLength]=='e' || argv[0][nLength]=='E')
					&& (argv[0][nLength-1]=='x' || argv[0][nLength-1]=='X')
					&& (argv[0][nLength-2]=='e' || argv[0][nLength-2]=='E')
					&& (argv[0][nLength-3]=='.'))
					bRet=Pack(fp,"",argv[0]);
				else
				{
					wsprintf(szExeFile,"%s.exe",argv[0]);
					bRet=Pack(fp,"",szExeFile);
				}
			}
			else
			{
				wsprintf(szExeFile,"%s.exe",argv[0]);
				bRet=Pack(fp,"",szExeFile);
			}
			fclose(fp);
			return bRet;
		}
		
		else if(strcmp(strlwr(argv[1]),"-l")==0)
			return ListPackedFile(argv[2]);
		else
			ErrorParam(argv[0]);
	}
	else if(argc==4)
	{
		if(strcmp(strlwr(argv[1]),"-a")==0)
		{
			FILE*fp=fopen(argv[3],"wb");
			if(!fp)
				return 0;
			fwrite(FILEID,strlen(FILEID),1,fp);
			char szExeFile[1024];
			memset(szExeFile,0,1024);
			int nLength=strlen(argv[0]);
			BOOL bRet;
			if(nLength>4)
			{
				if( (argv[0][nLength]=='e' || argv[0][nLength]=='E')
					&& (argv[0][nLength-1]=='x' || argv[0][nLength-1]=='X')
					&& (argv[0][nLength-2]=='e' || argv[0][nLength-2]=='E')
					&& (argv[0][nLength-3]=='.'))
					bRet=Pack(fp,"",argv[0]);
				else
				{
					wsprintf(szExeFile,"%s.exe",argv[0]);
					bRet=Pack(fp,argv[2],szExeFile);
				}
			}
			else
			{
				wsprintf(szExeFile,"%s.exe",argv[0]);
				bRet=Pack(fp,argv[2],szExeFile);
			}
			fclose(fp);
			return bRet;
		}
		else if(strcmp(strlwr(argv[1]),"-u")==0)
		{
			return UnPack(argv[2],argv[3]);
		}
		else if(strcmp(strlwr(argv[1]),"-f")==0)
		{
			FILE*fp=fopen(argv[3],"rb");
			if(!fp)
			{
				cout<<"Error:File "<<argv[3]<<" doesn't exist!"<<endl;
				return 0;
			}
			STPackerInfo PackInfo;
			BOOL bRet=GetFileFromPack(fp,strlwr(argv[2]),&PackInfo);
			fclose(fp);
			if(bRet)
			{
				cout<<"Find File "<<argv[2]<<" OK! Size="<<PackInfo.dwSize<<" Offsite="<<PackInfo.dwOffsite<<endl;
			}
			else
			{
				cout<<"Find File "<<argv[2]<<" Failed In File "<<argv[3]<<" !"<<endl;
			}
			return bRet;
		}
		else if(strcmp(strlwr(argv[1]),"-e")==0)
		{
			FILE*fp=fopen(argv[3],"rb");
			if(!fp)
			{
				cout<<"Error:File "<<argv[3]<<" doesn't exist!"<<endl;
				return 0;
			}
			STPackerInfo PackInfo;
			BOOL bRet=ExtractFileFromPack(fp,strlwr(argv[2]),&PackInfo);
			fclose(fp);
			if(bRet)
			{
				cout<<"Extract File "<<argv[2]<<" OK! Size="<<PackInfo.dwSize<<" Offsite="<<PackInfo.dwOffsite<<endl;
			}
			else
			{
				cout<<"File "<<argv[2]<<" Doesn't Exsit In File "<<argv[3]<<" !"<<endl;
			}
			return bRet;
		}
		else
			ErrorParam(argv[0]);
	
	}
	else 
		ErrorParam(argv[0]);
	return 0;
}
//编译成为Exe文件后，比如使用命令pack -a test.pak就能把当前目录和子目录下所有文件打包到test.pak中
//要使用这个文件，请看packlib.h,packlib.cpp