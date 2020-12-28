#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow the main window presented at startup
 * @param parent parent widget, if any
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAutoFillBackground(true);

    xpos = 10;
    ypos = 60;
    m_width = 200;
    m_height = 20;
    buffer = 10;
    listHeight = 300;
    buttonHeight = 20;
    styleFileName = "stylesheet.txt";

    ui->textEdit->setReadOnly(true);

    QRect tabSize(0, 60, 660, 420);

    //tabs
    linDeauthTab = new LinearDeauthTab(this);
    tabs.push_back(linDeauthTab);
    cracktab = new CrackTab(this);
    tabs.push_back(cracktab);
    manageCapturesTab = new ManageCapturesTab(this);
    tabs.push_back(manageCapturesTab);

    for(QWidget* w : tabs)
    {
        w->setStyleSheet("QTextEdit{color:lime;background-color:black;};QListWidget{color:lime;background-color:black;}");
        w->setGeometry(tabSize);
        w->hide();
    }
    tabs.push_back(ui->textEdit);
    ui->textEdit->show();
    ui->textEdit->setStyleSheet("QTextEdit{color:lime;background-color:black;}");

    //userConfig = "config.txt"; //saving themes and all that
    storedCaptureFileName = "Captured_Handshakes";
    dataFile = "data.txt";
    saveFolderName = "Data";//name of folder to make and send save handshake files to
    dataFilePath = saveFolderName + "/" + dataFile;

    cracktab->setFilePath(dataFilePath);
    manageCapturesTab->setDataPath(dataFilePath);
    manageCapturesTab->setDataFileFolder(saveFolderName);

    //style at end
    //see function for more bug details //ui setup needs to be called b4 this
    this->setStyleSheet(getThemeFromText("redline").c_str());

    //connect functions
    connect(ui->actionLinear_Deauth_Attack, SIGNAL(triggered()), this, SLOT(loadLinearDeauthAttackTab()));
    connect(ui->actionCrack_Handshake, SIGNAL(triggered()), this, SLOT(loadCrackHandshakeTab()));
    connect(ui->actionCaptured_handshakes, SIGNAL(triggered()), this, SLOT(loadManageHandshakeTab()));
}

/**
 * @brief MainWindow::~MainWindow destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::writeToMainText writes to the main cout of the MainWindow, textEdit
 * @param s string to written out
 */
void MainWindow::writeToMainText(std::string s)
{
    ui->textEdit->setText(QString(s.c_str()));
}

//slots for tabs//////////////////////////////////////////////////////////////

/**
 * @brief MainWindow::loadLinearDeauthAttackTab presents the linearDeauthTab to the user
 */
void MainWindow::loadLinearDeauthAttackTab()
{
    for(QWidget *w : tabs)
    {
        w->hide();
    }

    linDeauthTab->show();
}

/**
 * @brief MainWindow::loadCrackHandshakeTab presents the crackHandshakeTab to the user
 */
void MainWindow::loadCrackHandshakeTab()
{
    for(QWidget *w : tabs)
    {
        w->hide();
    }

    cracktab->show();
}

/**
 * @brief MainWindow::loadManageHandshakeTab presents the loadManageHandshakeTab to the user
 */
void MainWindow::loadManageHandshakeTab()
{
    for(QWidget *w : tabs)
    {
        w->hide();
    }

    manageCapturesTab->show();
}

//////////////////////////////////////////////////////////////////////////////

/**
 * @brief MainWindow::appendToTextEdit appends a string to cout
 * @param cout the QTextEdit to append text to
 * @param text string to be appended to cout, a QTextEdit
 */
void MainWindow::appendToTextEdit(QTextEdit * cout, std::string text)
{
    cout->append(text.c_str());
    cout->verticalScrollBar()->setValue(cout->verticalScrollBar()->maximum());
    cout->repaint();//need this to print out immediately
}

/**
 * @brief MainWindow::getThemeFromText
 * @param themeWanted theme to be looked for in the stylesheet.txt
 * @return string with the styling for a certain theme in stylesheet.txt
 */
std::string MainWindow::getThemeFromText(std::string themeWanted)
{
    //BUGGY STILL: Fetches proper theme string but depending on where the styling for the main widget is located(ie not QPushButton{//style})
    //it will behave differently. Needs more testing, but cracking should take priority now, since I've put that in the backburner for too long.
    //need to come back to this function to make it look prettier/effecient later, it works but I dont think this is acceptable
    //as for the bug, I think the parser gets confused when I mix styling specific widgets and the main widget. I can try using to styling calls,
    //one for the main widget(or the this widget) and the other for component widget(ie QPushButton{//style})

    std::ifstream stylesheet;
    std::string styleString;
    std::string temp;

    stylesheet.open(styleFileName.c_str());
    if(stylesheet.is_open())
    {
        while (std::getline(stylesheet, temp) && !stylesheet.eof())
        {
            if(temp.size() <= 0 || temp[0] == '#')//relying on || operator optimizations
            {
                continue;
            }

            //while it hasnt found theme tag
            if(temp[0] == '@' && temp.find(themeWanted) != std::string::npos)
            {
                //read until the @end_theme tag is reached
                while((std::getline(stylesheet, temp) && !stylesheet.eof()) && temp.find("@end_theme") == std::string::npos)
                {
                    if(temp.size() == 0 || temp.find('#') != std::string::npos)
                    {
                        continue;
                    }

                    styleString.append(temp);
                }

                break;
            }
            break;
        }

        stylesheet.close();
    }

    return styleString;
}
