#include "user_model.h"

namespace Database
{
  User::User(std::string id, std::string user_name, std::string phone_number, std::string user_location, std::string created_at, std::string updated_at)
  {
    this->id = id;
    this->user_name = user_name;
    this->phone_number = phone_number;
    this->user_location = user_location;
    this->created_at = created_at;
    this->updated_at = updated_at;
  }

  User::User(std::string user_name, std::string phone_number, std::string user_location)
  {
    this->id = id;
    this->user_name = user_name;
    this->phone_number = phone_number;
    this->user_location = user_location;
    this->created_at = created_at;
    this->updated_at = updated_at;
  }

  std::string User::get_id() { return id; }
  std::string User::get_user_name() { return user_name; }
  std::string User::get_phone_number() { return phone_number; }
  std::string User::get_user_location() { return user_location; }

  crow::json::wvalue User::to_json() const
  {
    crow::json::wvalue json;
    json["id"] = id;
    json["user_name"] = user_name;
    json["phone_number"] =phone_number ;
    json["user_location"] = user_location;
    json["created_at"] = created_at;
    json["updated_at"] = updated_at;
    
    return json;
  }
  void create_user(sql::Connection *con, User user)
  {
    sql::PreparedStatement *pstmt = con->prepareStatement("INSERT INTO users (user_name, phone_number,user_location) VALUES (?, ?, ?)");
    try
    {

      pstmt->setString(1, user.get_user_name());
      pstmt->setString(2, user.get_phone_number());
      pstmt->setString(3, user.get_user_location());

      pstmt->executeUpdate();
    }
    catch (const sql::SQLException &e)
    {
      std::cerr << e.what() << '\n';
    }
    delete pstmt;
  }

  User get_user(sql::Connection *con, const std::string id)
  {
    sql::PreparedStatement *ps = con->prepareStatement("SELECT * FROM users WHERE id = ?");
    ps->setString(1, id);
    std::unique_ptr<sql::ResultSet> res(ps->executeQuery());

    if (res->next())
    {
      User user_details(
          res->getString("id"),
          res->getString("user_name"),
          res->getString("phone_number"),
          res->getString("user_location"),
          res->getString("created_at"),
          res->getString("updated_at"));
      return user_details;
    }
    else
    {
      throw std::runtime_error("User not found.");
    }
  }

  std::vector<User> get_all_users(sql::Connection *con)
  {
    try
    {
      std::vector<User> users;
      std::unique_ptr<sql::PreparedStatement> ps(con->prepareStatement(
          "SELECT * FROM users"));
      std::unique_ptr<sql::ResultSet> res(ps->executeQuery());
      while(res->next()) {
        std::string id = res->isNull("id")?"": res->getString("id");
        std::string user_name = res->isNull("user_name") ? "" : res->getString("user_name");
        std::string user_location = res->isNull("user_location") ? "" : res->getString("user_location");
        std::string phone_number = res->isNull("phone_number") ? "" : res->getString("phone_number");
        std::string created_at = res->isNull("created_at") ? "" : res->getString("created_at");
        std::string updated_at = res->isNull("updated_at") ? "" : res->getString("updated_at");

          users.push_back(
            User(
            id,
            user_name,
            phone_number,
            user_location,
            created_at,
            updated_at)
          );
      }
      return users;
    }
    catch (const sql::SQLException &e)
    {
      std::cerr << "SQL error: " << e.what() << '\n';
      throw; // Rethrow to preserve the original error
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error: " << e.what() << '\n';
      throw;
    }
  }
  void update_user(sql::Connection *con, User user)
  {
    std::string stmt = "UPDATE users SET ";
    std::vector<std::string> params;
    bool hasFieldsToUpdate = false;

    if (!user.get_user_name().empty())
    {
      stmt += "user_name = ?";
      params.push_back(user.get_user_name());
      hasFieldsToUpdate = true;
    }
    if (!user.get_phone_number().empty())
    {
      stmt += (hasFieldsToUpdate ? ", " : "") + std::string("phone_number = ?");
      params.push_back(user.get_phone_number());
      hasFieldsToUpdate = true;
    }
    if (!user.get_user_location().empty())
    {
      stmt += (hasFieldsToUpdate ? ", " : "") + std::string("user_location = ?");
      params.push_back(user.get_user_location());
      hasFieldsToUpdate = true;
    }

    if (!hasFieldsToUpdate)
    {
      throw std::invalid_argument("Nothing to update: all fields are empty.");
    }

    stmt += " WHERE id = ?";
    params.push_back(user.get_id());

    std::unique_ptr<sql::PreparedStatement> ps(con->prepareStatement(stmt));
    for (size_t i = 0; i < params.size(); ++i)
    {
      ps->setString(i + 1, params[i]);
    }

    try
    {
      ps->executeUpdate();
      std::cout << "User updated successfully." << std::endl;
    }
    catch (sql::SQLException &e)
    {
      std::cerr << "SQL error while updating user: " << e.what() << std::endl;
    }
  }

  void delete_user(sql::Connection *con, std::string id)
  {
    try
    {
      sql::PreparedStatement *ps = con->prepareStatement("DELETE FROM users WHERE id=?");
      ps->setString(1, id);
      ps->executeUpdate();
      std::cout << "User deleted successfully." << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << '\n';
    }
  }
}