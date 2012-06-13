#include "webpageloader.h"
#include <QWebFrame>
#include <QDebug>

WebPageLoader::WebPageLoader(QObject *parent) :
    QObject(parent)
{
}

// Initialize the module
bool WebPageLoader::init()
{
    m_webView = new QWebView();
    if (m_webView == NULL)
    {
        qCritical("Error creating the web view in the page loader");
        return false;
    }

    m_asyncLoop = new QEventLoop();
    if (m_asyncLoop == NULL)
    {
        qCritical("Error creating the local event loop in the page loader");
        return false;
    }

    // Connect the "load finished" slot to the interruption of the local event loop
    connect(m_webView, SIGNAL(loadFinished(bool)), m_asyncLoop, SLOT(quit()));

    // Configure the timeout timer to interrupt the local event loop
    m_loadTimeout.setSingleShot(true);
    connect(&m_loadTimeout, SIGNAL(timeout()), m_asyncLoop, SLOT(quit()));

    return true;
}

// Set the active web page to the url passed
bool WebPageLoader::loadPage(QString url)
{
    // Start a 2sec timeout
    m_loadTimeout.start(2000);

    // Load the page
    m_webView->load(QUrl(url));

    // Wait for it to complete
    m_asyncLoop->exec();

    // Test if the timer is still running
    if(m_loadTimeout.isActive())
    {
        // download complete
        qDebug() << "Download page " << url << " completed";
        m_loadTimeout.stop();
    }
    else
    {
        // timeout
        qWarning() << "Fetching the page located at " << url << " timed out";
        return false;
    }

    return true;
}


// Get a list of all elements corresponding to the given CSS search pattern
QWebElementCollection WebPageLoader::getElements(QString searchPattern)
{
    QWebElementCollection elements = m_webView->page()->mainFrame()->findAllElements(searchPattern);
    return elements;
}
