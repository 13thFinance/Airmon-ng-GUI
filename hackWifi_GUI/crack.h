#ifndef CRACK_H
#define CRACK_H

#include "hackwifi.h"
#include <fstream>
#include <vector>

#include <QWidget>



namespace Ui {
class crack;
}

class CrackTab : public QWidget
{
    Q_OBJECT

public:
    explicit CrackTab(QWidget *parent = nullptr);
    void setFilePath(std::string in_filePath);
    ~CrackTab();
public slots:
    void reloadCaptures();
    void loadComment();
private:
    Ui::crack *ui;

    std::string dataFilePath;
    std::vector<Handshake> capturedHandshakes;

    std::vector<Handshake> readData();
};

#endif // CRACK_H
