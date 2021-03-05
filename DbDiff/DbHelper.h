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

//���ݿ�ṹ��Ϣ��ȡ�Ĺ�����
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
    
    
    //��ñ���Ϣ
    static TableList get_table_and_views(const std::set<std::string> & owners,
        soci::session & sql);

    //���ܱ�    
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

    //��ȡ����ֶ���Ϣ
    static ColumnList get_table_column(
        const std::string & table_name,
        const std::string & table_owner,
        soci::session & sql);

    //���û��б��ʼ��
    static StringSet set_from_string(const std::string & );

    //���ļ���ȡset
    static StringSet set_from_file(const std::string &);

    //��ú����͹�����Ϣ
    static ProcedureList get_procedure_and_function(soci::session & sql);

    //��ù��̺ͺ����ĳ����
    static ProcedureArgumentList get_procedure_arguments(std::string proc, soci::session & sql);    

    static AllProcedureArgs get_all_procedure_args(soci::session & sql);

    static IndexList get_all_indexs(soci::session & sql);
};

