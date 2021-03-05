#pragma once

#include <map>
#include "DbHelper.h"
#include "TableDiff.h"

class CompareContext;

//数据库比较的工具函数类
class DbCompareUtil final
{
public:
    using DiffList = std::map<std::string, CompareDiff>;
    
    //比较数据库表结构
    static DiffList compare_table(CompareContext & context);      

    //比较数据库过程差异
    static DiffList compare_procedure(CompareContext &); 

    //比较数据库表索引差异
    static DiffList compare_index(CompareContext &);

    //比较mybatis配置和源数据库的差异
    static DiffList check_mybatis_mappers(CompareContext&);
    
};

