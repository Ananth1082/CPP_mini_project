#include "booking_model.h"
namespace Database
{
  Booking::Booking(std::string id, std::string user_id, std::string listing_id, std::string remarks, std::string status, std::string created_at, std::string updated_at)
  {
    this->id = id;
    this->user_id = user_id;
    this->listing_id = listing_id;
    this->remarks = remarks;
    this->status = status;
    this->created_at = created_at;
    this->updated_at = updated_at;
  }

  std::string Booking::get_id() { return id; }
  std::string Booking::get_user_id() { return user_id; }
  std::string Booking::get_listing_id() { return listing_id; }
  std::string Booking::get_status() { return status; }
  std::string Booking::get_remark() { return remarks; }

  crow::json::wvalue Booking::to_json() const
  {
    crow::json::wvalue json;
    json["id"] = id;
    json["user_id"]=user_id;
    json["listing_id"] = listing_id;
    json["remarks"] = remarks;
    json["status"] = status;
    json["created_at"] = created_at;
    json["updated_at"] = updated_at;

    return json;
  }

  void create_booking(sql::Connection *con, Booking bkg)
  {
    sql::PreparedStatement *pstmt = con->prepareStatement("INSERT INTO bookings (user_id, listing_id,status,remarks) VALUES (?, ?, ?,?)");
    try
    {

      pstmt->setString(1, bkg.get_user_id());
      pstmt->setString(2, bkg.get_listing_id());
      pstmt->setString(3, bkg.get_status());
      pstmt->setString(4, bkg.get_remark());

      pstmt->executeUpdate();
    }
    catch (const sql::SQLException &e)
    {
      std::cerr << e.what() << '\n';
    }
    delete pstmt;
  }

  Booking get_booking(sql::Connection *con, const std::string id)
  {
    sql::PreparedStatement *ps = con->prepareStatement("SELECT * FROM bookings WHERE id = ?");
    ps->setString(1, id);
    std::unique_ptr<sql::ResultSet> res(ps->executeQuery());

    if (res->next())
    {
      Booking booking_details(
          res->getString("id"),
          res->getString("user_id"),
          res->getString("listing_id"),
          res->getString("remarks"),
          res->getString("status"),
          res->getString("created_at"),
          res->getString("updated_at"));
      return booking_details;
    }
    else
    {
      throw std::runtime_error("Booking not found.");
    }
  }

  std::vector<Booking> get_all_bookings(sql::Connection *con)
  {
    try
    {
      std::vector<Booking> bookings;
      std::unique_ptr<sql::PreparedStatement> ps(con->prepareStatement(
          "SELECT * FROM bookings"));
      std::unique_ptr<sql::ResultSet> res(ps->executeQuery());
      while (res->next())
      {
        std::string booking_id = res->isNull("id") ? "" : res->getString("id");
        std::string listing_id = res->isNull("listing_id") ? "" : res->getString("listing_id");
        std::string user_id = res->isNull("user_id") ? "" : res->getString("user_id");
        std::string remarks = res->isNull("remarks") ? "" : res->getString("remarks");
        std::string status = res->isNull("status") ? "" : res->getString("status");
        std::string created_at = res->isNull("created_at") ? "" : res->getString("created_at");
        std::string updated_at = res->isNull("updated_at") ? "" : res->getString("updated_at");

        bookings.push_back(
            Booking(
                booking_id,
                user_id,
                listing_id,
                remarks,
                status,
                created_at,
                updated_at));
      }
      return bookings;
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

  void delete_booking(sql::Connection *con, std::string id)
  {
    try
    {
      sql::PreparedStatement *ps = con->prepareStatement("DELETE FROM bookings WHERE id=?");
      ps->setString(1, id);
      ps->executeUpdate();
      std::cout << "Booking deleted successfully." << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << '\n';
    }
  }
}