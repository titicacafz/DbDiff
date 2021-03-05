#include <soci/soci.h>
#include "DbCompareUtil.h"


//比较信息的对象，方便传递参数等用途
class CompareContext final {

public:
    CompareContext(soci::session & src_conn, soci::session & dest_conn)         
    {
        src_sql = &src_conn;
        dest_sql = &dest_conn;
    }    

    CompareContext& set_src_db_name(const std::string & conn)
    {
        src_db_name = conn;
        return *this;
    }

    CompareContext& set_dest_db_name(const std::string & conn)
    {
        dest_db_name = conn;
        return *this;
    }

    //源数据库待比较对象
    DbHelper::TableList src_list;

    //目标数据库待比较对象
    DbHelper::TableList dest_list;

    //源数据库过程信息
    DbHelper::ProcedureList src_procedure_list;

    //目标数据库过程信息
    DbHelper::ProcedureList dest_procedure_list;

    //mybatis调用过程的信息
    DbHelper::ProcedureList mybatis_procedure_list;
    DbHelper::AllProcedureArgs mybatis_procedure_args;

    //整合在一起的所有数据库对象名称std::set
    DbHelper::StringSet all_list;
    
    //忽略列表,该列表中对象不比较
    DbHelper::StringSet ignore_list;

    DbHelper::AllProcedureArgs src_procedure_args;
    DbHelper::AllProcedureArgs dest_procedure_args;

    DbHelper::IndexList src_index_list;
    DbHelper::IndexList dest_index_list;

    const std::string & get_src_db_name()
    {
        return src_db_name;
    }

    const std::string & get_dest_db_name() {
        return dest_db_name;
    }

    soci::session & get_src_sql()
    {
        return *src_sql;
    }
    

    soci::session & get_dest_sql()
    {
        return *dest_sql;
    }

    

private:
    //数据库的名字app.conf中src_db_name
    std::string src_db_name;

    //数据库的名字app.conf中dest_db_name
    std::string dest_db_name;

    //源数据库对象
    soci::session * src_sql;

    //目标数据库对象
    soci::session * dest_sql;        
};

