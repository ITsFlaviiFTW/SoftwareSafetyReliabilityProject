#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <cstdint>

// Structure for packet header information.
struct PacketHeader {
    std::string src;       // Source identifier.
    std::string dst;       // Destination identifier.
    std::string protocol;  // Communication protocol (e.g., "TCP").
    uint32_t pktNum;       // Packet number.
};

// Structure for packet tail information.
struct PacketTail {
    std::string errorCode; // Error code if any error occurred.
    std::string endMarker; // End-of-packet marker (e.g., "<EOF>").
};

// Packet class represents a communication packet with dynamic payload.
class Packet {
public:
    Packet();
    ~Packet();
    PacketHeader header;
    char* payload;         // Dynamically allocated payload.
    size_t payloadSize;
    PacketTail tail;

    // Constructs the packet with provided header fields, payload data, and error code.
    void constructPacket(const std::string& src, const std::string& dst, 
                         const std::string& protocol, uint32_t pktNum, 
                         const char* data, size_t size, const std::string& errorCode);
};

#endif // PACKET_H
