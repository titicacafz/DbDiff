#pragma once
#include <string>
#include "Config.h"


class GlobalContext
{
public:
	
	//Ψһʵ�� ֻҪ�����ͺ���
	static GlobalContext & instance()
	{
		static GlobalContext context;
		return context;
	}	

	Config config;
};

#define  CONTEXT GlobalContext::instance()
