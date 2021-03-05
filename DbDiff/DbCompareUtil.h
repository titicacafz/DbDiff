#pragma once

#include <map>
#include "DbHelper.h"
#include "TableDiff.h"

class CompareContext;

//���ݿ�ȽϵĹ��ߺ�����
class DbCompareUtil final
{
public:
    using DiffList = std::map<std::string, CompareDiff>;
    
    //�Ƚ����ݿ��ṹ
    static DiffList compare_table(CompareContext & context);      

    //�Ƚ����ݿ���̲���
    static DiffList compare_procedure(CompareContext &); 

    //�Ƚ����ݿ����������
    static DiffList compare_index(CompareContext &);

    //�Ƚ�mybatis���ú�Դ���ݿ�Ĳ���
    static DiffList check_mybatis_mappers(CompareContext&);
    
};

