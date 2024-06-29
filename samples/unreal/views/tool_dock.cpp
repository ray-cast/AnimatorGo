#include "tool_dock.h"
#include "spdlog/spdlog.h"

namespace unreal
{
	ToolDock::ToolDock(const octoon::GameAppPtr& gameApp, const octoon::GameObjectPtr& behaviour, std::shared_ptr<UnrealProfile> profile) noexcept
		: profile_(profile)
		, behaviour_(behaviour)
	{
		this->setObjectName("ToolDock");
		this->setFeatures(DockWidgetFeature::NoDockWidgetFeatures);

		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		videoButton_ = new QToolButton;
		videoButton_->setObjectName("record");
		videoButton_->setText(tr("Video"));
		videoButton_->setToolTip(tr("Open Video Panel"));
		videoButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		videoButton_->setCheckable(true);

		sunButton_ = new QToolButton;
		sunButton_->setObjectName("sun");
		sunButton_->setText(tr("Main Light"));
		sunButton_->setToolTip(tr("Open Main Light Panel"));
		sunButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		sunButton_->setCheckable(true);

		environmentButton_ = new QToolButton;
		environmentButton_->setObjectName("environment");
		environmentButton_->setText(tr("Environment Light"));
		environmentButton_->setToolTip(tr("Open Environment Light Panel"));
		environmentButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		environmentButton_->setCheckable(true);
		environmentButton_->click();
		
		cameraButton_ = new QToolButton;
		cameraButton_->setObjectName("camera");
		cameraButton_->setText(tr("Camera"));
		cameraButton_->setToolTip(tr("Open Camera Panel"));
		cameraButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		cameraButton_->setCheckable(true);

		inspectorButton_ = new QToolButton;
		inspectorButton_->setObjectName("eye");
		inspectorButton_->setText(tr("Inspector"));
		inspectorButton_->setToolTip(tr("Open Inspector Panel"));
		inspectorButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		inspectorButton_->setCheckable(true);
		
		auto buttonGroup_ = new QButtonGroup();
		buttonGroup_->addButton(sunButton_, 0);
		buttonGroup_->addButton(environmentButton_, 1);
		buttonGroup_->addButton(cameraButton_, 2);
		buttonGroup_->addButton(videoButton_, 3);
		buttonGroup_->addButton(inspectorButton_, 3);

		auto layout = new QVBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(sunButton_, 0, Qt::AlignCenter);
		layout->addWidget(environmentButton_, 0, Qt::AlignCenter);
		layout->addWidget(cameraButton_, 0, Qt::AlignCenter);
		layout->addWidget(videoButton_, 0, Qt::AlignCenter);
		layout->addWidget(inspectorButton_, 0, Qt::AlignCenter);
		layout->addStretch();
		layout->setSpacing(0);
		layout->setContentsMargins(0, 0, 0, 0);
		
		auto mainWidget = new QWidget;
		mainWidget->setObjectName("ToolWidget");
		mainWidget->setLayout(layout);

		this->setWidget(mainWidget);

		this->connect(videoButton_, SIGNAL(clicked()), this, SLOT(recordEvent()));
		this->connect(sunButton_, SIGNAL(clicked()), this, SLOT(sunEvent()));
		this->connect(environmentButton_, SIGNAL(clicked()), this, SLOT(environmentEvent()));
		this->connect(cameraButton_, SIGNAL(clicked()), this, SLOT(cameraEvent()));
		this->connect(inspectorButton_, SIGNAL(clicked()), this, SLOT(inspectorEvent()));

		spdlog::debug("create tool dock");
	}

	ToolDock::~ToolDock() noexcept
	{
	}

	void
	ToolDock::recordEvent() noexcept
	{
		emit recordSignal();
	}

	void
	ToolDock::sunEvent() noexcept
	{
		emit sunSignal();
	}

	void
	ToolDock::environmentEvent() noexcept
	{
		emit environmentSignal();
	}

	void
	ToolDock::cameraEvent() noexcept
	{
		emit cameraSignal();
	}

	void
	ToolDock::inspectorEvent() noexcept
	{
		emit inspectorSignal();
	}
}