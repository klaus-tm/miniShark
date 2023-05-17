#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <winsock.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <json/json.h>
#include <windows.h>
#include <filesystem>

using namespace std;

uint32_t value = 0x12345678;
bool littleEndian = false;
int nr = 0;
struct PcapGlobalHeader {
    uint32_t magicNumber;   // magic number
    uint16_t versionMajor;  // major version number
    uint16_t versionMinor;  // minor version number
    int32_t  thiszone;      // GMT to local correction
    uint32_t sigfigs;       // accuracy of timestamps
    uint32_t snaplen;       // max length of captured packets, in octets
    uint32_t network;       // data link type
};

struct PcapPacketHeader {
    uint32_t tsSec;    // timestamp seconds
    uint32_t tsUsec;   // timestamp microseconds/nanoseconds
    uint32_t inclLen;  // number of octets of packet saved in file
    uint32_t origLen;  // actual length of packet
};

struct PcapPacket {
    PcapPacketHeader pcapPacketHeader;  // packet header
    vector<uint8_t> data;               // actual data of the packet
};

struct EthernetHeader {
    uint8_t destinationMAC[6];  // Destination MAC address
    uint8_t sourceMAC[6];       // Source MAC address
    uint16_t etherType;         // Ethernet type field
};

struct IPv4Header {
    uint8_t versionIHL;            // Version and Internet Header Length
    uint8_t typeOfService;         // Type of Service
    uint16_t totalLength;          // Total length of the IP packet
    uint16_t identification;       // Identification field
    uint16_t flagsFragmentOffset;  // Flags and Fragment Offset
    uint8_t timeToLive;            // Time to Live
    uint8_t protocol;              // Protocol type
    uint16_t headerChecksum;       // Header checksum
    uint32_t sourceIP;             // Source IP address
    uint32_t destinationIP;        // Destination IP address
};

struct ARPHeader {
    uint16_t hardwareType;     // Hardware type (e.g., Ethernet = 1)
    uint16_t protocolType;     // Protocol type (e.g., IPv4 = 0x0800)
    uint8_t hardwareLength;    // Length of hardware address (MAC address)
    uint8_t protocolLength;    // Length of protocol address (IP address)
    uint16_t operation;        // ARP operation (request = 1, reply = 2)
    uint8_t senderMAC[6];      // Sender's MAC address
    uint8_t senderIP[8];       // Sender's IP address
    uint8_t targetMAC[6];      // Target's MAC address
    uint8_t targetIP[8];       // Target's IP address
};

struct TCPHeader {
    uint16_t sourcePort;      // source port number
    uint16_t destPort;        // destination port number
    uint32_t sequenceNumber;  // sequence number
    uint32_t ackNumber;       // acknowledgement number
    uint8_t dataOffset;       // data offset and reserved bits
    uint8_t flags;            // TCP flags
    uint16_t windowSize;      // window size
    uint16_t checksum;        // checksum
    uint16_t urgentPointer;   // urgent pointer
};

struct ICMPHeader {
    uint8_t type;             // ICMP message type
    uint8_t code;             // ICMP message code
    uint16_t checksum;        // ICMP checksum
    uint16_t identifier;      // ICMP identifier (set to 0 if the type is error)
    uint16_t sequenceNumber;  // ICMP sequence number (set to 0 if the type is error)
};

struct UDPHeader {
    uint16_t sourcePort;  // source port number
    uint16_t destPort;    // destination port number
    uint16_t length;      // length of the UDP header and data
    uint16_t checksum;    // checksum
};

void checkEndianess(uint32_t magicNumber);

void printFileHeader(PcapGlobalHeader fileHeader);

void printPacket(vector<PcapPacket> packets);

void parseEthernet(vector<uint8_t> packetData, Json::Value root);

void parseIPv4(vector<uint8_t> packetData, Json::Value root);

void parseIPv4ICMP(vector<uint8_t> packetData, Json::Value root);

void parseIPv4TCP(vector<uint8_t> packetData, Json::Value root);

void parseIPv4UDP(vector<uint8_t> packetData, Json::Value root);

void parseARP(vector<uint8_t> packetData, Json::Value root);

int main(int argc, char* argv[]) {
    //initialisation of Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock.\n";
        exit(EXIT_FAILURE);
    }

    //delete all older jsons if they exist
    string path = __FILE__;
    path = path.substr(0, path.length() - 12);
    path = path + "output";
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        // Check if the file is a regular file and has a .json extension
        if (file.is_regular_file() && file.path().extension() == ".json") {
            // Delete the file
            std::filesystem::remove(file);
        }
    }

    //check if file path was provided by the child process call in js. NOT YET IMPLEMENTED!!!
//    if (argc < 2)
//        cerr << "File path not provided!\n";
//    else
//        string filePath = argv[1]; // use it to parse file path from js. NOT YET IMPLEMENTED!!!

    // Open the PCAP file. Test files: fuzz-2006-07-09-6023, yes, icmp, udp, arp
    ifstream file("C:\\Users\\cerce\\Desktop\\arp.pcap"/*change string with filePath*/, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening pcap file\n";
        exit(EXIT_FAILURE);
    }

    // Read the PCAP file header
    PcapGlobalHeader fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    //check for the endianess of the file
    checkEndianess(fileHeader.magicNumber);

    //print the rest of the pcap header. NOT NECESARRY FOR FINAL VERSION JUST FOR INTERNAL TESTING!!!
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

    cout << to_string(nr) << " done\n";
    WSACleanup();
    exit(EXIT_SUCCESS);
}

void checkEndianess(uint32_t magicNumber) {
    stringstream ss;
    ss << hex << magicNumber;

    //cout file endianess and note littleEndian value. NOT NECESARRY FOR FINAL VERSION JUST FOR INTERNAL TESTING!!! ONLY LITTLEENDIAN VALUE IS NECESARRY!!!
    if (ss.str() == "a1b2c3d4"){
        cout << "ENDIANESS: BIG ENDIAN\n";
        littleEndian = false;
    }
    else {
        cout << "ENDIANESS: LITTLE ENDIAN\n";
        littleEndian = true;
    }
}

void printFileHeader(PcapGlobalHeader fileHeader) {
    //NOT NECESARRY FOR FINAL VERSION JUST FOR INTERNAL TESTING!!!
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
        Json::Value root;
        root["Packet Number"] = to_string(i+1);
        nr++ ;

        if (littleEndian == true){
            packet.pcapPacketHeader.tsSec = ntohl(packet.pcapPacketHeader.tsSec);
            packet.pcapPacketHeader.tsUsec = ntohl(packet.pcapPacketHeader.tsUsec);
            packet.pcapPacketHeader.inclLen = ntohl(packet.pcapPacketHeader.inclLen);
            packet.pcapPacketHeader.origLen = ntohl(packet.pcapPacketHeader.origLen);
        }

        root["Packet Timestamp"] = to_string(packet.pcapPacketHeader.tsSec) + "." + to_string(packet.pcapPacketHeader.tsUsec);
        root["Packet Captured Length"] = to_string(packet.pcapPacketHeader.inclLen) + " bytes";
        root["Packet Original Length"] = to_string(packet.pcapPacketHeader.origLen) + " bytes";

        parseEthernet(packet.data, root);
        //cout << "\n\n";
    }
}

void parseEthernet(vector<uint8_t> packetData, Json::Value root) {
    if (packetData.size() < sizeof(EthernetHeader)){
        cerr << "Incomplete Ethernet header!\n";
        exit(EXIT_FAILURE);
    }

    //extract ETHERNET header from packet data vector + delete it for easy extractions for future headers
    EthernetHeader ethernetHeader;
    memcpy(&ethernetHeader, packetData.data(), sizeof(EthernetHeader));
    packetData.erase(packetData.begin(), packetData.begin() + sizeof(EthernetHeader));

    //ETHERNET macAdress
    stringstream macAdress;
    macAdress << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        macAdress << setw(2) << static_cast<int>(ethernetHeader.destinationMAC[i]);
        if (i < 5)
            macAdress << ":";
    }
    macAdress << dec;
    root["Ethernet Destination Mac Adress"] = macAdress.str();
    macAdress.str("");

    //ETHERNET sourceMac
    macAdress << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        macAdress << setw(2) << static_cast<int>(ethernetHeader.sourceMAC[i]);
        if (i < 5)
            macAdress << ":";
    }
    macAdress << dec;
    root["Ethernet Source Mac Adress"] = macAdress.str();

    //ether type
    ethernetHeader.etherType = ntohs(ethernetHeader.etherType);
    if (ethernetHeader.etherType == 2048){
        root["Ethernet Ether Type"] = "IPv4";

        //parsing IPv4 from the data vector
        parseIPv4(packetData, root);
    }
    else if (ethernetHeader.etherType == 34525){
        root["Ethernet Ether Type"] = "IPv6.....work in progress......";

        //parsing IPv6 from the data vector
        //parseIPv6(packetData, root);
    }
    else if (ethernetHeader.etherType == 2054){
        root["Ethernet Ether Type"] = "ITS A TRAP!!! Got it? TRAP from ARP. No? Oke fine its ARP and also......work in progress....";

        //parsing ARP from data vector
        parseARP(packetData, root);
    }
    else{
        root["Ethernet Ether Type"] = "I apologise but we're not network scientist we only know IPv4, IPv6, and ARP. Most likely this packet is corrupted so ye. Please refer to WireShark if you want a more professional tool :)";
    }

}

void parseIPv4(vector<uint8_t> packetData, Json::Value root) {
    if (packetData.size() < sizeof(IPv4Header)){
        cerr << "Incomplete IPv4 header!\n";
        exit(EXIT_FAILURE);
    }

    //extract IPv4 header from packet data vector + delete it for easy extractions for future headers using ihl(SEE BELOW!!)
    IPv4Header ipHeader;
    memcpy(&ipHeader, packetData.data(), sizeof(IPv4Header));

    //IPv4 version and internet header length (ihl)
    uint8_t version = (ipHeader.versionIHL >> 4) & 0x0F;
    root["IP Version"] = to_string(static_cast<unsigned int>(version));
    uint8_t ihl = (ipHeader.versionIHL & 0x0F) * 4;
    root["IP Internet Header Length(IHL)"] = to_string(static_cast<unsigned int>(ihl));

    //deleting IPv4 header from data vector using ihl
    packetData.erase(packetData.begin(), packetData.begin() + static_cast<unsigned int>(ihl));

    //IPv4 type of service and Differentiated Service Code Point(DSCP)
    uint8_t dscp = (ipHeader.typeOfService >> 2) & 0x3F;
    if (static_cast<unsigned int>(dscp) == 0)
        root["IP Type of Service"] = "Default";
    else if (static_cast<unsigned int>(dscp) == 46)
        root["IP Type of Service"] = "Expedited Forwarding";
    else if (dscp == 0x08 or dscp == 0x0C or dscp == 0x10 or dscp == 0x10 or dscp == 0x1C or dscp == 0x20 or dscp == 0x28 or dscp == 0x2C or dscp == 0x30 or dscp == 0x38 or dscp == 0x3C or dscp == 0x40)
        root["IP Type of Service"] = "Assured Forwarding";
    else root["IP Type of Service"] = "Unknown";

    //IPv4 total length
    ipHeader.totalLength = ntohs(ipHeader.totalLength);
    root["IP Total Length"] = to_string(ipHeader.totalLength) + " bytes";

    //IPv4 identifier field
    ipHeader.identification = ntohs(ipHeader.identification);
    root["IP Identification"] = to_string(ipHeader.identification);

    //IPv4 flags and fragment offset
    ipHeader.flagsFragmentOffset = ntohs(ipHeader.flagsFragmentOffset);
    uint16_t flag = (ipHeader.flagsFragmentOffset & 0xE000) >> 13;
    uint16_t fragmentOffset = ipHeader.flagsFragmentOffset & 0x1FFF;
    bool isDFSet = (flag & 0x2) != 0;
    bool isMFSet = (flag & 0x4) != 0;

    if (isDFSet)
        root["IP Flag"] = "Don't Fragment (DF)";
    else if (isMFSet)
        root["IP Flag"] = "More Fragments (MF)";
    else if (flag == 0)
        root["IP Flag"] = "Reserved";
    else root["IP Flag"] = "Unknown";
    root["IP Fragment Offset"] = to_string(fragmentOffset);

    //IPv4 time to live
    root["IP Time To Live"] = to_string(static_cast<int>(ipHeader.timeToLive));

    //IPv4 protocol
    if(static_cast<int>(ipHeader.protocol) == 1)
        root["IP Protocol"] = "ICMP";
    else if (static_cast<int>(ipHeader.protocol) == 6)
        root["IP Protocol"] = "TCP";
    else if (static_cast<int>(ipHeader.protocol) == 17)
        root["IP Protocol"] = "UDP";
    else if (static_cast<int>(ipHeader.protocol) == 41)//TO DISCUSS!!!
        root["IP Protocol"] = "IPv6 //WILL NOT BE IMPLEMENTED//";
    else if (static_cast<int>(ipHeader.protocol) == 50)
        root["IP Protocol"] = "ESP //WILL NOT BE IMPLEMENTED//";
    else if (static_cast<int>(ipHeader.protocol) == 51)
        root["IP Protocol"] = "AH //WILL NOT BE IMPLEMENTED//";
    else if (static_cast<int>(ipHeader.protocol) == 89)
        root["IP Protocol"] = "OSPF //WILL NOT BE IMPLEMENTED//";
    else if (static_cast<int>(ipHeader.protocol) == 132)
        root["IP Protocol"] = "SCTP //WILL NOT BE IMPLEMENTED//";

    //IPv4 checksum
    stringstream checksum;
    checksum << "0x" << hex << ipHeader.headerChecksum;
    root["IP Checksum"] = checksum.str();

    //source IP
    stringstream IP;
    ipHeader.sourceIP = ntohl(ipHeader.sourceIP);
    uint8_t octet1 = (ipHeader.sourceIP >> 24) & 0xFF;
    uint8_t octet2 = (ipHeader.sourceIP >> 16) & 0xFF;
    uint8_t octet3 = (ipHeader.sourceIP >> 8) & 0xFF;
    uint8_t octet4 = ipHeader.sourceIP & 0xFF;

    IP << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
       << static_cast<int>(octet3) << "." << static_cast<int>(octet4);
    root["IP Source IP"] = IP.str();
    IP.str("");//cleaning the IPv4 string stream to fill it with destination IP

    //destination IP
    ipHeader.destinationIP = ntohl(ipHeader.destinationIP);
    octet1 = (ipHeader.destinationIP >> 24) & 0xFF;
    octet2 = (ipHeader.destinationIP >> 16) & 0xFF;
    octet3 = (ipHeader.destinationIP >> 8) & 0xFF;
    octet4 = ipHeader.destinationIP & 0xFF;

    IP << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
       << static_cast<int>(octet3) << "." << static_cast<int>(octet4);
    root["IP Destination IP"] = IP.str();
    cout << dec;

    if(static_cast<int>(ipHeader.protocol) == 1)
        //ICMP
        parseIPv4ICMP(packetData, root);
    else if (static_cast<int>(ipHeader.protocol) == 6)
        //TCP
        parseIPv4TCP(packetData, root);
    else if (static_cast<int>(ipHeader.protocol) == 17)
        //UDP
        parseIPv4UDP(packetData, root);
}

void parseIPv4ICMP(vector<uint8_t> packetData, Json::Value root) {
    if (packetData.size() < sizeof(ICMPHeader)){
        cerr << "Incomplete ICMP header!\n";
        exit(EXIT_FAILURE);
    }

    //extract ICMP header from packet data vector + delete it for easy extractions for future headers
    ICMPHeader icmpHeader;
    memcpy(&icmpHeader, packetData.data(), sizeof(ICMPHeader));
    packetData.erase(packetData.begin(), packetData.begin() + sizeof(ICMPHeader));

    //ICMP message type + code. when type = "echo _" ICMP code is 0 or "same as type"
    if (icmpHeader.type == 0){
        root["ICMP Type"] = "Echo Reply";
        root["ICMP Code"] = "Same as type (0)";

        //because there are no errors we need identifier and sequence number.
        root["ICMP Identifier"] = to_string(icmpHeader.identifier);
        root["ICMP Sequence Number"] = to_string(icmpHeader.sequenceNumber);
    }
    else if (icmpHeader.type == 3){
        root["ICMP Type"] = "Destination Unreachable";

        //delete the IP + protocol headers encapsulated within the ICMP error message
        IPv4Header ipHeader;
        memcpy(&ipHeader, packetData.data(), sizeof(IPv4Header));
        uint8_t ihl = (ipHeader.versionIHL & 0x0F) * 4;
        packetData.erase(packetData.begin(), packetData.begin() + static_cast<unsigned int>(ihl));

        if (static_cast<int>(ipHeader.protocol) == 6){
            //TCP
            TCPHeader tcpHeader;
            memcpy(&tcpHeader, packetData.data(), sizeof(TCPHeader));
            packetData.erase(packetData.begin(), packetData.begin() + static_cast<int>(tcpHeader.dataOffset >> 4) * 4);
        }
        else if (static_cast<int>(ipHeader.protocol) == 17)
            //UDP
            packetData.erase(packetData.begin(), packetData.begin() + sizeof(UDPHeader));

        //now interpret ICMP code
        if (icmpHeader.code == 0)
            root["ICMP Code"] = "Network unreachable (0)";
        else if (icmpHeader.code == 1)
            root["ICMP Code"] = "Host unreachable (1)";
        else if (icmpHeader.code == 2)
            root["ICMP Code"] = "Protocol unreachable (2)";
        else if (icmpHeader.code == 3)
            root["ICMP Code"] = "Port unreachable (3)";
        else if (icmpHeader.code == 4)
            root["ICMP Code"] = "Fragmentation needed and DF (Don't Fragment) flag set (4)";
        else if (icmpHeader.code == 5)
            root["ICMP Code"] = "Source route failed (5)";
    }
    else if (icmpHeader.type == 8){
        root["ICMP Type"] = "Echo Request";
        root["ICMP Code"] = "Same as type (0)";

        //because there are no errors we need identifier and sequence number.
        root["ICMP Identifier"] = to_string(icmpHeader.identifier);
        root["ICMP Sequence Number"] = to_string(icmpHeader.sequenceNumber);
    }
    else if (icmpHeader.type == 11){
        root["ICMP Type"] = "Time Exceeded";

        //delete the IP + protocol headers encapsulated within the ICMP error message
        IPv4Header ipHeader;
        memcpy(&ipHeader, packetData.data(), sizeof(IPv4Header));
        uint8_t ihl = (ipHeader.versionIHL & 0x0F) * 4;
        packetData.erase(packetData.begin(), packetData.begin() + static_cast<unsigned int>(ihl));

        if (static_cast<int>(ipHeader.protocol) == 6){
            //TCP
            TCPHeader tcpHeader;
            memcpy(&tcpHeader, packetData.data(), sizeof(TCPHeader));
            packetData.erase(packetData.begin(), packetData.begin() + static_cast<int>(tcpHeader.dataOffset >> 4) * 4);
        }
        else if (static_cast<int>(ipHeader.protocol) == 17)
            //UDP
            packetData.erase(packetData.begin(), packetData.begin() + sizeof(UDPHeader));

        //now interpret ICMP code
        if (icmpHeader.code == 0)
            root["ICMP Code"] = "TTL expired in transit (0)";
        else root["ICMP Code"] = "Fragment reassembly time exceeded (1)";
    }
    else root["ICMP Type"] = "Unknown";

    //ICMP checksum
    stringstream checksum;
    checksum << hex << ntohs(icmpHeader.checksum);
    root["ICMP Checksum"] = checksum.str();

    //now collect the packet data and put it into json
    stringstream data;
    for (int i = 0; i < packetData.size(); ++i) {
        data << packetData[i];
    }
    root["Packet Data"] = data.str();

    //create json file
    Json::StreamWriterBuilder writerBuilder;
    string jsonString = Json::writeString(writerBuilder, root);
    ofstream outputPacket("../../backend/output/packet" + root["Packet Number"].asString() + ".json");
    if (outputPacket.is_open())
        outputPacket << jsonString;
    else {
        cerr << "Unable to create Json!\n";
        exit(EXIT_FAILURE);
    }
}

void parseIPv4TCP(vector<uint8_t> packetData, Json::Value root) {
    if (packetData.size() < sizeof(TCPHeader)) {
        cerr << "Incomplete TCP header!\n";
        exit(EXIT_FAILURE);
    }

    //extract TCP header from packet data vector + delete it for easy extractions for future headers
    TCPHeader tcpHeader;
    memcpy(&tcpHeader, packetData.data(), sizeof(TCPHeader));
    packetData.erase(packetData.begin(), packetData.begin() + static_cast<int>(tcpHeader.dataOffset >> 4) * 4);

    //byte swapping of the TCP fields: Ports, Sequence + Acknowledge Numbers, Window Size and puting them into json
    root["TCP Source Port"] = to_string(ntohs(tcpHeader.sourcePort));
    root["TCP Destination Port"] = to_string(ntohs(tcpHeader.destPort));
    root["TCP Sequence Number"] = to_string(ntohl(tcpHeader.sequenceNumber));
    root["TCP Acknowledgement Number"] = to_string(ntohl(tcpHeader.ackNumber));
    root["TCP Window Size"] = to_string(ntohs(tcpHeader.windowSize));

    //calculate header length using data offset and put it into json
    root["TCP Header Length"] = to_string(static_cast<int>(tcpHeader.dataOffset >> 4) * 4) + " (" + to_string(static_cast<int>(tcpHeader.dataOffset >> 4)) + ")";

    //TCP flags
    if ((tcpHeader.flags & 0x02) >> 1)
        root["TCP Flags"] = "SYN (Synchronize)";
    else if ((tcpHeader.flags & 0x04) >> 2)
        root["TCP Flags"] = "RST (Reset)";
    else if ((tcpHeader.flags & 0x08) >> 3)
        root["TCP Flags"] = "PSH (Push)";
    else if ((tcpHeader.flags & 0x10) >> 4)
        root["TCP Flags"] = "ACK (Acknowledgment)";
    else if ((tcpHeader.flags & 0x20) >> 5)
        root["TCP Flags"] = "URG (Urgent)";
    else root["TCP Flags"] = "Reserved ";

    //TCP checksum
    stringstream checksum;
    checksum << "0x" << hex << ntohs(tcpHeader.checksum) << dec;
    root["TCP Checksum"] = checksum.str();

    //TCP urgent pointer
    root["TCP Urgent Pointer"] = to_string(ntohs(tcpHeader.urgentPointer));

    //now collect the packet data and put it into json
    stringstream data;
    for (int i = 0; i < packetData.size(); ++i) {
        data << packetData[i];
    }
    root["Packet Data"] = data.str();

    //create json file
    Json::StreamWriterBuilder writerBuilder;
    string jsonString = Json::writeString(writerBuilder, root);
    ofstream outputPacket("../../backend/output/packet" + root["Packet Number"].asString() + ".json");
    if (outputPacket.is_open())
        outputPacket << jsonString;
    else {
        cerr << "Unable to create Json!\n";
        exit(EXIT_FAILURE);
    }
}

void parseIPv4UDP(vector<uint8_t> packetData, Json::Value root) {
    if (packetData.size() < sizeof(UDPHeader)){
        cerr << "Incomplete UDP header!\n";
        exit(EXIT_FAILURE);
    }

    //extract UDP header from packet data vector + delete it for easy extractions for future headers
    UDPHeader udpHeader;
    memcpy(&udpHeader, packetData.data(), sizeof(UDPHeader));
    packetData.erase(packetData.begin(), packetData.begin() + sizeof(UDPHeader));

    //byte swapping of the UDP fields: Ports, Length and putting them into json
    root["UDP Source Port"] = to_string(ntohs(udpHeader.sourcePort));
    root["UDP Destination Port"] = to_string(ntohs(udpHeader.destPort));
    root["UDP Length"] = to_string(ntohs(udpHeader.length));

    //UDP checksum
    stringstream checksum;
    checksum << "0x" << hex << ntohs(udpHeader.checksum) << dec;
    root["UDP Checksum"] = checksum.str();

    //now collect the packet data and put it into json
    stringstream data;
    for (int i = 0; i < packetData.size(); ++i) {
        data << packetData[i];
    }
    root["Packet Data"] = data.str();

    //create json file
    Json::StreamWriterBuilder writerBuilder;
    string jsonString = Json::writeString(writerBuilder, root);
    ofstream outputPacket("../../backend/output/packet" + root["Packet Number"].asString() + ".json");
    if (outputPacket.is_open())
        outputPacket << jsonString;
    else {
        cerr << "Unable to create Json!\n";
        exit(EXIT_FAILURE);
    }
}

void parseARP(vector<uint8_t> packetData, Json::Value root) {
    if (packetData.size() < sizeof(ARPHeader)){
        cerr << "Incomplete ARP header!\n";
        exit(EXIT_FAILURE);
    }

    //extract ARP header from packet data vector + delete it for easy extractions for future headers
    ARPHeader arpHeader;
    memcpy(&arpHeader, packetData.data(), sizeof(ARPHeader));
    packetData.erase(packetData.begin(), packetData.begin() + sizeof(ARPHeader));

    //ARP hardware type
    if (ntohs(arpHeader.hardwareType) == 1)
        root["ARP Hardware Type"] = "Ethernet";
    else if (ntohs(arpHeader.hardwareType) == 6)
        root["ARP Hardware Type"] = "IEEE 802 networks";
    else if (ntohs(arpHeader.hardwareType) == 15)
        root["ARP Hardware Type"] = "Frame Relay";
    else if (ntohs(arpHeader.hardwareType) == 16)
        root["ARP Hardware Type"] = "Asynchronous Transfer Mode (ATM)";
    else if (ntohs(arpHeader.hardwareType) == 17)
        root["ARP Hardware Type"] = "HDLC";
    else if (ntohs(arpHeader.hardwareType) == 18)
        root["ARP Hardware Type"] = "Fibre Channel";
    else root["ARP Hardware Type"] = "Unknown";

    //ARP protocol type
    if (arpHeader.protocolType == 0x0008)
        root["ARP Protocol Type"] = "IPv4";
    else if (arpHeader.protocolType == 0xDD86)
        root["ARP Protocol Type"] = "IPv6";
    else if (arpHeader.protocolType == 0x0608)
        root["ARP Protocol Type"] = "ARP";
    else root["ARP Protocol Type"] = "Unknown";

    //ARP hardware length
    root["ARP Hardware Length"] = to_string(static_cast<int>(arpHeader.hardwareLength));

    //ARP protocol length
    root["ARP Protocol Length"] = to_string(static_cast<int>(arpHeader.protocolLength));

    //ARP operation type
    if (ntohs(arpHeader.operation) == 1)
        root["ARP Operation"] = "ARP Request";
    else if (ntohs(arpHeader.operation) == 2)
        root["ARP Operation"] = "ARP Reply";
    else if (ntohs(arpHeader.operation) == 3)
        root["ARP Operation"] = "RARP Request";
    else if (ntohs(arpHeader.operation) == 4)
        root["ARP Operation"] = "RARP Reply";
    else root["ARP Operation"] = "Unknown";

    //ARP senderMac
    stringstream adress;
    adress << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        adress << setw(2) << static_cast<int>(arpHeader.senderMAC[i]);
        if (i < 5)
            adress << ":";
    }
    adress << dec;
    root["ARP Sender Mac Adress"] = adress.str();
    adress.str("");

    //ARP senderIP
    adress << (int)arpHeader.senderIP[0] << "." << (int)arpHeader.senderIP[1] << "."
         << (int)arpHeader.senderIP[2] << "." << (int)arpHeader.senderIP[3];
    root["ARP Sender IP Adress"] = adress.str();
    adress.str("");

    //ARP targetMac
    adress << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        adress << setw(2) << static_cast<int>(arpHeader.targetMAC[i]);
        if (i < 5)
            adress << ":";
    }
    adress << dec;
    root["ARP Target Mac Adress"] = adress.str();
    adress.str("");

    //ARP targetIP
    adress << (int)arpHeader.targetIP[0] << "." << (int)arpHeader.targetIP[1] << "."
         << (int)arpHeader.targetIP[2] << "." << (int)arpHeader.targetIP[3];
    root["ARP Target IP Adress"] = adress.str();

    //create json file
    Json::StreamWriterBuilder writerBuilder;
    string jsonString = Json::writeString(writerBuilder, root);
    ofstream outputPacket("../../backend/output/packet" + root["Packet Number"].asString() + ".json");
    if (outputPacket.is_open())
        outputPacket << jsonString;
    else {
        cerr << "Unable to create Json!\n";
        exit(EXIT_FAILURE);
    }
}
