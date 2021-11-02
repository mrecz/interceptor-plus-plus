#ifdef _WIN32
#ifndef CUSTOMEVENTFILTER_H
#define CUSTOMEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include "Windows.h"

class CustomEventFilter: public QObject,  public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit CustomEventFilter(int winId, QObject* parent = nullptr);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void* message, long long* result) override;

signals:
    void registredKeyPressed();
    void captureCurrentScreen();

private:
    HWND winId;

};

#endif // CUSTOMEVENTFILTER_H
#endif // _WIN32
