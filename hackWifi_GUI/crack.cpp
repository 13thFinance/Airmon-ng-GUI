#include "crack.h"
#include "ui_crack.h"

CrackTab::CrackTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::crack)
{
    ui->setupUi(this);
    ui->captureInformationSimpleTextEdit->setReadOnly(true);
    //ui->captureListWidget->setSelectionBehavior();

    dataFilePath = "Data/data.txt";

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->reloadCapturesButton, SIGNAL(clicked()), this, SLOT(reloadCaptures()));
    connect(ui->captureListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(loadComment()));
}

//slots///////////////////////////////////////////////////////////

/**
 * @brief CrackTab::reloadCaptures reloads the captured handshakes from file
 */
void CrackTab::reloadCaptures()
{
    capturedHandshakes = readData();

    for(Handshake h : capturedHandshakes)
    {
        ui->captureListWidget->addItem(h.essid.c_str());
    }

    ui->captureInformationSimpleTextEdit->clear();
}

void CrackTab::loadComment()
{
    ui->captureInformationSimpleTextEdit->clear();

    int item = ui->captureListWidget->currentRow();

    if(item < 0)
    {
        return;
    }

    Handshake temp = capturedHandshakes[ulong(item)];

    ui->captureInformationSimpleTextEdit->appendPlainText(std::string("ESSID:" + temp.essid).c_str());
    ui->captureInformationSimpleTextEdit->appendPlainText(std::string("Address:" + temp.address).c_str());
    ui->captureInformationSimpleTextEdit->appendPlainText(std::string("Channel:" + std::to_string(temp.channel)).c_str());
    ui->captureInformationSimpleTextEdit->appendPlainText(std::string("Comment:" + temp.comment).c_str());
}
//end_slots//////////////////////////////////////////////////////////

/**
 * @brief CrackTab::setFilePath sets the data file path for where to read the captured information
 * @param in_dataFilePath data file path
 */
void CrackTab::setFilePath(std::string in_dataFilePath)
{
    dataFilePath = in_dataFilePath;
}

/**
 * @brief CrackTab::readData reads data from the given file
 * @return returns a vector of handshakes read from file
 */
std::vector<Handshake> CrackTab::readData()
{
    ui->captureListWidget->clear();

    std::ifstream data (dataFilePath);
    std::vector<Handshake> handshakes;
    std::string temp;
    Handshake handshake;

    while(data.is_open() && !data.eof())
    {
        std::getline(data, temp);

        if(temp.size() == 0)
        {
            continue;
        }

        if(temp.find("directory:") != std::string::npos)
        {
            std::string dir;
            for(ulong i = temp.find(":") + 1; i < temp.size(); i++)
            {
                dir.push_back(temp[i]);
            }

            handshake.directory = dir;
        }
        else if(temp.find("ESSID:") != std::string::npos)
        {
            std::string essid;
            for(ulong i = temp.find(":") + 1; i < temp.size(); i++)
            {
                essid.push_back(temp[i]);
            }

            handshake.essid = essid;
        }
        else if(temp.find("Address:") != std::string::npos)
        {
            std::string address;
            for(ulong i = temp.find(":") + 1; i < temp.size(); i++)
            {
                address.push_back(temp[i]);
            }

            handshake.address = address;
        }
        else if(temp.find("Channel:") != std::string::npos)
        {
            std::string channel;
            for(ulong i = temp.find(":") + 1; i < temp.size(); i++)
            {
                channel.push_back(temp[i]);
            }

            handshake.channel = std::stoi(channel);
        }
        else if(temp.find("start_comment:") != std::string::npos)
        {
            while(data.is_open())
            {
                if(temp.find("end_comment:") == std::string::npos)
                {
                    break;
                }

                std::getline(data, temp);

                handshake.comment.append(temp + "\n");
            }

            handshakes.push_back(handshake);
            handshake.address.clear();
            handshake.channel = 0;
            handshake.comment.clear();
            handshake.essid.clear();
            handshake.directory.clear();
        }
    }

    data.close();
    return handshakes;
}

CrackTab::~CrackTab()
{
    delete ui;
}
