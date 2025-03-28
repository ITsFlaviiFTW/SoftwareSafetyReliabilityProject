#include "packet.h"
#include <cstring>
#include <cstdlib>

// Constructor initializes payload pointer and sets fixed tail marker.
Packet::Packet() : payload(nullptr), payloadSize(0) {
    tail.endMarker = "<EOF>"; // Fixed marker indicating end of packet.
}

// Destructor frees the dynamically allocated payload.
Packet::~Packet() {
    if (payload) {
        free(payload);
    }
}

// Construct the packet with header, payload, and tail information.
void Packet::constructPacket(const std::string& src, const std::string& dst, 
                             const std::string& protocol, uint32_t pktNum, 
                             const char* data, size_t size, const std::string& errorCode) {
    header.src = src;
    header.dst = dst;
    header.protocol = protocol;
    header.pktNum = pktNum;
    payloadSize = size;
    payload = (char*)malloc(size);
    if (payload && data) {
        memcpy(payload, data, size);
    }
    tail.errorCode = errorCode;
}
