#pragma once
#include <string>
#include <soci/soci.h>

//表结构信息类
struct TableMeta final
{
    //表名
    std::string name;

    //表注释
    std::string comment;

    //表用户
    std::string owner;

    //表类型:TABLE 或 VIEW
    std::string type;
};

namespace soci {
    template<>
    struct type_conversion<TableMeta>
    {
        typedef values base_type;
        static void from_base(const values& v, indicator ind, TableMeta& tableMeta)
        {           
            
            tableMeta.name = v.get<std::string>("TABLE_NAME", "");
            tableMeta.comment = v.get<std::string>("COMMENTS", "");
            tableMeta.owner = v.get<std::string>("OWNER", "");
            tableMeta.type = v.get<std::string>("TABLE_TYPE", "");
        }
        /*
        static void to_base(const TableMeta& tableMeta, values& v, indicator& ind)
        {
            v.set("TABLE_NAME", tableMeta.name);
            v.set("COMMENTS", tableMeta.comment);
            v.set("OWNER", tableMeta.owner);
            v.set("TABLE_TYPE", tableMeta.type);            
        }
        */
    };
}

