#include "crow.h"
#include "model/listing_model.h"
#include "model/db/db.h"
#include <memory>
#include <mutex>

int main()
{
    std::cout << "Starting crow server...\n";
    crow::SimpleApp app;

    const int POOL_SIZE = 10;
    std::vector<std::unique_ptr<sql::Connection>> connection_pool;
    std::mutex pool_mutex;

    try
    {
        for (int i = 0; i < POOL_SIZE; i++)
        {
            sql::Connection *con = nullptr;
            Database::connect_to_db(con);
            if (con != nullptr)
            {
                connection_pool.push_back(std::unique_ptr<sql::Connection>(con));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to initialize connection pool: " << e.what() << '\n';
        return 1;
    }

    auto get_connection = [&]() -> sql::Connection *
    {
        std::lock_guard<std::mutex> lock(pool_mutex);
        if (!connection_pool.empty())
        {
            sql::Connection *con = connection_pool.back().release();
            connection_pool.pop_back();
            return con;
        }
        sql::Connection *con = nullptr;
        Database::connect_to_db(con);
        return con;
    };

    auto return_connection = [&](sql::Connection *con)
    {
        if (con != nullptr)
        {
            std::lock_guard<std::mutex> lock(pool_mutex);
            connection_pool.push_back(std::unique_ptr<sql::Connection>(con));
        }
    };

    try
    {
        auto con = get_connection();
        if (con)
        {
            Database::Listing lst = Database::get_listing(con, "018be850-985e-11ef-861c-0242ac110002");
            std::cout << "Test listing name: " << lst.get_listing_name() << '\n';
            return_connection(con);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Test query failed: " << e.what() << '\n';
    }
    CROW_ROUTE(app, "/listing/<string>")
        .methods(crow::HTTPMethod::GET)([&](const crow::request &req, crow::response &res, const std::string &id)
                                        {
        auto con = get_connection();

        try {
            Database::Listing lst = Database::get_listing(con, id);
            res.code = 200;
            res.write(lst.to_json().dump());
        } catch (const std::exception& e) {
            std::cerr << "Error processing request: " << e.what() << '\n';
            res.code = 404;
            res.write("No listing found");
        }

        return_connection(con);
        res.end(); });

    CROW_ROUTE(app, "/listing")
        .methods(crow::HTTPMethod::POST)([&](const crow::request &req, crow::response &res)
                                         {
        auto con = get_connection();

        try {
            crow::json::rvalue req_body = crow::json::load(req.body);
            if(!req_body.has("listing_name") || !req_body.has("user_id") || !req_body.has("price")) {
                res.code=400;
                res.write("Missing required fields");
                res.end();
                return;
            }
            Database::Listing new_listing(
            req_body["listing_name"].s(),
            req_body["user_id"].s(),
            req_body["address"].s(),
            req_body["listing_location"].s(),
            req_body["description"].s(),
            req_body["tags"].s(),
            req_body["price"].i());
            Database::create_listing(con, new_listing);
            res.code = 201;
            res.write("Created");
        } catch (const std::exception& e) {
            std::cerr << "Error processing request: " << e.what() << '\n';
            res.code = 400;
            res.write("Bad request");
        }

        return_connection(con);
        res.end(); });

    app.port(18080)
        .multithreaded()
        .run();

    connection_pool.clear();

    return 0;
}