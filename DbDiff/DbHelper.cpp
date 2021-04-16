#include "DbHelper.h"
#include <soci/soci.h>
#include <nanolog.h>
#include <regex>
#include "SystemUtil.h"
#include <list>

DbHelper::TableList DbHelper::get_table_and_views(const std::set<std::string>& owners,
    soci::session & sql)
{
    std::map<std::string,TableMeta>  tables{};

    for (auto & owner : owners) {
        //1表结构比较
        soci::rowset<TableMeta> rs = (
            sql.prepare
            << "select * from all_tab_comments where owner = :owner and (table_type='TABLE' or table_type='VIEW') order by table_name", soci::use(owner)
            );

        //记录遍历       
        for (soci::rowset<TableMeta>::iterator it = rs.begin(); it != rs.end(); ++it)
        {
            const TableMeta& tableMeta = *it;
            tables.emplace(tableMeta.name, tableMeta);
            LOG_INFO << tableMeta.owner.c_str() << "." << tableMeta.name.c_str();
        }
    }

    return std::move(tables);
}

DbHelper::ColumnList DbHelper::get_table_column(
    const std::string & table_name,
    const std::string & table_owner,
    soci::session & sql)
{    
    DbHelper::ColumnList table_columns;
    soci::rowset<TableColumnMeta> rs = (
        sql.prepare
        << "select a.* from all_tab_columns a where a.table_name = :table_name and owner = :owner order by a.column_name",
        soci::use(table_name),
        soci::use(table_owner)
        );

    //记录遍历       
    for (soci::rowset<TableColumnMeta>::iterator it = rs.begin(); it != rs.end(); ++it)
    {
        const TableColumnMeta& tableMeta = *it;
        table_columns.emplace(tableMeta.name, tableMeta);        
    }
    return std::move(table_columns);
}

DbHelper::StringSet DbHelper::set_from_string(const std::string & in)
{
    DbHelper::StringSet dbusers;
    try
    {
        std::regex re{ "," };
        return DbHelper::StringSet{
            std::sregex_token_iterator(in.begin(), in.end(), re, -1),
                std::sregex_token_iterator()
        };
    }
    catch (const std::exception& e)
    {
        LOG_INFO << "error:" << e.what();
    }
    
    return std::move(dbusers);
}

DbHelper::StringSet DbHelper::set_from_file(const std::string &file)
{
    DbHelper::StringSet ignore_list;
    
    std::fstream f(SystemUtil::get_module_path()+file);//创建一个fstream文件流对象

    std::string line; //保存读入的每一行
    while (getline(f, line, '\r'))//会自动把\n换行符去掉 
    {
        ignore_list.insert(line);
    }

    return std::move(ignore_list);
}

DbHelper::ProcedureList DbHelper::get_procedure_and_function(soci::session & sql)
{
    DbHelper::ProcedureList procedure_list;
    soci::rowset<ProcedureMeta> rs = (
        sql.prepare
        << "select * from user_procedures where OBJECT_TYPE in ('FUNCTION' , 'PROCEDURE')"
        );

    //记录遍历       
    for (soci::rowset<ProcedureMeta>::iterator it = rs.begin(); it != rs.end(); ++it)
    {
        const ProcedureMeta& procedure_meta = *it;
        procedure_list.emplace(procedure_meta.name, procedure_meta);
        LOG_INFO  << procedure_meta.name.c_str();
    }
    return std::move(procedure_list);
}

DbHelper::ProcedureArgumentList DbHelper::get_procedure_arguments(std::string proc, soci::session & sql)
{
    DbHelper::ProcedureArgumentList proc_args;
    soci::rowset<ProcedureArgumentMeta> rs = (
        sql.prepare
        << "select * from all_arguments where object_name = :object_name", soci::use(proc)
        );

    //记录遍历       
    for (soci::rowset<ProcedureArgumentMeta>::iterator it = rs.begin(); it != rs.end(); ++it)
    {
        const ProcedureArgumentMeta& procedure_meta = *it;
        proc_args.emplace(procedure_meta.name, procedure_meta);       
    }
    return std::move(proc_args);
}

DbHelper::AllProcedureArgs DbHelper::get_all_procedure_args(soci::session & sql)
{
    AllProcedureArgs all_args;
    std::list<ProcedureArgumentMeta> argument_list;
    soci::rowset<ProcedureArgumentMeta> rs = (
        sql.prepare
        << "select * from all_arguments where owner = 'SD_HOSPITAL' order by owner, object_name"
        );
    
    for (soci::rowset<ProcedureArgumentMeta>::iterator it = rs.begin(); it != rs.end(); ++it)
    {
        try {
            const ProcedureArgumentMeta& row = *it;
            argument_list.emplace_back(row);
        }
        catch (std::exception & e) {
            throw e;
        }                       
    }

    for (auto & v : argument_list) {
        if (all_args.find(v.object) == all_args.end())
        {
            all_args.emplace(v.object, std::list<ProcedureArgumentMeta>{});
        }
        all_args[v.object].emplace_back(v);
    }

    return std::move(all_args);
}

DbHelper::IndexList DbHelper::get_all_indexs(soci::session & sql)
{    
    //key 索引名，索引字段结构
    IndexList index_list;

    
    soci::rowset<TableIndex> rs = (
        sql.prepare
        << "select * from all_ind_columns where index_owner = 'SD_HOSPITAL' order by table_owner,table_name,column_name,column_position"
        );

    for (soci::rowset<TableIndex>::iterator it = rs.begin(); it != rs.end(); ++it)
    {
        try {
            const TableIndex& row = *it;
            if (index_list.find(row.index_name) == index_list.end())
            {
                index_list.emplace(row.index_name, IndexMeta{});
            }
            index_list[row.index_name].table_name = row.table_name;
            index_list[row.index_name].index_name = row.index_name;
            index_list[row.index_name].index_columns.push_back(row);
        }
        catch (std::exception & e) {
            throw e;
        }
    }
    
    
    
    return std::move(index_list);
}
