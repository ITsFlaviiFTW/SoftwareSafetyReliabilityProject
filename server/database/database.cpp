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

// Creates a connection to the database
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

// Closes connection to the database
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

// Inserts an aircraft into the database
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

// Gets a list of all aircrafts
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
            aircraft.LastUpdated = row[4].get<std::string>();
            aircrafts.push_back(aircraft);
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Aircrafts Error: " << err.what() << std::endl;
    }
    return aircrafts;
}

// Searches for an aircraft in the database by their ID
Aircraft Database::getAircraftByID(int aircraftID) {
    Aircraft aircraft;
    try {
        mysqlx::Table table = schema->getTable("Aircraft");
        mysqlx::RowResult res = table.select("*")
            .where("AircraftID = :aircraftID")
            .bind("aircraftID", aircraftID)
            .execute();

        mysqlx::Row row = res.fetchOne();
        if (!row.isNull()) {
            aircraft.AircraftID  = row[0].get<int>();
            aircraft.TailNumber  = row[1].get<std::string>();
            aircraft.Model       = row[2].get<std::string>();
            aircraft.Status      = row[3].get<std::string>();
            aircraft.LastUpdated = row[4].get<std::string>();
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Aircraft By ID Error: " << err.what() << std::endl;
    }
    return aircraft; // Returns an empty Aircraft if no match is found
}

// Inserts a technician into the database
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

// Gets a list of all technicians
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
            tech.CreatedAt    = row[5].get<std::string>();
            techs.push_back(tech);
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Technicians Error: " << err.what() << std::endl;
    }
    return techs;
}

// Searches for a technician in the database by their email address
Technician Database::getTechnicianByEmail(const std::string& email) {
    Technician tech;
    try {
        mysqlx::Table table = schema->getTable("Technicians");
        mysqlx::RowResult res = table.select("*")
            .where("Email = :email")
            .bind("email", email)
            .execute();

        mysqlx::Row row = res.fetchOne();
        if (!row.isNull()) {
            tech.TechnicianID = row[0].get<int>();
            tech.FirstName    = row[1].get<std::string>();
            tech.LastName     = row[2].get<std::string>();
            tech.Email        = row[3].get<std::string>();
            tech.Password     = row[4].get<std::string>();
            tech.CreatedAt    = row[5].get<std::string>();
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Technician By Email Error: " << err.what() << std::endl;
    }
    return tech; // Returns default Technician if no match is found
}

bool Database::insertTicket(const Ticket& ticket) {
    try {
        mysqlx::Table table = schema->getTable("Tickets");
        table.insert("AircraftID", "TechnicianEmail", "Name", "Description")
            .values(ticket.AircraftID, ticket.TechnicianEmail, ticket.Name, ticket.Description)
            .execute();
    } catch (const mysqlx::Error &err) {
        std::cerr << "Insert Ticket Error: " << err.what() << std::endl;
        return false;
    }
    return true;
}

std::vector<Ticket> Database::getTickets() {
    std::vector<Ticket> tickets;
    try {
        mysqlx::Table table = schema->getTable("Tickets");
        mysqlx::RowResult res = table.select("*").execute();

        for (mysqlx::Row row : res) {
            Ticket ticket;
            ticket.TicketID = row[0].get<int>();
            ticket.AircraftID = row[1].get<int>();
            ticket.TechnicianEmail = row[2].get<std::string>();
            ticket.Name = row[3].get<std::string>();
            ticket.Description = row[4].get<std::string>();
            ticket.CreatedAt = row[5].get<std::string>();
            tickets.push_back(ticket);
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Tickets Error: " << err.what() << std::endl;
    }
    return tickets;
}

std::vector<Ticket> Database::getTicketsByAircraft(int aircraftID) {
    std::vector<Ticket> tickets;
    try {
        mysqlx::Table table = schema->getTable("Tickets");
        mysqlx::RowResult res = table.select("*")
            .where("AircraftID = :aircraftID")
            .bind("aircraftID", aircraftID)
            .execute();

        for (mysqlx::Row row : res) {
            Ticket ticket;
            ticket.TicketID = row[0].get<int>();
            ticket.AircraftID = row[1].get<int>();
            ticket.TechnicianEmail = row[2].get<std::string>();
            ticket.Name = row[3].get<std::string>();
            ticket.Description = row[4].get<std::string>();
            ticket.CreatedAt = row[5].get<std::string>();
            tickets.push_back(ticket);
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Tickets By Aircraft Error: " << err.what() << std::endl;
    }
    return tickets;
}

std::vector<Ticket> Database::getTicketsByTechnician(const std::string& email) {
    std::vector<Ticket> tickets;
    try {
        mysqlx::Table table = schema->getTable("Tickets");
        mysqlx::RowResult res = table.select("*")
            .where("TechnicianEmail = :email")
            .bind("email", email)
            .execute();

        for (mysqlx::Row row : res) {
            Ticket ticket;
            ticket.TicketID = row[0].get<int>();
            ticket.AircraftID = row[1].get<int>();
            ticket.TechnicianEmail = row[2].get<std::string>();
            ticket.Name = row[3].get<std::string>();
            ticket.Description = row[4].get<std::string>();
            ticket.CreatedAt = row[5].get<std::string>();
            tickets.push_back(ticket);
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Get Tickets By Technician Error: " << err.what() << std::endl;
    }
    return tickets;
}