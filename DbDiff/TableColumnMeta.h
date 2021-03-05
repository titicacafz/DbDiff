#pragma once
#include <string>

//表字段的信息类
struct TableColumnMeta final
{
    //列名
    std::string  name;

    //列注释
    std::string  comments;

    //类型
    std::string  type;

    //长度
    double  length;

    //是否允许为空
    std::string  nullable;

    //默认值
    std::string  default_val;
};

namespace soci {
    template<>
    struct type_conversion<TableColumnMeta>
    {
        typedef values base_type;
        static void from_base(const values& v, indicator ind, TableColumnMeta& tableColumnMeta)
        {
            try
            {
                tableColumnMeta.name = v.get<std::string>("COLUMN_NAME", "");
                tableColumnMeta.comments = "";
                tableColumnMeta.type = v.get<std::string>("DATA_TYPE", "");
                //tableColumnMeta.length = v.get<double>("DATA_LENGTH", -1);
                tableColumnMeta.nullable = v.get<std::string>("NULLABLE", "");
                //tableColumnMeta.default_val = v.get<std::string>("DATA_DEFAULT", "");
            }
            catch (const std::exception& e)
            {
                throw e;
            }
                                   
        }
        /*
        static void to_base(const TableColumnMeta& tableColumnMeta, values& v, indicator& ind)
        {
           v.set("COLUMN_NAME", tableColumnMeta.name);
           v.set("COMMENTS", tableColumnMeta.comments);
           v.set("DATA_TYPE", tableColumnMeta.type);
           v.set("DATA_LENGTH", tableColumnMeta.length);
           v.set("NULLABLE", tableColumnMeta.nullable);
           v.set("DATA_DEFAULT", tableColumnMeta.default_val);           
        }
        */
    };
}

