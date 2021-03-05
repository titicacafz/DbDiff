#pragma once
#include <string>
#include <soci/soci.h>

//表字段的信息类
struct ProcedureMeta final
{
    //列名
    std::string  name;

    //列注释
    std::string  comments;

    std::string  type;
    

};

namespace soci {
    template<>
    struct type_conversion<ProcedureMeta>
    {
        typedef values base_type;
        static void from_base(const values& v, indicator ind, ProcedureMeta& tableColumnMeta)
        {
            try
            {
                tableColumnMeta.name = v.get<std::string>("OBJECT_NAME", "");
                tableColumnMeta.comments = "";
                tableColumnMeta.type = v.get<std::string>("OBJECT_TYPE", "");                
            }
            catch (const std::exception& e)
            {
                throw e;
            }

        }
        /*
        static void to_base(const ProcedureMeta& tableColumnMeta, values& v, indicator& ind)
        {
            v.set("OBJECT_NAME", tableColumnMeta.name);            
            v.set("DATA_TYPE", tableColumnMeta.type);
        }
        */
    };
}

