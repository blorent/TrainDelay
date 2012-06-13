#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QWebPage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::init()
{
    // Initialize the HTML parser
    m_htmlParser.init();

    // Initialize the db connector
    if (!m_db.init())
    {
        qCritical() << "Error initializing the database bridge";
        return false;
    }

    // Create a dummy list of trains to watch
    Train testTrain;
    testTrain.name = "IC2428";
    testTrain.number = 2428;
    testTrain.startPoint = "Namur";
    testTrain.endPoint = "Bruxelles-Luxembourg";
    m_trainList.append(testTrain);

    ui->progressBar->setValue(0);

    return true;
}

void MainWindow::run()
{
    // Disable the button
    ui->processButton->setEnabled(false);

    // Collection to hold all the html elements found
    QWebElementCollection elements;

    // Keep track of the date
    QDate day = QDate::currentDate();

    // Go back 7 days in time (by default)railtime.be doesn't keep infos longer than that)
    int daysInPast = 7;
    QDate lastDateToTest = QDate::currentDate();
    lastDateToTest = lastDateToTest.addDays(-daysInPast);

    // Record progress
    int totalQueriesToMake;
    int daysToProcess = daysInPast;
    int daysProcessed = 0;

    // Do a dry-run pass on all days to get the exact number of days to process
    while (day > lastDateToTest)
    {
        if (day.dayOfWeek() >= Qt::Saturday)
            daysToProcess--;
        day = day.addDays(-1);
    }
    day = QDate::currentDate();
    totalQueriesToMake = daysToProcess*m_trainList.count();

    // Inspect all week days backwards
    while (day > lastDateToTest)
    {
        // Test if this day needs to be considered
        if (day.dayOfWeek() >= Qt::Saturday)
        {
            day = day.addDays(-1);
            continue;
        }
        daysProcessed++;

        // Iterate through the list of trains
        for (int i=0; i<m_trainList.count(); i++)
        {
            // Update the progress
            ui->progressBar->setValue((int) (daysProcessed*(i+1)*100/(float) totalQueriesToMake));

            // Generate the url
            QString datestr = day.toString(Qt::ISODate);
            datestr = datestr.remove('-'); // Remove dashes
            QString railtimeUrlStr = QString("http://www.railtime.be/website/ShowTrain.aspx?l=FR&smc=1&dep=0&tn=%1&tr=00:00-60-%2&stn=1").arg(m_trainList[i].number).arg(datestr);

            // Load the desired page
            m_htmlParser.loadPage(railtimeUrlStr);

            // Get the list of the stop stations
            int startPointIndex = -1;
            int endPointIndex = -1;
            elements = m_htmlParser.getElements("td.TrainColumnStationPast");
            for (int stopNumber=0; stopNumber < elements.count(); stopNumber++)
            {
                // Find the index of the departure station
                if (startPointIndex == -1 && elements[stopNumber].toPlainText() == m_trainList[i].startPoint)
                {
                    startPointIndex = stopNumber;
                    continue;
                }

                // Find the idnex of the arrival station
                if (endPointIndex == -1 && elements[stopNumber].toPlainText() == m_trainList[i].endPoint)
                {
                    endPointIndex = stopNumber;
                    continue;
                }
            }

            // Check that we found the departure and arrival sations in the list
            if (startPointIndex == -1)
            {
                qCritical() << "Error : the wanted departure station " << m_trainList[i].startPoint << " could not be found (train=" << m_trainList[i].name << ", date=" << day.toString() << ")";
                continue;
            }
            else if (endPointIndex == -1)
            {
                qCritical() << "Error : the wanted arrival station " << m_trainList[i].endPoint << " could not be found (train=" << m_trainList[i].name << ", date=" << day.toString() << ")";
                continue;
            }

            // Check if it was late
            elements = m_htmlParser.getElements("td.TrainColumnArrivalDelayPast");
            QTime arrivalDelay = QTime::fromString(elements[endPointIndex].toPlainText(), "+ hh:mm");
            if (arrivalDelay.hour() == 0 && arrivalDelay.minute() == 0)
            {
                qDebug() << "The train " << m_trainList[i].name << " was not late at " << m_trainList[i].endPoint << " on the day " << day.toString();
                continue;
            }

            qDebug() << day.toString(Qt::ISODate) << " : delay at arrival for train " << m_trainList[i].name << " : " << arrivalDelay.hour() << "h" << arrivalDelay.minute();

            // It's late, let's create an object to record it
            DelayedTravel delay;
            delay.delay = arrivalDelay.hour()*60 + arrivalDelay.minute() ;
            delay.startPoint = m_trainList[i].startPoint;
            delay.endPoint = m_trainList[i].endPoint;
            delay.date = day;
            delay.trainNumber = m_trainList[i].number;

            // Get all the timing informations
            elements = m_htmlParser.getElements("td.TrainColumnDeparturePast");
            delay.expectedDepartureTime = QTime::fromString(elements[endPointIndex].toPlainText(), "hh:mm");

            elements = m_htmlParser.getElements("td.TrainColumnArrivalPast");
            delay.expectedArrivalTime = QTime::fromString(elements[endPointIndex].toPlainText(), "hh:mm");

            // Record the delay
            m_delaysList.append(delay);

        }
        // Go back in time
        day = day.addDays(-1);

        // Give the control back to the event loop
        QCoreApplication::processEvents();
    }

    // fill the text view
    QString text;
    for (int i=0; i < m_delaysList.count(); i++)
    {
        text += QString("%1 : Train %2").arg(m_delaysList[i].date.toString(Qt::ISODate)).arg(m_delaysList[i].trainNumber);
        text += " ";
        text += m_delaysList[i].startPoint;
        text += " - ";
        text += m_delaysList[i].endPoint;
        text += " : delay of ";
        text += QString("%1 minutes").arg(m_delaysList[i].delay);
        text += "\n";
    }
    ui->textViewer->setPlainText(text);

    // Enable the button
    ui->processButton->setEnabled(true);
}


void MainWindow::on_processButton_clicked()
{
    run();
}
