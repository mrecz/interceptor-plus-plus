#ifdef _WIN32
#ifndef CUSTOMEVENTFILTER_H
#define CUSTOMEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include "Windows.h"
#include "vector"

class CustomEventFilter: public QObject,  public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit CustomEventFilter(QObject* parent = nullptr);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void* message, long long* result) override;

private:
    std::vector<HWND> winIDs;

public:
    /** In case of multiple screen setup, IDs of all overlays(one per screen) must be stored */
    template<typename CollectionT>
    void setWinIDs(CollectionT const& collection)
    {
        winIDs.clear();
        for(const auto& id : collection)
        {
            winIDs.push_back(static_cast<HWND>(IntToPtr(id)));
        }
    }

signals:
    void registredKeyPressed();
    void captureCurrentScreen();

};


#endif // CUSTOMEVENTFILTER_H
#endif // _WIN32
