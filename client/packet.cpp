#include "packet.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <openssl/md5.h>  // Ensure OpenSSL is installed & linked on Windows
#include <sstream>
#include <iomanip>

static std::string md5Hex(const std::string& input) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(input.data()), input.size(), digest);

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        oss << std::setw(2) << static_cast<int>(digest[i]);
    }
    return oss.str();
}

std::string createPacket(const std::string& source,
                         const std::string& destination,
                         const std::string& protocol,
                         int packetNumber,
                         const std::string& payload)
{
    std::ostringstream header;
    header << "SRC=" << source << ";"
           << "DST=" << destination << ";"
           << "PROT=" << protocol << ";"
           << "NUM=" << packetNumber << ";";

    std::string checksum = md5Hex(payload);

    std::ostringstream tail;
    tail << "TAIL_END;ERRCHK=" << checksum;

    return header.str() + payload + tail.str();
}

std::string extractPayload(const std::string& fullPacket) {
    size_t tailPos = fullPacket.find("TAIL_END;");
    if (tailPos == std::string::npos) {
        return "";
    }

    size_t numPos = fullPacket.find("NUM=");
    if (numPos == std::string::npos) {
        return "";
    }
    size_t payloadStart = fullPacket.find(";", numPos);
    if (payloadStart == std::string::npos) {
        return "";
    }
    payloadStart += 1; // Skip the semicolon

    if (payloadStart >= tailPos) {
        return "";
    }

    return fullPacket.substr(payloadStart, tailPos - payloadStart);
}
