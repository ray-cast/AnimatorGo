#include "asset_browse_dock.h"
#include "setting_window.h"
#include "spdlog/spdlog.h"
#include <qscrollarea.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qmimedata.h>
#include <qprogressdialog.h>
#include <QtConcurrent/qtconcurrentrun.h>

namespace unreal
{
	AssetBrowseDock::AssetBrowseDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept
		: profile_(profile)
		, behaviour_(behaviour)
	{
		this->setObjectName("AssetBrowseDock");
		this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		this->setFeatures(DockWidgetFeature::NoDockWidgetFeatures);

		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		materialButton_ = new QToolButton;
		materialButton_->setObjectName("material");
		materialButton_->setText(tr("Material Library"));
		materialButton_->setToolTip(tr("Open Material Panel"));
		materialButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		materialButton_->setCheckable(true);
		materialButton_->click();
		
		textureButton_ = new QToolButton;
		textureButton_->setObjectName("images");
		textureButton_->setText(tr("Texture Library"));
		textureButton_->setToolTip(tr("Open Texture Panel"));
		textureButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		textureButton_->setCheckable(true);
		textureButton_->click();

		modelButton_ = new QToolButton;
		modelButton_->setObjectName("model");
		modelButton_->setText(tr("Model Library"));
		modelButton_->setToolTip(tr("Open Model Panel"));
		modelButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		modelButton_->setCheckable(true);

		motionButton_ = new QToolButton;
		motionButton_->setObjectName("motion");
		motionButton_->setText(tr("Motion Library"));
		motionButton_->setToolTip(tr("Open Motion Panel"));
		motionButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		motionButton_->setCheckable(true);

		lightButton_ = new QToolButton;
		lightButton_->setObjectName("sun");
		lightButton_->setText(tr("Light"));
		lightButton_->setToolTip(tr("Open Light Panel"));
		lightButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		lightButton_->hide();
		
		settingsButton_ = new QToolButton;
		settingsButton_->setObjectName("setting");
		settingsButton_->setText(tr("Settings"));
		settingsButton_->setToolTip(tr("Settings"));
		settingsButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		importButton_ = new QToolButton;
		importButton_->setObjectName("import");
		importButton_->setText(tr("Import Library"));
		importButton_->setToolTip(tr("Import Resource File(.pmm, .pmx)"));
		importButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		importButton_->installEventFilter(this);

		buttonGroup_ = new QButtonGroup();
		buttonGroup_->addButton(materialButton_, 0);
		buttonGroup_->addButton(textureButton_, 1);
		buttonGroup_->addButton(modelButton_, 2);
		buttonGroup_->addButton(motionButton_, 3);
		buttonGroup_->addButton(lightButton_, 4);

		auto mainLayout = new QVBoxLayout;
		mainLayout->addWidget(materialButton_, 0, Qt::AlignCenter);
		mainLayout->addWidget(textureButton_, 0, Qt::AlignCenter);
		mainLayout->addWidget(modelButton_, 0, Qt::AlignCenter);
		mainLayout->addWidget(motionButton_, 0, Qt::AlignCenter);
		mainLayout->addWidget(lightButton_, 0, Qt::AlignCenter);
		mainLayout->addWidget(importButton_, 0, Qt::AlignCenter);
		mainLayout->addStretch();
		mainLayout->addWidget(settingsButton_, 0, Qt::AlignCenter | Qt::AlignBottom);
		mainLayout->setSpacing(0);
		mainLayout->setContentsMargins(0, 0, 0, 0);

		auto mainWidget = new QWidget;
		mainWidget->setObjectName("AssetBrowseWidget");
		mainWidget->setLayout(mainLayout);

		this->setWidget(mainWidget);

		this->connect(materialButton_, SIGNAL(clicked()), this, SLOT(materialEvent()));
		this->connect(textureButton_, SIGNAL(clicked()), this, SLOT(textureEvent()));
		this->connect(modelButton_, SIGNAL(clicked()), this, SLOT(modelEvent()));
		this->connect(motionButton_, SIGNAL(clicked()), this, SLOT(motionEvent()));
		this->connect(lightButton_, SIGNAL(clicked()), this, SLOT(lightEvent()));
		this->connect(importButton_, SIGNAL(clicked()), this, SLOT(importEvent()));
		this->connect(settingsButton_, SIGNAL(clicked()), this, SLOT(settingsEvent()));
	}

	AssetBrowseDock::~AssetBrowseDock() noexcept
	{
	}
	
	void
	AssetBrowseDock::materialEvent() noexcept
	{
		emit materialSignal();
	}

	void
	AssetBrowseDock::textureEvent() noexcept
	{
		emit textureSignal();
	}

	void
	AssetBrowseDock::modelEvent() noexcept
	{
		emit modelSignal();
	}

	void
	AssetBrowseDock::motionEvent() noexcept
	{
		emit motionSignal();
	}

	void
	AssetBrowseDock::lightEvent() noexcept
	{
		emit lightSignal();
	}

	void
	AssetBrowseDock::importEvent() noexcept
	{
		emit importSignal();
	}

	void
	AssetBrowseDock::closeEvent(QCloseEvent* event)
	{
		if (profile_->playerModule->isPlaying)
			event->ignore();
		else
			event->accept();
	}

	void
	AssetBrowseDock::settingsEvent() noexcept
	{
		SettingWindow* settings_dialog_ = new SettingWindow(this->behaviour_->getComponent<UnrealBehaviour>());
		connect(settings_dialog_, SIGNAL(languageChangeSignal(QString)), this, SIGNAL(languageChangeSignal(QString)));

		settings_dialog_->show();
	}
}