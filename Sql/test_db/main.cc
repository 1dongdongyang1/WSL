#include <iostream>
#include <mysql/mysql.h>

int main()
{
    std::cout << "mysql client version: " << mysql_get_client_info() << std::endl;
    return 0;
}