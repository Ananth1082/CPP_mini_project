#include "listing_model.h"
namespace Database
{
  Listing::Listing(std::string id, std::string listing_name, std::string user_id, std::string address, std::string listing_location, std::string description, std::string tags, long int price, std::string created_at, std::string updated_at)
  {
    this->id = id;
    this->listing_name = listing_name;
    this->user_id = user_id;
    this->address = address;
    this->listing_location = listing_location;
    this->description = description;
    this->price = price;
    this->tags = tags;
    this->created_at = created_at;
    this->updated_at = updated_at;
  }

  Listing::Listing(std::string listing_name, std::string user_id, std::string address, std::string listing_location, std::string description, std::string tags, long int price)
  {
    this->listing_name = listing_name;
    this->user_id = user_id;
    this->address = address;
    this->listing_location = listing_location;
    this->description = description;
    this->price = price;
    this->tags = tags;
  }

  std::string Listing::get_id() { return id; }
  std::string Listing::get_listing_name() { return listing_name; }
  std::string Listing::get_user_id() { return user_id; }
  std::string Listing::get_listing_location() { return listing_location; }
  std::string Listing::get_description() { return description; }
  long int Listing::get_price() { return price; }
  std::string Listing::get_tags() { return tags; }
  std::string Listing::get_address() { return address; }

  void create_listing(sql::Connection *con, Listing lst)
  {
    sql::PreparedStatement *pstmt = con->prepareStatement("INSERT INTO listings (listing_name, user_id,address,listing_location,price,description,tags) VALUES (?, ?, ?,?,?,?,?)");
    try
    {

      pstmt->setString(1, lst.get_listing_name());
      pstmt->setString(2, lst.get_user_id());
      pstmt->setString(3, lst.get_address());
      pstmt->setString(4, lst.get_listing_location());
      pstmt->setInt(5, lst.get_price());
      pstmt->setString(6, lst.get_description());
      pstmt->setString(7, lst.get_tags());

      pstmt->executeUpdate();
    }
    catch (const sql::SQLException &e)
    {
      std::cerr << e.what() << '\n';
    }
    delete pstmt;
  }

  Listing get_listing(sql::Connection *con, std::string id) {
    try {
        // Use smart pointers for resource management
        std::unique_ptr<sql::PreparedStatement> ps(con->prepareStatement("SELECT * FROM listings WHERE id = ?"));
        ps->setString(1, id);
        
        std::unique_ptr<sql::ResultSet> res(ps->executeQuery());

        if (res->next()) {
            // Directly construct Listing from ResultSet
            return Listing(
                res->getString("id"),
                res->getString("listing_name"),
                res->getString("user_id"),
                res->getString("address"),
                res->getString("listing_location"),
                res->getString("description"),
                res->getString("tags"),
                res->getInt64("price"),
                res->getString("created_at"),
                res->getString("updated_at")
            );
        } else {
            throw std::runtime_error("Listing not found.");
        }
    } catch (const sql::SQLException &e) {
        std::cerr << "SQL error: " << e.what() << '\n';
        throw std::runtime_error("Error getting listing details"); 
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        throw std::runtime_error("Error getting listing details"); 
    }
}
  void update_listing(sql::Connection *con, Listing lst)
  {
    std::string stmt = "UPDATE listings SET ";
    std::vector<std::string> params;
    bool hasFieldsToUpdate = false;

    if (!lst.get_listing_name().empty())
    {
      stmt += "listing_name = ?";
      params.push_back(lst.get_listing_name());
      hasFieldsToUpdate = true;
    }
    if (!lst.get_listing_location().empty())
    {
      stmt += (hasFieldsToUpdate ? ", " : "") + std::string("listing_location = ?");
      params.push_back(lst.get_listing_location());
      hasFieldsToUpdate = true;
    }
    if (!lst.get_listing_location().empty())
    {
      stmt += (hasFieldsToUpdate ? ", " : "") + std::string("listing_location = ?");
      params.push_back(lst.get_listing_location());
      hasFieldsToUpdate = true;
    }

    if (!hasFieldsToUpdate)
    {
      throw std::invalid_argument("Nothing to update: all fields are empty.");
    }

    stmt += " WHERE id = ?";
    params.push_back(lst.get_id());

    std::unique_ptr<sql::PreparedStatement> ps(con->prepareStatement(stmt));
    for (size_t i = 0; i < params.size(); ++i)
    {
      ps->setString(i + 1, params[i]);
    }

    try
    {
      ps->executeUpdate();
      std::cout << "Listing updated successfully." << std::endl;
    }
    catch (sql::SQLException &e)
    {
      std::cerr << "SQL error while updating listing: " << e.what() << std::endl;
    }
  }

  void delete_listing(sql::Connection *con, std::string id)
  {
    try
    {
      sql::PreparedStatement *ps = con->prepareStatement("DELETE FROM listings WHERE id=?");
      ps->setString(1, id);
      ps->executeUpdate();
      std::cout << "Listing deleted successfully." << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << '\n';
    }
  }
}