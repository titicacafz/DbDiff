#pragma once
#include <string>

//���ݿ����Ƚϲ�����Ϣ
struct CompareDiff final {    

    //���ݿ������
    std::string object_name;

    //�������
    std::string object_difference;

    //�����л��߲�������
    std::string columns_difference;

    //���ݿ��������
    std::string type;
};
