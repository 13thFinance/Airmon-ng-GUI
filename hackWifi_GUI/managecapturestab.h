#ifndef MANAGECAPTURESTAB_H
#define MANAGECAPTURESTAB_H
#include "hackwifi.h"
#include <QWidget>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fstream>

namespace Ui {
class ManageCapturesTab;
}

class ManageCapturesTab : public QWidget
{
    Q_OBJECT

public:
    explicit ManageCapturesTab(QWidget *parent = nullptr);
    ~ManageCapturesTab();

    void setDataPath(std::string);
    void setDataFileFolder(std::string);
private slots:
    void loadCaptureFile();
    void loadHandshake();
    void removeHandshake();
signals:

private:
    Ui::ManageCapturesTab *ui;
    std::string dataFilePath;
    std::string dataFileFolder;
    int numberHandshakes;
    std::vector<Handshake> handshakes;

    int parseNumberHandshakes(std::string);
    std::string getLineAfterToken(std::string, std::string);
    void updateDataFile();
};

#endif // MANAGECAPTURESTAB_H
