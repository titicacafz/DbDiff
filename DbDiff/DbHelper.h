#pragma once
#include <map>
#include <set>
#include <list>
#include "TableMeta.h"
#include "TableColumnMeta.h"
#include "ProcedureMeta.h"
#include "ProcedureArgumentsMeta.h"
#include "TableIndex.h"


struct IndexMeta
{
    std::string index_name;
    std::string table_name;
    std::vector<TableIndex> index_columns;
};

//数据库结构信息获取的工具类
class DbHelper final
{
public:
    using StringSet = std::set<std::string>;
    using TableList = std::map<std::string, TableMeta>;
    using ColumnList = std::map<std::string, TableColumnMeta>;
    using ProcedureList = std::map<std::string, ProcedureMeta>;
    using ProcedureArgumentList = std::map<std::string, ProcedureArgumentMeta>;
    using AllProcedureArgs = std::map<std::string, std::list<ProcedureArgumentMeta>>;
    using IndexList = std::map<std::string, IndexMeta>;
    
    
    //获得表信息
    static TableList get_table_and_views(const std::set<std::string> & owners,
        soci::session & sql);

    //汇总表    
    template<typename T>
    static StringSet merge_map_key(const T & src, const T & dest)
    {
        StringSet table_set;
        for (auto & v : src) {
            table_set.emplace(v.first);
        }
        for (auto & v : dest) {
            table_set.emplace(v.first);
        }
        return std::move(table_set);
    }

    //获取表的字段信息
    static ColumnList get_table_column(
        const std::string & table_name,
        const std::string & table_owner,
        soci::session & sql);

    //表用户列表初始化
    static StringSet set_from_string(const std::string & );

    //从文件获取set
    static StringSet set_from_file(const std::string &);

    //获得函数和过程信息
    static ProcedureList get_procedure_and_function(soci::session & sql);

    //获得过程和函数的出入参
    static ProcedureArgumentList get_procedure_arguments(std::string proc, soci::session & sql);    

    static AllProcedureArgs get_all_procedure_args(soci::session & sql);

    static IndexList get_all_indexs(soci::session & sql);
};

