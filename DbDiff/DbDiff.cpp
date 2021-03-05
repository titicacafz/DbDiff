#include <soci/soci.h>
#include <soci/oracle/soci-oracle.h>
#include <nanolog.h>
#include "SystemUtil.h"

#include "GlobalContext.h"
#include "DbCompareUtil.h"
#include "ReportUtil.h"
#include "CompareContext.h"

#include <filesystem>
#include <iostream>
#include <regex>
#include "pugixml/pugixml.hpp"
#include <algorithm>

using std::string;
using namespace std::filesystem;
using namespace soci;

//解析mybatis配置文件，生成过程信息
void parse_mybatis(const string & callable, CompareContext & context)
{
    std::list<ProcedureArgumentMeta> argument_list;

    //生成数据
    auto pos_begin = callable.find("CALL ");
    auto pos_end = callable.find_first_of("(");
    string procedure_name = callable.substr(pos_begin+5, pos_end- pos_begin -5);
    procedure_name = SystemUtil::replace_all(procedure_name, " ", "");

    LOG_INFO << procedure_name.c_str();

    ProcedureMeta meta;
    meta.name = procedure_name;

    context.mybatis_procedure_list.emplace(procedure_name, meta);
    
    pos_begin = callable.find("(");
    pos_end = callable.find_last_of(")");
    
    string paras = callable.substr(pos_begin+1, pos_end-pos_begin - 2);

    if (paras.length() > 0)
    {
        //
        string para_name;
        while (paras.find("#{") != paras.npos)
        {
            auto now_char = paras.at(0);
            if (now_char == ',' || now_char == '\n' || now_char == ' ')
            {
                paras.erase(0, 1);
                continue;
            }
            
                
            auto param_begin = paras.find_first_of("{");
            auto param_end = paras.find_first_of(",");
            if (param_begin == paras.npos || param_end == paras.npos)
                continue;
            para_name = paras.substr(param_begin+1, param_end- param_begin -1);
            ProcedureArgumentMeta argmeta;
            argmeta.name = SystemUtil::replace_all(para_name, " ", "");
            argmeta.object = procedure_name;
            argument_list.push_back(argmeta);
            param_end = paras.find_first_of("}");
            if(param_begin == paras.npos || param_end == paras.npos)
                continue;
            paras = paras.substr(param_end, paras.npos - param_end);
            //如果有注释，去掉注释
            param_begin = paras.find_first_of("--");
            param_end = paras.find_first_of('\n');
            paras = paras.substr(param_end+1, paras.npos - param_begin-1);
        }
    }
    if (context.mybatis_procedure_args.find(procedure_name) == context.mybatis_procedure_args.end())
    {
        context.mybatis_procedure_args.emplace(procedure_name, argument_list);
    }
    //context.mybatis_procedure_args[procedure_name].emplace_back(argument_list);
}

void load_mybatis_mappers(CompareContext& context)
{
    //比较mybatis配置的存储过程和数据库是否有差异
        //获取源码目录
    string src_code_dir = CONTEXT.config.read_val("database", "src_code_dir");

    //遍历所有mybatis配置文件
    path path_file_root(src_code_dir);
    if (exists(path_file_root)) {
        std::regex mybatis_xml(".*\\\\mybatis\\\\.*.xml");
        recursive_directory_iterator files_list(path_file_root);

        for (auto& it : files_list)
        {
            string full_file_name = it.path().string();
            if (std::regex_match(full_file_name, mybatis_xml))
            {
                pugi::xml_document mybatis_mapper_doc;
                mybatis_mapper_doc.load_file(full_file_name.c_str());
                pugi::xpath_node_set callables = mybatis_mapper_doc.select_nodes("//mapper/*[@statementType=\"CALLABLE\"]");
                int count = 0;
                for (pugi::xpath_node node : callables)
                {
                    pugi::xml_node xml_node = node.node();
                    string child_value = xml_node.child_value();
                    std::transform(child_value.begin(), child_value.end(), child_value.begin(), ::toupper);
                    //解析调用文本，获取过程名和出入参名字数组
                    parse_mybatis(child_value, context);
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    nanolog::initialize(nanolog::GuaranteedLogger(), SystemUtil::get_module_path(), "db_compare_log", 1);
    
    
    try
    {        
        //基准数据库
        string src_conn = CONTEXT.config.read_val("database", "src_conn");
        //目标数据库        
        string dest_conn = CONTEXT.config.read_val("database", "dest_conn");        

        //表用户字典    
        //SD_HOSPITAL,SD_MZ,SD_ZY,SD_SF,SD_YF,SD_YK
        DbHelper::StringSet compare_dbusers = DbHelper::set_from_string(CONTEXT.config.read_val("database", "compare_dbusers"));
        
        //
        DbHelper::StringSet compare_object = DbHelper::set_from_string(CONTEXT.config.read_val("database", "compare_type"));
        
        if (compare_object.size() <= 0)
            return -1;       

        //1、连接数据库
        soci::session src_sql(*soci::factory_oracle(), src_conn);                
        soci::session dest_sql(*soci::factory_oracle(), dest_conn);

        

        CompareContext compare_context(src_sql, dest_sql);
        
        compare_context
            .set_src_db_name(CONTEXT.config.read_val("database", "src_db_name"))
            .set_dest_db_name(CONTEXT.config.read_val("database", "dest_db_name"));

        compare_context.ignore_list = DbHelper::set_from_file("ignore_list.txt");

        //这里把配置文件里面的过程调用都添加到CompareContext::mybatis_procedure_list
        //                                    CompareContext::mybatis_procedure_args;
        load_mybatis_mappers(compare_context);

        //比较表结构
        if (compare_object.find("table") != compare_object.end()) {
            //2、表和视图结构比对
            //2.1、表记录收集
            compare_context.src_list = DbHelper::get_table_and_views(compare_dbusers, src_sql);
            compare_context.dest_list = DbHelper::get_table_and_views(compare_dbusers, dest_sql);

            //2.2、把表名汇总成一个set
            compare_context.all_list = DbHelper::merge_map_key<DbHelper::TableList>(
                compare_context.src_list, 
                compare_context.dest_list);

            //2.3、表结构比较
            DbCompareUtil::DiffList diff_list = DbCompareUtil::compare_table(compare_context);

            //3、显示比较结果
            ReportUtil::report(diff_list, "table_report.txt");
            
            //4、比较表索引
            compare_context.src_index_list = DbHelper::get_all_indexs(src_sql);
            compare_context.dest_index_list = DbHelper::get_all_indexs(dest_sql);

            DbCompareUtil::DiffList index_diff_list =  DbCompareUtil::compare_index(compare_context);
            ReportUtil::report(index_diff_list, "index_report.txt");
            
        }

        //比较过程和函数
        if (compare_object.find("procedure") != compare_object.end()) {
           
            //4、比较存储过程和函数
            compare_context.src_procedure_list = DbHelper::get_procedure_and_function(src_sql);
            compare_context.src_procedure_args = DbHelper::get_all_procedure_args(src_sql);
            
            string compare_mybatis = CONTEXT.config.read_val("database", "compare_mybatis");

            if (compare_mybatis == "1") {
                //
                DbCompareUtil::DiffList mybytis_diff = DbCompareUtil::check_mybatis_mappers(compare_context);
                //3、显示比较结果
                ReportUtil::report(mybytis_diff, "mybatis_mapper_report.txt");
            }
            else {
                compare_context.dest_procedure_list = DbHelper::get_procedure_and_function(dest_sql);
                compare_context.dest_procedure_args = DbHelper::get_all_procedure_args(dest_sql);

                compare_context.all_list = DbHelper::merge_map_key<DbHelper::ProcedureList>
                    (compare_context.src_procedure_list, compare_context.dest_procedure_list);

                DbCompareUtil::DiffList db_diff = DbCompareUtil::compare_procedure(compare_context);

                //3、显示比较结果
                ReportUtil::report(db_diff, "procedure_report.txt");
            }
        }  
        
    }
    catch (const std::exception& e)
    {
        LOG_INFO << e.what();
    }

    return 0;
}
