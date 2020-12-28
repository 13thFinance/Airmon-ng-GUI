#include "managecapturestab.h"
#include "ui_managecapturestab.h"

ManageCapturesTab::ManageCapturesTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageCapturesTab)
{
    ui->setupUi(this);

    dataFilePath = "";
    dataFileFolder = "";

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(loadCaptureFile()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeHandshake()));
    connect(ui->capturedHandshakesList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(loadHandshake()));

}

//should do exception handling here/////////////////////////////////////////////////////////
void ManageCapturesTab::loadCaptureFile()
{
    ui->captureInformationTextEdit->clear(); //clear cout
    ui->capturedHandshakesList->clear(); //clear the list
    handshakes.clear(); //clear currently held handshakes

    if(dataFilePath.length() == 0)
    {
        return;
    }

    std::ifstream in;
    in.open(dataFilePath);
    std::string line;
    Handshake handshake;

    while(!in.eof())
    {
        std::getline(in, line);//ifsteam has a c-like one, which may be unsafe

        if(line.size() == 0){continue;}//skips blanks

        if(line.find("HANDSHAKES:") != std::string::npos)
        {
            numberHandshakes = parseNumberHandshakes(line);
        }
        else if(line.find("directory:") != std::string::npos)
        {
            handshake.directory = getLineAfterToken(line, ":");
        }
        else if(line.find("ESSID:") != std::string::npos)
        {
            handshake.essid = getLineAfterToken(line, ":");
        }
        else if(line.find("Address:") != std::string::npos)
        {
            handshake.address = getLineAfterToken(line, ":");
        }
        else if(line.find("Channel:") != std::string::npos)
        {
            handshake.channel = std::stoi(getLineAfterToken(line, ":"));
        }
        else if(line.find("start_comment:") != std::string::npos)
        {
            std::getline(in, line);

            while(line.find("end_comment:") == std::string::npos)//while end_comment string is not found
            {
                handshake.comment.append(line);
                std::getline(in, line);
            }

            //once it is found we have a complete handshake
            handshakes.push_back(handshake);
            handshake = *new Handshake;/////////////////////////////////////////// although I understand this I do not know if it safe, need more research
        }
    }

    ui->captureInformationTextEdit->appendPlainText(("Number Handshakes: " + std::to_string(numberHandshakes)).c_str());

    for(Handshake h : handshakes)
    {
        ui->capturedHandshakesList->addItem(h.essid.c_str());
    }

    in.close();
}

void ManageCapturesTab::loadHandshake()
{
    ui->captureInformationTextEdit->clear(); //clear cout
    //append
    if(ui->capturedHandshakesList->count() > 0)
    {
        Handshake h = handshakes.at(ui->capturedHandshakesList->currentRow());
        std::string temp = air::handshakeToString(h);
        ui->captureInformationTextEdit->appendPlainText(temp.c_str());
    }
}

void ManageCapturesTab::removeHandshake()
{
    if(dataFilePath.length() == 0){return;}//in case data file has not been specified

    //remove from handshakes vector and store temporarily
    int removeIndex = ui->capturedHandshakesList->currentRow();

    if(removeIndex < 0){return;}//if nothing is selected

    Handshake h = handshakes[removeIndex];
    handshakes.erase(handshakes.begin()+removeIndex);

    //remove directory
    std::string removeCommand = "rm -r " + dataFileFolder + "/" + h.directory;
    system(removeCommand.c_str());

    //update data file
    updateDataFile();//updates datafile with current handshakes vector

    //reload list
    loadCaptureFile();
}

std::string ManageCapturesTab::getLineAfterToken(std::string line, std::string token)
{
    int index = line.find(token);
    index++;
    std::string temp;
    temp.append(line.begin()+index, line.end()); //////////super ifffffyyyyyy
    return temp;
}

int ManageCapturesTab::parseNumberHandshakes(std::string line)
{
    int index = line.find(":");

    std::string numberString;
    char c = (int)line.at(++index);
    while(index <= line.length() - 1 && (c >= '0' && c <= '9'))
    {
        numberString.push_back(c);
        c = line.at(index++);
    }

    return std::stoi(numberString);
}

void ManageCapturesTab::setDataPath(std::string dataPath)
{
    dataFilePath = dataPath;
}

void ManageCapturesTab::setDataFileFolder(std::string path)
{
    dataFileFolder = path;
}

/**
 * @brief ManageCapturesTab::updateDataFile updates data.txt with current handshakes vector
 */
void ManageCapturesTab::updateDataFile()
{
    std::ofstream dataFile;
    dataFile.open(dataFilePath.c_str());

    dataFile << "HANDSHAKES:" + std::to_string(handshakes.size()) + "\n\n";

    for(Handshake h : handshakes)
    {
        dataFile << air::handshakeToString(h);
    }

    dataFile.close();
}

ManageCapturesTab::~ManageCapturesTab()
{
    delete ui;
}
