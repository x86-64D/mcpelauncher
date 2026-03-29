#pragma once

#include <QWebEngineUrlSchemeHandler>
#include <utility>

class SchemeHandler : public QWebEngineUrlSchemeHandler {
private:
    QString endUrl;
public:
    explicit SchemeHandler(QString endUrl) : endUrl(std::move(endUrl)) {}
    void requestStarted(QWebEngineUrlRequestJob *request) override;

};
