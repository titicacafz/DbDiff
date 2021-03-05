#pragma once
#include <string>

//数据库对象比较差异信息
struct CompareDiff final {    

    //数据库对象名
    std::string object_name;

    //对象差异
    std::string object_difference;

    //对象列或者参数差异
    std::string columns_difference;

    //数据库对象类型
    std::string type;
};
