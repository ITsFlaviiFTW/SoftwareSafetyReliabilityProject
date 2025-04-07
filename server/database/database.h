#ifndef DATABASE_H
#define DATABASE_H

#include <mysqlx/xdevapi.h>
#include "mysqlx/xdevapi.h"
#include <vector>
#include "models.h"

using namespace mysqlx;

class Database {
public:
    Database();
    ~Database();
    bool connect();
    void close();

    bool insertTechnician(const Technician&);
    bool insertAircraft(const Aircraft&);
    bool Database::insertTicket(const Ticket&);
    std::vector<Technician> getTechnicians();
    std::vector<Aircraft> getAircrafts();
    std::vector<Ticket> getTickets();
    std::vector<Ticket> getTicketsByAircraft(int);
    std::vector<Ticket> getTicketsByTechnician(const std::string&);
    

    Technician getTechnicianByEmail(const std::string&);
    Aircraft getAircraftByID(int aircraftID);

private:
    Session* session;
    Schema* schema;
};

#endif // DATABASE_H
