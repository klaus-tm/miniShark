#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <pcap.h>
#include <stdio.h>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include <vector>
#include <filesystem>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

using namespace std;
using json = nlohmann::json;

pcap_t* handle;
char errbuf[PCAP_ERRBUF_SIZE];
string path = __FILE__;
string outputDirectory;
string pcapOutputDirectory;

// Structure for defining an IP
typedef struct iphdr {
    unsigned char ip_hl : 4;
    unsigned char ip_v : 4;
    u_char ip_tos;
    short ip_len;
    u_short ip_id;
    short ip_off;
    u_char ip_ttl;
    u_char ip_p;
    u_short ip_sum;
    struct in_addr ip_src;
    struct in_addr ip_dst;
} ip;

// Function to retrieve the source IP address from the packet
string getSourceIP(const u_char* packetData) {
    const ip* ipHeader = (const ip*)(packetData + 14);
    char sourceAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->ip_src), sourceAddr, INET_ADDRSTRLEN);
    return sourceAddr;
}

// Function to retrieve the destination IP address from the packet
string getDestinationIP(const u_char* packetData) {
    const ip* ipHeader = (const ip*)(packetData + 14);
    char destAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->ip_dst), destAddr, INET_ADDRSTRLEN);
    return destAddr;
}

// Function to retrieve the protocol from the packet
string getProtocol(const u_char* packetData) {
    const ip* ipHeader = (const ip*)(packetData + 14);
    switch (ipHeader->ip_p) {
        case IPPROTO_TCP:
            return "TCP";
        case IPPROTO_UDP:
            return "UDP";
        case IPPROTO_ICMP:
            return "ICMP";
        default:
            return "Unknown";
    }
}

// Function to save the packet as a PCAP file
void savePacketAsPcap(const u_char* packetData, int packetLength) {
    // Create a unique filename for the PCAP file using the current timestamp
    time_t now = time(nullptr);
    string filename = pcapOutputDirectory + to_string(now) + ".pcap";

    // Open the PCAP file for writing
    pcap_t* pcapOutput = pcap_open_dead(DLT_EN10MB, 65536);
    pcap_dumper_t* pcapDumper = pcap_dump_open(pcapOutput, filename.c_str());

    // Write the packet data to the PCAP file
    pcap_pkthdr packetHeader;
    packetHeader.ts.tv_sec = now;
    packetHeader.ts.tv_usec = 0;
    packetHeader.caplen = packetLength;
    packetHeader.len = packetLength;
    pcap_dump((u_char*)pcapDumper, &packetHeader, packetData);

    // Close the PCAP file
    pcap_dump_close(pcapDumper);
    pcap_close(pcapOutput);

    cout << "Packet saved as PCAP file: " << filename << endl;
}

// Packet handling function
void packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packetData) {
    // Create a JSON object to store packet information
    json packetInfo;
    packetInfo["timestamp"] = pkthdr->ts.tv_sec;
    packetInfo["length"] = pkthdr->len;

    // Retrieve and add additional details to the packetInfo object
    json details;
    details["source_ip"] = getSourceIP(packetData);
    details["destination_ip"] = getDestinationIP(packetData);
    details["protocol"] = getProtocol(packetData);
    // Add more details as needed

    packetInfo["details"] = details;

    // Create a unique filename for the JSON file using the timestamp
    string filename = outputDirectory + to_string(pkthdr->ts.tv_sec) + ".json";

    // Open the file for writing
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error creating output file: " << filename << endl;
        return;
    }

    // Write the JSON object to the file
    outputFile << packetInfo.dump(4); // Use pretty printing with 4 spaces indentation
    outputFile.close();

    cout << "Packet saved to file: " << filename << endl;

    // Save the packet as a PCAP file
    savePacketAsPcap(packetData, pkthdr->len);
}

// Function to retrieve a list of available network interfaces
vector<string> getInterfaces() {
    vector<string> interfaces;
    pcap_if_t* alldevs;

    // Retrieve the list of network interfaces
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        cerr << "Error finding network interfaces: " << errbuf << endl;
        return interfaces;
    }

    // Iterate through the list of interfaces and store their names
    for (pcap_if_t* dev = alldevs; dev != NULL; dev = dev->next) {
        interfaces.push_back(dev->name);
    }

    // Free the allocated memory
    pcap_freealldevs(alldevs);

    return interfaces;
}

int main() {
    pcap_if_t* alldevs;
    pcap_if_t* selectedDev;
    int interfaceIndex = 0;

    // Fix paths
    path = path.substr(0, path.length() - 13);
    outputDirectory = path + "jsonOutput/";
    pcapOutputDirectory = path + "pcapOutput/";

    // Create output directories if they don't exist
    if (!std::filesystem::exists(outputDirectory))
        std::filesystem::create_directory(outputDirectory);
    if (!std::filesystem::exists(pcapOutputDirectory))
        std::filesystem::create_directory(pcapOutputDirectory);

    // Retrieve the list of network interfaces
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        cerr << "Error finding network interfaces: " << errbuf << endl;
        return 1;
    }

    // Print the available interfaces and indicate the default one
    cout << "Available network interfaces:" << endl;
    for (pcap_if_t* dev = alldevs; dev != NULL; dev = dev->next) {
        cout << interfaceIndex << ". " << dev->name;
        if (dev->flags & PCAP_IF_LOOPBACK) {
            cout << " (Default)";
        }
        cout << endl;
        interfaceIndex++;
    }

    cout << "Enter the interface index to capture packets from: ";
    cin >> interfaceIndex;

    // Find the selected interface in the list
    selectedDev = alldevs;
    for (int i = 0; i < interfaceIndex; i++) {
        if (selectedDev == NULL) {
            cerr << "Invalid interface index." << endl;
            pcap_freealldevs(alldevs);
            return 1;
        }
        selectedDev = selectedDev->next;
    }

    // Open the selected interface for packet capture
    handle = pcap_open_live(selectedDev->name, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        cerr << "Error opening device: " << errbuf << endl;
        pcap_freealldevs(alldevs);
        return 1;
    }

    cout << "Using network interface: " << selectedDev->name;
    if (selectedDev->flags & PCAP_IF_LOOPBACK) {
        cout << " (Default)";
    }
    cout << endl;

    // Loop to continuously capture packets
    pcap_loop(handle, -1, packetHandler, NULL);

    // Free the allocated memory
    pcap_freealldevs(alldevs);

    return 0;
}
