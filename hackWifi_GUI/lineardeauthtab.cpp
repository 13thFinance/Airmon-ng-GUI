#include "lineardeauthtab.h"
#include "ui_lineardeauthtab.h"

LinearDeauthTab::LinearDeauthTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LinearDeauthTab)
{
    ui->setupUi(this);

    ui->progressBar->hide();
    ui->saveCaptureButton->hide();
    ui->cout->setReadOnly(true);
    ui->commentLabel->hide();
    ui->commentTextEdit->hide();

    captured = false;
    //userConfig = "config.txt"; //saving themes and all that
    storedCaptureFileName = "Captured_Handshakes";
    dataFile = "data.txt";
    saveFolderName = "Data";//name of folder to make and send save handshake files to
    dataFilePath = saveFolderName + "/" + dataFile;

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->rescanApsButton, SIGNAL(clicked()), this, SLOT(rescanApListSlot()));
    connect(ui->rescanInterfacesButton, SIGNAL(clicked()), this, SLOT(rescanInterfaceListSlot()));
    connect(ui->captureButton, SIGNAL(clicked()), this, SLOT(captureHandshakeSlot()));
    connect(ui->saveCaptureButton, SIGNAL(clicked()), this, SLOT(saveHandshakeSlot()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearCoutSlot()));
    connect(ui->rescanTargetsButton, SIGNAL(clicked()), this, SLOT(rescanTargetListSlot()));
}

//SLOTS///////////////////////////////////////

/**
 * @brief LinearDeauthTab::rescanApListSlot rescans access points and updates the access points list(APList)
*/
void LinearDeauthTab::rescanApListSlot()
{
    populateApList();
}

/**
 * @brief LinearDeauthTab::rescanInterfaceListSlot rescans the interfaces(Wifi) and updates the interface list
 */
void LinearDeauthTab::rescanInterfaceListSlot()
{
    populateInterfaceList();
}

/**
 * @brief LinearDeauthTab::rescanTargetListSlot rescans the targets connected to an access point using the currently selected interface
 */
void LinearDeauthTab::rescanTargetListSlot()
{
    populateTargetList();
}

/**
 * @brief LinearDeauthTab::captureHandshakeSlot attempts an deauth attack and attempts to capture a handshake
 */
void LinearDeauthTab::captureHandshakeSlot()
{
    ui->commentLabel->hide();
    ui->commentTextEdit->hide();
    this->repaint();

    int targetIndex = ui->apListWidget->currentRow();
    int interfaceIndex = ui->interfaceListWidget->currentRow();

    if(targetIndex == -1 || interfaceIndex == -1)
    {
        appendToTextEdit("\nHey Buddy, you gotta choose an AP and interface first...\n");
        return;
    }

    ap target = apVector[size_t(targetIndex)];
    std::string interface = interfaceVector[size_t(interfaceIndex)];

    currentAp = target;//save for other function use
    currentInterface = interface;//save for other function use

    appendToTextEdit("Attempting to capture Handshake on:\n");
    appendToTextEdit("ESSID: " + target.essid + "\n" + "ADDRESS: " + target.address + "\n" + "CHANNEL: " + target.channel + "\n\n");
    int counter = 0;
    captured = false;

    ui->progressBar->setValue(0);
    ui->progressBar->show();//show progressbar
    this->repaint();
    int maxAttacks = 4;
    while(!captured && counter < maxAttacks)
    {
        ui->progressBar->setValue(int((double(counter) / double(maxAttacks)) * ui->progressBar->maximum()));
        captured = air::attack(target, interface, storedCaptureFileName, 6);
        counter++;

    }

    if(captured)
    {
        appendToTextEdit("Captured!\n");
        ui->commentLabel->show();
        ui->commentTextEdit->show();
        ui->saveCaptureButton->show();
    }
    else
    {
        appendToTextEdit("Failed to capture handshake...");
        ui->commentLabel->hide();
        ui->commentTextEdit->hide();
    }

    ui->progressBar->hide();
}

/**
 * @brief LinearDeauthTab::saveHandshakeSlot saves a captured handshake and writes out relevant data to a file to keep track
 */
void LinearDeauthTab::saveHandshakeSlot()
{
    if(captured == true && currentAp.address.size() != 0 && currentAp.channel.size() != 0)//explicit for clarity
    {
        saveHandshake(currentAp, ui->commentTextEdit->toPlainText().toStdString());

        ui->commentLabel->hide();
        ui->commentTextEdit->hide();
        ui->saveCaptureButton->hide();

        captured = false;
    }
    else
    {
        appendToTextEdit("\nCapture something first...\n");
    }
}

/**
 * @brief LinearDeauthTab::clearCoutSlot clears the std output
 */
void LinearDeauthTab::clearCoutSlot()
{
    ui->cout->clear();
}
//////////////////////////////////////////////

/**
 * @brief LinearDeauthTab::writeToMainText writes to cout of the tab if there exists one
 * @param s string to write to cout
 */
void LinearDeauthTab::writeToMainText(std::string s)
{
    ui->cout->setText(QString(s.c_str()));
}

/**
 * @brief LinearDeauthTab::appendToTextEdit appends a string to std out(cout) and moves the scrollbar down
 * @param text string to be appended
 */
void LinearDeauthTab::appendToTextEdit(std::string text)
{
    ui->cout->append(text.c_str());
    ui->cout->verticalScrollBar()->setValue(ui->cout->verticalScrollBar()->maximum());
    ui->cout->repaint();//update gui
}

/**
 * @brief LinearDeauthTab::populateApList populates the apList with available access points
 */
void LinearDeauthTab::populateApList()
{
    apVector.clear();
    ui->apListWidget->clear();

    std::string apList = "apList.txt";

    std::string interface;

    //if computer does not have an interface to scan with
    if(ui->apListWidget->count() == 0 && interfaceVector.size() == 0)
    {
        appendToTextEdit("Error! No interfaces to scan with!\nScan Interfaces first and select one... or maybe you don't have any?\n");
        return;
    }
    else
    {
        //get a valid interface
        int index = ui->interfaceListWidget->currentRow();///////////////////////////

        if(index != -1)
        {
            appendToTextEdit(std::string("\nScanning with: ") + interfaceVector[size_t(index)] + "\n");
        }
        else//scan with first item aka index == -1
        {
            appendToTextEdit(std::string("\nNo interface choosen!\nScanning with: ") + interfaceVector[0] + "\n");
        }
    }

    std::string error = air::scanWifi(apList, interface);

    std::ifstream aps;
    aps.open(apList.c_str());

    std::string line;
    std::vector<std::string> listOfAps;

    if(aps.is_open())
    {
        //read in aps
        while(std::getline(aps, line))
        {
            listOfAps.push_back(line);
        }
        aps.close();//close file since we dont need it anymore

        apVector = air::toApVector(listOfAps);
    }

    system(std::string("rm " + apList).c_str());

    for(ap a : apVector)
    {
        ui->apListWidget->addItem(a.essid.c_str());
    }

    appendToTextEdit("\nFinished AP scan\n");
}

/**
 * @brief LinearDeauthTab::populateInterfaceList adds items of available interfaces(wifi) to interfaceList
 */
void LinearDeauthTab::populateInterfaceList()
{
    //clear interface list and object storage
    interfaceVector.clear();
    ui->interfaceListWidget->clear();

    //scan for interfaces
    interfaceVector = air::getInterface();

    for(std::string s : interfaceVector)
    {
        ui->interfaceListWidget->addItem(s.c_str());
    }
}

//UNFINISHED
/**
 * @brief LinearDeauthTab::populateTargetList populates available targets/users to disconnect from an AP/wifi target
 */
void LinearDeauthTab::populateTargetList()
{
    //make sure AP and interface has been choosen
    if(ui->interfaceListWidget->currentRow() == -1 || ui->apListWidget->currentRow() == -1)
    {
        appendToTextEdit("Choose AP and interface first!\n");
        return;
    }

    //get user's choosen ap and interface to use in airdump-ng
    currentAp = apVector[ui->apListWidget->currentRow()];
    currentInterface = interfaceVector[ui->interfaceListWidget->currentRow()];
    std::vector<std::string> targets = air::getConnectedUsers(currentAp, currentInterface); //GET CONNECTED USERS DOESNT WORK

    //print targets out
    ui->cout->append("Targets aquired\nPrinting...\n------------\n");
    for(std::string target : targets)
    {
        ui->cout->append(target.c_str());
    }

    ui->cout->append("------------\nDone!\n");
}

/**
 * @brief saveHandshake saves handshake files to folderpath directory, and then under a subdirectory, and stores this information in a dataFile
 * @param a ap whose handshake has been captured
 * @param comment comment to attach to the capture
 */
void LinearDeauthTab::saveHandshake(ap a, std::string comment)
{
    system(std::string("mkdir " + saveFolderName).c_str());//create the folder if it doesn't already exist
    //make sure data.txt exists INSIDE Data/
    system(std::string("touch " + dataFilePath).c_str());//use touch to create text file to save summary of information to, using touch because it does not delete previous text file information if it already exists

    //read in data.txt to see whats the name of the new directory we should save the new capture under
    int numberHandshakes = getNumberOfSavedCaptures();

    if(numberHandshakes == -1)//if file is malformed or empty recreate storage
    {
        createStorage();
        saveHandshake(a, comment);
    }
    else
    {
        std::string folderNameToSaveTo = "cap_" + std::to_string(numberHandshakes + 1);
        system(std::string("mkdir " + saveFolderName + "/" + folderNameToSaveTo).c_str());//makes the directory to save to
        //move the captured files into the subdirectory
        system(std::string("mv " + storedCaptureFileName + "* " + saveFolderName + "/" + folderNameToSaveTo).c_str());
        std::string error = incrementCaptureCount();

        if(error.size() != 0)//if we have an error
        {
            appendToTextEdit(error);
            return;
        }
        addCaptureToData(a, comment, folderNameToSaveTo);
        appendToTextEdit("Capture Saved\n");
    }
}

/**
 * @brief LinearDeauthTab::updateCurrentItems updates
 */
void LinearDeauthTab::updateCurrentItems()
{
    int index = ui->apListWidget->currentRow();

    if(index != -1)
    {
        currentAp = apVector[size_t(index)];
    }

    index = ui->interfaceListWidget->currentRow();

    if(index != -1)
    {
        currentInterface = interfaceVector[size_t(index)];
    }

    index = ui->connectedUserList->currentRow();

    if(index != -1)
    {
        std::string connectUserTarget = connectedUsersVector[size_t(index)];
    }

}

/**
 * @brief LinearDeauthTab::getNumberOfSavedCaptures reads a file to get the number of saved captured handshakes
 * @return the number of captures handshakes which have been saved
 */
int LinearDeauthTab::getNumberOfSavedCaptures()
{
    std::ifstream data(dataFilePath);//open Data/data.txt
    std::string handshakeLine;//number of captured handshakes is always stored in the first line ALWAYS
    if(data.is_open())
    {
        std::getline(data, handshakeLine);
        data.close();
    }

    //if the dataFile was messed with
    if(handshakeLine.size() == 0 || handshakeLine.find("HANDSHAKES:") == std::string::npos)
    {
        return -1;
    }
    else
    {
        std::stringstream numberString(std::string(handshakeLine, handshakeLine.find(":") + 1, handshakeLine.size()));
        int number = 0;
        numberString >> number;
        return number;
    }
}

/**
 * @brief LinearDeauthTab::createStorage deletes everything in the save folder and creates a new one with a properly formated data.txt
 */
void LinearDeauthTab::createStorage()
{
    //delete everything in the save folder, including the folder
    system(std::string("rm -r " + saveFolderName).c_str());

    //remake directory
    system(std::string("mkdir " + saveFolderName).c_str());

    //remake data.txt inside Data/
    system(std::string("touch " + dataFilePath).c_str());

    //append zero handshakes captured to it since we deleted everything
    std::ofstream data(dataFilePath);

    if(data.is_open())
    {
        data << "HANDSHAKES:0\n";
        data.close();
    }
}

/**
 * @brief LinearDeauthTab::incrementCaptureCount increments the count in data.txt while keeping an old save
 * @return returns a string containing error encountered if any, if "" returned then there was no issues
 */
std::string LinearDeauthTab::incrementCaptureCount()
{
    std::ifstream data(dataFilePath);//open from Data/data.txt

    //save all lines to string vector, then edit the first string vector to increase the count

    if(data.is_open())
    {
        std::vector<std::string> info;
        std::string buffer;
        while(!data.eof())//while not end of file
        {
            std::getline(data, buffer);
            info.push_back(buffer);
        }
        data.close();
        //wholeText now contains everything in the file
        //and the first item should hold the number of handshakes captured

        //rename current data.txt to data.txt.old
        system(std::string("mv " + dataFilePath + " " + dataFilePath + ".old").c_str());

        //edit CAPTURED:
        std::stringstream bufferStream(std::string(info[0], info[0].find(":") + 1, info.size()));//grab current number
        int count = 0;
        bufferStream >> count;
        info[0] = std::string("HANDSHAKES:" + std::to_string((count + 1)) + "\n");//edit string with updated count

        //write out info to a new Data/data.txt
        std::ofstream out(dataFilePath);

        if(out.is_open())
        {
            for(std::string s : info)
            {
                out << s + "\n";
            }
            out.close();
        }
        else
        {
            return "\nERROR: Could not update Data/data.txt!\n";
        }

        return "";
    }

    return "\nERROR: Could not update Data/data.txt!\n";
}

/**
 * @brief LinearDeauthTab::addCaptureToData adds captured handshake to data.txt
 * @param a access point to add to the data.txt
 * @param comment user made comment
 * @param folderNameToSaveTo name of the directory where the capture files are being saved
 */
void LinearDeauthTab::addCaptureToData(ap a, std::string comment, std::string folderNameToSaveTo)
{
    std::vector<std::string> textVector = readData();

    //append relavant info
    textVector.push_back(std::string("\ndirectory:" + folderNameToSaveTo + "\n"));
    textVector.push_back(std::string("ESSID:" + a.essid + "\n"));
    textVector.push_back(std::string("Address:" + a.address + "\n"));
    textVector.push_back(std::string("Channel:" + a.channel + "\n"));
    textVector.push_back(std::string("start_comment:") + folderNameToSaveTo);
    textVector.push_back("\n" + comment + "\n");
    textVector.push_back(std::string("end_comment:") + folderNameToSaveTo + "\n");

    writeData(textVector);
}

/**
 * @brief LinearDeauthTab::readData reads data from the dataFilePath
 * @return  a vector of strings filled with the lines in the dataFilePath text
 */
std::vector<std::string> LinearDeauthTab::readData()
{
    std::ifstream data(dataFilePath);

    std::vector<std::string> textVector;

    if(data.is_open())
    {
        //while not end of file
        std::string buffer;
        while(!data.eof())
        {
            std::getline(data, buffer);
            textVector.push_back(buffer + "\n");
        }

        data.close();
    }

    return textVector;
}

/**
 * @brief LinearDeauthTab::writeData writes out to a dataFilePath
 * @param textVector a vector filled with strings to be written out dataFilePath
 */
void LinearDeauthTab::writeData(std::vector<std::string> textVector)
{
    //write to new Data/data.txt file
    std::ofstream data(dataFilePath);

    if(data.is_open())
    {
        for(std::string s : textVector)
        {
            data << s;
        }
    }
}

/**
 * @brief LinearDeauthTab::~LinearDeauthTab destructor
 */
LinearDeauthTab::~LinearDeauthTab()
{
    delete ui;
}
