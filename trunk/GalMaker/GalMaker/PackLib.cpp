#include<iostream>
#include<windows.h>
#include<stdio.h>
#include"PackLib.h"

#define BUFSIZE	65536
#define FILEID	"Kylinx File Packer"
class CPackFileInstance:public CPackFile
{
	FILE*m_fp;
	struct STPackerInfo
	{
		DWORD dwOffsite;
		DWORD dwSize;
	};
	

	virtual BOOL OpenPackFile(char*szPackFile);
	virtual BOOL ClosePackFile();
	virtual BOOL GetFileInfo(char*szPackFile,DWORD &dwOffsite,DWORD &dwSize);
	virtual BOOL CreateTempFile(char*szPackFile,char*szTempFile);
private:
	BOOL Pack(char* szDir,char*szPackFileName);
	BOOL UnPack(char*szPackFileName,char* szDestDir);
	BOOL CreateTempFile(char* szFileInPackName);
	BOOL PackFile(FILE*fp,char*szFileName);
	BOOL ListPackedFile(char*szPackName);
	BOOL ListPackedFile(FILE*fpPack);
	BOOL GetFileInfoFromPack(FILE*fpPack,char*szFile,STPackerInfo*pPackInfo);
	int Pack(FILE*fpPack,char*dir);
public:
	CPackFileInstance();
	virtual ~CPackFileInstance();
};
CPackFile* CPackFile::CreatePackFileInstance()
{
	return new CPackFileInstance();
}
CPackFileInstance::CPackFileInstance()
{
	m_fp=NULL;
}
CPackFileInstance::~CPackFileInstance()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp=NULL;
	}
}
BOOL CPackFileInstance::GetFileInfo(char*szPackFile,DWORD &dwOffsite,DWORD &dwSize)
{
	STPackerInfo info;
	if(!GetFileInfoFromPack(m_fp,szPackFile,&info))
		return false;
	dwOffsite=info.dwOffsite;
	dwSize=info.dwSize;
	return true;
}
BOOL CPackFileInstance::CreateTempFile(char*szPackFile,char*szTempFile)
{
	if(!szPackFile || !szTempFile)
		return false;
	if(!m_fp)
		return false;
	rewind(m_fp);
	fseek(m_fp,strlen(FILEID),SEEK_SET);
	DWORD dwOffsite=strlen(FILEID);
	char szName[4096];
	int nLen;
	DWORD dwSize=0;
	while(1)
	{
		memset(szName,0,4096);
		if(!fread(&nLen,sizeof(int),1,m_fp))
			break;
		fread(szName,nLen,1,m_fp);
		fread(&dwSize,sizeof(DWORD),1,m_fp);
		dwOffsite+=sizeof(int)+nLen+sizeof(DWORD);
		if(strcmp(strlwr(szName),strlwr(szPackFile))==0)
		{
			SetFileAttributes(szTempFile,FILE_ATTRIBUTE_NORMAL);	//确保文件属性为可写
			FILE*fp=fopen(szTempFile,"wb");
			if(!fp)
				return false;
			char*pFileBuf=new char[BUFSIZE];
			if(!pFileBuf)
				return false;
			for(;(int)dwSize>0;)
			{
				DWORD dwBufSize=dwSize >BUFSIZE ? BUFSIZE :dwSize;
				::fread(pFileBuf,dwBufSize,1,m_fp);
				::fwrite(pFileBuf,dwBufSize,1,fp);
				dwSize-=BUFSIZE;
			}
			delete [] pFileBuf;
			fclose(fp);
			return true;
		}	
		dwOffsite+=dwSize;
		fseek(m_fp,dwSize,SEEK_CUR);
	}
	return false;
}
BOOL CPackFileInstance::ClosePackFile()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp=NULL;
		return true;
	}
	return false;
}
BOOL CPackFileInstance::OpenPackFile(char*szPackFile)
{
	ClosePackFile();
	m_fp=fopen(szPackFile,"rb");
	if(!m_fp)
	{
		return false;
	}
	else
	{
		char str[256];
		memset(str,0,256);
		fread(str,strlen(FILEID),1,m_fp);
		if(strcmp(str,FILEID)!=0)
		{
			fclose(m_fp);
			return false;
		}
	}
	return true;
}
BOOL CPackFileInstance::CreateTempFile(char* szFileInPackName)
{
	if(!szFileInPackName)
		return false;
	rewind(m_fp);
	fseek(m_fp,strlen(FILEID),SEEK_SET);
	DWORD dwOffsite=strlen(FILEID);
	char szName[4096];
	int nLen;
	DWORD dwSize=0;
	while(1)
	{
		memset(szName,0,4096);
		if(!fread(&nLen,sizeof(int),1,m_fp))
			break;
		fread(szName,nLen,1,m_fp);
		fread(&dwSize,sizeof(DWORD),1,m_fp);
		dwOffsite+=sizeof(int)+nLen+sizeof(DWORD);
		if(strcmp(strlwr(szName),strlwr(szFileInPackName))==0)
		{
			SetFileAttributes(szName,FILE_ATTRIBUTE_NORMAL);	//确保文件属性为可写
			FILE*fp=fopen(szName,"wb");
			if(!fp)
				return false;
			char*pFileBuf=new char[BUFSIZE];
			if(!pFileBuf)
				return false;
			for(;(int)dwSize>0;)
			{
				DWORD dwBufSize=dwSize >BUFSIZE ? BUFSIZE :dwSize;
				::fread(pFileBuf,dwBufSize,1,m_fp);
				::fwrite(pFileBuf,dwBufSize,1,fp);
				dwSize-=BUFSIZE;
			}
			delete [] pFileBuf;
			fclose(fp);
			return true;
		}	
		dwOffsite+=dwSize;
		fseek(m_fp,dwSize,SEEK_CUR);
	}
	return false;
}
BOOL CPackFileInstance::Pack(char*szDir,char*szPackFileName)
{
	if(!szDir)
		return false;
	if(!szPackFileName)
		return false;
	SetFileAttributes(szPackFileName,FILE_ATTRIBUTE_NORMAL);	//确保文件属性为可写
	FILE*fp=fopen(szPackFileName,"wb");
	BOOL bRet=Pack(fp,szDir);
	fclose(fp);
	return bRet;
}
BOOL CPackFileInstance::UnPack(char*szPackFileName,char*szDestDir)
{

	if(!szPackFileName || !szDestDir)
		return false;
#ifdef _CONSOLE
	cout<<"Begin UnPack File: "<<szPackFileName<<"..."<<endl;
#endif
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
#ifdef _CONSOLE
		cout<<"Not A KPack File!"<<endl;
#endif
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

		FILE*fp=fopen(szDestName,"wb");
		if(!fp)
		{
			
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

	}
	delete [] pFileBuf;
	fclose(fpPack);
#ifdef _CONSOLE
	cout<<"UnPack File: "<<szPackFileName<<" OK!"<<endl;
#endif
	return true;
}

BOOL CPackFileInstance::PackFile(FILE*fp,char*szFileName)
{
	
	HANDLE hf=NULL;
	DWORD dwFileSize=0;
	char*pFileBuf=new char[BUFSIZE];
	if(!pFileBuf)
		return false;
	hf=::CreateFile(szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,SECURITY_ANONYMOUS);
	if(INVALID_HANDLE_VALUE!=hf)
	{
#ifdef _CONSOLE
		cout<<"Packing File: "<<szFileName<<"..."<<endl;
#endif
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
#ifdef _CONSOLE
		cout<<"Packing File: "<<szFileName<<" OK!"<<endl;
		cout<<"----------------------------------------------------------------------------"<<endl;
#endif
	}
	delete [] pFileBuf;

	return true;
}
BOOL CPackFileInstance::ListPackedFile(char*szPackName)
{
	FILE*fpPack=fopen(szPackName,"rb");
	if(!fpPack)
		return false;
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
#ifdef _CONSOLE
		cout<<"File:"<<szName<<" Size="<<dwSize<<" Offsite="<<dwOffsite<<endl;
#endif
		fseek(fpPack,dwSize,SEEK_CUR);
		dwOffsite+=dwSize;
	}
	fclose(fpPack);
	return true;
}
BOOL CPackFileInstance::ListPackedFile(FILE*fpPack)
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
#ifdef _CONSOLE
		cout<<"File:"<<szName<<" Size="<<dwSize<<" Offsite="<<dwOffsite<<endl;
#endif
		fseek(fpPack,dwSize,SEEK_CUR);
		dwOffsite+=dwSize;
	}
	return true;
}
BOOL CPackFileInstance::GetFileInfoFromPack(FILE*fpPack,char*szFile,STPackerInfo*pPackInfo)
{
	if(!szFile || !pPackInfo)
		return false;
	if(!fpPack)
		return false;
	rewind(fpPack);
	fseek(fpPack,strlen(FILEID),SEEK_SET);
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

int CPackFileInstance::Pack(FILE*fpPack,char*dir)
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
		if(strcmp(fd.cFileName,".")==0 || strcmp(fd.cFileName,"..")==0)
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
					Pack(fpPack,fd.cFileName);
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
					Pack(fpPack,str);
				}
			}
			
		}
		while(::FindNextFile(hff,&fd))
		{
			if(strcmp(fd.cFileName,".")==0 || strcmp(fd.cFileName,"..")==0)
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
						Pack(fpPack,fd.cFileName);
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
						Pack(fpPack,str);

					}
				}
			}
		}
	}
	return 1;
}
