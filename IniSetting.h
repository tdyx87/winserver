#ifndef __INI_SETTING__
#define __INI_SETTING__
#include "windows.h"
#include <string>
using namespace std;

class IniSetting
{
public:
	IniSetting(char *inifile);
	~IniSetting();
	char *readIni(char *section,char*key);
	void setini(char *section,char *key,char *value);
	static string getCurrentPath(HANDLE handle=0);


private:
	char *m_iniFile;
};



#endif