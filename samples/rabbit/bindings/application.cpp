#include "application.h"
#include <qmessagebox.h>
#include <qqmlcontext.h>
#include <qqmlcomponent.h>
#include <qwindow.h>
#include <qquickitem.h>
#include <qquickwindow.h>

namespace rabbit
{
	Application::Application()
		: behaviour_(octoon::GameObject::create())
		, profile_(rabbit::RabbitProfile::load("./config/config.conf"))
	{
		timer_ = new Timer();

		engine_ = std::make_unique<QQmlApplicationEngine>();
		engine_->rootContext()->setContextProperty("$global", this);
		engine_->load(QUrl(QLatin1String("qrc:/res/qml/main.qml")));
	}

	Application::~Application()
	{
		engine_.reset();
	}

	bool
	Application::open(const QString& name) noexcept
	{
		assert(!gameApp_);

		try
		{
			if (gameApp_)
				return false;

			QObject* qobject = engine_->findChild<QObject*>(name);
			if (!qobject)
			{
				for (auto& it : engine_->rootObjects())
				{
					if (it->objectName() == name)
						qobject = it;
					else
						qobject = it->findChild<QObject*>(name);

					if (qobject) break;
				}
			}

			if (qobject)
			{
				auto widget = dynamic_cast<QQuickItem*>(qobject);

				auto w = widget->width();
				auto h = widget->height();

				gameApp_ = std::make_shared<octoon::GameApp>();
				gameApp_->open((octoon::WindHandle)widget->window()->winId(), w, h, w, h);
				gameApp_->setActive(true);

				behaviour_->addComponent<rabbit::RabbitBehaviour>(profile_);
			}

			return true;
		}
		catch (const std::exception&)
		{
			QMessageBox msg;
			msg.setWindowTitle((const char*)u8"错误");
			msg.setText((const char*)u8"当前您的显卡可能并不支持OpenCL，或正在使用核显");
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();

			gameApp_.reset();
			gameApp_ = nullptr;

			exit(0);
		}
	}

	void
	Application::update() noexcept
	{
		if (gameApp_)
			gameApp_->update();
	}
}