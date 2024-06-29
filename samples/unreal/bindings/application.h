#ifndef UNREAL_APPLICATION_H_
#define UNREAL_APPLICATION_H_

#include <qobject.h>
#include <qqmlapplicationengine.h>
#include <octoon/game_app.h>
#include <octoon/game_object.h>
#include <octoon/game_listener.h>

#include "timer.h"

#include "unreal_profile.h"
#include "unreal_behaviour.h"

namespace unreal
{
    class Application;
    class ApplicationListener : public octoon::GameListener
    {
    public:
        ApplicationListener(Application* application);

        void onMessage(std::string_view message) noexcept override;

    public:
        Application* application_;
    };

    class Application : public QObject
    {
        Q_OBJECT
    public:
        Q_PROPERTY(Timer* timer MEMBER timer_);

        Application();
        ~Application();

        Q_INVOKABLE bool open(const QString& winid) noexcept;
        Q_INVOKABLE void update() noexcept;

        QObject* findChild(const QString& name) noexcept;

        void sendMessage(std::string_view message) noexcept;
        Q_INVOKABLE QString getMessage() const noexcept;

    private:
        Timer* timer_;

        QString message_;

        octoon::GameAppPtr gameApp_;
        octoon::GameObjectPtr behaviour_;

        std::unique_ptr<QQmlApplicationEngine> engine_;
        std::shared_ptr<unreal::UnrealProfile> profile_;
    };
}

#endif