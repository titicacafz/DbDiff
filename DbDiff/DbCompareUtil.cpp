#include <nanolog.h>

#include "DbCompareUtil.h"
#include "CompareContext.h"

std::map<std::string, CompareDiff> DbCompareUtil::compare_table(CompareContext & context)
{
    std::map<std::string, CompareDiff> db_diff;
    try
    {
        int count = 0;
        int all_tables_size = context.all_list.size();
        std::string src_title{ "[" + context.get_src_db_name() + "]" };
        std::string dest_title{ "[" + context.get_dest_db_name() + "]" };
        for (auto & v : context.all_list) {
            count++;
            //std::cout << "正在比较[" << v << "]" << count << "/" << all_tables_size << std::endl;
            LOG_INFO << "正在比较["<< v.c_str() <<"]" << count << "/" << all_tables_size;
            if (context.ignore_list.find(v) != context.ignore_list.end()) {
                continue;
            }
            CompareDiff table_diff{};
            table_diff.object_name = v;

            if (context.src_list.find(v) == context.src_list.end())
            {
                //源数据库表不存在                
                table_diff.object_difference = "在"+src_title+"不存在\r";
                db_diff[v] = table_diff;
            }
            else if (context.dest_list.find(v) == context.dest_list.end()) {
                //目标数据库表不存在
                table_diff.object_difference = "在"+dest_title+"不存在\r";
                db_diff[v] = table_diff;
            }
            else if(context.src_list.find(v) != context.src_list.end() &&
                context.dest_list.find(v) != context.dest_list.end())
            {
                table_diff.type = context.dest_list.find(v)->second.type;
                //两边都存在的才进行表结构比较           
                auto src_columns = DbHelper::get_table_column(
                    v,
                    context.src_list[v].owner,
                    context.get_src_sql()
                );
                auto dest_columns = DbHelper::get_table_column(
                    v,
                    context.dest_list[v].owner,
                    context.get_dest_sql()
                );

                LOG_INFO << "比较[" << v << "]字段";
                for (auto & column : src_columns) {
                    if (dest_columns.find(column.first) == dest_columns.end())
                    {
                        table_diff.columns_difference
                            .append("字段:")
                            .append(column.first)
                            .append("在" + dest_title + "不存在!\r");

                        db_diff[v] = table_diff;
                        continue;
                    }
                }
                for (auto & column : dest_columns) {
                    if (src_columns.find(column.first) == src_columns.end())
                    {
                        table_diff.columns_difference
                            .append("字段:")
                            .append(column.first)
                            .append("在" + src_title + "不存在!]\r");
                        db_diff[v] = table_diff;
                        continue;
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        throw e;
    }
    
    return std::move(db_diff);
}

DbCompareUtil::DiffList DbCompareUtil::compare_procedure(CompareContext & context)
{
    std::map<std::string, CompareDiff> db_diff;
    int count = 0;
    int all_tables_size = context.all_list.size();
    std::string src_title{ "[" + context.get_src_db_name() + "]" };
    std::string dest_title{ "[" + context.get_dest_db_name() + "]" };
    for (auto & v : context.all_list) {
        count++;
        LOG_INFO << "正在比较[" << v << "]" << count << "/" << all_tables_size;
        if (context.ignore_list.find(v) != context.ignore_list.end()) {
            continue;
        }
        CompareDiff table_diff{};
        table_diff.object_name = v;
        if (context.src_procedure_list.find(v) == context.src_procedure_list.end())
        {
            //源数据库表不存在                
            table_diff.object_difference = "在"+ src_title +"不存在\r";
            db_diff[v] = table_diff;
        }
        else if (context.dest_procedure_list.find(v) == context.dest_procedure_list.end()) {
            //目标数据库表不存在
            table_diff.object_difference = "在"+dest_title+"不存在\r";
            db_diff[v] = table_diff;
        }
        else {
            //两边都存在的才进行过程或函数参数比较
            LOG_INFO << "比较[" << v << "]参数";
            if (context.src_procedure_args.find(v) == context.src_procedure_args.end()) {
                continue;
            }
            if (context.dest_procedure_args.find(v) == context.dest_procedure_args.end()) {
                continue;
            }

            auto &src_columns = context.src_procedure_args.find(v)->second;
            auto &dest_columns = context.dest_procedure_args.find(v)->second;            
            
            for (auto & column : src_columns) {
                bool has = false;
                for (auto & dest_column : dest_columns) {
                    if (column.name == dest_column.name)
                    {
                        has = true;                        
                        break;
                    }
                }   
                if (!has) {
                    table_diff.columns_difference
                        .append("参数:")
                        .append(column.name)
                        .append("在" + dest_title + "中不存在!\r");

                    db_diff[v] = table_diff;
                }
            }
            for (auto & column : dest_columns) {
                bool has = false;
                for (auto & src_column : src_columns) {
                    if (column.name == src_column.name)
                    {
                        has = true;
                        break;
                    }
                }
                if (!has) {
                    table_diff.columns_difference
                        .append("参数:")
                        .append(column.name)
                        .append("在" + src_title + "中不存在!\r");

                    db_diff[v] = table_diff;
                }                
            }
            
        }
    }
    return std::move(db_diff);
}

DbCompareUtil::DiffList DbCompareUtil::compare_index(CompareContext & context)
{
    std::map<std::string, CompareDiff> db_diff;
    int count = 0;
    std::string src_title{ "[" + context.get_src_db_name() + "]" };
    std::string dest_title{ "[" + context.get_dest_db_name() + "]" };
    
    auto & src_index_columns = context.src_index_list;
    
    for (auto & v : src_index_columns) {                
        //索引不一致
        CompareDiff diff;
        auto& second = v.second;
        //忽略列表的索引不比较
        if (context.ignore_list.find(second.table_name) == context.ignore_list.end()) {
            continue;
        }

        diff.object_name = v.first;
        //索引是否都存在的判断
        if (src_index_columns.find(v.first) == src_index_columns.end()) {
                       
            diff.object_difference = "在[" + dest_title + "]不存在";
            db_diff.emplace(v.first, diff);
            continue;
        }
        
        auto & src_column = v.second.index_columns;
        auto & dest_column = src_index_columns[v.first].index_columns;        
        //如果索引字段数不一致直接提示
        if (src_column.size() != dest_column.size())
        {
            diff.object_difference = "索引字段个数不一致";
            db_diff.emplace(v.first, diff);
            continue;
        }

        //到这里应该索引字段数一致的
        for (size_t i = 0; i < src_column.size(); i++)
        {
            auto & src_column_v = src_column[i];
            auto & dest_column_v = dest_column[i];
            if (src_column_v.column != dest_column_v.column)
            {
                diff.columns_difference = "索引字段或字段顺序不一致";
                db_diff.emplace(v.first, diff);
                //没必要比较了，直接通知出来就好
                continue;
            }
        }

    }
    
    return std::move(db_diff);
}

DbCompareUtil::DiffList DbCompareUtil::check_mybatis_mappers(CompareContext& context)
{
    std::map<std::string, CompareDiff> db_diff;
    int count = 0;
    int all_tables_size = context.mybatis_procedure_list.size();
    std::string src_title{ "[" + context.get_src_db_name() + "]" };
    std::string dest_title{ "[mybatis配置文件]" };
    LOG_INFO << "检查mybatis配置开始";
    for (auto& v : context.mybatis_procedure_list) {
        count++;
        LOG_INFO << v.first.c_str();
        //两边都存在的才进行过程或函数参数比较
        if (context.src_procedure_args.find(v.first) == context.src_procedure_args.end()) {
            continue;
        }
        
        CompareDiff table_diff{};
        table_diff.object_name = v.first;

        auto& src_columns = context.src_procedure_args.find(v.first)->second;
        auto& dest_columns = context.mybatis_procedure_args.find(v.first)->second;

        for (auto& column : src_columns) {
            bool has = false;
            for (auto& dest_column : dest_columns) {
                if (column.name == dest_column.name)
                {
                    has = true;
                    break;
                }
            }
            if (!has) {
                table_diff.columns_difference
                    .append("参数:")
                    .append(column.name)
                    .append("在" + dest_title + "中不存在!\r");

                db_diff[v.first] = table_diff;
            }
        }
        for (auto& column : dest_columns) {
            bool has = false;
            for (auto& src_column : src_columns) {
                if (column.name == src_column.name)
                {
                    has = true;
                    break;
                }
            }
            if (!has) {
                table_diff.columns_difference
                    .append("参数:")
                    .append(column.name)
                    .append("在" + src_title + "中不存在!\r");

                db_diff[v.first] = table_diff;
            }
        }

    }
    LOG_INFO << "检查mybatis配置结束";
    return std::move(db_diff);
}
