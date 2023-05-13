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

struct TCPHeader {
    uint16_t sourcePort;      // source port number
    uint16_t destPort;        // destination port number
    uint32_t sequenceNumber;  // sequence number
    uint32_t ackNumber;       // acknowledgement number
    uint8_t dataOffset;       // data offset and reserved bits
    uint8_t flags;             // TCP flags
    uint16_t windowSize;      // window size
    uint16_t checksum;         // checksum
    uint16_t urgentPointer;   // urgent pointer
};

void checkEndianess(uint32_t magicNumber);

void printFileHeader(PcapGlobalHeader fileHeader);

void printPacket(vector<PcapPacket> packets);

void parseEthernet(vector<uint8_t> packetData, Json::Value root);

void parseIPv4(vector<uint8_t> packetData, Json::Value root);

void parseIPv4TCP(vector<uint8_t> packetData);

void parseIPv4ICMP(vector<uint8_t> packetData);

void parseIPv4UDP(vector<uint8_t> packetData);

void parseIPv4IPv6(vector<uint8_t> packetData);

void parseIPv4ESP(vector<uint8_t> packetData);

void parseIPv4AH(vector<uint8_t> packetData);

void parseIPv4OSPF(vector<uint8_t> packetData);

void parseIPv4SCTP(vector<uint8_t> packetData);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }
    //string filePath = argv[1];
    // Open the PCAP file fuzz-2006-07-09-6023
    ifstream file("C:\\Users\\cerce\\Desktop\\yes.pcap", ios::binary);
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
        Json::Value root;
        root["Packet"] = to_string(i+1);
        //cout << "Packet " << i+1 << "\n";

        if (littleEndian == true){
            packet.pcapPacketHeader.tsSec = ntohl(packet.pcapPacketHeader.tsSec);
            packet.pcapPacketHeader.tsUsec = ntohl(packet.pcapPacketHeader.tsUsec);
            packet.pcapPacketHeader.inclLen = ntohl(packet.pcapPacketHeader.inclLen);
            packet.pcapPacketHeader.origLen = ntohl(packet.pcapPacketHeader.origLen);
        }

        root["Timestamp"] = to_string(packet.pcapPacketHeader.tsSec) + "." + to_string(packet.pcapPacketHeader.tsUsec);
        root["Captured Length"] = to_string(packet.pcapPacketHeader.inclLen) + " bytes";
        root["Original Length"] = to_string(packet.pcapPacketHeader.origLen) + " bytes";

//        cout << "TIMESTAMP: " << packet.pcapPacketHeader.tsSec << "." << packet.pcapPacketHeader.tsUsec << "\n";
//        cout << "CAPTURED LENGTH: " << packet.pcapPacketHeader.inclLen << " bytes\n";
//        cout << "ORIGINAL LENGTH: " << packet.pcapPacketHeader.origLen << " bytes\n";

        parseEthernet(packet.data, root);

        cout << "\n\n";
    }
}

void parseEthernet(vector<uint8_t> packetData, Json::Value root) {
    EthernetHeader ethernetHeader;
    memcpy(&ethernetHeader, packetData.data(), sizeof(EthernetHeader));
    packetData.erase(packetData.begin(), packetData.begin() + sizeof(EthernetHeader));

    //macAdress printing
    stringstream macAdress;
    macAdress << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        macAdress << setw(2) << static_cast<int>(ethernetHeader.destinationMAC[i]);
        if (i < 5)
            macAdress << ":";
    }
    macAdress << dec;
    root["Destination Mac Adress"] = macAdress.str();
    macAdress.str("");

//    cout << "DESTINATION MAC ADRESS: " << hex << setfill('0');
//    for (int i = 0; i < 6; ++i) {
//        cout << setw(2) << static_cast<int>(ethernetHeader.destinationMAC[i]);
//        if (i < 5)
//            cout << ":";
//    }
//    cout << dec << "\n";

    //sourceMac printing
    macAdress << hex << setfill('0');
    for (int i = 0; i < 6; ++i) {
        macAdress << setw(2) << static_cast<int>(ethernetHeader.sourceMAC[i]);
        if (i < 5)
            macAdress << ":";
    }
    macAdress << dec;
    root["Source Mac Adress"] = macAdress.str();

//    cout << "SOURCE MAC ADRESS: " << hex << setfill('0');
//    for (int i = 0; i < 6; ++i) {
//        cout << setw(2) << static_cast<int>(ethernetHeader.sourceMAC[i]);
//        if (i < 5)
//            cout << ":";
//    }
//    cout << dec << "\n";

    //cout << "TYPE: ";
    ethernetHeader.etherType = ntohs(ethernetHeader.etherType);
    if (ethernetHeader.etherType == 2048){
        root["Ether Type"] = "IPv4";
        //cout << "IPv4\n";

        //parsing IPv4 from the data vector
        parseIPv4(packetData, root);
    }
    else if (ethernetHeader.etherType == 34525){
        root["Ether Type"] = "IPv6.....work in progress......";
        //cout << "IPv6.....work in progress......\n";
    }
    else if (ethernetHeader.etherType == 2054){
        root["Ether Type"] = "ITS A TRAP!!! Got it? TRAP from ARP. No? Oke fine its ARP and also......work in progress....";
        //cout << "ITS A TRAP!!! Got it? TRAP from ARP. No? Oke fine its ARP and also......work in progress....\n";
    }
    else{
        root["Ether Type"] = "I apologise but we're not network scientist we only know IPv4, IPv6, and ARP. Most likely this packet is corrupted so ye. Please refer to WireShark if you want a more professional tool :)";
        //cout << "I apologise but we're not network scientist we only know IPv4, IPv6, and ARP. Most likely this packet is corrupted so ye. Please refer to WireShark if you want a more professional tool :)\n";
    }

}

void parseIPv4(vector<uint8_t> packetData, Json::Value root) {
    //ipheader collecting
    IPHeader ipHeader;
    memcpy(&ipHeader, packetData.data(), sizeof(IPHeader));

    //version and internet header length (ihl)
    uint8_t version = (ipHeader.versionIHL >> 4) & 0x0F;
    root["IP Version"] = to_string(static_cast<unsigned int>(version));
    //cout << "VERSION: " << static_cast<unsigned int>(version) << "\n";

    uint8_t ihl = (ipHeader.versionIHL & 0x0F) * 4;
    root["Internet Header Length(IHL)"] = to_string(static_cast<unsigned int>(ihl));
    //cout << "INTERNET HEADER LENGTH(IHL): " << static_cast<unsigned int>(ihl) <<"\n";
    packetData.erase(packetData.begin(), packetData.begin() + static_cast<unsigned int>(ihl));

    //type of service and Differentiated Service Code Point(DSCP)
    //cout << "TYPE OF SERVICE: ";
    uint8_t dscp = (ipHeader.typeOfService >> 2) & 0x3F;
    if (static_cast<unsigned int>(dscp) == 0)
        root["Type of Service"] = "Default";
        //cout << "Default\n";
    else if (static_cast<unsigned int>(dscp) == 46)
        root["Type of Service"] = "Expedited Forwarding";
        //cout << "Expedited Forwarding\n";
    else if (dscp == 0x08 or dscp == 0x0C or dscp == 0x10 or dscp == 0x10 or dscp == 0x1C or dscp == 0x20 or dscp == 0x28 or dscp == 0x2C or dscp == 0x30 or dscp == 0x38 or dscp == 0x3C or dscp == 0x40)
        root["Type of Service"] = "Assured Forwarding";
        //cout << "Assured Forwarding\n";
    else root["Type of Service"] = "Unknown"; //cout << "Unknown\n";

    //total length
    ipHeader.totalLength = ntohs(ipHeader.totalLength);
    root["Total Length"] = to_string(ipHeader.totalLength) + " bytes";
    //cout << "TOTAL LENGTH: " << ipHeader.totalLength << " bytes\n";

    //identification field
    ipHeader.identification = ntohs(ipHeader.identification);
    root["Identification"] = to_string(ipHeader.identification);
    //cout << "IDENTIFICATION: " << ipHeader.identification << "\n";

    //flags and fragment offset
    ipHeader.flagsFragmentOffset = ntohs(ipHeader.flagsFragmentOffset);
    uint16_t flag = (ipHeader.flagsFragmentOffset & 0xE000) >> 13;
    uint16_t fragmentOffset = ipHeader.flagsFragmentOffset & 0x1FFF;
    bool isDFSet = (flag & 0x2) != 0;
    bool isMFSet = (flag & 0x4) != 0;

    //cout << "FLAG: ";
    if (isDFSet)
        root["IP Flag"] = "Don't Fragment (DF)";
        //cout << "Don't Fragment (DF)\n";
    else if (isMFSet)
        root["IP Flag"] = "More Fragments (MF)";
        //cout << "More Fragments (MF)\n";
    else if (flag == 0)
        root["IP Flag"] = "Reserved";
        //cout << "Reserved\n";
    else root["IP Flag"] = "Unknown"; //cout << "Unknown\n";
    root["Fragment Offset"] = to_string(fragmentOffset);
    //cout << "FRAGMENT OFFSET: " << fragmentOffset << "\n";

    //time to live
    root["Time To Live"] = to_string(static_cast<int>(ipHeader.timeToLive));
    //cout << "TIME TO LIVE: " << static_cast<int>(ipHeader.timeToLive) << "\n";

    //protocol
    //cout << "PROTOCOL: ";
    if(static_cast<int>(ipHeader.protocol) == 1)
        root["IP Protocol"] = "ICMP //TO DO//";
        //cout << "ICMP //TO DO//\n";
    else if (static_cast<int>(ipHeader.protocol) == 6)
        root["IP Protocol"] = "TCP //TO DO//";
        //cout << "TCP //TO DO//\n";
    else if (static_cast<int>(ipHeader.protocol) == 17)
        root["IP Protocol"] = "UDP //TO DO//";
        //cout << "UDP //TO DO//\n";
    else if (static_cast<int>(ipHeader.protocol) == 41)//TO DISCUSS!!!
        root["IP Protocol"] = "IPv6 //TO DO//";
        //cout << "IPv6?? //TO DO//\n";
    else if (static_cast<int>(ipHeader.protocol) == 50)
        root["IP Protocol"] = "ESP //TO DO//";
        //cout << "ESP //TO DO//\n";
    else if (static_cast<int>(ipHeader.protocol) == 51)
        root["IP Protocol"] = "AH //TO DO//";
        //cout << "AH //TO DO//\n";
    else if (static_cast<int>(ipHeader.protocol) == 89)
        root["IP Protocol"] = "OSPF //TO DO//";
        //cout << "OSPF //TO DO//\n";
    else if (static_cast<int>(ipHeader.protocol) == 132)
        root["IP Protocol"] = "SCTP //TO DO//";
        //cout << "SCTP //TO DO//\n";

    //checksum
    stringstream checksum;
    checksum << "0x" << hex << ipHeader.headerChecksum;
    root["IP Checksum"] = checksum.str();
    //cout << "CHECKSUM: 0x" << hex << ipHeader.headerChecksum << "\n";

    //source IP
    stringstream IP;
    ipHeader.sourceIP = ntohl(ipHeader.sourceIP);
    uint8_t octet1 = (ipHeader.sourceIP >> 24) & 0xFF;
    uint8_t octet2 = (ipHeader.sourceIP >> 16) & 0xFF;
    uint8_t octet3 = (ipHeader.sourceIP >> 8) & 0xFF;
    uint8_t octet4 = ipHeader.sourceIP & 0xFF;

    IP << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
              << static_cast<int>(octet3) << "." << static_cast<int>(octet4);
    root["Source IP"] = IP.str();
    IP.str("");
//    cout << "SOURCE IP: "
//         << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
//         << static_cast<int>(octet3) << "." << static_cast<int>(octet4) << "\n";

    //destination IP
    ipHeader.destinationIP = ntohl(ipHeader.destinationIP);
    octet1 = (ipHeader.destinationIP >> 24) & 0xFF;
    octet2 = (ipHeader.destinationIP >> 16) & 0xFF;
    octet3 = (ipHeader.destinationIP >> 8) & 0xFF;
    octet4 = ipHeader.destinationIP & 0xFF;

    IP << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
              << static_cast<int>(octet3) << "." << static_cast<int>(octet4);
    root["Destination IP"] = IP.str();
//    cout << "DESTINATION IP: "
//         << dec << static_cast<int>(octet1) << "." << static_cast<int>(octet2) << "."
//         << static_cast<int>(octet3) << "." << static_cast<int>(octet4) << "\n";

    cout << dec;
    Json::StreamWriterBuilder writerBuilder;
    string jsonString = Json::writeString(writerBuilder, root);
    ofstream outputPacket("D:\\projects\\c++\\miniShark\\backend\\output\\packet" + root["Packet"].asString() + ".json");
    if (outputPacket.is_open())
        outputPacket << jsonString;
    else cout << "operatie esuata!\n";
    if(static_cast<int>(ipHeader.protocol) == 1)
        //ICMP
        parseIPv4ICMP(packetData);
    else if (static_cast<int>(ipHeader.protocol) == 6)
        //TCP
        parseIPv4TCP(packetData);
    else if (static_cast<int>(ipHeader.protocol) == 17)
        //UDP
        parseIPv4UDP(packetData);
    else if (static_cast<int>(ipHeader.protocol) == 41)//TO DISCUSS!!!
        //IPv6
        parseIPv4IPv6(packetData);
    else if (static_cast<int>(ipHeader.protocol) == 50)
        //ESP
        parseIPv4ESP(packetData);
    else if (static_cast<int>(ipHeader.protocol) == 51)
        //AH
        parseIPv4AH(packetData);
    else if (static_cast<int>(ipHeader.protocol) == 89)
        //OSPF
        parseIPv4OSPF(packetData);
    else if (static_cast<int>(ipHeader.protocol) == 132)
        //SCTP
        parseIPv4SCTP(packetData);
}

void parseIPv4ICMP(vector<uint8_t> packetData) {

}

void parseIPv4TCP(vector<uint8_t> packetData) {
    if (packetData.size() < sizeof(TCPHeader)) {
        cout << "Incomplete TCP header.\n";
        return;
    }

    TCPHeader tcpHeader;
    memcpy(&tcpHeader, packetData.data(), sizeof(TCPHeader));

    tcpHeader.sourcePort = ntohs(tcpHeader.sourcePort);
    tcpHeader.destPort = ntohs(tcpHeader.destPort);
    tcpHeader.sequenceNumber = ntohl(tcpHeader.sequenceNumber);
    tcpHeader.ackNumber = ntohl(tcpHeader.ackNumber);
    tcpHeader.windowSize = ntohs(tcpHeader.windowSize);

    cout << "Source Port: " << tcpHeader.sourcePort << "\n";
    cout << "Destination Port: " << tcpHeader.destPort << "\n";
    cout << "Sequence Number: " << tcpHeader.sequenceNumber << "\n";
    cout << "Acknowledgement Number: " << tcpHeader.ackNumber << "\n";
    cout << "Flags: " << bitset<8>(tcpHeader.flags) << "\n";
    cout << "Window Size: " << tcpHeader.windowSize << "\n";
}

void parseIPv4UDP(vector<uint8_t> packetData) {

}

void parseIPv4IPv6(vector<uint8_t> packetData) {

}

void parseIPv4ESP(vector<uint8_t> packetData) {

}

void parseIPv4AH(vector<uint8_t> packetData) {

}

void parseIPv4OSPF(vector<uint8_t> packetData) {

}

void parseIPv4SCTP(vector<uint8_t> packetData) {

}