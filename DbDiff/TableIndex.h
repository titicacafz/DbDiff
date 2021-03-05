#pragma once
#include <string>
#include <soci/soci.h>

//表结构信息类
struct TableIndex final
{
    //拥有者
    std::string owner;
    
    //索引名
    std::string index_name;

    //索引表
    std::string table_name;

    //索引字段
    std::string column;
};

namespace soci {
    template<>
    struct type_conversion<TableIndex>
    {
        typedef values base_type;
        static void from_base(const values& v, indicator ind, TableIndex& tableMeta)
        {

            tableMeta.owner = v.get<std::string>("INDEX_OWNER", "");
            tableMeta.index_name = v.get<std::string>("INDEX_NAME", "");
            tableMeta.table_name = v.get<std::string>("TABLE_NAME", "");
            tableMeta.column = v.get<std::string>("COLUMN_NAME", "");
        }        
    };
}