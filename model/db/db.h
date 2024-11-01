#ifndef DB
#include <mysql_connection.h>

namespace Database {
  void connect_to_db(sql::Connection* con);
}

#endif