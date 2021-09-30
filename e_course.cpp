#include "e_course.h"

E_course::E_course()
{
    QDir dir;
    path_directory = dir.currentPath();
    cookiefilename = path_directory.toStdString() + "//cookie.txt";
}

std::string E_course::escape(CURL* curl_handle, const std::string& text)
{
    std::string result;
    char* esc_text = curl_easy_escape(curl_handle, text.c_str(), text.length());
    if (!esc_text) throw std::runtime_error("Can not convert string to URL");

    result = esc_text;
    curl_free(esc_text);

    return result;
}

bool E_course::Cheak_authenticated(std::string path)
{
    std::string result;
    std::ifstream reading(path);
    if (!reading)
    {
        return 0;
    }

    std::string finded = { "        <title>Обзорная панель</title>" };
    int cnt = 0;
    while (!reading.eof())
    {
        std::string tmp;
        std::getline(reading, tmp);
        if ((tmp == finded))
        {
            tmp = finded;
            ++cnt;
            reading.close();
            return 1;
        }


    }
    reading.close();
    return 0;
}


std::string E_course::request_logintoken(std::string path)
{
    std::string result;
    std::ifstream reading(path);
    if (!reading)
    {
        std::cerr << "Error. Can`t open file " << path << '\n';
        return "0";
    }

    std::string finded = {"                    <script>document.getElementById('anchor').value = location.hash;</script>" };
    int cnt = 0;
    while (!reading.eof())
    {
        std::string tmp;
        std::getline(reading, tmp);
        if (cnt == 1)
        {
            result = tmp;
            cnt = 0;
        }
            if ((tmp == finded))
            {
                tmp = finded;
                ++cnt;
            }


    }
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_string(result.c_str());
    pugi::xml_node panel = doc.first_child();
    pugi::xml_attribute attr = panel.first_attribute().next_attribute().next_attribute();

    return attr.value();
}

std::string E_course::get_id_test(std::string url)
{
    size_t length = url.length();
    size_t pos = url.find("id=");
    if (pos != std::string::npos)
    {
        return url.substr(pos + 3, length);
    }
    pos = url.find("cmid=");
        if (pos != std::string::npos)
        {
            return url.substr(pos + 5, length);
        }
        else
        {
            std::cout << " F(id_test) isn't right link\n";
            exit(1);
        }
    return "0";
}

size_t E_course::write_data(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    if (data)
    {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }
    else
        return 0;  // ����� ������
}

int E_course::autorisate(std::string login, std::string password)
{
    //const  std::string cookiefilename = path_directory.toStdString() + "//cookie.txt";
    CURL* curl_handle;
    curl_handle = curl_easy_init();
    if (curl_handle)
    {
        std::ofstream xmlfile1(path_directory.toStdString() + "//xmlfile.txt");
        std:: string content = "";

        curl_easy_setopt(curl_handle, CURLOPT_URL, "https://e.sfu-kras.ru/login/index.php");
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        CURLcode res = curl_easy_perform(curl_handle);


        xmlfile1 << content;
        xmlfile1.close();

      curl_easy_strerror(res);
        //proverka << content;
      //First post ������ - ����� �-����� ���������, ��� � �� ���.
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 20);
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, "");


        std::string token = request_logintoken(path_directory.toStdString() + "//xmlfile.txt");
        std::cout << token << "\n";
        std::string post_data;

        post_data += "anchor=";
        post_data += "&logintoken=" + escape(curl_handle, token);
        post_data += "&username=" + escape(curl_handle, login);
        post_data += "&password=" + escape(curl_handle, password);

        //anchor=&logintoken=MSjsiJQUi6jqY9hzxDTxrFCEZi7DszVy&username=DGlebov-KI20&password=Digit321
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.1) Gecko/20061204 Firefox/2.0.0.1");
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, post_data.length());

        //second post ������, � ��� � ������ ������������
        content = "";
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        res = curl_easy_perform(curl_handle);
        if (res)  std::cout << content << std::endl;
        else      std::cerr << "E-course connection-2 status:  " << curl_easy_strerror(res) << ".......\n";

        std::ofstream xmlfile2(path_directory.toStdString() + "//xmlfile.txt");
        xmlfile2 << content;
        xmlfile2.close();
        token = request_logintoken(path_directory.toStdString() + "//xmlfile.txt");
        std::cout << token << "\n";
        post_data = "";
        post_data += "anchor=";
        post_data += "&logintoken=" + escape(curl_handle, token);
        post_data += "&username=" + escape(curl_handle, login);
        post_data += "&password=" + escape(curl_handle, password);

        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.1) Gecko/20061204 Firefox/2.0.0.1");
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, post_data.length());


        std::ofstream xmlfile3(path_directory.toStdString() + "//xmlfile.txt");
        content = "";
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        res = curl_easy_perform(curl_handle);
        xmlfile3 << content;
        xmlfile3.close();
        curl_easy_setopt(curl_handle, CURLOPT_COOKIELIST, "FLUSH");
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, cookiefilename.c_str());



        if (res)  std::cout << content << std::endl;
        else      std::cerr << "E-course connection-3 status:  " << curl_easy_strerror(res) << ".......\n";


        if (Cheak_authenticated(path_directory.toStdString() + "//xmlfile.txt"))
        {
          curl_easy_cleanup(curl_handle);
            return 1;
        }
        else
        {
           curl_easy_cleanup(curl_handle);
            return 0;
        }


        curl_easy_cleanup(curl_handle);
        return 0;
    }
    return 0;
}

std::string E_course::get_attempt(std::string url)
{
    size_t pos = url.find("attempt=");
    size_t end = url.find("&");
        if (pos != std::string::npos)
        {
            return url.substr(pos + 8, end - pos-8);
        }
        else
        {
            std::cout << "isn't right link\n";
            exit(1);
        }
}

std::vector<std::vector<std::string>> E_course::parse_schedule()
{
    std::vector<std::vector<std::string>> schedule; // Дата, курс
    QDir qdr;
    std::string path_XMLFille  = qdr.currentPath().toStdString() + "//xmlfile.txt";
    QTextStream out(stdout);
    QFile file(path_XMLFille.c_str());
    QString time_buf_for_time;

      if (file.open(QIODevice::ReadOnly))
      {
          QTextStream in(&file);
          while (!in.atEnd())
          {
            QString line = in.readLine(); // метод readLine() считывает одну строку из потока
            int comPos = line.indexOf("class=\"day\" title=\"");
            int comEndPos = line.indexOf("\">", comPos+8);
            QString subStr = line.mid(comPos+19, comEndPos - comPos-19);
            if(comPos != -1 && comEndPos != -1)
            {
                time_buf_for_time = subStr;
            }
            else
            {
                comPos = line.indexOf("<a data-action=\"view-event\" data-event-id=\"");
                if(comPos != -1)
                {
                    comPos = line.indexOf("href=\"");
                    comEndPos = line.indexOf("\"", comPos+8);
                    subStr = line.mid(comPos+6, comEndPos - comPos-6);
                    if(comPos != -1 && comEndPos != -1)
                    {
                        std::vector<std::string> vec{time_buf_for_time.toStdString(), subStr.toStdString()};
                        schedule.push_back(vec);
                    }
                }
            }
          }

          file.close();
      }

  return schedule;
}

void E_course::parse_xml(std::vector<std::vector<std::string>> &schedule, std::vector<std::vector<std::string>> &users, int count_users, int *bool_look)
{
    for(int i=0; i < count_users; i++)
         if(autorisate(users[i][0], users[i][1]))
             {
                 QDir dir;

                 CURL* curl_handle_cookie;
                 curl_handle_cookie = curl_easy_init();
                 std::ofstream xmlfile(path_directory.toStdString() + "//xmlfile.txt");;

                if (curl_handle_cookie)
                  {
                     curl_easy_setopt(curl_handle_cookie, CURLOPT_URL, "https://e.sfu-kras.ru/calendar/view.php?view=month");  //   https://e.sfu-kras.ru/my/ https://e.sfu-kras.ru/login/index.php
                     curl_easy_setopt(curl_handle_cookie, CURLOPT_COOKIEFILE, cookiefilename.c_str());
                     std::string content;
                     curl_easy_setopt(curl_handle_cookie, CURLOPT_WRITEFUNCTION, write_data);
                     curl_easy_setopt(curl_handle_cookie, CURLOPT_WRITEDATA, &content);
                     curl_easy_perform(curl_handle_cookie);
                     xmlfile << content;
                     xmlfile.close();
                     //curl_easy_cleanup(curl_handle_cookie);
                     std::vector<std::vector<std::string>> heads = parse_schedule();
                     find_head(curl_handle_cookie, heads);

                  }
             }

}

std::string E_course::get_head()
{
    std::vector<std::vector<std::string>> schedule; // Дата, курс
    QDir qdr;
    std::string path_XMLFille  = qdr.currentPath().toStdString() + "//xmlfile.txt";
    QTextStream out(stdout);
    QFile file(path_XMLFille.c_str());

      if (file.open(QIODevice::ReadOnly))
      {
          QTextStream in(&file);
          while (!in.atEnd())
          {
            QString line = in.readLine(); // метод readLine() считывает одну строку из потока
            int comPos = line.indexOf("<title>");
            int comEndPos = line.indexOf("<", comPos+5);
            QString subStr = line.mid(comPos+7, comEndPos - comPos-7);
            if(comPos != -1 && comEndPos != -1)
            {
                return subStr.toStdString();
            }
          }

          file.close();
      }
}

void E_course::find_head(CURL *curl_handle, std::vector<std::vector<std::string>> &heads)
{
    std::ofstream xmlfile(path_directory.toStdString() + "//xmlfile.txt");;
    for(size_t i=0; i<heads.size(); i++)
    {
        std::ofstream xmlfile(path_directory.toStdString() + "//xmlfile.txt");;
        curl_easy_setopt(curl_handle, CURLOPT_URL, heads[i][1].c_str());
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, cookiefilename.c_str());
        std::string content;
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        curl_easy_perform(curl_handle);
        xmlfile << content;
        xmlfile.close();

        heads[i][1] = get_head();
        std::cout << heads[i][0] << "\n" << heads[i][1] << "\n\n";
    }

}

void E_course::start()
{
    std::vector<std::vector<std::string>> schedule;
    std::vector<std::vector<std::string>> users;
    std::vector<std::string> vec {"DGlebov-KI20","Digit321" };
    users.push_back(vec);
    parse_xml(schedule, users, 1, 0);
}


