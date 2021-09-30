#ifndef E_COURSE_H
#define E_COURSE_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <QDir>
#include "curl/curl.h"
#include <cctype>
#include <QTextStream>
#include "pugixml/pugiconfig.hpp"
#include "pugixml/pugixml.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
class E_course
{
public:
    E_course();
    void parse_xml(std::vector<std::vector<std::string>> &schedule, std::vector<std::vector<std::string>> &users, int count_users);
    std::string get_id_test(std::string url);
    int autorisate(std::string login, std::string password);
    static size_t write_data(char* ptr, size_t size, size_t nmemb, std::string* data);
    std::string request_logintoken(std::string path);
    std::string escape(CURL* curl_handle, const std::string& text);
    bool Cheak_authenticated(std::string path);
    std::string get_attempt(std::string url);
    std::vector<std::vector<std::string>> parse_schedule();
    void start();
    void find_head(CURL *curl_handle, std::vector<std::vector<std::string>> &heads);
    void save_heads_json(QString name, std::vector<std::vector<std::string>> &heads);
    std::string get_head();
    std::string cookiefilename;
    QString path_directory;
};


#endif // E_COURSE_H
