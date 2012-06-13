#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QDate>
#include "webpageloader.h"
#include "dbaccess.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    // Structure that represent a train
    struct Train {
        QString name;
        int number;
        QString startPoint;
        QString endPoint;
    };
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool init();
    void run();

private slots:
    void on_processButton_clicked();
    
private:

    // Data members
    Ui::MainWindow *ui;
    QList<Train> m_trainList;
    QList<DelayedTravel> m_delaysList;
    WebPageLoader m_htmlParser;
    dbAccess m_db;

    // Functions
    bool readFile();
    bool writeFile();
};

#endif // MAINWINDOW_H
