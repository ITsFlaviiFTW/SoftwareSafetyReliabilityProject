#ifndef DATABASE_H
#define DATABASE_H

#include <mysqlx/xdevapi.h>
#include <vector>
#include "models.h"

using namespace std;
using namespace mysqlx;

class Database {
public:
    Database();
    ~Database();
    bool connect();
    void close();

    bool insertTechnician(const Technician& tech);
    bool insertAircraft(const Aircraft& aircraft);
    vector<Technician> getTechnicians();
    vector<Aircraft> getAircrafts();

private:
    // Using pointers so we can manage connection lifetime explicitly.
    Session* session;
    Schema* schema;
};

#endif // DATABASE_H
