#ifndef LISTING_MODEL
#define LISTING_MODEL

#include "crow.h"
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
  class Listing
  {
  private:
    std::string id, listing_name, user_id, listing_location, address, description, tags, created_at, updated_at;

    long int price;

  public:
    Listing(std::string id, std::string listing_name, std::string user_id, std::string address, std::string listing_location, std::string description, std::string tags, long int price, std::string created_at, std::string updated_at);

    Listing(std::string listing_name, std::string user_id, std::string address, std::string listing_location, std::string description, std::string tags, long int price);

    std::string get_id();
    std::string get_listing_name();
    std::string get_user_id();
    std::string get_listing_location();
    long int get_price();
    std::string get_description();
    std::string get_address();
    std::string get_tags();

    crow::json::wvalue to_json() const;
  };

  void create_listing(sql::Connection *con, Listing listing);
  Listing get_listing(sql::Connection *con, std::string id);
  std::vector<Listing> get_all_listings(sql::Connection *con);
  void update_listing(sql::Connection *con, Listing listing);
  void delete_listing(sql::Connection *con, std::string id);
}
#endif
