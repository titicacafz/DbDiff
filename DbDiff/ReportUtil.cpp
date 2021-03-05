#include "ReportUtil.h"
#include <fstream>
#include <nanolog.h>
#include "SystemUtil.h"
#include "GlobalContext.h"
#include "CompareContext.h"

void ReportUtil::report(CompareContext & context, DbCompareUtil::DiffList & diff_list)
{
    std::ofstream report_file{ SystemUtil::get_module_path() + CONTEXT.config.read_val("report","report_name") };    

    char * buffer = nullptr;
    {
        std::ifstream report_tpl{ SystemUtil::get_module_path() + "report.tpl" };
        report_tpl.seekg(0, std::ios::end);
        size_t length = (size_t)report_tpl.tellg();
        report_tpl.seekg(0, std::ios::beg);    
        buffer = new char[length];    
        report_tpl.read(buffer, length);
    }
    
    std::string out{buffer};
    ReportUtil::string_replace(out, "${root.referenceDB}", context.get_src_db_name());
    ReportUtil::string_replace(out, "${root.targetDB}", context.get_dest_db_name());
    ReportUtil::string_replace(out, "${root.reportTime}", SystemUtil::get_date());

    //表头部分
    std::string header = out.substr(0, out.find("${root.diff}"));
    report_file << header;
    string_replace(out, header,"");
    string_replace(out, "${root.diff}", "");    

    //结尾部分
    
    int pos = 0;
    int all_size = context.all_list.size();
    for (auto & v : context.all_list) {
        pos++;
        try
        {
            std::string tr_txt;
            LOG_INFO << pos << "/" << all_size << v;
            if (context.src_list.find(v) == context.src_list.end())
            {                
                tr_txt.append(context.dest_list.find(v)->second.type + ",");
                tr_txt.append(context.dest_list.find(v)->second.name + ",");
                tr_txt.append("不存在,");
                tr_txt.append("存在,");
                tr_txt.append(",");
                tr_txt.append(",");
                tr_txt.append(",");
                tr_txt.append(",");
            }
            else if (context.dest_list.find(v) == context.dest_list.end()) {
                
                tr_txt.append(context.src_list.find(v)->second.type + ",");
                tr_txt.append(context.src_list.find(v)->second.name + ",");
                tr_txt.append("存在,");
                tr_txt.append("不存在,");
                tr_txt.append(",");
                tr_txt.append(",");
                tr_txt.append(",");
                tr_txt.append(",");

            }
            else if (diff_list.size() > 0) {
                for (auto & vv : diff_list) {                   
                    tr_txt.append(vv.second.type + ",");
                    tr_txt.append(vv.second.object_name + ",");
                    tr_txt.append("存在,");
                    tr_txt.append("存在,");
                    tr_txt.append( vv.second.columns_difference + ",");
                    tr_txt.append(",");
                    tr_txt.append(",");
                    tr_txt.append(",");
                    tr_txt.append(",");
                }
            }
            
            report_file << tr_txt <<"\r";
        }
        catch (const std::exception& e)
        {
            throw e;
        }
        
    }
    
    //std::string end_tag = "</html>";    
    //out = out.substr(0, out.find(end_tag) + end_tag.length());
    //report_file << out;
}

void ReportUtil::report(DbCompareUtil::DiffList & diff_list, const std::string & file)
{
    std::ofstream report_file{ SystemUtil::get_module_path() + file};
    int pos = 0;
    int all_size = diff_list.size();
    for (auto & v : diff_list) {
        LOG_INFO << ++pos << "/" << all_size << v.first.c_str();

        report_file << v.second.object_name << "\r"
            << v.second.object_difference 
            << v.second.columns_difference;
        report_file << "--------------------------------------------------------------\r";
    }
}

void ReportUtil::string_replace(std::string & strBase, std::string strSrc, std::string strDes)
{
    std::string::size_type pos = 0;
    std::string::size_type srcLen = strSrc.size();
    std::string::size_type desLen = strDes.size();
    pos = strBase.find(strSrc, pos);
    while ((pos != std::string::npos))
    {
        strBase.replace(pos, srcLen, strDes);
        pos = strBase.find(strSrc, (pos + desLen));
    }
}
