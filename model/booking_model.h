#ifndef BOOKING_MODEL
#define BOOKING_MODEL

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
namespace Database
{
  class Booking
  {
  private:
    std::string id, user_id, listing_id, remarks, status,created_at, updated_at;

  public:
    Booking(std::string id, std::string user_id, std::string listing_id, std::string remarks,std::string status ,std::string created_at, std::string updated_at);
    std::string get_id();
    std::string get_user_id();
    std::string get_listing_id();
    std::string get_status();
    std::string get_remark();
  };

  void create_booking(sql::Connection *con, Booking user);
  Booking get_booking(sql::Connection *con, std::string id);
  void delete_booking(sql::Connection *con, std::string id);
}
#endif
