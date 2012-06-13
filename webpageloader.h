#ifndef WEBPAGELOADER_H
#define WEBPAGELOADER_H

#include <QObject>
#include <QWebView>
#include <QWebElementCollection>
#include <QTimer>
#include <QEventLoop>

class WebPageLoader : public QObject
{
    Q_OBJECT
public:
    explicit WebPageLoader(QObject *parent = 0);

    bool init();
    bool loadPage(QString url);
    QWebElementCollection getElements(QString searchPattern);
    
signals:
    
public slots:

private:
    QWebView* m_webView;
    QTimer m_loadTimeout;
    QEventLoop *m_asyncLoop;
    
};

#endif // WEBPAGELOADER_H
