#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "../user_model.h"
#include "../listing_model.h"
#include "../booking_model.h"
#include "db.h"

std::vector<std::string> splitSQLStatements(const std::string &sql)
{
  std::vector<std::string> statements;
  std::string statement;
  std::istringstream stream(sql);
  while (std::getline(stream, statement, ';'))
  {
    if (!statement.empty())
    {
      statements.push_back(statement + ";");
    }
  }
  return statements;
}

void executeSQLFile(sql::Connection *con, const std::string &filename)
{
  std::ifstream file(filename);
  std::stringstream buffer;
  buffer << file.rdbuf();

  std::string sql = buffer.str();
  std::vector<std::string> statements = splitSQLStatements(sql);

  sql::Statement *stmt = con->createStatement();
  for (const auto &statement : statements)
  {
    try
    {
      stmt->execute(statement);
      std::cout << "Executed: " << statement << std::endl;
    }
    catch (sql::SQLException &e)
    {
      std::cerr << "Error executing SQL command: " << statement << "\nError: " << e.what() << std::endl;
    }
  }

  delete stmt;
  std::cout << "Schema created successfully from " << filename << std::endl;
}

void migrations(sql::Connection *con)
{
  executeSQLFile(con, "schema.sql");
}
void Database::connect_to_db(sql::Connection*& con)  // Note the & here
{
    try {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        if (!driver) {
            throw std::runtime_error("Failed to get MySQL driver instance");
        }

        con = driver->connect("tcp://127.0.0.1:3306", "root", "root_password");
        if (!con) {
            throw std::runtime_error("Failed to establish database connection");
        }

        std::unique_ptr<sql::Statement> useStatement(con->createStatement());
        useStatement->execute("use real_estate_db");
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << '\n';
        std::cerr << "SQL State: " << e.getSQLState() << '\n';
        if (con) {
            delete con;
            con = nullptr;
        }
        throw;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        if (con) {
            delete con;
            con = nullptr;
        }
        throw;
    }
}
