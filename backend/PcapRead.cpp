#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


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

    cout << "Magic number: " << file_header.magic_number<< "\n";
    cout << "Version major: " <<file_header.version_major<< "\n";
    cout << "Version minor: " <<file_header.version_minor<< "\n";
    cout << "Thiszone: " <<file_header.thiszone<< "\n";
    cout << "Sigfigs: " <<file_header.sigfigs<< "\n";
    cout << "Snaplen: " <<file_header.snaplen<< "\n";
    cout << "Network: " <<file_header.network<< "\n";

    // Loop through the packets in the file
    int packet_count = 0;
    while (file.peek() != EOF) {
        // Read the packet header
        pcap_packet_header packet_header;
        file.read(reinterpret_cast<char*>(&packet_header), sizeof(packet_header));

        // Read the packet data
        char* packet_data = new char[packet_header.incl_len];
        file.read(packet_data, packet_header.incl_len);

        // Do something with the packet
        cout << "Packet #" << ++packet_count << ": \n";
        cout << "Timestamp seconds: " << packet_header.ts_sec << "\n";
        cout << "Timestamp microseconds/nanoseconds: " << packet_header.ts_usec << "\n";
        cout << "Number of octets saved into the capture: " << packet_header.incl_len << "\n";
        cout << "Actual packet size: " << packet_header.orig_len << "\n";

        // Free the packet data buffer
        delete[] packet_data;
    }

    // Close the PCAP file
    file.close();

    return 0;

}
