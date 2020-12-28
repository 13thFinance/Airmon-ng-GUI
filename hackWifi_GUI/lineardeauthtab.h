#ifndef LINEARDEAUTHTAB_H
#define LINEARDEAUTHTAB_H

#include "hackwifi.h"

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QScrollBar>

namespace Ui {
class LinearDeauthTab;
}

class LinearDeauthTab : public QWidget
{
    Q_OBJECT

public:
    explicit LinearDeauthTab(QWidget *parent = nullptr);
    ~LinearDeauthTab();
public slots:
    void rescanApListSlot();
    void rescanInterfaceListSlot();
    void rescanTargetListSlot();
    void captureHandshakeSlot();
    void saveHandshakeSlot();
    void clearCoutSlot();
private:
    std::vector<ap> apVector;
    std::vector<std::string> interfaceVector;
    std::vector<std::string> connectedUsersVector;
    bool captured;
    ap currentAp;
    std::string currentInterface;
    std::string currentTarget;
    std::string dataFile;
    std::string storedCaptureFileName;
    std::string saveFolderName;
    std::string dataFilePath;

    void writeToMainText(std::string);

    void appendToTextEdit(std::string);
    void populateApList();
    void populateInterfaceList();
    void populateTargetList();

    void updateCurrentItems();

    /**
     * @brief saveHandshake saves handshake files to folderpath directory, and then under a subdirectory, and stores this information in a dataFile
     * @param a ap whose handshake has been captured
     * @param comment comment to attach to the capture
     */
    void saveHandshake(ap a, std::string comment);

    int getNumberOfSavedCaptures();
    void createStorage();
    std::string incrementCaptureCount();
    void addCaptureToData(ap a, std::string comment, std::string folderNameToSaveTo);

    std::vector<std::string> readData();
    void writeData(std::vector<std::string>);

    Ui::LinearDeauthTab *ui;
};

#endif // LINEARDEAUTHTAB_H
