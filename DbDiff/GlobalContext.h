#pragma once
#include <string>
#include "Config.h"


class GlobalContext
{
public:
	
	//唯一实例 只要这个类就好了
	static GlobalContext & instance()
	{
		static GlobalContext context;
		return context;
	}	

	Config config;
};

#define  CONTEXT GlobalContext::instance()
