#pragma once
#include <string>

//���ֶε���Ϣ��
struct TableColumnMeta final
{
    //����
    std::string  name;

    //��ע��
    std::string  comments;

    //����
    std::string  type;

    //����
    double  length;

    //�Ƿ�����Ϊ��
    std::string  nullable;

    //Ĭ��ֵ
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

