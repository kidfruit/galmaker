//打包类
//创建于2003/8/14 by Kylinx

#ifndef KLIB_PACK_H_
#define KLIB_PACK_H_

#include<windows.h>

class CPackFile
{
public:
	static CPackFile* CreatePackFileInstance();
	virtual BOOL OpenPackFile(char*szPackFile)=0;
	virtual BOOL ClosePackFile()=0;
	virtual BOOL GetFileInfo(char*szPackFile,DWORD &dwOffsite,DWORD &dwSize)=0;
	virtual BOOL CreateTempFile(char*szPackFile,char*szTempFile)=0;
};


#endif