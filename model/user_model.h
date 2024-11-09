#ifndef USER_MODEL
#define USER_MODEL

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include "crow.h"

namespace Database
{
  class User
  {
  private:
    std::string id, user_name, phone_number, user_location, created_at, updated_at;

  public:
    User(std::string id, std::string user_name, std::string phone_number, std::string user_location, std::string created_at, std::string updated_at);
    User(std::string user_name, std::string phone_number, std::string user_location);
    std::string get_id();
    std::string get_user_name();
    std::string get_phone_number();
    std::string get_user_location();

    crow::json::wvalue to_json() const;
  };

  void create_user(sql::Connection *con, User user);
  User get_user(sql::Connection *con, std::string id);
  std::vector<User> get_all_users(sql::Connection *con); 
  void update_user(sql::Connection *con, User user);
  void delete_user(sql::Connection *con, std::string id);
}
#endif
