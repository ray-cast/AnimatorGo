#ifndef FLOWER_TIMER_H_
#define FLOWER_TIMER_H_

#include <qwidget.h>
#include <qtimer.h>
#include <qdrag.h>
#include <qdatetime.h>

namespace flower
{
    class Timer : public QObject
    {
        Q_OBJECT
    public:
        Q_PROPERTY(QString appName MEMBER m_appName);

        Q_INVOKABLE QDateTime currentDateTime() const
        {
            return QDateTime::currentDateTime();
        }

    private:
        QString m_appName = "MyApp";
    };
}

#endif