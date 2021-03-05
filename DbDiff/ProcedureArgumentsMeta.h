#pragma once
#include <string>
#include <soci/soci.h>

struct ProcedureArgumentMeta final
{
    std::string object;
    std::string name;
    std::string type;
    std::string inout;
};

namespace soci {
    template<>
    struct type_conversion<ProcedureArgumentMeta>
    {
        typedef values base_type;
        static void from_base(const values& v, indicator ind, ProcedureArgumentMeta& procedureArgumentMeta)
        {
            try
            {             
                procedureArgumentMeta.object = v.get<std::string>("OBJECT_NAME", "");
                procedureArgumentMeta.name = v.get<std::string>("ARGUMENT_NAME", "");
                procedureArgumentMeta.type = v.get<std::string>("DATA_TYPE", "");
                procedureArgumentMeta.inout = v.get<std::string>("IN_OUT", "");
            }
            catch (const std::exception& e)
            {
                throw e;
            }
        }        
    };
}