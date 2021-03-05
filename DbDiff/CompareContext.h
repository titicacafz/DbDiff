#include <soci/soci.h>
#include "DbCompareUtil.h"


//�Ƚ���Ϣ�Ķ��󣬷��㴫�ݲ�������;
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

    //Դ���ݿ���Ƚ϶���
    DbHelper::TableList src_list;

    //Ŀ�����ݿ���Ƚ϶���
    DbHelper::TableList dest_list;

    //Դ���ݿ������Ϣ
    DbHelper::ProcedureList src_procedure_list;

    //Ŀ�����ݿ������Ϣ
    DbHelper::ProcedureList dest_procedure_list;

    //mybatis���ù��̵���Ϣ
    DbHelper::ProcedureList mybatis_procedure_list;
    DbHelper::AllProcedureArgs mybatis_procedure_args;

    //������һ����������ݿ��������std::set
    DbHelper::StringSet all_list;
    
    //�����б�,���б��ж��󲻱Ƚ�
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
    //���ݿ������app.conf��src_db_name
    std::string src_db_name;

    //���ݿ������app.conf��dest_db_name
    std::string dest_db_name;

    //Դ���ݿ����
    soci::session * src_sql;

    //Ŀ�����ݿ����
    soci::session * dest_sql;        
};

