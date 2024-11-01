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

  void create_booking(sql::Connection *con, Booking bkg)
  {
    sql::PreparedStatement *pstmt = con->prepareStatement("INSERT INTO bookings (user_id, listing_id,status,remark) VALUES (?, ?, ?,?)");
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