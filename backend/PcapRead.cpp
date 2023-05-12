#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <winsock.h>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;
uint32_t value = 0x12345678;
struct PcapGlobalHeader {
    uint32_t magic_number;  /* magic number */
    uint16_t version_major; /* major version number */
    uint16_t version_minor; /* minor version number */
    int32_t  thiszone;      /* GMT to local correction */
    uint32_t sigfigs;       /* accuracy of timestamps */
    uint32_t snaplen;       /* max length of captured packets, in octets */
    uint32_t network;       /* data link type */
};

struct PcapPacketHeader {
    uint32_t ts_sec;   // timestamp seconds
    uint32_t ts_usec;  // timestamp microseconds/nanoseconds
    uint32_t incl_len; // number of octets of packet saved in file
    uint32_t orig_len; // actual length of packet
};

struct PcapPacket {
    PcapPacketHeader pcapPacketHeader;
    vector<uint8_t> data;
};

struct EthernetHeader {
    uint8_t destinationMAC[6];  // Destination MAC address
    uint8_t sourceMAC[6];       // Source MAC address
    uint16_t etherType;         // Ethernet type field
};

struct IPHeader {
    uint8_t versionIHL;         // Version and Internet Header Length
    uint8_t typeOfService;      // Type of Service
    uint16_t totalLength;       // Total length of the IP packet
    uint16_t identification;    // Identification field
    uint16_t flagsFragmentOffset;  // Flags and Fragment Offset
    uint8_t timeToLive;         // Time to Live
    uint8_t protocol;           // Protocol type
    uint16_t headerChecksum;    // Header checksum
    uint32_t sourceIP;          // Source IP address
    uint32_t destinationIP;     // Destination IP address
};

int main() {
    string filePath;
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }
    // Open the PCAP file fuzz-2006-07-09-6023
    ifstream file("C:\\Users\\cerce\\Desktop\\fuzz-2006-07-09-6023.pcap", ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening pcap file" << endl;
        return 1;
    }

    // Read the PCAP file header
    PcapGlobalHeader file_header;
    file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header)); //reads pcap file header and interprets it
//    file_header.magic_number = ntohl(file_header.magic_number);//ntohl
//    file_header.version_major = ntohs(file_header.version_major);
//    file_header.version_minor = ntohs(file_header.version_minor);
//    file_header.thiszone = ntohl(file_header.thiszone);//
//    file_header.sigfigs = ntohl(file_header.sigfigs);//
//    file_header.snaplen = ntohl(file_header.snaplen);//
//    file_header.network = ntohl(file_header.network);//
    uint32_t networkValue = htonl(value);

    if (value == networkValue) {
        std::cout << "System is using big-endian byte order." << std::endl;
    } else {
        std::cout << "System is using little-endian byte order." << std::endl;
    }

    WSACleanup();
    stringstream ss;
    ss << hex << file_header.magic_number;
    if (ss.str() == "a1b2c3d4")
        cout << "ENDIANESS: BIG ENDIAN\n";
    else cout << "ENDIANESS: LITTLE ENDIAN\n";
    cout << "VERSION: " << file_header.version_major << "." << file_header.version_minor << "\n";
    cout << "Thiszone(not important): " << file_header.thiszone << "\n";
    cout << "Sigfigs(not important): " << file_header.sigfigs << "\n";
    cout << "MAX BYTES ALLOWED IN PACKETS: " << file_header.snaplen << " bytes\n";
    if (file_header.network == 1)
        cout << "NETWORK: ETHERNET\n";
    else if (file_header.network == 105)
        cout << "NETWORK: IEEE 802.11 wireless LAN\n";
    else if (file_header.network == 9)
        cout << "NETWORK: PPP(Point-to-point-protocol)\n";
    else if (file_header.network == 104)
        cout << "NETWORK: HDLC(High-Level Data Link Control)\n";
    else cout << "NETWORK: FDDI(Fiber Distributed Data Interface)\n";
    cout << "\n\n";

    // Loop through the packets in the file
    vector<PcapPacket>packets;
    PcapPacketHeader packetHeader;
    while (file.read(reinterpret_cast<char*>(&packetHeader), sizeof(packetHeader))) {
        PcapPacket pcapPacket;
        pcapPacket.pcapPacketHeader = packetHeader;
        pcapPacket.data.resize(ntohl(packetHeader.incl_len));
        file.read(reinterpret_cast<char*>(pcapPacket.data.data()), ntohl(packetHeader.incl_len));
        packets.push_back(pcapPacket);
    }
    // Close the PCAP file
    file.close();

    for (int i = 0; i < packets.size(); ++i) {
        PcapPacket packet = packets[i];
        cout << "Packet " << i+1 << "\n";
        packet.pcapPacketHeader.ts_sec = ntohl(packet.pcapPacketHeader.ts_sec);
        packet.pcapPacketHeader.ts_usec = ntohl(packet.pcapPacketHeader.ts_usec);
        packet.pcapPacketHeader.incl_len = ntohl(packet.pcapPacketHeader.incl_len);
        packet.pcapPacketHeader.orig_len = ntohl(packet.pcapPacketHeader.orig_len);

        cout << "TIMESTAMP: " << packet.pcapPacketHeader.ts_sec << "." << packet.pcapPacketHeader.ts_usec << "\n";
        cout << "CAPTURED LENGTH: " << packet.pcapPacketHeader.incl_len << " bytes\n";
        cout << "ORIGINAL LENGTH: " << packet.pcapPacketHeader.orig_len << " bytes\n";
        for (size_t i = 0; i < packet.data.size(); ++i) {
            cout << packet.data[i];

            if ((i + 1) % 16 == 0)
                cout << "\n";
        }
        cout << "\n\n";
    }
    //print packets
    return 0;

}
void parseEthernet(vector<uint8_t> packetData) {
    EthernetHeader ethernetHeader;
    memcpy(&ethernetHeader, packetData.data(), sizeof(EthernetHeader));

    //byte order reversing for ethernet header
    //byte order for destinationMac and printing
    reverse(begin(ethernetHeader.destinationMAC), end(ethernetHeader.destinationMAC));
    cout << "DESTINATION MAC ADRESS: " << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        cout << setw(2) << static_cast<int>(ethernetHeader.destinationMAC[i]);
        if (i < 5)
            cout << ":";
    }
    cout << dec << "\n";

    //byte order for sourceMac and printing
    reverse(begin(ethernetHeader.sourceMAC), end(ethernetHeader.sourceMAC));
    cout << "SOURCE MAC ADRESS: " << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        cout << setw(2) << static_cast<int>(ethernetHeader.sourceMAC[i]);
        if (i < 5)
            cout << ":";
    }
    cout << dec << "\n";

    cout << "TYPE: ";
    ethernetHeader.etherType = ntohs(ethernetHeader.etherType);
    if (ethernetHeader.etherType == 2048){
        cout << "IPv4\n";
        IPHeader ipHeader;
        memcpy(&ipHeader, packetData.data() + sizeof(EthernetHeader), sizeof(IPHeader));

        //version and internet header length (ihl)
        uint8_t version = (ipHeader.versionIHL >> 4) & 0x0F;
        cout << "VERSION: " << static_cast<unsigned int>(version) << "\n";
        uint8_t ihl = (ipHeader.versionIHL & 0x0F) * 4;
        cout << "INTERNET HEADER LENGTH(IHL): " << static_cast<unsigned int>(ihl) <<"\n";

        //type of service and Differentiated Service Code Point(DSCP)
        cout << "TYPE OF SERVICE: ";
        uint8_t dscp = (ipHeader.typeOfService >> 2) & 0x3F;
        if (static_cast<unsigned int>(dscp) == 0)
            cout << "Default\n";
        else if (static_cast<unsigned int>(dscp) == 46)
            cout << "Expedited Forwarding\n";
        else if (dscp == 0x08 or dscp == 0x0C or dscp == 0x10 or dscp == 0x10 or dscp == 0x1C or dscp == 0x20 or dscp == 0x28 or dscp == 0x2C or dscp == 0x30 or dscp == 0x38 or dscp == 0x3C or dscp == 0x40)
            cout << "Assured Forwarding\n";
        else cout << "Unknown\n";

        //total length
        ipHeader.totalLength = ntohs(ipHeader.totalLength);
        cout << "TOTAL LENGTH: " << ipHeader.totalLength << " bytes\n";

        //identification field
        ipHeader.identification = ntohs(ipHeader.identification);
        cout << "IDENTIFICATION: " << ipHeader.identification << "\n";

        //flags and fragment offset
        ipHeader.flagsFragmentOffset = ntohs(ipHeader.flagsFragmentOffset);
        uint16_t flag = (ipHeader.flagsFragmentOffset & 0xE000) >> 13;
        uint16_t fragmentOffset = ipHeader.flagsFragmentOffset & 0x1FFF;
        bool isDFSet = (flag & 0x4000) != 0;
        bool isMFSet = (flag & 0x2000) != 0;

        cout << "FLAG: ";
        if (isDFSet)
            cout << "Don't Fragment (DF)\n";
        else if (isMFSet)
            cout << "More Fragments (MF)\n";
        else if (flag == 0)
            cout << "Reserved\n";
        else cout << "Unknown\n";
        cout << "FRAGMENT OFFSET: " << fragmentOffset << "\n";

        //time to live
        cout << "TIME TO LIVE: " << static_cast<int>(ipHeader.timeToLive) << "\n";

        //protocol
        cout << "PROTOCOL: ";
        if(static_cast<int>(ipHeader.protocol) == 1){
            cout << "ICMP //TO DO//\n";
        }
        else if (static_cast<int>(ipHeader.protocol) == 6){
            cout << "TCP //TO DO//\n";
        }
        else if (static_cast<int>(ipHeader.protocol) == 17){
            cout << "UDP //TO DO//\n";
        }
        else if (static_cast<int>(ipHeader.protocol) == 41){ //TO DISCUSS!!!
            cout << "IPv6?? //TO DO//\n";
        }
        else if (static_cast<int>(ipHeader.protocol) == 50){
            cout << "ESP //TO DO//\n";
        }
        else if (static_cast<int>(ipHeader.protocol) == 51){
            cout << "AH //TO DO//\n";
        }
        else if (static_cast<int>(ipHeader.protocol) == 89){
            cout << "OSPF //TO DO//\n";
        }
        else if (static_cast<int>(ipHeader.protocol) == 132){
            cout << "SCTP //TO DO//\n";
        }

        //checksum
        cout << "CHECKSUM: 0x" << hex << ipHeader.headerChecksum << "\n";

        //source IP
        uint8_t octet1 = (ipHeader.sourceIP >> 24) & 0xFF;
        uint8_t octet2 = (ipHeader.sourceIP >> 16) & 0xFF;
        uint8_t octet3 = (ipHeader.sourceIP >> 8) & 0xFF;
        uint8_t octet4 = ipHeader.sourceIP & 0xFF;

        cout << "SOURCE IP: " << (int)octet1 << "." << (int)octet2 << "." << (int)octet3 << "." << (int)octet4 << "\n";

        //destination IP
        octet1 = (ipHeader.destinationIP >> 24) & 0xFF;
        octet2 = (ipHeader.destinationIP >> 16) & 0xFF;
        octet3 = (ipHeader.destinationIP >> 8) & 0xFF;
        octet4 = ipHeader.destinationIP & 0xFF;

        cout << "DESTINATION IP: " << (int)octet1 << "." << (int)octet2 << "." << (int)octet3 << "." << (int)octet4 << "\n";

        cout << dec;
        //data
        cout << "DATA: ";
        cout << packetData.size() << "\n";
//        for (size_t i = sizeof(EthernetHeader) + static_cast<unsigned int>(ihl); i < packetData.size(); ++i) {
//            cout << i << "\n";
//
////            if ((i + 1) % 16 == 0)
////                cout << "\n";
//        }

    }
    else if (ethernetHeader.etherType == 34525){
        cout << "IPv6.....work in progress......\n";
    }
    else if (ethernetHeader.etherType == 2054){
        cout << "ITS A TRAP!!! Got it? TRAP from ARP. No? Oke fine its ARP and also......work in progress....\n";
    }
    else{
        cout << "I apologise but we're not network scientist we only know IPv4, IPv6, and ARP. Most likely this packet is corrupted so ye. Please refer to WireShark if you want a more professional tool :)\n";
    }

}
