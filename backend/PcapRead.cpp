#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <winsock.h>
#include <iomanip>
#include <sstream>

using namespace std;

struct pcap_global_header {
    uint32_t magic_number;  /* magic number */
    uint16_t version_major; /* major version number */
    uint16_t version_minor; /* minor version number */
    int32_t  thiszone;      /* GMT to local correction */
    uint32_t sigfigs;       /* accuracy of timestamps */
    uint32_t snaplen;       /* max length of captured packets, in octets */
    uint32_t network;       /* data link type */
};

struct pcap_packet_header {
    uint32_t ts_sec;   // timestamp seconds
    uint32_t ts_usec;  // timestamp microseconds/nanoseconds
    uint32_t incl_len; // number of octets of packet saved in file
    uint32_t orig_len; // actual length of packet
};

struct pcap_packet {
    pcap_packet_header pcapPacketHeader;
    vector<uint8_t> data;
};

int main() {
    string filePath;
    // Open the PCAP file
    ifstream file("C:\\Users\\cerce\\Desktop\\fuzz-2006-07-09-6023.pcap", ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening pcap file" << endl;
        return 1;
    }

    // Read the PCAP file header
    pcap_global_header file_header;
    file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header)); //reads pcap file header and interprets it
    file_header.magic_number = ntohl(file_header.magic_number);//ntohl
    file_header.version_major = ntohs(file_header.version_major);
    file_header.version_minor = ntohs(file_header.version_minor);
    file_header.thiszone = ntohl(file_header.thiszone);//
    file_header.sigfigs = ntohl(file_header.sigfigs);//
    file_header.snaplen = ntohl(file_header.snaplen);//
    file_header.network = ntohl(file_header.network);//

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
    vector<pcap_packet>packets;
    pcap_packet_header packetHeader;
    while (file.read(reinterpret_cast<char*>(&packetHeader), sizeof(packetHeader))) {
        pcap_packet pcapPacket;
        pcapPacket.pcapPacketHeader = packetHeader;
        pcapPacket.data.resize(ntohl(packetHeader.incl_len));
        file.read(reinterpret_cast<char*>(pcapPacket.data.data()), ntohl(packetHeader.incl_len));
        packets.push_back(pcapPacket);
    }
    // Close the PCAP file
    file.close();

    for (int i = 0; i < packets.size(); ++i) {
        pcap_packet packet = packets[i];
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
