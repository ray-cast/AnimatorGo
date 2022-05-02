#include "login_window.h"
#include <qgraphicsview.h>
#include <qgraphicsproxywidget.h>
#include <qgraphicslinearlayout.h>
#include <qapplication.h>

namespace flower
{
	LoginWidget::LoginWidget(QWidget* parent) noexcept
		: QWidget(parent)
		, validator_(std::make_unique<QRegExpValidator>(QRegExp(R"([a-zA-Z0-9\~\!\@\#\￥\%\&\*\_]+$)")))
	{
		this->setObjectName("loginWidget");
		this->setMouseTracking(true);
		this->setTabletTracking(true);
		this->setUpdatesEnabled(true);

		userAccount_ = std::make_unique<QAction>();
		userAccount_->setIcon(QIcon(":res/icons/user.png"));

		passAccount_ = std::make_unique<QAction>();
		passAccount_->setIcon(QIcon(":res/icons/pass.png"));

		eyeAccount_ = std::make_unique<QAction>();
		eyeAccount_->setIcon(QIcon(":res/icons/eye.png"));
		eyeAccount_->setCheckable(true);

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		logoButton_ = std::make_unique<QPushButton>();
		logoButton_->setObjectName("logo");
		logoButton_->setIconSize(QSize(100, 100));
		logoButton_->setFocusPolicy(Qt::NoFocus);

		selectButton_ = std::make_unique<QLabel>();
		selectButton_->setObjectName("selector");
		selectButton_->setText(u8"账号登陆");

		validButton_ = std::make_unique<QLabel>();
		validButton_->setObjectName("valid");

		usernameEdit_ = std::make_unique<LineEdit>();
		usernameEdit_->setObjectName("username");
		usernameEdit_->setPlaceholderText(u8"帐号/邮箱/手机");
		usernameEdit_->setMaxLength(20);
		usernameEdit_->setValidator(validator_.get());
		usernameEdit_->addAction(userAccount_.get(), QLineEdit::LeadingPosition);

		passwordEdit_ = std::make_unique<LineEdit>();
		passwordEdit_->setObjectName("password");
		passwordEdit_->setPlaceholderText(u8"密码");
		passwordEdit_->setEchoMode(QLineEdit::Password);
		passwordEdit_->setMaxLength(20);
		passwordEdit_->setValidator(validator_.get());
		passwordEdit_->addAction(passAccount_.get(), QLineEdit::LeadingPosition);
		passwordEdit_->addAction(eyeAccount_.get(), QLineEdit::TrailingPosition);

		loginButton_ = std::make_unique<QToolButton>();
		loginButton_->setObjectName("login");
		loginButton_->setText(u8"登录");
		loginButton_->setContentsMargins(0, 20, 0, 20);

		autoLoginBox_ = std::make_unique<QCheckBox>();
		autoLoginBox_->setObjectName("login");
		autoLoginBox_->setText(u8"自动登录");
		autoLoginBox_->setContentsMargins(0, 20, 0, 20);

		registerButton_ = std::make_unique<QPushButton>();
		registerButton_->setObjectName("register");
		registerButton_->setText(u8"注册账号");

		forgetButton_ = std::make_unique<QPushButton>();
		forgetButton_->setObjectName("forget");
		forgetButton_->setText(u8"忘记密码");

		hLayout_ = std::make_unique<QHBoxLayout>();
		hLayout_->addWidget(registerButton_.get(), 0, Qt::AlignRight | Qt::AlignTop);
		hLayout_->addSpacing(20);
		hLayout_->addWidget(forgetButton_.get(), 0, Qt::AlignLeft | Qt::AlignTop);

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addWidget(closeButton_.get(), 0, Qt::AlignRight | Qt::AlignTop);
		layout_->addSpacing(40);
		layout_->addWidget(logoButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(50);
		layout_->addWidget(selectButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(validButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(usernameEdit_.get(), 0, Qt::AlignBottom | Qt::AlignHCenter);
		layout_->addWidget(passwordEdit_.get(), 0, Qt::AlignTop | Qt::AlignHCenter);
		layout_->addWidget(autoLoginBox_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(loginButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(10);
		layout_->addLayout(hLayout_.get());
		layout_->addSpacing(20);
		layout_->setContentsMargins(0, 0, 0, 0);
		layout_->setSpacing(0);

		this->setTabOrder(usernameEdit_.get(), passwordEdit_.get());
		this->setTabOrder(passwordEdit_.get(), loginButton_.get());

		this->connect(eyeAccount_.get(), SIGNAL(toggled(bool)), this, SLOT(eyeToggled(bool)));
		this->connect(passwordEdit_.get(), SIGNAL(returnPressed()), loginButton_.get(), SIGNAL(clicked()), Qt::UniqueConnection);
	}

	LoginWidget::~LoginWidget() noexcept
	{
		validator_.reset();

		userAccount_.reset();
		passAccount_.reset();
		eyeAccount_.reset();

		autoLoginBox_.reset();
		closeButton_.reset();
		logoButton_.reset();
		selectButton_.reset();
		userLabel_.reset();
		passLabel_.reset();
		usernameEdit_.reset();
		passwordEdit_.reset();
		loginButton_.reset();
		forgetButton_.reset();
		registerButton_.reset();

		hLayout_.reset();
		layout_.reset();
	}

	void
	LoginWidget::eyeToggled(bool toggle)
	{
		if (toggle)
			passwordEdit_->setEchoMode(QLineEdit::Normal);
		else
			passwordEdit_->setEchoMode(QLineEdit::Password);
	}

	ForgetWidget::ForgetWidget(QWidget* parent) noexcept
		: QWidget(parent)
		, validator_(std::make_unique<QRegExpValidator>(QRegExp("[a-zA-Z0-9]+$")))
	{
		this->setObjectName("loginWidget");
		this->setMouseTracking(true);
		this->setTabletTracking(true);
		this->setUpdatesEnabled(true);

		userAccount_ = std::make_unique<QAction>();
		userAccount_->setIcon(QIcon(":res/icons/user.png"));

		passAccount_ = std::make_unique<QAction>();
		passAccount_->setIcon(QIcon(":res/icons/pass.png"));

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		logoButton_ = std::make_unique<QPushButton>();
		logoButton_->setObjectName("logo");
		logoButton_->setIconSize(QSize(100, 100));
		logoButton_->setFocusPolicy(Qt::NoFocus);

		selectButton_ = std::make_unique<QLabel>();
		selectButton_->setObjectName("selector");
		selectButton_->setText(u8"找回密码");

		validButton_ = std::make_unique<QLabel>();
		validButton_->setObjectName("valid");

		usernameEdit_ = std::make_unique<LineEdit>();
		usernameEdit_->setObjectName("username");
		usernameEdit_->setPlaceholderText(u8"手机号/邮箱");
		usernameEdit_->setMaxLength(20);
		usernameEdit_->setValidator(validator_.get());
		usernameEdit_->addAction(userAccount_.get(), QLineEdit::LeadingPosition);

		passwordEdit_ = std::make_unique<LineEdit>();
		passwordEdit_->setObjectName("password");
		passwordEdit_->setPlaceholderText(u8"验证码");
		passwordEdit_->setMaxLength(20);
		passwordEdit_->setValidator(validator_.get());
		passwordEdit_->addAction(passAccount_.get(), QLineEdit::LeadingPosition);

		forgetButton_ = std::make_unique<QToolButton>();
		forgetButton_->setObjectName("login");
		forgetButton_->setText(u8"发送验证码");
		forgetButton_->setContentsMargins(0, 20, 0, 20);

		returnButton_ = std::make_unique<QPushButton>();
		returnButton_->setObjectName("register");
		returnButton_->setText(u8"返回");

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addWidget(closeButton_.get(), 0, Qt::AlignRight | Qt::AlignTop);
		layout_->addSpacing(40);
		layout_->addWidget(logoButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(50);
		layout_->addWidget(selectButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(validButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(usernameEdit_.get(), 0, Qt::AlignBottom | Qt::AlignHCenter);
		layout_->addWidget(passwordEdit_.get(), 0, Qt::AlignTop | Qt::AlignHCenter);
		layout_->addWidget(forgetButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(10);
		layout_->addWidget(returnButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(20);
		layout_->setContentsMargins(0, 0, 0, 0);
		layout_->setSpacing(0);

		this->setTabOrder(usernameEdit_.get(), passwordEdit_.get());
		this->setTabOrder(passwordEdit_.get(), forgetButton_.get());
		this->connect(passwordEdit_.get(), SIGNAL(returnPressed()), forgetButton_.get(), SIGNAL(clicked()), Qt::UniqueConnection);
	}

	ForgetWidget::~ForgetWidget() noexcept
	{
		validator_.reset();

		userAccount_.reset();
		passAccount_.reset();

		closeButton_.reset();
		logoButton_.reset();
		selectButton_.reset();
		userLabel_.reset();
		passLabel_.reset();
		usernameEdit_.reset();
		passwordEdit_.reset();
		forgetButton_.reset();
		returnButton_.reset();

		layout_.reset();
	}

	RegisterWidget::RegisterWidget(QWidget* parent) noexcept
		: QWidget(parent)
		, validator_(std::make_unique<QRegExpValidator>(QRegExp("[a-zA-Z0-9]+$")))
	{
		this->setObjectName("loginWidget");
		this->setMouseTracking(true);
		this->setTabletTracking(true);
		this->setUpdatesEnabled(true);

		userAccount_ = std::make_unique<QAction>();
		userAccount_->setIcon(QIcon(":res/icons/user.png"));

		passAccount_ = std::make_unique<QAction>();
		passAccount_->setIcon(QIcon(":res/icons/pass.png"));

		eyeAccount_ = std::make_unique<QAction>();
		eyeAccount_->setIcon(QIcon(":res/icons/eye.png"));
		eyeAccount_->setCheckable(true);

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		logoButton_ = std::make_unique<QPushButton>();
		logoButton_->setObjectName("logo");
		logoButton_->setIconSize(QSize(100, 100));
		logoButton_->setFocusPolicy(Qt::NoFocus);

		selectButton_ = std::make_unique<QLabel>();
		selectButton_->setObjectName("selector");
		selectButton_->setText(u8"用户注册");

		validButton_ = std::make_unique<QLabel>();
		validButton_->setObjectName("valid");

		usernameEdit_ = std::make_unique<LineEdit>();
		usernameEdit_->setObjectName("username");
		usernameEdit_->setPlaceholderText(u8"帐号/邮箱");
		usernameEdit_->setMaxLength(20);
		usernameEdit_->setValidator(validator_.get());
		usernameEdit_->addAction(userAccount_.get(), QLineEdit::LeadingPosition);

		passwordEdit_ = std::make_unique<LineEdit>();
		passwordEdit_->setObjectName("password");
		passwordEdit_->setPlaceholderText(u8"密码(大于6位小于20位)");
		passwordEdit_->setEchoMode(QLineEdit::PasswordEchoOnEdit);
		passwordEdit_->setMaxLength(20);
		passwordEdit_->setValidator(validator_.get());
		passwordEdit_->addAction(passAccount_.get(), QLineEdit::LeadingPosition);
		passwordEdit_->addAction(eyeAccount_.get(), QLineEdit::TrailingPosition);

		registerButton_ = std::make_unique<QToolButton>();
		registerButton_->setObjectName("login");
		registerButton_->setText(u8"注册");
		registerButton_->setContentsMargins(0, 20, 0, 20);

		returnButton_ = std::make_unique<QPushButton>();
		returnButton_->setObjectName("register");
		returnButton_->setText(u8"返回");

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addWidget(closeButton_.get(), 0, Qt::AlignRight | Qt::AlignTop);
		layout_->addSpacing(40);
		layout_->addWidget(logoButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(50);
		layout_->addWidget(selectButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(validButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(usernameEdit_.get(), 0, Qt::AlignBottom | Qt::AlignHCenter);
		layout_->addWidget(passwordEdit_.get(), 0, Qt::AlignTop | Qt::AlignHCenter);
		layout_->addSpacing(10);
		layout_->addWidget(registerButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(10);
		layout_->addWidget(returnButton_.get(), 0, Qt::AlignCenter);
		layout_->addSpacing(20);
		layout_->setContentsMargins(0, 0, 0, 0);
		layout_->setSpacing(0);

		this->setTabOrder(usernameEdit_.get(), passwordEdit_.get());
		this->setTabOrder(passwordEdit_.get(), registerButton_.get());

		this->connect(eyeAccount_.get(), SIGNAL(toggled(bool)), this, SLOT(eyeToggled(bool)));
		this->connect(passwordEdit_.get(), SIGNAL(returnPressed()), registerButton_.get(), SIGNAL(clicked()), Qt::UniqueConnection);
	}

	RegisterWidget::~RegisterWidget() noexcept
	{
		validator_.reset();

		userAccount_.reset();
		passAccount_.reset();
		eyeAccount_.reset();

		closeButton_.reset();
		logoButton_.reset();
		selectButton_.reset();
		userLabel_.reset();
		passLabel_.reset();
		usernameEdit_.reset();
		passwordEdit_.reset();
		registerButton_.reset();
		returnButton_.reset();

		layout_.reset();
	}

	void
	RegisterWidget::eyeToggled(bool toggle)
	{
		if (toggle)
			passwordEdit_->setEchoMode(QLineEdit::Normal);
		else
			passwordEdit_->setEchoMode(QLineEdit::PasswordEchoOnEdit);
	}

	LoginWindow::LoginWindow(const std::shared_ptr<flower::FlowerBehaviour>& behaviour) noexcept
		: behaviour_(behaviour)
		, allowMove_(false)
		, loginWidget_(std::make_unique<LoginWidget>(this))
		, forgetWidget_(std::make_unique<ForgetWidget>(this))
		, registerWidget_(std::make_unique<RegisterWidget>(this))
		, isRoratingWindow_(false)
		, nextPageIndex_(0)
	{
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
		this->setAttribute(Qt::WA_TranslucentBackground);
		this->setWindowModality(Qt::ApplicationModal);
		this->setProperty("rotateValue", 0);
		this->setWindowTitle(u8"用户");
		this->setMouseTracking(true);
		this->setTabletTracking(true);
		this->setUpdatesEnabled(true);

		this->addWidget(loginWidget_.get());
		this->addWidget(forgetWidget_.get());
		this->addWidget(registerWidget_.get());

		loginWidget_->autoLoginBox_->setChecked(behaviour->getProfile()->clientModule->autoLogin);

		closeAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
		closeAnimation_->setDuration(100);
		closeAnimation_->setEasingCurve(QEasingCurve::InCubic);
		closeAnimation_->setStartValue(1);
		closeAnimation_->setEndValue(0);		

		rotateAnimation_ = std::make_unique<QPropertyAnimation>(this, "rotateValue");
		rotateAnimation_->setDuration(500);
		rotateAnimation_->setEasingCurve(QEasingCurve::InCubic);
		rotateAnimation_->setStartValue(0);
		rotateAnimation_->setEndValue(180);

		this->connect(closeAnimation_.get(), SIGNAL(finished()), this, SLOT(close()));
		this->connect(rotateAnimation_.get(), SIGNAL(valueChanged(QVariant)), this, SLOT(repaint()));
		this->connect(rotateAnimation_.get(), SIGNAL(finished()), this, SLOT(rotateFinished()));
		this->connect(loginWidget_->closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		this->connect(loginWidget_->loginButton_.get(), SIGNAL(clicked()), this, SLOT(loginEvent()));
		this->connect(loginWidget_->registerButton_.get(), SIGNAL(clicked()), this, SLOT(gotoRegisterEvent()));
		this->connect(loginWidget_->forgetButton_.get(), SIGNAL(clicked()), this, SLOT(gotoForgetEvent()));
		this->connect(loginWidget_->autoLoginBox_.get(), SIGNAL(toggled(bool)), this, SLOT(autoLoginToggled(bool)));
		this->connect(forgetWidget_->forgetButton_.get(), SIGNAL(clicked()), this, SLOT(forgetEvent()));
		this->connect(forgetWidget_->closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		this->connect(forgetWidget_->returnButton_.get(), SIGNAL(clicked()), this, SLOT(gotoLoginEvent()));
		this->connect(registerWidget_->registerButton_.get(), SIGNAL(clicked()), this, SLOT(registerEvent()));
		this->connect(registerWidget_->closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		this->connect(registerWidget_->returnButton_.get(), SIGNAL(clicked()), this, SLOT(gotoLoginEvent()));
	}

	LoginWindow::~LoginWindow() noexcept
	{
		loginWidget_.reset();
		registerWidget_.reset();
		positionAnimation_.reset();
		behaviour_.reset();
	}

	void
	LoginWindow::closeEvent()
	{
		closeAnimation_->start();
	}

	void
	LoginWindow::mousePressEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = true;
		startPos_ = e->globalPos();
		clickPos_ = e->pos();
	}

	void
	LoginWindow::mouseReleaseEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = false;
	}

	void
	LoginWindow::mouseMoveEvent(QMouseEvent* e) noexcept
	{
		if (allowMove_)
			this->move(e->globalPos() - clickPos_);
	}

	void
	LoginWindow::keyPressEvent(QKeyEvent* event) noexcept
	{
	}
	
	void
	LoginWindow::keyReleaseEvent(QKeyEvent* event) noexcept
	{
	}

	void
	LoginWindow::loginEvent()
	{
		auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
		if (client)
		{
			auto username = loginWidget_->usernameEdit_->text();
			auto password = loginWidget_->passwordEdit_->text();

			if (username.isEmpty())
			{
				loginWidget_->validButton_->setText(u8"用户名不能为空");
				this->shakeEvent();
				return;
			}

			if (username.length() > 20)
			{
				loginWidget_->validButton_->setText(u8"用户名长度不能大于20位");
				this->shakeEvent();
				return;
			}

			if (password.isEmpty())
			{
				loginWidget_->validButton_->setText(u8"密码不能为空");
				this->shakeEvent();
				return;
			}

			if (password.length() < 6 || password.length() > 20)
			{
				loginWidget_->validButton_->setText(u8"密码长度必须要大于6位小于20位");
				this->shakeEvent();
				return;
			}

			try
			{
				client->login(username.toStdString(), password.toStdString());
				loginWidget_->usernameEdit_->clear();
				loginWidget_->passwordEdit_->clear();
				emit loginSuccess();
				this->closeEvent();
			}
			catch (const std::exception& e)
			{
				loginWidget_->validButton_->setText(e.what());
				this->shakeEvent();
			}
		}
	}

	void
	LoginWindow::registerEvent()
	{
		auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
		if (client)
		{
			auto username = registerWidget_->usernameEdit_->text();
			auto password = registerWidget_->passwordEdit_->text();

			if (username.isEmpty())
			{
				registerWidget_->validButton_->setText(u8"用户名不能为空");
				this->shakeEvent();
				return;
			}

			if (username.length() > 20)
			{
				registerWidget_->validButton_->setText(u8"用户名长度不能大于20位");
				this->shakeEvent();
				return;
			}

			if (password.isEmpty())
			{
				registerWidget_->validButton_->setText(u8"密码不能为空");
				this->shakeEvent();
				return;
			}

			if (password.length() < 6 || password.length() > 20)
			{
				registerWidget_->validButton_->setText(u8"密码长度必须要大于6位小于20位");
				this->shakeEvent();
				return;
			}

			try
			{
				client->signUp(username.toStdString(), password.toStdString());
				client->login(username.toStdString(), password.toStdString());
				registerWidget_->usernameEdit_->clear();
				registerWidget_->passwordEdit_->clear();
				emit loginSuccess();
				this->closeEvent();
			}
			catch (const std::exception& e)
			{
				registerWidget_->validButton_->setText(e.what());
				this->shakeEvent();
			}
		}
	}

	void
	LoginWindow::forgetEvent()
	{
		auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
		if (client)
		{
			forgetWidget_->usernameEdit_->clear();
			auto username = forgetWidget_->usernameEdit_->text();
			client->forget(username.toStdString());
			forgetWidget_->validButton_->setText(u8"正在开发中");
			this->shakeEvent();
		}
	}

	void
	LoginWindow::gotoRegisterEvent()
	{
		nextPageIndex_ = 2;
		isRoratingWindow_ = true;
		rotateAnimation_->start();
		this->currentWidget()->hide();
	}
	
	void
	LoginWindow::gotoForgetEvent()
	{
		nextPageIndex_ = 1;
		isRoratingWindow_ = true;
		rotateAnimation_->start();
		this->currentWidget()->hide();
	}

	void
	LoginWindow::gotoLoginEvent()
	{
		nextPageIndex_ = 0;
		isRoratingWindow_ = true;
		rotateAnimation_->start();
		this->currentWidget()->hide();
	}

	void
	LoginWindow::rotateFinished()
	{
		isRoratingWindow_ = false;
		loginWidget_->usernameEdit_->clear();
		loginWidget_->passwordEdit_->clear();
		this->setCurrentIndex(nextPageIndex_);
		this->repaint();
	}

	void
	LoginWindow::autoLoginToggled(bool enable)
	{
		behaviour_->getProfile()->clientModule->autoLogin = enable;
	}

	void 
	LoginWindow::paintEvent(QPaintEvent* event)
	{
		if (isRoratingWindow_)
		{
			QPainter painter(this);
			painter.setRenderHint(QPainter::Antialiasing);

			QPixmap rotatePixmap(this->size());

			int rotateValue = this->property("rotateValue").toInt();
			if (rotateValue <= 90)
			{
				this->currentWidget()->render(&rotatePixmap);

				QTransform transform;
				transform.translate(width() / 2, height() / 2);
				transform.rotate(rotateValue, Qt::YAxis);
				transform.scale(1.0f - rotateValue / 90.0f * 0.3f, 1.0f - rotateValue / 90.0f * 0.3f);

				painter.setTransform(transform);
				painter.drawPixmap(width() / -2, height() / -2, rotatePixmap);
			}
			else
			{
				this->widget(nextPageIndex_)->render(&rotatePixmap);

				QTransform transform;
				transform.translate(width() / 2, height() / 2);
				transform.rotate(rotateValue + 180, Qt::YAxis);
				transform.scale(0.7f + rotateValue / 180.0f * 0.3f, 0.7f + rotateValue / 180.0f * 0.3f);

				painter.setTransform(transform);
				painter.drawPixmap(width() / -2, height() / -2, rotatePixmap);
			}
		}
		else
		{
			QPainterPath pathBack;
			pathBack.setFillRule(Qt::WindingFill);
			pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);

			QPainter painter(this);
			painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
			painter.fillPath(pathBack, this->palette().base());

			QStackedWidget::paintEvent(event);
		}
	}

	void
	LoginWindow::shakeEvent()
	{
		positionAnimation_ = std::make_unique<QPropertyAnimation>(this, "pos");
		positionAnimation_->setDuration(200);
		positionAnimation_->setLoopCount(1);
		positionAnimation_->setKeyValueAt(0, QPoint(geometry().x() - 3, geometry().y() - 3));
		positionAnimation_->setKeyValueAt(0.1, QPoint(geometry().x() + 4, geometry().y() + 4));
		positionAnimation_->setKeyValueAt(0.2, QPoint(geometry().x() - 4, geometry().y() + 4));
		positionAnimation_->setKeyValueAt(0.3, QPoint(geometry().x() + 4, geometry().y() - 4));
		positionAnimation_->setKeyValueAt(0.4, QPoint(geometry().x() - 4, geometry().y() - 4));
		positionAnimation_->setKeyValueAt(0.5, QPoint(geometry().x() + 4, geometry().y() + 4));
		positionAnimation_->setKeyValueAt(0.6, QPoint(geometry().x() - 4, geometry().y() + 4));
		positionAnimation_->setKeyValueAt(0.7, QPoint(geometry().x() + 4, geometry().y() - 4));
		positionAnimation_->setKeyValueAt(0.8, QPoint(geometry().x() - 4, geometry().y() - 4));
		positionAnimation_->setKeyValueAt(0.9, QPoint(geometry().x() + 4, geometry().y() + 4));
		positionAnimation_->setKeyValueAt(1, QPoint(geometry().x() - 3, geometry().y() - 3));
		positionAnimation_->start();
		QApplication::beep();
	}
}