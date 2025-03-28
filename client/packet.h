#ifndef PACKET_H
#define PACKET_H

#include <string>

/**
 * Packet format (very simplified):
 *   Header: "SRC=...;DST=...;PROT=...;NUM=...;"
 *   Payload: (the data)
 *   Tail: "TAIL_END;ERRCHK=<checksum>"
 */

std::string createPacket(const std::string& source,
                         const std::string& destination,
                         const std::string& protocol,
                         int packetNumber,
                         const std::string& payload);

std::string extractPayload(const std::string& fullPacket);

#endif // PACKET_H
