#include "crow.h"
#include "model/listing_model.h"
#include "model/user_model.h"
#include "model/booking_model.h"

#include "model/db/db.h"
#include <memory>
#include <mutex>

template <typename T>
crow::json::wvalue vector_to_json(const std::vector<T> &objects)
{
    crow::json::wvalue json_array;
    json_array = crow::json::wvalue::list();

    int index = 0;
    for (const auto &obj : objects)
    {
        json_array[index++] = obj.to_json();
    }

    return json_array;
}
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
    CROW_ROUTE(app, "/user/<string>")
        .methods(crow::HTTPMethod::GET)(
            [&](const crow::request &req, crow::response &res, std::string id)
            {
                auto con = get_connection();

                try
                {
                    Database::User user = Database::get_user(con, id);
                    res.code = 200;
                    res.write(user.to_json().dump());
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No user found");
                }

                return_connection(con);
                res.end();
            });
    CROW_ROUTE(app, "/user")
        .methods(crow::HTTPMethod::GET)(
            [&](const crow::request &req, crow::response &res)
            {
                auto con = get_connection();

                try
                {
                    auto users = Database::get_all_users(con);
                    res.code = 200;
                    res.write(vector_to_json(users).dump());
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No users found");
                }

                return_connection(con);
                res.end();
            });

    CROW_ROUTE(app, "/user")
        .methods(crow::HTTPMethod::POST)(
            [&](const crow::request &req, crow::response &res)
            {
        auto con = get_connection();

        try {
            crow::json::rvalue req_body = crow::json::load(req.body);
            if(!req_body.has("user_name")) {
                res.code=400;
                res.write("Missing required fields");
                res.end();
                return;
            }
            Database::User new_user(
            req_body["user_name"].s(),
            req_body["phone_number"].s(),
            req_body["user_location"].s());
            Database::create_user(con, new_user);
            res.code = 201;
            res.write("Created");
        } catch (const std::exception& e) {
            std::cerr << "Error processing request: " << e.what() << '\n';
            res.code = 400;
            res.write("Bad request");
        }

        return_connection(con);
        res.end(); });

    CROW_ROUTE(app, "/user/<string>")
        .methods(crow::HTTPMethod::DELETE)(
            [&](const crow::request &req, crow::response &res, const std::string user_id)
            {
                auto con = get_connection();

                try
                {
                    Database::delete_user(con, user_id);
                    res.code = 200;
                    res.write("deleted user");
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No user found");
                }
                return_connection(con);
                res.end();
            });
    CROW_ROUTE(app, "/listing")
        .methods(crow::HTTPMethod::GET)(
            [&](const crow::request &req, crow::response &res)
            {
                auto con = get_connection();
                try
                {
                    auto listings = Database::get_all_listings(con);
                    res.code = 200;
                    res.write(vector_to_json(listings).dump());
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No listing found");
                }
                return_connection(con);
                res.end();
            });

    CROW_ROUTE(app, "/listing/<string>")
        .methods(crow::HTTPMethod::GET)(
            [&](const crow::request &req, crow::response &res, const std::string &id)
            {
                auto con = get_connection();

                try
                {
                    Database::Listing lst = Database::get_listing(con, id);
                    res.code = 200;
                    res.write(lst.to_json().dump());
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No listing found");
                }

                return_connection(con);
                res.end();
            });
    CROW_ROUTE(app, "/listing")
        .methods(crow::HTTPMethod::POST)(
            [&](const crow::request &req, crow::response &res)
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
    CROW_ROUTE(app, "/listing/<string>")
        .methods(crow::HTTPMethod::DELETE)(
            [&](const crow::request &req, crow::response &res, const std::string lisitng_id)
            {
                auto con = get_connection();

                try
                {
                    Database::delete_listing(con, lisitng_id);
                    res.code = 200;
                    res.write("deleted listing");
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No listing found");
                }
                return_connection(con);
                res.end();
            });
    CROW_ROUTE(app, "/booking")
        .methods(crow::HTTPMethod::GET)(
            [&](const crow::request &req, crow::response &res)
            {
                auto con = get_connection();
                try
                {
                    auto bookings = Database::get_all_bookings(con);
                    res.code = 200;
                    res.write(vector_to_json(bookings).dump());
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No booking found");
                }
                return_connection(con);
                res.end();
            });
    CROW_ROUTE(app, "/booking/<string>")
        .methods(crow::HTTPMethod::GET)(
            [&](const crow::request &req, crow::response &res, const std::string &id)
            {
                auto con = get_connection();

                try
                {
                    auto booking = Database::get_booking(con, id);
                    res.code = 200;
                    res.write(booking.to_json().dump());
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No Booking found");
                }

                return_connection(con);
                res.end();
            });
    CROW_ROUTE(app, "/booking")
        .methods(crow::HTTPMethod::POST)(
            [&](const crow::request &req, crow::response &res)
            {
        auto con = get_connection();

        try {
            crow::json::rvalue req_body = crow::json::load(req.body);
            Database::Booking new_booking(
            "",
            req_body["user_id"].s(),
            req_body["listing_id"].s(),
            req_body["remarks"].s(),
            req_body["status"].s(),
            "","");
            Database::create_booking(con, new_booking);
            res.code = 201;
            res.write("Created");
        } catch (const std::exception& e) {
            std::cerr << "Error processing request: " << e.what() << '\n';
            res.code = 400;
            res.write("Bad request");
        }

        return_connection(con);
        res.end(); });
    CROW_ROUTE(app, "/booking/<string>")
        .methods(crow::HTTPMethod::DELETE)(
            [&](const crow::request &req, crow::response &res, const std::string booking_id)
            {
                auto con = get_connection();

                try
                {
                    Database::delete_user(con, booking_id);
                    res.code = 200;
                    res.write("deleted booking");
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error processing request: " << e.what() << '\n';
                    res.code = 404;
                    res.write("No booking found");
                }
                return_connection(con);
                res.end();
            });
    app.port(18080)
        .multithreaded()
        .run();

    connection_pool.clear();

    return 0;
}