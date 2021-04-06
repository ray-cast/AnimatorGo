#ifndef RABBIT_APPLICATION_H_
#define RABBIT_APPLICATION_H_

#include <qobject.h>
#include <qqmlapplicationengine.h>
#include <octoon/game_app.h>
#include <octoon/game_object.h>

#include "timer.h"

#include "rabbit_profile.h"
#include "rabbit_behaviour.h"

namespace rabbit
{
    class Application : public QObject
    {
        Q_OBJECT
    public:
        Q_PROPERTY(Timer* timer MEMBER timer_);

        Application();
        ~Application();

        Q_INVOKABLE bool open(const QString& winid) noexcept;
        Q_INVOKABLE void update() noexcept;

    private:
        Timer* timer_;

        octoon::GameAppPtr gameApp_;
        octoon::GameObjectPtr behaviour_;

        std::unique_ptr<QQmlApplicationEngine> engine_;
        std::shared_ptr<rabbit::RabbitProfile> profile_;
    };
}

#endif