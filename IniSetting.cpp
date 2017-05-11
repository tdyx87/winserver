#include "stdafx.h"

#include "iniSetting.h"

#include "windows.h"

IniSetting::IniSetting(char *iniFile)
{
	string currentAppPath = getCurrentPath();

	string tmpinifile=currentAppPath+"\\"+iniFile;

    const char *tmp =  (tmpinifile.data());
	
	m_iniFile = new char[strlen(tmp)+1];
	memset(m_iniFile,0,strlen(tmp)+1);
	memcpy(m_iniFile,tmp,strlen(tmp));

}

IniSetting::~IniSetting()
{
	delete m_iniFile;
}

string IniSetting::getCurrentPath(HANDLE handle)
{
	char szAppPath[MAX_PATH]; 
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	(strrchr(szAppPath, '\\'))[0] = 0; 
	return string(szAppPath);
}

char *IniSetting::readIni(char *section,char *key)
{
	char buffer[4096]={'\0'};
	long size = 4096;
	::GetPrivateProfileString(section,key,NULL,buffer,size, m_iniFile);
	char *result = new char[4096];
	memset(result,'\0',4096);
	memcpy(result,buffer,strlen(buffer));
	return result;
}

void IniSetting::setini(char* section,char *key,char *value)
{
	::WritePrivateProfileString(section,key,value,m_iniFile);
}