#pragma once

#include <string>

//��ʱ
template<typename T>
class Singleton {

public:

	//
	static T& Instance() {   //
		static T t;
		return t;
	}

	Singleton(const Singleton &another) = delete;

private:
	Singleton();
};


struct  SystemUtil
{
    //utf8 ת gbk
    static std::string utf8_to_string(const std::string & str);

    //gbk ת utf8
    static std::string string_to_utf8(const std::string & str);
   

    //���dll����·��
    static std::string get_module_path();

    //��õ�ǰ����
    static std::string get_date();

	/* ��д������
 * string& replace_all (string& src, const string& old_value, const string& new_value);
 * ������Դ�ַ���src    ���滻���Ӵ�old_value    �滻���Ӵ�new_value
 *
 * ���ܣ��� Դ��src �� �Ӵ�old_value ȫ�����滻Ϊ new_value
 */
	static std::string& replace_all(std::string& src, const std::string& old_value, const std::string& new_value);
};
