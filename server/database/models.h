#ifndef MODELS_H
#define MODELS_H

#include <string>

struct Technician {
    int TechnicianID;
    std::string FirstName;
    std::string LastName;
    std::string Email;
    std::string Password;
    std::string CreatedAt;
};

struct Aircraft {
    int AircraftID;
    std::string TailNumber;
    std::string Model;
    std::string Status;
    std::string LastUpdated;
};

struct Ticket {
    int TicketID;
    int AircraftID;
    std::string TechnicianEmail;
    std::string Name;
    std::string Description;
    std::string CreatedAt;
};

#endif // MODELS_H