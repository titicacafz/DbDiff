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
            //std::cout << "���ڱȽ�[" << v << "]" << count << "/" << all_tables_size << std::endl;
            LOG_INFO << "���ڱȽ�["<< v.c_str() <<"]" << count << "/" << all_tables_size;
            if (context.ignore_list.find(v) != context.ignore_list.end()) {
                continue;
            }
            CompareDiff table_diff{};
            table_diff.object_name = v;

            if (context.src_list.find(v) == context.src_list.end())
            {
                //Դ���ݿ������                
                table_diff.object_difference = "��"+src_title+"������\r";
                db_diff[v] = table_diff;
            }
            else if (context.dest_list.find(v) == context.dest_list.end()) {
                //Ŀ�����ݿ������
                table_diff.object_difference = "��"+dest_title+"������\r";
                db_diff[v] = table_diff;
            }
            else if(context.src_list.find(v) != context.src_list.end() &&
                context.dest_list.find(v) != context.dest_list.end())
            {
                table_diff.type = context.dest_list.find(v)->second.type;
                //���߶����ڵĲŽ��б�ṹ�Ƚ�           
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

                LOG_INFO << "�Ƚ�[" << v << "]�ֶ�";
                for (auto & column : src_columns) {
                    if (dest_columns.find(column.first) == dest_columns.end())
                    {
                        table_diff.columns_difference
                            .append("�ֶ�:")
                            .append(column.first)
                            .append("��" + dest_title + "������!\r");

                        db_diff[v] = table_diff;
                        continue;
                    }
                }
                for (auto & column : dest_columns) {
                    if (src_columns.find(column.first) == src_columns.end())
                    {
                        table_diff.columns_difference
                            .append("�ֶ�:")
                            .append(column.first)
                            .append("��" + src_title + "������!]\r");
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
        LOG_INFO << "���ڱȽ�[" << v << "]" << count << "/" << all_tables_size;
        if (context.ignore_list.find(v) != context.ignore_list.end()) {
            continue;
        }
        CompareDiff table_diff{};
        table_diff.object_name = v;
        if (context.src_procedure_list.find(v) == context.src_procedure_list.end())
        {
            //Դ���ݿ������                
            table_diff.object_difference = "��"+ src_title +"������\r";
            db_diff[v] = table_diff;
        }
        else if (context.dest_procedure_list.find(v) == context.dest_procedure_list.end()) {
            //Ŀ�����ݿ������
            table_diff.object_difference = "��"+dest_title+"������\r";
            db_diff[v] = table_diff;
        }
        else {
            //���߶����ڵĲŽ��й��̻��������Ƚ�
            LOG_INFO << "�Ƚ�[" << v << "]����";
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
                        .append("����:")
                        .append(column.name)
                        .append("��" + dest_title + "�в�����!\r");

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
                        .append("����:")
                        .append(column.name)
                        .append("��" + src_title + "�в�����!\r");

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
        //������һ��
        CompareDiff diff;
        auto& second = v.second;
        //�����б���������Ƚ�
        if (context.ignore_list.find(second.table_name) == context.ignore_list.end()) {
            continue;
        }

        diff.object_name = v.first;
        //�����Ƿ񶼴��ڵ��ж�
        if (src_index_columns.find(v.first) == src_index_columns.end()) {
                       
            diff.object_difference = "��[" + dest_title + "]������";
            db_diff.emplace(v.first, diff);
            continue;
        }
        
        auto & src_column = v.second.index_columns;
        auto & dest_column = src_index_columns[v.first].index_columns;        
        //��������ֶ�����һ��ֱ����ʾ
        if (src_column.size() != dest_column.size())
        {
            diff.object_difference = "�����ֶθ�����һ��";
            db_diff.emplace(v.first, diff);
            continue;
        }

        //������Ӧ�������ֶ���һ�µ�
        for (size_t i = 0; i < src_column.size(); i++)
        {
            auto & src_column_v = src_column[i];
            auto & dest_column_v = dest_column[i];
            if (src_column_v.column != dest_column_v.column)
            {
                diff.columns_difference = "�����ֶλ��ֶ�˳��һ��";
                db_diff.emplace(v.first, diff);
                //û��Ҫ�Ƚ��ˣ�ֱ��֪ͨ�����ͺ�
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
    std::string dest_title{ "[mybatis�����ļ�]" };
    LOG_INFO << "���mybatis���ÿ�ʼ";
    for (auto& v : context.mybatis_procedure_list) {
        count++;
        LOG_INFO << v.first.c_str();
        //���߶����ڵĲŽ��й��̻��������Ƚ�
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
                    .append("����:")
                    .append(column.name)
                    .append("��" + dest_title + "�в�����!\r");

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
                    .append("����:")
                    .append(column.name)
                    .append("��" + src_title + "�в�����!\r");

                db_diff[v.first] = table_diff;
            }
        }

    }
    LOG_INFO << "���mybatis���ý���";
    return std::move(db_diff);
}
