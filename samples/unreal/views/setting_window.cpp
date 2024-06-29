#include "setting_window.h"
#include "../unreal_version.h"
#include "../controllers/client_component.h"
#include <qlabel.h>
#include <qmessagebox.h>
#include <qscrollbar.h>
#include <QPushButton>
#include <spdlog/spdlog.h>

#include "../widgets/uspinbox.h"
#include "../widgets/udoublespinbox.h"

namespace unreal
{
	SettingMainPlaneGeneral::SettingMainPlaneGeneral(QWidget* parent, const std::shared_ptr<unreal::UnrealBehaviour>& behaviour)
		: UPanel(parent)
	{
		setupUI();
	}

	void
	SettingMainPlaneGeneral::retranslate()
	{
		infoLabel->setText(tr("Version"));
		infoButton->setText(tr("Check Updates"));
		versionLabel->setText(tr("Current Version: ") + QString::fromStdString(UNREAL_VERSION));
	}

	void
	SettingMainPlaneGeneral::setupUI()
	{
		infoLabel = new QLabel(this);
		infoLabel->setText(tr("Version"));

		infoButton = new QPushButton(this);
		infoButton->setText(tr("Check Updates"));
		infoButton->setFixedSize(152, 28);

		versionLabel = new QLabel(this);
		versionLabel->setText(tr("Current Version: ") + QString::fromStdString(UNREAL_VERSION));

		layout_ = new QVBoxLayout(this);
		layout_->addWidget(infoLabel);
		layout_->addWidget(infoButton);
		layout_->addWidget(versionLabel);
		layout_->setContentsMargins(0, 0, 0, 0);
	}

	SettingMainPlaneInterface::SettingMainPlaneInterface(QWidget* parent)
		: UPanel(parent)
	{
		setupUI();
		installEventFilter(this);
	}
	
	void
	SettingMainPlaneInterface::setupUI()
	{
		languages_ = std::vector<QString>{
			tr("Chinese (Simplified)"),
			tr("English (United State)"),
			tr("Japanese (Japan)")
		};
		langLabel_ = new QLabel(this);
		langLabel_->setText(tr("Language"));

		langCombo_ = new QComboBox(this);
		for (auto item : languages_)
			langCombo_->addItem(item);

		layout_ = new QVBoxLayout(this);
		layout_->addSpacing(8);
		layout_->addWidget(langLabel_);
		layout_->addSpacing(8);
		layout_->addWidget(langCombo_);
		layout_->addSpacing(160);
	}

	void
	SettingMainPlaneInterface::retranslate()
	{
		langLabel_->setText(tr("Language"));
		languages_ = std::vector<QString>{
			tr("Chinese (Simplified)"),
			tr("English (United State)"),
			tr("Japanese (Japan)")
		};
		for (int i = 0; i < languages_.size(); ++i)
			langCombo_->setItemText(i, languages_[i]);
	}

	SettingContextPlane::SettingContextPlane(QWidget* parent, const std::shared_ptr<unreal::UnrealBehaviour>& behaviour) noexcept(false)
		: UPanel(parent)
		, behaviour_(behaviour)
	{
		this->setObjectName("settingContext");

		QStringList strList{ tr("General") };

		listWidget_ = new QListWidget(this);
		listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		listWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		listWidget_->verticalScrollBar()->setFixedWidth(6);

		for (qsizetype i = 0; i < strList.size(); i++)
		{
			listWidgetItems_[i] = new QListWidgetItem();
			listWidgetItems_[i]->setText(strList[i]);
			listWidgetItems_[i]->setSizeHint(QSize(144, 24));
			listWidgetItems_[i]->setTextAlignment(Qt::AlignCenter);

			listWidget_->addItem(listWidgetItems_[i]);
		}

		listWidgetItems_[0]->setSelected(true);

		scrollWidget_ = new QWidget(this);
		scrollWidget_->setFixedWidth(392);

		mainPlaneGeneral_ = new SettingMainPlaneGeneral(scrollWidget_, behaviour);
		mainPlaneInterface_ = new SettingMainPlaneInterface(scrollWidget_);

		gridLayout_ = new QVBoxLayout(scrollWidget_);
		gridLayout_->addWidget(mainPlaneGeneral_);
		gridLayout_->addWidget(mainPlaneInterface_);

		scrollArea_ = new QScrollArea(this);
		scrollArea_->setWidget(scrollWidget_);
		scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea_->verticalScrollBar()->setObjectName("settingScrollBar");

		layout_ = new QHBoxLayout(this);
		layout_->addWidget(listWidget_);
		layout_->addWidget(scrollArea_);
		layout_->setSpacing(0);
		layout_->setContentsMargins(0, 0, 0, 0);

		connect(scrollArea_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
		connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		connect(mainPlaneGeneral_->infoButton, SIGNAL(clicked()), this, SLOT(onCheckVersion()));
		connect(mainPlaneInterface_->langCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLangCombo(int)));

		this->installEventFilter(this);
	}

	SettingContextPlane::~SettingContextPlane()
	{
	}

	void
	SettingContextPlane::retranslate()
	{
		QStringList strList{ tr("General"), tr("Interface"), tr("Graphics") };
		for (int i = 0; i < strList.size(); i++)
		{
			listWidgetItems_[i]->setText(strList[i]);
		}
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
	SettingContextPlane::onCheckVersion()
	{
		// auto client = dynamic_cast<ClientComponent*>(behaviour_->getComponent<ClientComponent>());
		// auto version = client->version();
		// if (version == behaviour_->getProfile()->clientModule->version)
		{
			QMessageBox::information(this, tr("Information"), tr("You are using the latest version of the renderer, no need to update!"));
		}
	}

	void
	SettingContextPlane::onLangCombo(int index)
	{
		if (index == -1)
			return;
		QString filename;
		if (index == 0)
			filename = "zh_CN.qm";
		else if (index == 1)
			filename = "en_US.qm";
		else if (index == 2)
			filename = "ja_JP.qm";
		else
			throw std::runtime_error("SettingContextPlane::onLangCombo: language not found");
		emit languageChangeSignal(filename);
	}

	SettingWindow::SettingWindow(const std::shared_ptr<unreal::UnrealBehaviour>& behaviour) noexcept
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
		contextLayout_->setContentsMargins(0, 0, 0, 0);
		contextLayout_->setSpacing(0);

		closeAnimation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
		closeAnimation_->setDuration(100);
		closeAnimation_->setEasingCurve(QEasingCurve::InCubic);
		closeAnimation_->setStartValue(1);
		closeAnimation_->setEndValue(0);

		this->setFixedSize(520, 360);
		this->connect(closeAnimation_.get(), SIGNAL(finished()), this, SLOT(close()));
		this->connect(settingTitleWindow_.get(), &TitleBar::closeSignal, this, &SettingWindow::closeEvent);

		this->connect(settingContextPlane_.get(), SIGNAL(languageChangeSignal(QString)), this, SIGNAL(languageChangeSignal(QString)));
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