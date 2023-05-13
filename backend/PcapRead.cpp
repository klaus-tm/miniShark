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
bool littleEndian = false;
struct PcapGlobalHeader {
    uint32_t magicNumber;  /* magic number */
    uint16_t versionMajor; /* major version number */
    uint16_t versionMinor; /* minor version number */
    int32_t  thiszone;      /* GMT to local correction */
    uint32_t sigfigs;       /* accuracy of timestamps */
    uint32_t snaplen;       /* max length of captured packets, in octets */
    uint32_t network;       /* data link type */
};

struct PcapPacketHeader {
    uint32_t tsSec;   // timestamp seconds
    uint32_t tsUsec;  // timestamp microseconds/nanoseconds
    uint32_t inclLen; // number of octets of packet saved in file
    uint32_t origLen; // actual length of packet
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

void checkEndianess(uint32_t magicNumber);

void printFileHeader(PcapGlobalHeader fileHeader);

void printPacket(vector<PcapPacket> packets);

void parseEthernet(vector<uint8_t> packetData);

void parseIPv4(IPHeader ipHeader, vector<uint8_t> packetData);

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
    //check if system is either big or little endian
    uint32_t networkValue = htonl(value);

    if (value == networkValue)
        cout << "System is using big-endian byte order.\n";
    else
        cout << "System is using little-endian byte order.\n";

    // Read the PCAP file header
    PcapGlobalHeader fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    //check for the endianess of the file
    checkEndianess(fileHeader.magicNumber);

    //print the rest of the pcap header
    printFileHeader(fileHeader);

    // Loop through the packets in the file
    vector<PcapPacket>packets;
    PcapPacketHeader packetHeader;
    while (file.read(reinterpret_cast<char*>(&packetHeader), sizeof(packetHeader))) {
        PcapPacket pcapPacket;
        pcapPacket.pcapPacketHeader = packetHeader;
        if (littleEndian == true){
            pcapPacket.data.resize(ntohl(packetHeader.inclLen));
            file.read(reinterpret_cast<char*>(pcapPacket.data.data()), ntohl(packetHeader.inclLen));
        }
        else{
            pcapPacket.data.resize(packetHeader.inclLen);
            file.read(reinterpret_cast<char*>(pcapPacket.data.data()), packetHeader.inclLen);
        }
        packets.push_back(pcapPacket);
    }
    // Close the PCAP file
    file.close();
    //print the packets captured
    printPacket(packets);

    WSACleanup();
    return 0;

}

void checkEndianess(uint32_t magicNumber) {
    stringstream ss;
    ss << hex << magicNumber;
    if (ss.str() == "a1b2c3d4")
        cout << "ENDIANESS: BIG ENDIAN\n";
    else{
        cout << "ENDIANESS: LITTLE ENDIAN\n";
        littleEndian = true;
    }
}

void printFileHeader(PcapGlobalHeader fileHeader) {
    if (littleEndian == true){
        fileHeader.versionMajor = ntohs(fileHeader.versionMajor);
        fileHeader.versionMinor = ntohs(fileHeader.versionMinor);
        fileHeader.thiszone = ntohl(fileHeader.thiszone);//
        fileHeader.sigfigs = ntohl(fileHeader.sigfigs);//
        fileHeader.snaplen = ntohl(fileHeader.snaplen);//
        fileHeader.network = ntohl(fileHeader.network);//
    }

    cout << "VERSION: " << fileHeader.versionMajor << "." << fileHeader.versionMinor << "\n";
    cout << "Thiszone(not important): " << fileHeader.thiszone << "\n";
    cout << "Sigfigs(not important): " << fileHeader.sigfigs << "\n";
    cout << "MAX BYTES ALLOWED IN PACKETS: " << fileHeader.snaplen << " bytes\n";
    if (fileHeader.network == 1)
        cout << "NETWORK: ETHERNET\n";
    else if (fileHeader.network == 105)
        cout << "NETWORK: IEEE 802.11 wireless LAN\n";
    else if (fileHeader.network == 9)
        cout << "NETWORK: PPP(Point-to-point-protocol)\n";
    else if (fileHeader.network == 104)
        cout << "NETWORK: HDLC(High-Level Data Link Control)\n";
    else cout << "NETWORK: FDDI(Fiber Distributed Data Interface)\n";
    cout << "\n\n";
}

void printPacket(vector<PcapPacket> packets) {
    for (int i = 0; i < packets.size(); ++i) {
        PcapPacket packet = packets[i];
        cout << "Packet " << i+1 << "\n";

        if (littleEndian == true){
            packet.pcapPacketHeader.tsSec = ntohl(packet.pcapPacketHeader.tsSec);
            packet.pcapPacketHeader.tsUsec = ntohl(packet.pcapPacketHeader.tsUsec);
            packet.pcapPacketHeader.inclLen = ntohl(packet.pcapPacketHeader.inclLen);
            packet.pcapPacketHeader.origLen = ntohl(packet.pcapPacketHeader.origLen);
        }

        cout << "TIMESTAMP: " << packet.pcapPacketHeader.tsSec << "." << packet.pcapPacketHeader.tsUsec << "\n";
        cout << "CAPTURED LENGTH: " << packet.pcapPacketHeader.inclLen << " bytes\n";
        cout << "ORIGINAL LENGTH: " << packet.pcapPacketHeader.origLen << " bytes\n";

        parseEthernet(packet.data);

        cout << "\n\n";
    }
}

void parseEthernet(vector<uint8_t> packetData) {
    EthernetHeader ethernetHeader;
    memcpy(&ethernetHeader, packetData.data(), sizeof(EthernetHeader));

    //byte order reversing for ethernet header
    //destinationMac printing
    cout << "DESTINATION MAC ADRESS: " << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        cout << setw(2) << static_cast<int>(ethernetHeader.destinationMAC[i]);
        if (i < 5)
            cout << ":";
    }
    cout << dec << "\n";

    //sourceMac printing
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

        parseIPv4(ipHeader, packetData);


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

void parseIPv4(IPHeader ipHeader, vector<uint8_t> packetData) {
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
    bool isDFSet = (flag & 0x2) != 0;
    bool isMFSet = (flag & 0x4) != 0;

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
    ipHeader.sourceIP = ntohl(ipHeader.sourceIP);
    uint8_t octet1 = (ipHeader.sourceIP >> 24) & 0xFF;
    uint8_t octet2 = (ipHeader.sourceIP >> 16) & 0xFF;
    uint8_t octet3 = (ipHeader.sourceIP >> 8) & 0xFF;
    uint8_t octet4 = ipHeader.sourceIP & 0xFF;

    cout << "SOURCE IP: "
         << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
         << static_cast<int>(octet3) << "." << static_cast<int>(octet4) << "\n";

    //destination IP
    ipHeader.destinationIP = ntohl(ipHeader.destinationIP);
    octet1 = (ipHeader.destinationIP >> 24) & 0xFF;
    octet2 = (ipHeader.destinationIP >> 16) & 0xFF;
    octet3 = (ipHeader.destinationIP >> 8) & 0xFF;
    octet4 = ipHeader.destinationIP & 0xFF;

    cout << "DESTINATION IP: "
         << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
         << static_cast<int>(octet3) << "." << static_cast<int>(octet4) << "\n";

    cout << dec;
    //data printing
    cout << "DATA: ";
        for (size_t i = sizeof(EthernetHeader) + static_cast<unsigned int>(ihl); i < packetData.size(); ++i) {
            cout << hex << packetData[i];

            if ((i + 1) % 16 == 0)
                cout << dec << "\n";
        }
}
