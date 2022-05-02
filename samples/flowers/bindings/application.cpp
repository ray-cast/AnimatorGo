#include "application.h"
#include <qmessagebox.h>
#include <qqmlcontext.h>
#include <qqmlcomponent.h>
#include <qwindow.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qquicktextdocument.h>
#include <qapplication.h>

namespace rabbit
{
	ApplicationListener::ApplicationListener(Application* app)
		: application_(app)
	{
	}

	void
	ApplicationListener::onMessage(std::string_view message) noexcept
	{
		if (application_)
			application_->sendMessage(message);
	}

	Application::Application()
		: behaviour_(octoon::GameObject::create())
		, profile_(rabbit::RabbitProfile::load("./config/config.conf"))
	{
		timer_ = new Timer();
		message_ = "Loading...";

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

			QObject* qobject = this->findChild(name);
			if (qobject)
			{
				auto widget = dynamic_cast<QQuickItem*>(qobject);

				auto w = widget->width();
				auto h = widget->height();

				gameApp_ = std::make_shared<octoon::GameApp>();
				gameApp_->setGameListener(std::make_shared<ApplicationListener>(this));
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

	QObject*
	Application::findChild(const QString& name) noexcept
	{
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

		return qobject;
	}

	void
	Application::sendMessage(std::string_view message) noexcept
	{
		auto messageObject = this->findChild("loadingMessage");
		if (messageObject)
		{
			auto text = dynamic_cast<QQuickItem*>(messageObject);
			text->setProperty("text", QString::fromStdString(std::string(message)));
			QCoreApplication::processEvents();
			this->message_ = QString::fromStdString(std::string(message));
		}
	}

	QString
	Application::getMessage() const noexcept
	{
		return message_;
	}
}