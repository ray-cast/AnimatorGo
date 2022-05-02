#include "setting_window.h"
#include <qlabel.h>
#include <qscrollbar.h>
#include <qmessagebox.h>
#include "../controllers/client_component.h"

namespace flower
{
	class SpinBox final : public QSpinBox
	{
	public:
		void focusInEvent(QFocusEvent* event) override
		{
			this->grabKeyboard();
			QSpinBox::focusInEvent(event);
		}

		void focusOutEvent(QFocusEvent* event) override
		{
			this->releaseKeyboard();
			QSpinBox::focusOutEvent(event);
		}
	};

	class DoubleSpinBox final : public QDoubleSpinBox
	{
	public:
		void focusInEvent(QFocusEvent* event) override
		{
			this->grabKeyboard();
			QDoubleSpinBox::focusInEvent(event);
		}

		void focusOutEvent(QFocusEvent* event) override
		{
			this->releaseKeyboard();
			QDoubleSpinBox::focusOutEvent(event);
		}
	};

	SettingMainPlane::SettingMainPlane(QWidget* parent, const std::shared_ptr<flower::FlowerBehaviour>& behaviour)
		: QWidget(parent)
	{
		infoLabel = std::make_unique<QLabel>(this);
		infoLabel->setText(u8"版本信息");
		infoLabel->setStyleSheet("color: rgb(255,255,255);");

		infoButton = std::make_unique<QToolButton>(this);
		infoButton->setText(u8"检查更新");
		infoButton->setStyleSheet("background: rgb(50,50,50); border:2px solid #646464;border-radius:4px;color: rgb(200,200,200);");
		infoButton->setFixedSize(190, 35);

		versionLabel = std::make_unique<QLabel>(this);
		versionLabel->setText((u8"当前版本：v" + behaviour->getProfile()->clientModule->version).c_str());
		versionLabel->setStyleSheet("color: rgb(200,200,200);");

		/*QLabel* startupLabel = new QLabel(this);
		startupLabel->setText(u8"启动项");
		startupLabel->setStyleSheet("color: rgb(255,255,255);");

		QCheckBox* checkBox = new QCheckBox(this);
		checkBox->setText(u8"开机自动启动");
		checkBox->setStyleSheet("color: rgb(200,200,200);");

		QLabel* langLabel = new QLabel(this);
		langLabel->setText(u8"语言");
		langLabel->setStyleSheet("color: rgb(255,255,255);");

		QComboBox* gamCombo = new QComboBox(this);
		gamCombo->addItem(u8"中文");
		gamCombo->setStyleSheet("color: rgb(200,200,200);");
		gamCombo->setFont(QFont("Microsoft YaHei", 9, 50));

		QLabel* powerLabel = new QLabel(this);
		powerLabel->setText(u8"性能");
		powerLabel->setStyleSheet("color: rgb(255,255,255);");

		QCheckBox* lowpowerBox = new QCheckBox(this);
		lowpowerBox->setText(u8"低功耗模式");
		lowpowerBox->setStyleSheet("color: rgb(200,200,200);");*/

		resetLabel = std::make_unique<QLabel>(this);
		resetLabel->setText(u8"恢复默认设置");
		resetLabel->setStyleSheet("color: rgb(255,255,255);");

		resetButton = std::make_unique<QToolButton>(this);
		resetButton->setText(u8"立即恢复");
		resetButton->setStyleSheet("background: rgb(50,50,50); border:2px solid #646464; border-radius:4px;color: rgb(200,200,200);");
		resetButton->setFixedSize(190, 35);

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addWidget(infoLabel.get());
		layout_->addWidget(infoButton.get());
		layout_->addWidget(versionLabel.get());
		/*layout_->addSpacing(10);
		layout_->addWidget(startupLabel);
		layout_->addSpacing(10);
		layout_->addWidget(checkBox);
		layout_->addSpacing(10);
		layout_->addWidget(langLabel);
		layout_->addSpacing(10);
		layout_->addWidget(gamCombo);
		layout_->addSpacing(10);
		layout_->addWidget(powerLabel);
		layout_->addSpacing(10);
		layout_->addWidget(lowpowerBox);*/
		layout_->addSpacing(10);
		layout_->addWidget(resetLabel.get());
		layout_->addSpacing(10);
		layout_->addWidget(resetButton.get());
		layout_->setContentsMargins(0, 0, 0, 10);
	}

	SettingMainPlane2::SettingMainPlane2(QWidget* parent)
		: QWidget(parent)
	{
		renderLabel = std::make_unique<QLabel>();
		renderLabel->setText(u8"渲染设置");
		renderLabel->setStyleSheet("color: rgb(255,255,255);");

		resolutionLabel = std::make_unique<QLabel>();
		resolutionLabel->setText(u8"分辨率");
		resolutionLabel->setStyleSheet("color: rgb(200,200,200);");

		resolutionCombo = std::make_unique<QComboBox>();
		resolutionCombo->addItem(u8"720*480");
		resolutionCombo->addItem(u8"800*480");
		resolutionCombo->addItem(u8"1024*576");
		resolutionCombo->addItem(u8"1280x720");
		resolutionCombo->addItem(u8"1920x1080");
		resolutionCombo->addItem(u8"540x960");
		resolutionCombo->addItem(u8"720x1280");
		resolutionCombo->addItem(u8"1080x1920");
		resolutionCombo->setStyleSheet("color: rgb(200,200,200);");
		resolutionCombo->setFont(QFont("Microsoft YaHei", 9, 50));

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addWidget(renderLabel.get());
		layout_->addSpacing(10);
		layout_->addWidget(resolutionLabel.get());
		layout_->addSpacing(10);
		layout_->addWidget(resolutionCombo.get());
		layout_->addSpacing(200);
	}

	SettingMainPlane3::SettingMainPlane3(QWidget* parent)
		: QWidget(parent)
	{
	}

	SettingContextPlane::SettingContextPlane(QWidget* parent, const std::shared_ptr<flower::FlowerBehaviour>& behaviour) noexcept
		: QWidget(parent)
		, behaviour_(behaviour)
	{
		this->setObjectName("settingContext");

		QStringList strList{ tr(u8"主面板") };

		listWidget_ = std::make_unique<QListWidget>(this);
		listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		listWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		listWidget_->verticalScrollBar()->setFixedWidth(8);

		for (std::size_t i = 0; i < strList.size(); i++)
		{
			listWidgetItems_[i] = std::make_unique<QListWidgetItem>();
			listWidgetItems_[i]->setText(strList[(int)i]);
			listWidgetItems_[i]->setSizeHint(QSize(180, 30));
			listWidgetItems_[i]->setTextAlignment(Qt::AlignCenter);

			listWidget_->addItem(listWidgetItems_[i].get());
		}

		listWidgetItems_[0]->setSelected(true);

		scrollWidget_ = std::make_unique<QWidget>(this);
		scrollWidget_->setFixedWidth(490);
		scrollWidget_->setStyleSheet("background-color: rgb(40,40,40);");

		mainPlane_ = std::make_unique<SettingMainPlane>(scrollWidget_.get(), behaviour);
		mainPlane2_ = std::make_unique<SettingMainPlane2>(scrollWidget_.get());
		mainPlane3_ = std::make_unique<SettingMainPlane3>(scrollWidget_.get());

		gridLayout_ = std::make_unique<QVBoxLayout>(scrollWidget_.get());
		gridLayout_->addWidget(mainPlane_.get());
		gridLayout_->addWidget(mainPlane2_.get());
		gridLayout_->addWidget(mainPlane3_.get());

		scrollArea_ = std::make_unique<QScrollArea>(this);
		scrollArea_->setWidget(scrollWidget_.get());
		scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea_->verticalScrollBar()->setObjectName("settingScrollBar");

		layout_ = std::make_unique<QHBoxLayout>(this);
		layout_->addWidget(listWidget_.get());
		layout_->addWidget(scrollArea_.get());
		layout_->setSpacing(0);
		layout_->setContentsMargins(0, 0, 0, 0);

		auto& profile = behaviour->getProfile();
		if (profile->recordModule->width == 720 && profile->recordModule->height == 480)
			mainPlane2_->resolutionCombo->setCurrentIndex(0);
		else if (profile->recordModule->width == 800 && profile->recordModule->height == 480)
			mainPlane2_->resolutionCombo->setCurrentIndex(1);
		else if (profile->recordModule->width == 1024 && profile->recordModule->height == 576)
			mainPlane2_->resolutionCombo->setCurrentIndex(2);
		else if (profile->recordModule->width == 1280 && profile->recordModule->height == 720)
			mainPlane2_->resolutionCombo->setCurrentIndex(3);
		else if (profile->recordModule->width == 1920 && profile->recordModule->height == 1080)
			mainPlane2_->resolutionCombo->setCurrentIndex(4);
		else if (profile->recordModule->width == 540 && profile->recordModule->height == 960)
			mainPlane2_->resolutionCombo->setCurrentIndex(5);
		else if (profile->recordModule->width == 720 && profile->recordModule->height == 1280)
			mainPlane2_->resolutionCombo->setCurrentIndex(6);
		else if (profile->recordModule->width == 1080 && profile->recordModule->height == 1920)
			mainPlane2_->resolutionCombo->setCurrentIndex(7);

		connect(scrollArea_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
		connect(listWidget_.get(), SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		connect(mainPlane_->resetButton.get(), SIGNAL(clicked()), this, SLOT(onResetButton()));
		connect(mainPlane_->infoButton.get(), SIGNAL(clicked()), this, SLOT(onCheckVersion()));
		connect(mainPlane2_->resolutionCombo.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(onResolutionCombo(int)));
	}

	SettingContextPlane::~SettingContextPlane()
	{
		mainPlane_.reset();
		mainPlane2_.reset();
		mainPlane3_.reset();
		gridLayout_.reset();
		scrollWidget_.reset();
		scrollArea_.reset();
		layout_.reset();
	}

	void
	SettingContextPlane::valueChanged(int value)
	{
		if (scrollArea_->widget()->layout()->count() != listWidget_->count())
			return;

		if (!m_sign)
		{
			for (int i = 0; i < listWidget_->count(); i++)
			{
				auto widget = scrollArea_->widget()->layout()->itemAt(i)->widget();
				if (!widget->visibleRegion().isEmpty())
				{
					listWidget_->item(i)->setSelected(true);
					return;
				}
				else
				{
					listWidget_->item(i)->setSelected(false);
				}
			}
		}

		m_sign = false;
	}

	void
	SettingContextPlane::itemClicked(QListWidgetItem* item)
	{
		if (scrollArea_->widget()->layout()->count() != listWidget_->count())
			return;

		m_sign = true;

		for (int i = 0; i < listWidget_->count(); i++)
		{
			if (item == listWidget_->item(i))
			{
				auto widget = scrollArea_->widget()->layout()->itemAt(i)->widget();
				scrollArea_->verticalScrollBar()->setSliderPosition(widget->pos().y());
			}
		}
	}

	void
	SettingContextPlane::onResetButton()
	{
		mainPlane2_->resolutionCombo->setCurrentIndex(3);
	}

	void
	SettingContextPlane::onResolutionCombo(int index)
	{
		auto& profile = behaviour_->getProfile();
		switch (mainPlane2_->resolutionCombo->currentIndex())
		{
		case 0:
		{
			profile->recordModule->width = 720;
			profile->recordModule->height = 480;
		}
		break;
		case 1:
		{
			profile->recordModule->width = 800;
			profile->recordModule->height = 480;
		}
		break;
		case 2:
		{
			profile->recordModule->width = 1024;
			profile->recordModule->height = 576;
		}
		break;
		case 3:
		{
			profile->recordModule->width = 1280;
			profile->recordModule->height = 720;
		}
		break;
		case 4:
		{
			profile->recordModule->width = 1920;
			profile->recordModule->height = 1080;
		}
		break;
		case 5:
		{
			profile->recordModule->width = 540;
			profile->recordModule->height = 960;
		}
		break;
		case 6:
		{
			profile->recordModule->width = 720;
			profile->recordModule->height = 1280;
		}
		break;
		case 7:
		{
			profile->recordModule->width = 1080;
			profile->recordModule->height = 1920;
		}
		break;
		}
	}

	void
	SettingContextPlane::onCheckVersion()
	{
		//auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
		//auto version = client->version();
		//if (version == behaviour_->getProfile()->clientModule->version)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(QString::fromUtf8(u8"提示"));
			msg.setText(u8"您当前使用的花楹渲染器已是最新版，无需更新！");
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	SettingWindow::SettingWindow(const std::shared_ptr<flower::FlowerBehaviour>& behaviour) noexcept
		: settingTitleWindow_(std::make_unique<TitleBar>(this))
		, settingContextPlane_(std::make_unique<SettingContextPlane>(this, behaviour))
	{
		this->setObjectName("settingWidget");
		this->setWindowFlags(Qt::FramelessWindowHint);
		this->setWindowModality(Qt::ApplicationModal);
		this->setMouseTracking(true);

		contextLayout_ = std::make_unique<QVBoxLayout>(this);
		contextLayout_->addWidget(settingTitleWindow_.get());
		contextLayout_->addWidget(settingContextPlane_.get());
		contextLayout_->setMargin(0);
		contextLayout_->setSpacing(0);

		closeAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
		closeAnimation_->setDuration(100);
		closeAnimation_->setEasingCurve(QEasingCurve::InCubic);
		closeAnimation_->setStartValue(1);
		closeAnimation_->setEndValue(0);

		this->setFixedSize(650, 450);
		this->connect(closeAnimation_.get(), SIGNAL(finished()), this, SLOT(close()));
		this->connect(settingTitleWindow_.get(), &TitleBar::closeSignal, this, &SettingWindow::closeEvent);
	}

	SettingWindow::~SettingWindow() noexcept
	{
		settingContextPlane_.reset();
		settingTitleWindow_.reset();
	}

	void
	SettingWindow::closeEvent()
	{
		closeAnimation_->start();
	}
}