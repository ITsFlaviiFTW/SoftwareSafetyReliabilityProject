#ifndef MODELS_H
#define MODELS_H

#include <string>

using namespace std;

struct Technician {
    int TechnicianID;
    string FirstName;
    string LastName;
    string Email;
    string Password;
    string CreatedAt;
};

struct Aircraft {
    int AircraftID;
    string TailNumber;
    string Model;
    string Status;
    string LastUpdated;
};

#endif // MODELS_H