#pragma once
#include <string>
#include <soci/soci.h>

//��ṹ��Ϣ��
struct TableIndex final
{
    //ӵ����
    std::string owner;
    
    //������
    std::string index_name;

    //������
    std::string table_name;

    //�����ֶ�
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