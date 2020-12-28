#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hackwifi.h"

#include "lineardeauthtab.h"
#include "crack.h"
#include "managecapturestab.h"

#include <string>
#include <vector>

#include <QApplication>
#include <QVector>
#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <qlistview.h>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QScrollBar>
#include <QPalette>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void writeToMainText(std::string);
private slots:
    void loadLinearDeauthAttackTab();
    void loadCrackHandshakeTab();
    void loadManageHandshakeTab();
signals:
private:
    enum TEXT{COUT = 0, COMMENT = 1};

    std::string dataFile;
    std::string storedCaptureFileName;
    std::string saveFolderName;
    std::string dataFilePath;
    std::string styleFileName;

    std::string style;
    int xpos, ypos;
    int m_width, m_height;
    int buffer;
    int listHeight;
    int buttonHeight;


    Ui::MainWindow* ui;

    //tabs
    QVector<QWidget*> tabs;/////////////////////////////////////////////////////////////////
    LinearDeauthTab* linDeauthTab;
    CrackTab* cracktab;
    ManageCapturesTab* manageCapturesTab;

    ap currentAp;
    std::string currentInterface;

    void appendToTextEdit(QTextEdit*, std::string);
    std::string getThemeFromText(std::string themeWanted);

};

#endif // MAINWINDOW_H
