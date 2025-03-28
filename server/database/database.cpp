#include "database.h"
#include <iostream>

#define DB_HOST "database-1.cfl4q3qmo0sa.us-east-2.rds.amazonaws.com"
#define DB_PORT 3306
#define DB_USER "admin"
#define DB_PASS "manateesareprettycool1"
#define DB_NAME "airport-task-manager"

Database::Database() : session(nullptr), schema(nullptr) {}

Database::~Database() {
    close();
}

bool Database::connect() {
    try {
        // Establish a session using the X DevAPI.
        session = new mysqlx::Session(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_NAME);
        // Retrieve the schema (database) object.
        schema = new mysqlx::Schema(session->getSchema(DB_NAME));
    } catch (const mysqlx::Error &err) {
        std::cerr << "MySQL X DevAPI Connection Error: " << err.what() << std::endl;
        return false;
    } catch (std::exception &ex) {
        std::cerr << "STD Exception: " << ex.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error occurred while connecting." << std::endl;
        return false;
    }
    return true;
}

void Database::close() {
    if (session) {
        session->close();
        delete session;
        session = nullptr;
    }
    if (schema) {
        delete schema;
        schema = nullptr;
    }
}

bool Database::insertTechnician(const Technician& tech) {
    try {
        // Get the table object for 'Technicians'
        mysqlx::Table table = schema->getTable("Technicians");
        table.insert("FirstName", "LastName", "Email", "Password")
            .values(tech.FirstName, tech.LastName, tech.Email, tech.Password)
            .execute();
    } catch (const mysqlx::Error &err) {
        std::cerr << "Insert Technician Error: " << err.what() << std::endl;
        return false;
    }
    return true;
}

bool Database::insertAircraft(const Aircraft& aircraft) {
    try {
        // Get the table object for 'Aircraft'
        mysqlx::Table table = schema->getTable("Aircraft");
        table.insert("TailNumber", "Model", "Status")
            .values(aircraft.TailNumber, aircraft.Model, aircraft.Status)
            .execute();
    } catch (const mysqlx::Error &err) {
        std::cerr << "Insert Aircraft Error: " << err.what() << std::endl;
        return false;
    }
    return true;
}

std::vector<Technician> Database::getTechnicians() {
    std::vector<Technician> techs;
    try {
        mysqlx::Table table = schema->getTable("Technicians");
        mysqlx::RowResult res = table.select("*").execute();
        for (mysqlx::Row row : res) {
            Technician tech;
            tech.TechnicianID = row[0].get<int>();
            tech.FirstName    = row[1].get<std::string>();
            tech.LastName     = row[2].get<std::string>();
            tech.Email        = row[3].get<std::string>();
            tech.Password     = row[4].get<std::string>();
            tech.CreatedAt    = row[5].get<std::string>(); // Assuming TIMESTAMP is convertible to std::string
            techs.push_back(tech);
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Technicians Error: " << err.what() << std::endl;
    }
    return techs;
}

std::vector<Aircraft> Database::getAircrafts() {
    std::vector<Aircraft> aircrafts;
    try {
        mysqlx::Table table = schema->getTable("Aircraft");
        mysqlx::RowResult res = table.select("*").execute();
        for (mysqlx::Row row : res) {
            Aircraft aircraft;
            aircraft.AircraftID = row[0].get<int>();
            aircraft.TailNumber = row[1].get<std::string>();
            aircraft.Model      = row[2].get<std::string>();
            aircraft.Status     = row[3].get<std::string>();
            aircraft.LastUpdated = row[4].get<std::string>(); // Assuming TIMESTAMP is convertible to std::string
            aircrafts.push_back(aircraft);
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Aircrafts Error: " << err.what() << std::endl;
    }
    return aircrafts;
}
