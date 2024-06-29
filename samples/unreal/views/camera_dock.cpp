#include "camera_dock.h"
#include <octoon/io/fstream.h>
#include <octoon/asset_importer.h>
#include <octoon/asset_database.h>
#include <qapplication.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

namespace unreal
{
	CameraDock::CameraDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept
		: behaviour_(behaviour)
		, profile_(profile)
	{
		this->setObjectName("CameraDock");
		this->setWindowTitle(tr("Camera"));
		this->setFeatures(QDockWidget::NoDockWidgetFeatures);

		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		title_ = new QLabel;
		title_->setObjectName("title");
		title_->setText(tr("Camera Editor"));

		headerLine_ = new QFrame;
		headerLine_->setObjectName("HLine");
		headerLine_->setFixedHeight(1);
		headerLine_->setFrameShape(QFrame::NoFrame);
		headerLine_->setFrameShadow(QFrame::Plain);
		headerLine_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

		auto titleLayout_ = new QVBoxLayout();
		titleLayout_->addWidget(title_, 0, Qt::AlignLeft);
		titleLayout_->addSpacing(8);
		titleLayout_->addWidget(headerLine_);
		titleLayout_->setSpacing(0);
		titleLayout_->setContentsMargins(4, 0, 4, 0);

		fovSpinbox_ = UDoubleSpinLine::create(this, tr("fov:"), 1.0f, 120.0f, 1.0f, 60.0f);
		fovSpinbox_->doublespinbox_->setSuffix(u8"бу");
		fovSpinbox_->doublespinbox_->setDecimals(1);

		fovSlider = new QSlider(Qt::Horizontal);
		fovSlider->setMinimum(100.0f);
		fovSlider->setMaximum(12000.0f);
		fovSlider->setValue(100.f);
		fovSlider->setMinimumWidth(215);
		fovSlider->installEventFilter(this);

		dofInfoLabel_ = new QLabel();
		dofInfoLabel_->setText(tr("* The following parameters take effect on rendering"));
		dofInfoLabel_->setStyleSheet("color: rgb(100,100,100);");

		dofLabel_ = new QLabel();
		dofLabel_->setText(tr("Depth Of Filed:"));

		dofButton_ = new QCheckBox();
		dofButton_->setCheckState(Qt::CheckState::Unchecked);
		dofButton_->installEventFilter(this);

		auto dofLayout_ = new QHBoxLayout();
		dofLayout_->addWidget(dofLabel_, 0, Qt::AlignLeft);
		dofLayout_->addWidget(dofButton_, 0, Qt::AlignLeft);
		dofLayout_->setSpacing(0);
		dofLayout_->setContentsMargins(0, 0, 0, 0);

		apertureSpinbox_ = UDoubleSpinLine::create(this, tr("Aperture:"), 1.0f, 64.0f, 0.1f, 64.0f);
		apertureSpinbox_->doublespinbox_->setPrefix(tr("f/"));
		apertureSpinbox_->doublespinbox_->setDecimals(1);

		focalLengthSpinbox_ = UDoubleSpinLine::create(this, tr("Focal Length (35mmfilm):"), 1.0f, 1200.0f, 1.0f, 31.18f);
		focalLengthSpinbox_->doublespinbox_->setSuffix(tr("mm"));

		focusDistanceSpinbox_ = UDoubleSpinLine::create(this, tr("Focus Distance:"), 0.0f, std::numeric_limits<float>::infinity(), 0.1f, 0.0f);
		focusDistanceSpinbox_->doublespinbox_->setSuffix(tr("m"));
		
		focusDistanceName_ = new QLabel();
		focusDistanceName_->setText(tr("Target: Empty"));
		
		focusTargetButton_ = new DraggableButton();
		focusTargetButton_->setIcon(QIcon(":res/icons/target.png"));
		focusTargetButton_->setIconSize(QSize(38, 38));
		focusTargetButton_->setToolTip(tr("Drag and drop this icon onto the target model to enable autofocus"));
		focusTargetButton_->installEventFilter(this);

		loadButton_ = new QToolButton();
		loadButton_->setObjectName("Load");
		loadButton_->setText(tr("Load Animation"));
		loadButton_->setContentsMargins(0, 0, 0, 0);
		loadButton_->installEventFilter(this);

		unloadButton_ = new QToolButton();
		unloadButton_->setObjectName("Unload");
		unloadButton_->setText(tr("Uninstall"));
		unloadButton_->setContentsMargins(0, 0, 0, 0);
		unloadButton_->installEventFilter(this);

		auto focusTargetLayout = new QHBoxLayout;
		focusTargetLayout->addWidget(focusTargetButton_, 0, Qt::AlignLeft);
		focusTargetLayout->addWidget(focusDistanceName_);
		focusTargetLayout->addStretch();

		auto animtionLayout = new QHBoxLayout;
		animtionLayout->addStretch();
		animtionLayout->addWidget(loadButton_, 0, Qt::AlignRight);
		animtionLayout->addWidget(unloadButton_, 0, Qt::AlignLeft);
		animtionLayout->addStretch();
		animtionLayout->setContentsMargins(0, 0, 0, 0);

		auto contextLayout_ = new QVBoxLayout;
		contextLayout_->addWidget(fovSpinbox_);
		contextLayout_->addWidget(fovSlider);
		contextLayout_->addWidget(dofInfoLabel_, 0, Qt::AlignLeft);
		contextLayout_->addLayout(dofLayout_);
		contextLayout_->addWidget(apertureSpinbox_);
		contextLayout_->addWidget(focalLengthSpinbox_);
		contextLayout_->addWidget(focusDistanceSpinbox_);
		contextLayout_->addLayout(focusTargetLayout);
		contextLayout_->addLayout(animtionLayout);
		contextLayout_->setContentsMargins(16, 0, 16, 0);

		mainLayout_ = new QVBoxLayout;
		mainLayout_->addLayout(titleLayout_);
		mainLayout_->addLayout(contextLayout_);
		mainLayout_->addStretch();
		mainLayout_->setContentsMargins(0, 12, 0, 8);

		mainWidget_ = new QWidget;
		mainWidget_->setObjectName("CameraWidget");
		mainWidget_->setLayout(mainLayout_);

		this->setWidget(mainWidget_);

		profile_->cameraModule->fov += [this](float fov) {
			this->fovSpinbox_->blockSignals(true);
			this->fovSpinbox_->setValue(fov);
			this->fovSpinbox_->blockSignals(false);

			this->fovSlider->blockSignals(true);
			this->fovSlider->setValue(fov * 100);
			this->fovSlider->blockSignals(false);
		};

		profile_->cameraModule->useDepthOfFiled += [this](bool value) {
			this->dofButton_->blockSignals(true);
			this->dofButton_->setChecked(value);
			this->dofButton_->blockSignals(false);
		};

		profile_->cameraModule->aperture += [this](float fov) {
			this->apertureSpinbox_->blockSignals(true);
			this->apertureSpinbox_->setValue(fov);
			this->apertureSpinbox_->blockSignals(false);
		};

		profile_->cameraModule->focalLength += [this](float fov) {
			this->focalLengthSpinbox_->blockSignals(true);
			this->focalLengthSpinbox_->setValue(fov);
			this->focalLengthSpinbox_->blockSignals(false);
		};

		profile_->cameraModule->animation += [this](const std::shared_ptr<octoon::Animation>& animation) {
			if (animation)
				unloadButton_->setEnabled(true);
			else
				unloadButton_->setEnabled(false);
		};
		
		connect(focusTargetButton_, SIGNAL(mouseMoveSignal()), this, SLOT(onUpdateTarget()));
		connect(fovSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(onFovChanged(double)));
		connect(fovSlider, SIGNAL(valueChanged(int)), this, SLOT(onFovSliderEvent(int)));
		connect(apertureSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(onApertureChanged(double)));
		connect(dofButton_, SIGNAL(stateChanged(int)), this, SLOT(dofEvent(int)));
		connect(focalLengthSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(onFocalLengthChanged(double)));
		connect(focusDistanceSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(onFocusDistanceChanged(double)));
		connect(loadButton_, SIGNAL(clicked()), this, SLOT(onLoadAnimation()));
		connect(unloadButton_, SIGNAL(clicked()), this, SLOT(onUnloadAnimation()));
	}

	CameraDock::~CameraDock() noexcept
	{
	}

	void
	CameraDock::onUpdateTarget()
	{
		if (!profile_->playerModule->isPlaying)
		{
			auto hit = profile_->selectorModule->selectedItemHover_.getValue();
			if (hit)
			{
				profile_->playerModule->dofTarget = hit;

				auto object = hit->object.lock();
				auto renderer = object->getComponent<octoon::MeshRendererComponent>();
				auto material = renderer->getMaterial(profile_->playerModule->dofTarget->mesh);

				focusDistanceName_->setText(tr("Target: %1").arg(material->getName().c_str()));
				focusDistanceSpinbox_->setValue(0);
				focusDistanceSpinbox_->setSpecialValueText(tr("Auto-measuring"));
			}
			else
			{
				profile_->playerModule->dofTarget = hit;

				focusDistanceName_->setText(tr("Target: Empty"));
				focusDistanceSpinbox_->setValue(10);
				focusDistanceSpinbox_->setSpecialValueText(QString());
			}
		}
	}

	void
	CameraDock::onFovChanged(double value)
	{
		profile_->cameraModule->fov = value;
	}

	void
	CameraDock::onFovSliderEvent(int value)
	{
		profile_->cameraModule->fov = value / 100.0f;
	}

	void
	CameraDock::onFocalLengthChanged(double value)
	{
		profile_->cameraModule->focalLength = value;
	}

	void
	CameraDock::onApertureChanged(double value)
	{
		profile_->cameraModule->aperture = value;
	}

	void
	CameraDock::dofEvent(int checked)
	{
		if (checked == Qt::CheckState::Checked)
			profile_->cameraModule->useDepthOfFiled = true;
		else
			profile_->cameraModule->useDepthOfFiled = false;
	}

	void
	CameraDock::onFocusDistanceChanged(double value)
	{
		auto& camera = profile_->cameraModule->camera.getValue();
		if (camera)
		{
			if (!focusDistanceSpinbox_->specialValueText().isEmpty())
			{
				profile_->playerModule->dofTarget = std::nullopt;

				focusDistanceName_->setText(tr("Target: Empty"));
				focusDistanceSpinbox_->setSpecialValueText(QString());
				focusDistanceSpinbox_->setValue(camera->getComponent<octoon::FilmCameraComponent>()->getFocalDistance());
			}
			else
			{
				profile_->cameraModule->focusDistance = value;
			}
		}
	}

	void
	CameraDock::onLoadAnimation()
	{
		try
		{
			QString filepath = QFileDialog::getOpenFileName(this, tr("Load Animation"), "", tr("VMD Files (*.vmd)"));
			if (!filepath.isEmpty())
			{
				this->profile_->cameraModule->animation = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Animation>(filepath.toStdWString());

				auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
				if (behaviour)
					behaviour->getComponent<PlayerComponent>()->updateTimeLength();
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	CameraDock::onUnloadAnimation()
	{
		this->profile_->cameraModule->animation = nullptr;
		this->profile_->cameraModule->translate = octoon::math::float3(0, 10, -10);
		this->profile_->cameraModule->rotation = octoon::math::float3::Zero;

		auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
		if (behaviour)
			behaviour->getComponent<PlayerComponent>()->updateTimeLength();
	}

	void
	CameraDock::showEvent(QShowEvent* event)
	{
		auto mainCamera = profile_->cameraModule->camera.getValue();

		fovSpinbox_->setValue(profile_->cameraModule->fov);
		fovSlider->setValue(profile_->cameraModule->fov * 100.0f);
		focalLengthSpinbox_->setValue(profile_->cameraModule->focalLength);
		apertureSpinbox_->setValue(profile_->cameraModule->aperture);
		dofButton_->setChecked(profile_->cameraModule->useDepthOfFiled);
		unloadButton_->setEnabled(mainCamera->getComponent<octoon::AnimatorComponent>() ? true : false);

		if (profile_->playerModule->dofTarget)
		{
			auto object = profile_->playerModule->dofTarget->object.lock();
			auto renderer = object->getComponent<octoon::MeshRendererComponent>();
			auto material = renderer->getMaterial(profile_->playerModule->dofTarget->mesh);

			focusDistanceSpinbox_->setValue(0);
			focusDistanceSpinbox_->setSpecialValueText(tr("Auto-measuring"));
			focusDistanceName_->setText(tr("Target: %1").arg(material->getName().c_str()));
		}
		else
		{
			focusDistanceName_->setText(tr("Target: Empty"));
			focusDistanceSpinbox_->setValue(mainCamera->getComponent<octoon::FilmCameraComponent>()->getFocalDistance());
		}
	}

	void
	CameraDock::closeEvent(QCloseEvent* event)
	{
		if (profile_->playerModule->isPlaying)
			event->ignore();
		else
			event->accept();
	}

	bool
	CameraDock::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() != QEvent::Paint &&
			event->type() != QEvent::Resize)
		{
			if (profile_->playerModule->isPlaying)
			{
				return true;
			}
		}

		return UDockWidget::eventFilter(watched, event);
	}
}