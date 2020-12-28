#ifndef HACKWIFI_H
#define HACKWIFI_H

#include <fstream> //getting information from files made by system calls
#include <string.h> //command manip to c_str for system()
#include <string>
#include <vector>
#include <sstream> //sanitizing input
#include <limits.h> //trash the buffer for std::cin

/**
 * @brief The ap struct holds relevant information about an Access point
 */
struct ap
{
    std::string essid;
    std::string address;
    std::string channel;
};

struct Handshake
{
    std::string directory;
    std::string essid;
    std::string address;
    int channel;
    std::string comment;
};

namespace air
{
    bool checkTools();
    void fixInstalls(bool&, bool&, bool&, std::stringstream&cout);//using reference
    void checkVersions(bool&, bool&, bool&);

    std::string scanWifi(std::string apList, std::string interface);
    std::vector<ap> toApVector(std::vector<std::string>&);
    bool duplicate(ap&, std::vector<ap>&);

    template <class T>
    int getNumber(std::vector<T>);
    std::vector<std::string> getInterface();
    std::vector<std::string> getConnectedUsers(ap host, std::string interface);
    std::vector<std::string> parseTargetsFile(std::string file);

    //the important 1
    int init(std::stringstream&cout);

    bool attack(ap, std::string interface, std::string fileName, int secondsTimeOut);//grabs handshake by kicking everyone off
    void runAirodump(ap target, std::string interface, std::string fileName, std::string handshakeStreamFile, int secondsTimeOut);
    void runAireplay(ap target, std::string interface);
    bool checkHandshakeFile(std::string handshakeFile);
    void crackPassword(ap target);
    void createApHandshakeFile(std::string file, std::vector<ap> aps);

    void genericAttack(std::string apHandshakeFile);
    void crackAttack(std::string apHandshakeFile);

    void trashTheBuffer();
    void pressEnterToContinue();

    //convinence
    std::string apToString(ap);
    std::string handshakeToString(Handshake);

}

#endif // HACKWIFI_H
