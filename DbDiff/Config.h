#pragma once
#include "ConfigFile.h"
//
class Config
{
public:
	Config()
	{}
	~Config() 
	{}
	

    //获取ini中某字段的值
    std::string read_val(const std::string & sector, const std::string name);

private:
	
    ConfigFile conf_file;       //	
};

//extern Config DLL_CONFIG;
