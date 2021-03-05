#include "Config.h"
#include "SystemUtil.h"
#include "DbHelper.h"




//��ȡini��ĳ�ֶε�ֵ
std::string Config::read_val(const std::string & sector, const std::string name)
{
	std::string inifile = SystemUtil::get_module_path() + "app.conf";
	std::map<std::string, std::string> fconfig;
    conf_file.read_config(inifile, fconfig, sector.c_str());
	return fconfig[name];
}

