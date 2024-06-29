#include "info_window.h"
#include "../libs/qrcodegen/qrcodegen.h"
#include <qpainter.h>
#include <qmessagebox.h>

namespace unreal
{
	InfoWindow::InfoWindow(QWidget* widget, const std::shared_ptr<unreal::UnrealBehaviour>& behaviour) noexcept
		: parent_(widget)
		, behaviour_(behaviour)
		, allowMove_(false)
		, timer_(std::make_unique<QTimer>(this))
		, tradeTimer_(std::make_unique<QTimer>(this))
		, tradeType_(0)
	{
		this->setObjectName("infoWindow");
		this->setWindowFlags(Qt::FramelessWindowHint);
		this->setMouseTracking(true);
		this->setWindowModality(Qt::ApplicationModal);
		this->installEventFilter(this);

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(tr("Close"));

		logo_ = std::make_unique<QPushButton>();
		logo_->setObjectName("logo");

		loginOutButton_ = std::make_unique<QToolButton>();
		loginOutButton_->setObjectName("loginOut");
		loginOutButton_->setText(tr("logout"));

		username_ = std::make_unique<QPushButton>();
		username_->setObjectName("user");
		//username_->setText(behaviour->getProfile()->clientModule->username.c_str());

		countLabel_ = std::make_unique<QPushButton>();
		countLabel_->setObjectName("count");
		//countLabel_->setText(tr("balance") + QString::fromStdString(std::to_string(behaviour->getProfile()->clientModule->coin)) + tr("$"));

		vipLabel_ = std::make_unique<QLabel>();
		vipLabel_->setObjectName("vip");
		vipLabel_->setText(tr("VIP Privilege"));
		
		selectButton1_ = std::make_unique<QToolButton>();
		selectButton1_->setObjectName("select1");
		selectButton1_->setText(tr("5000 Coin\n$5 (Original cost $10)"));
		selectButton1_->setMinimumSize(200, 90);
		selectButton1_->setCheckable(true);

		selectButton2_ = std::make_unique<QToolButton>();
		selectButton2_->setObjectName("select2");
		selectButton2_->setText(tr("10000 Coin\n$10 (Original cost $20)"));
		selectButton2_->setMinimumSize(200, 90);
		selectButton2_->setCheckable(true);

		selectButton3_ = std::make_unique<QToolButton>();
		selectButton3_->setObjectName("select3");
		selectButton3_->setText(tr("20000 Coin\n$20 (Original cost $40)"));
		selectButton3_->setMinimumSize(200, 90);
		selectButton3_->setCheckable(true);

		payLabel_ = std::make_unique<QLabel>();
		payLabel_->setObjectName("vip");
		payLabel_->setText(tr("Scan qrcode in Alipay to pay"));

		qrcodeButton_ = std::make_unique<QPushButton>();
		qrcodeButton_->setObjectName("qrcode");
		qrcodeButton_->setContentsMargins(10, 10, 10, 10);

		vipButton_ = std::make_unique<QPushButton>();
		vipButton_->setObjectName("vipLicense");
		vipButton_->setText(tr("Flowers End User Agreement"));

		licenseButton_ = std::make_unique<QPushButton>();
		licenseButton_->setObjectName("license");
		licenseButton_->setText(tr("Flowers End User Agreement"));

		closeAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
		closeAnimation_->setDuration(100);
		closeAnimation_->setEasingCurve(QEasingCurve::InCubic);
		closeAnimation_->setStartValue(1);
		closeAnimation_->setEndValue(0);

		titleLayout_ = std::make_unique<QHBoxLayout>();
		titleLayout_->addWidget(logo_.get(), 10, Qt::AlignLeft);
		titleLayout_->addWidget(username_.get(), 0, Qt::AlignLeft);
		titleLayout_->addWidget(closeButton_.get(), 500, Qt::AlignRight);
		titleLayout_->setContentsMargins(0, 10, 10, 10);

		selectLayout_ = std::make_unique<QHBoxLayout>();
		selectLayout_->addWidget(selectButton1_.get(), 0, Qt::AlignVCenter | Qt::AlignTop);
		selectLayout_->addWidget(selectButton2_.get(), 0, Qt::AlignVCenter | Qt::AlignTop);
		selectLayout_->addWidget(selectButton3_.get(), 0, Qt::AlignVCenter | Qt::AlignTop);

		lisenseLayout_ = std::make_unique<QHBoxLayout>();
		lisenseLayout_->addWidget(vipButton_.get(), 0, Qt::AlignRight | Qt::AlignBottom);
		lisenseLayout_->addWidget(licenseButton_.get(), 0, Qt::AlignLeft | Qt::AlignBottom);
		lisenseLayout_->setContentsMargins(20, 10, 10, 10);

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addLayout(titleLayout_.get());
		layout_->addWidget(countLabel_.get(), 0, Qt::AlignLeft);
		layout_->addWidget(vipLabel_.get(), 0, Qt::AlignLeft | Qt::AlignTop);
		layout_->addLayout(selectLayout_.get());
		layout_->addWidget(payLabel_.get(), 0, Qt::AlignLeft | Qt::AlignBottom);
		layout_->addWidget(qrcodeButton_.get(), 0, Qt::AlignCenter);
		layout_->addLayout(lisenseLayout_.get());
		layout_->addWidget(loginOutButton_.get(), 0, Qt::AlignRight);
		layout_->setContentsMargins(20, 0, 10, 10);

		this->connect(timer_.get(), SIGNAL(timeout()), this, SLOT(timeEvent()));
		this->connect(tradeTimer_.get(), SIGNAL(timeout()), this, SLOT(tradeEvent()));		
		this->connect(closeAnimation_.get(), SIGNAL(finished()), this, SLOT(close()));
		this->connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		this->connect(loginOutButton_.get(), SIGNAL(clicked()), this, SLOT(loginOutEvent()));
		this->connect(selectButton1_.get(), SIGNAL(clicked()), this, SLOT(select1Event()));
		this->connect(selectButton2_.get(), SIGNAL(clicked()), this, SLOT(select2Event()));
		this->connect(selectButton3_.get(), SIGNAL(clicked()), this, SLOT(select3Event()));
		this->connect(countLabel_.get(), SIGNAL(clicked()), this, SLOT(refreshEvent()));
	}

	InfoWindow::~InfoWindow() noexcept
	{
		username_.reset();
		closeButton_.reset();
		titleLayout_.reset();
		selectButton1_.reset();
		selectButton2_.reset();
		selectButton3_.reset();
		selectLayout_.reset();
		vipButton_.reset();
		licenseButton_.reset();
		lisenseLayout_.reset();
	}

	void
	InfoWindow::closeEvent()
	{
		tradeTimer_->stop();
		closeAnimation_->start();
	}

	void
	InfoWindow::loginOutEvent()
	{
		try
		{
			auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
			if (client)
			{
				client->loginOut();
				emit loginOut();
				this->close();
			}
		}
		catch (const std::exception&)
		{
			emit loginOut();
			this->close();
		}
	}

	bool
	InfoWindow::eventFilter(QObject* watched, QEvent* event)
	{
		if (watched == this)
		{
			if (QEvent::WindowActivate == event->type())
			{
				if (parent_->isActiveWindow())
					parent_->activateWindow();
				return true;
			}
		}

		return QWidget::eventFilter(watched, event);
	}

	void
	InfoWindow::mousePressEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = true;
		startPos_ = e->globalPos();
		clickPos_ = e->pos();
	}

	void
	InfoWindow::mouseReleaseEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = false;
	}

	void
	InfoWindow::mouseMoveEvent(QMouseEvent* e) noexcept
	{
		if (allowMove_)
			this->move(e->globalPos() - clickPos_);
	}

	void 
	InfoWindow::showEvent(QShowEvent* event) noexcept
	{
		selectButton1_->click();
	}

	void
	InfoWindow::select1Event()
	{
		selectButton1_->setChecked(true);
		selectButton2_->setChecked(false);
		selectButton3_->setChecked(false);
		this->qrcodeClickEvent();
	}

	void
	InfoWindow::select2Event()
	{
		selectButton1_->setChecked(false);
		selectButton2_->setChecked(true);
		selectButton3_->setChecked(false);
		this->qrcodeClickEvent();		
	}

	void
	InfoWindow::select3Event()
	{
		selectButton1_->setChecked(false);
		selectButton2_->setChecked(false);
		selectButton3_->setChecked(true);
		this->qrcodeClickEvent();
	}

	void
	InfoWindow::refreshEvent()
	{
		try
		{
			auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
			if (client)
				client->refresh();

			//countLabel_->setText(tr("balance") + QString::fromStdString(std::to_string(behaviour_->getProfile()->clientModule->coin)) + tr("$"));
		}
		catch (const std::exception&)
		{

		}
	}

	void
	InfoWindow::qrcodeClickEvent()
	{
		if (selectButton1_->isChecked())
		{
			tradeType_ = 4;
			qrcodeButton_->setIcon(QIcon());
			if (!timer_->isActive())
				timer_->start();
		}
		else if (selectButton2_->isChecked())
		{
			tradeType_ = 5;
			qrcodeButton_->setIcon(QIcon());
			if (!timer_->isActive())
				timer_->start();
		}
		else if (selectButton3_->isChecked())
		{
			tradeType_ = 6;
			qrcodeButton_->setIcon(QIcon());
			if (!timer_->isActive())
				timer_->start();
		}		
	}

	void
	InfoWindow::timeEvent()
	{
		try
		{
			if (!future_.valid())
			{
				auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
				if (client)
					future_ = std::move(client->alipay(tradeType_));
			}

			auto status = future_.wait_for(std::chrono::milliseconds(10));
			if (status != std::future_status::ready)
				return;

			auto json = future_.get();
			auto release = std::move(future_);

			if (json.find("success") == json.end())
				return;

			if ((bool)json["success"])
			{
				std::string qr_code = json["qr_code"];
				trade_no_ = json["trade_no"].get<nlohmann::json::string_t>();

				std::uint8_t qr0[qrcodegen_BUFFER_LEN_MAX];
				std::uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
				bool ok = qrcodegen_encodeText(qr_code.c_str(), tempBuffer, qr0, qrcodegen_Ecc_MEDIUM, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
				if (ok)
				{
					int size = qrcodegen_getSize(qr0);
					auto labelsz = qrcodeButton_->size();
					auto width = labelsz.width() - 20;
					auto height = labelsz.height() - 20;
					auto pixmap = QImage(width, height, QImage::Format_RGB888);

					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int u = float(x) / width * size;
							int v = float(y) / height * size;

							QPen pen;
							if (qrcodegen_getModule(qr0, u, v))
								pixmap.setPixelColor(x, y, QColor(0, 0, 0));
							else
								pixmap.setPixelColor(x, y, QColor(255, 255, 255));
						}
					}

					qrcodeButton_->setIcon(QPixmap::fromImage(pixmap));
					qrcodeButton_->setIconSize(QSize(width, height));
				}

				tradeTimer_->setInterval(500);
				tradeTimer_->start();
				timer_->stop();
			}
		}
		catch (const std::exception&)
		{
		}
	}

	void
	InfoWindow::tradeEvent()
	{
		if (trade_no_.empty())
			return;

		try
		{
			auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
			if (client)
			{
				if (client->invoice(trade_no_))
				{
					QMessageBox msg(this);
					msg.setWindowTitle(tr("Info"));
					msg.setText(tr("Successful recharge"));
					msg.setIcon(QMessageBox::NoIcon);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();

					this->refreshEvent();
					tradeTimer_->stop();
				}
			}
		}
		catch (const std::exception&)
		{
		}
	}
}