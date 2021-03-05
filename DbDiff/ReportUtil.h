#pragma once
#include "DbCompareUtil.h"

class CompareContext;

struct ReportUtil final
{
    static void report(CompareContext &, DbCompareUtil::DiffList &);

    static void report(DbCompareUtil::DiffList &, const std::string & );

    static void string_replace(std::string &strBase, std::string strSrc, std::string strDes);
    
};

