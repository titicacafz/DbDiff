#include "Config.h"
#include "SystemUtil.h"
#include "DbHelper.h"




//获取ini中某字段的值
std::string Config::read_val(const std::string & sector, const std::string name)
{
	std::string inifile = SystemUtil::get_module_path() + "app.conf";
	std::map<std::string, std::string> fconfig;
    conf_file.read_config(inifile, fconfig, sector.c_str());
	return fconfig[name];
}

