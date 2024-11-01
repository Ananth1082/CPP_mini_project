#include "crow.h"
#include "model/listing_model.h"
#include "model/db/db.h"
int main()
{
    crow::SimpleApp app;
    int count = 0;
    sql::Connection *con;
    Database::connect_to_db(con);

    // serve html pages
    //   CROW_ROUTE(app, "/")
    //   ([]()
    //    {
    //        auto page = crow::mustache::load_text("index.html");
    //        return page; });

    //    //return string
    //    CROW_ROUTE(app, "/about").methods(crow::HTTPMethod::POST)
    //     ([]() {
    //         return "This is the about page.";
    //     });

    //     //Dynamic routes
    //     CROW_ROUTE(app, "/user/<string>/<string>")
    //     ([](std::string userid,std::string userAddress) {
    //         return "Hello user" + userid + "\nAddress: "+userAddress;
    //     });
    // CROW_ROUTE(app,"/listings").methods(crow::HTTPMethod::GET)([](){

    // });
    CROW_ROUTE(app, "/listing/<string>").methods(crow::HTTPMethod::GET)([con](const crow::request &req, crow::response &res, const std::string &id)
    {
    try {
        Database::Listing lst = Database::get_listing(con, id);
        res.write(lst.to_json().dump()); // Assuming to_json() returns a crow::json::wvalue
        res.end();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        res.code = 404; // Not Found
        res.write("No listing found");
        res.end();
    } });
    // CROW_ROUTE(app,"/listing").methods(crow::HTTPMethod::POST);
    // CROW_ROUTE(app,"/listing/").methods(crow::HTTPMethod::PUT);
    // CROW_ROUTE(app,"/listing").methods(crow::HTTPMethod::DELETE);

    app.port(18080)
        .multithreaded()
        .run();
}