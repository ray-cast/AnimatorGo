#include "main_light_dock.h"
#include <qscrollbar.h>
#include <octoon/transform_component.h>
#include <octoon/directional_light_component.h>

namespace unreal
{
	MainLightDock::MainLightDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<unreal::UnrealProfile>& profile)
		: profile_(profile)
	{
		this->setWindowTitle(tr("Main Light"));
		this->setObjectName("MainLightDock");
		this->setFeatures(QDockWidget::NoDockWidgetFeatures);
		
		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		auto title_ = new QLabel;
		title_->setObjectName("title");
		title_->setText(tr("Main Light Editor"));

		auto headerLine = new QFrame;
		headerLine->setObjectName("HLine");
		headerLine->setFixedHeight(1);
		headerLine->setFrameShape(QFrame::NoFrame);
		headerLine->setFrameShadow(QFrame::Plain);
		headerLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

		auto titleLayout_ = new QVBoxLayout();
		titleLayout_->addWidget(title_, 0, Qt::AlignLeft);
		titleLayout_->addSpacing(8);
		titleLayout_->addWidget(headerLine);
		titleLayout_->setSpacing(0);
		titleLayout_->setContentsMargins(4, 0, 4, 0);

		auto color = profile->mainLightModule->color.getValue();
		
		colorDialog_ = new ColorDialog();
		colorDialog_->setCurrentColor(QColor::fromRgbF(color.x, color.y, color.z));
		colorDialog_->installEventFilter(this);

		resetButton_ = new QToolButton();
		resetButton_->setObjectName("Reset");
		resetButton_->setText(tr("Reset"));
		resetButton_->setContentsMargins(0, 0, 8, 0);
		resetButton_->installEventFilter(this);

		labelIntensity_ = new QLabel();
		labelIntensity_->setText(tr("Intensity"));

		intensitySpinBox_ = new UDoubleSpinBox();
		intensitySpinBox_->setFixedWidth(48);
		intensitySpinBox_->setMaximum(10.0f);
		intensitySpinBox_->setSingleStep(0.1f);
		intensitySpinBox_->setAlignment(Qt::AlignRight);
		intensitySpinBox_->setValue(profile->mainLightModule->intensity);
		intensitySpinBox_->setDecimals(1);
		intensitySpinBox_->setSuffix("cd");
		intensitySpinBox_->installEventFilter(this);

		intensitySlider_ = new QSlider();
		intensitySlider_->setObjectName("Intensity");
		intensitySlider_->setOrientation(Qt::Horizontal);
		intensitySlider_->setMinimum(0);
		intensitySlider_->setMaximum(100);
		intensitySlider_->setValue(profile->mainLightModule->intensity * 10.0f);
		intensitySlider_->setFixedWidth(200);
		intensitySlider_->installEventFilter(this);

		layoutIntensity_ = new QHBoxLayout();
		layoutIntensity_->addWidget(labelIntensity_, 0, Qt::AlignLeft);
		layoutIntensity_->addWidget(intensitySpinBox_, 0, Qt::AlignRight);
		layoutIntensity_->setContentsMargins(22, 5, 21, 0);

		labelSize_ = new QLabel();
		labelSize_->setText(tr("Size"));

		sizeSpinBox_ = new UDoubleSpinBox();
		sizeSpinBox_->setFixedWidth(48);
		sizeSpinBox_->setMaximum(1.0f);
		sizeSpinBox_->setSingleStep(0.05f);
		sizeSpinBox_->setAlignment(Qt::AlignRight);
		sizeSpinBox_->setValue(profile->mainLightModule->size);
		sizeSpinBox_->setDecimals(1);
		sizeSpinBox_->installEventFilter(this);

		sizeSlider_ = new QSlider();
		sizeSlider_->setObjectName("Size");
		sizeSlider_->setOrientation(Qt::Horizontal);
		sizeSlider_->setMinimum(0);
		sizeSlider_->setMaximum(100);
		sizeSlider_->setValue(profile->mainLightModule->size * 100.0f);
		sizeSlider_->setFixedWidth(200);
		sizeSlider_->installEventFilter(this);

		layoutSize_ = new QHBoxLayout();
		layoutSize_->addWidget(labelSize_, 0, Qt::AlignLeft);
		layoutSize_->addWidget(sizeSpinBox_, 0, Qt::AlignRight);
		layoutSize_->setContentsMargins(22, 5, 21, 0);

		labelRotationX_ = new QLabel();
		labelRotationX_->setText(tr("Rotation X"));

		editRotationX_ = new UDoubleSpinBox();
		editRotationX_->setFixedWidth(48);
		editRotationX_->setMaximum(360.0f);
		editRotationX_->setSingleStep(1.0f);
		editRotationX_->setAlignment(Qt::AlignRight);
		editRotationX_->setValue(profile->mainLightModule->rotation.getValue().x);
		editRotationX_->setDecimals(1);
		editRotationX_->setSuffix(u8"\u00B0");
		editRotationX_->installEventFilter(this);

		sliderRotationX_ = new QSlider();
		sliderRotationX_->setObjectName("RotationX");
		sliderRotationX_->setOrientation(Qt::Horizontal);
		sliderRotationX_->setMinimum(0);
		sliderRotationX_->setMaximum(360);
		sliderRotationX_->setValue(profile->mainLightModule->rotation.getValue().x);
		sliderRotationX_->setFixedWidth(200);
		sliderRotationX_->installEventFilter(this);

		layoutRotationX_ = new QHBoxLayout();
		layoutRotationX_->addWidget(labelRotationX_, 0, Qt::AlignLeft);
		layoutRotationX_->addWidget(editRotationX_, 0, Qt::AlignRight);
		layoutRotationX_->setContentsMargins(22, 5, 21, 0);

		labelRotationY_ = new QLabel();
		labelRotationY_->setText(tr("Rotation Y"));

		editRotationY_ = new UDoubleSpinBox();
		editRotationY_->setFixedWidth(48);
		editRotationY_->setMaximum(360.0f);
		editRotationY_->setSingleStep(1.0f);
		editRotationY_->setAlignment(Qt::AlignRight);
		editRotationY_->setValue(profile->mainLightModule->rotation.getValue().y);
		editRotationY_->setDecimals(1);
		editRotationY_->setSuffix(u8"\u00B0");
		editRotationY_->installEventFilter(this);

		sliderRotationY_ = new QSlider();
		sliderRotationY_->setObjectName("RotationY");
		sliderRotationY_->setOrientation(Qt::Horizontal);
		sliderRotationY_->setMinimum(0);
		sliderRotationY_->setMaximum(360);
		sliderRotationY_->setValue(profile->mainLightModule->rotation.getValue().y);
		sliderRotationY_->setFixedWidth(200);
		sliderRotationY_->installEventFilter(this);

		layoutRotationY_ = new QHBoxLayout();
		layoutRotationY_->addWidget(labelRotationY_, 0, Qt::AlignLeft);
		layoutRotationY_->addWidget(editRotationY_, 0, Qt::AlignRight);
		layoutRotationY_->setContentsMargins(22, 5, 21, 0);

		labelRotationZ_ = new QLabel();
		labelRotationZ_->setText(tr("Rotation Z"));

		editRotationZ_ = new UDoubleSpinBox();
		editRotationZ_->setFixedWidth(48);
		editRotationZ_->setMaximum(360.0f);
		editRotationZ_->setSingleStep(1.0f);
		editRotationZ_->setAlignment(Qt::AlignRight);
		editRotationZ_->setValue(profile->mainLightModule->rotation.getValue().z);
		editRotationZ_->setDecimals(1);
		editRotationZ_->setSuffix(u8"\u00B0");
		editRotationZ_->installEventFilter(this);

		sliderRotationZ_ = new QSlider();
		sliderRotationZ_->setObjectName("RotationZ");
		sliderRotationZ_->setOrientation(Qt::Horizontal);
		sliderRotationZ_->setMinimum(0);
		sliderRotationZ_->setMaximum(360);
		sliderRotationZ_->setValue(profile->mainLightModule->rotation.getValue().z);
		sliderRotationZ_->setFixedWidth(200);
		sliderRotationZ_->installEventFilter(this);

		layoutRotationZ_ = new QHBoxLayout();
		layoutRotationZ_->addWidget(labelRotationZ_, 0, Qt::AlignLeft);
		layoutRotationZ_->addWidget(editRotationZ_, 0, Qt::AlignRight);
		layoutRotationZ_->setContentsMargins(22, 5, 21, 0);

		scrollLayout_ = new QVBoxLayout();
		scrollLayout_->addWidget(colorDialog_, 0, Qt::AlignHCenter | Qt::AlignTop);
		scrollLayout_->addLayout(layoutIntensity_, 0);
		scrollLayout_->addWidget(intensitySlider_, 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutSize_, 0);
		scrollLayout_->addWidget(sizeSlider_, 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationX_, 0);
		scrollLayout_->addWidget(sliderRotationX_, 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationY_, 0);
		scrollLayout_->addWidget(sliderRotationY_, 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationZ_, 0);
		scrollLayout_->addWidget(sliderRotationZ_, 0, Qt::AlignHCenter);
		scrollLayout_->addStretch();

		scrollWidget_ = new QWidget();
		scrollWidget_->setLayout(scrollLayout_);

		scrollArea_ = new QScrollArea();
		scrollArea_->setWidget(scrollWidget_);
		scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea_->setWidgetResizable(true);

		auto contextLayout_ = new QVBoxLayout;
		contextLayout_->addWidget(scrollArea_);
		contextLayout_->addStretch();
		contextLayout_->addWidget(resetButton_, 0, Qt::AlignBottom | Qt::AlignRight);
		contextLayout_->setContentsMargins(8, 0, 8, 0);

		mainLayout_ = new QVBoxLayout();
		mainLayout_->addLayout(titleLayout_);
		mainLayout_->addLayout(contextLayout_);
		mainLayout_->setContentsMargins(0, 12, 0, 8);

		mainWidget_ = new QWidget();
		mainWidget_->setObjectName("MainLightWidget");
		mainWidget_->setLayout(mainLayout_);
		
		this->setWidget(mainWidget_);

		this->profile_->mainLightModule->intensity += [this](float value)
		{
			intensitySlider_->blockSignals(true);
			intensitySlider_->setValue(value * 10.0f);
			intensitySlider_->blockSignals(false);

			intensitySpinBox_->blockSignals(true);
			intensitySpinBox_->setValue(value);
			intensitySpinBox_->blockSignals(false);
		};

		this->profile_->mainLightModule->size += [this](float value)
		{
			sizeSlider_->blockSignals(true);
			sizeSlider_->setValue(value * 100.0f);
			sizeSlider_->blockSignals(false);

			sizeSpinBox_->blockSignals(true);
			sizeSpinBox_->setValue(value);
			sizeSpinBox_->blockSignals(false);
		};

		this->profile_->mainLightModule->color += [this](const octoon::math::float3& color)
		{
			colorDialog_->blockSignals(true);
			colorDialog_->setCurrentColor(QColor::fromRgbF(color.x, color.y, color.z));
			colorDialog_->blockSignals(false);
		};

		this->profile_->mainLightModule->rotation += [this](const octoon::math::float3& rotation)
		{
			editRotationX_->blockSignals(true);
			editRotationX_->setValue(rotation.x + 180.0f);
			editRotationX_->blockSignals(false);

			sliderRotationX_->blockSignals(true);
			sliderRotationX_->setValue(rotation.x + 180.0f);
			sliderRotationX_->blockSignals(false);

			editRotationY_->blockSignals(true);
			editRotationY_->setValue(rotation.y + 180.0f);
			editRotationY_->blockSignals(false);

			editRotationZ_->blockSignals(true);
			editRotationZ_->setValue(rotation.z);
			editRotationZ_->blockSignals(false);

			sliderRotationY_->blockSignals(true);
			sliderRotationY_->setValue(rotation.y + 180.0f);
			sliderRotationY_->blockSignals(false);

			sliderRotationZ_->blockSignals(true);
			sliderRotationZ_->setValue(rotation.z);
			sliderRotationZ_->blockSignals(false);
		};

		connect(resetButton_, SIGNAL(clicked()), this, SLOT(resetEvent()));
		connect(intensitySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(intensityEditEvent(double)));
		connect(intensitySlider_, SIGNAL(valueChanged(int)), this, SLOT(intensitySliderEvent(int)));
		connect(sizeSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(sizeEditEvent(double)));
		connect(sizeSlider_, SIGNAL(valueChanged(int)), this, SLOT(sizeSliderEvent(int)));
		connect(editRotationX_, SIGNAL(valueChanged(double)), this, SLOT(editRotationXEvent(double)));
		connect(sliderRotationX_, SIGNAL(valueChanged(int)), this, SLOT(sliderRotationXEvent(int)));
		connect(editRotationY_, SIGNAL(valueChanged(double)), this, SLOT(editRotationYEvent(double)));
		connect(sliderRotationY_, SIGNAL(valueChanged(int)), this, SLOT(sliderRotationYEvent(int)));
		connect(editRotationZ_, SIGNAL(valueChanged(double)), this, SLOT(editRotationZEvent(double)));
		connect(sliderRotationZ_, SIGNAL(valueChanged(int)), this, SLOT(sliderRotationZEvent(int)));
		connect(colorDialog_, SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChanged(QColor)));
	}

	MainLightDock::~MainLightDock()
	{
	}

	void
	MainLightDock::paintEvent(QPaintEvent* e) noexcept
	{
		int left, top, bottom, right;
		mainLayout_->getContentsMargins(&left, &top, &right, &bottom);
		scrollArea_->resize(scrollArea_->size().width(), mainWidget_->size().height() - resetButton_->height() - (top + bottom) * 2);

		QDockWidget::paintEvent(e);
	}

	void 
	MainLightDock::currentColorChanged(QColor color)
	{
		if (color.isValid())
			profile_->mainLightModule->color = octoon::math::float3(color.redF(), color.greenF(), color.blueF());
	}

	void
	MainLightDock::resetEvent()
	{
		profile_->mainLightModule->reset();
	}

	void
	MainLightDock::intensitySliderEvent(int value)
	{
		profile_->mainLightModule->intensity = value / 10.0f;
	}

	void
	MainLightDock::intensityEditEvent(double value)
	{
		profile_->mainLightModule->intensity = value;
	}

	void
	MainLightDock::sizeSliderEvent(int value)
	{
		profile_->mainLightModule->size = value / 100.0f;
	}

	void
	MainLightDock::sizeEditEvent(double value)
	{
		profile_->mainLightModule->size = value;
	}

	void
	MainLightDock::sliderRotationXEvent(int value)
	{
		profile_->mainLightModule->rotation = octoon::math::float3(value - 180.0f, profile_->mainLightModule->rotation.getValue().y, profile_->mainLightModule->rotation.getValue().z);
	}

	void
	MainLightDock::sliderRotationYEvent(int value)
	{
		profile_->mainLightModule->rotation = octoon::math::float3(profile_->mainLightModule->rotation.getValue().x, value - 180.0f, profile_->mainLightModule->rotation.getValue().z);
	}

	void
	MainLightDock::sliderRotationZEvent(int value)
	{
		profile_->mainLightModule->rotation = octoon::math::float3(profile_->mainLightModule->rotation.getValue().x, profile_->mainLightModule->rotation.getValue().y, value);
	}

	void
	MainLightDock::editRotationXEvent(double value)
	{
		profile_->mainLightModule->rotation = octoon::math::float3(value - 180.0f, profile_->mainLightModule->rotation.getValue().y, profile_->mainLightModule->rotation.getValue().z);
	}

	void
	MainLightDock::editRotationYEvent(double value)
	{
		profile_->mainLightModule->rotation = octoon::math::float3(profile_->mainLightModule->rotation.getValue().x, value - 180.0f, profile_->mainLightModule->rotation.getValue().z);
	}

	void
	MainLightDock::editRotationZEvent(double value)
	{
		profile_->mainLightModule->rotation = octoon::math::float3(profile_->mainLightModule->rotation.getValue().x, profile_->mainLightModule->rotation.getValue().y, value);
	}

	void
	MainLightDock::showEvent(QShowEvent* event)
	{
		auto color = profile_->mainLightModule->color.getValue();
		auto rotation = profile_->mainLightModule->rotation.getValue();

		sizeSpinBox_->setValue(profile_->mainLightModule->size);
		sizeSlider_->setValue(profile_->mainLightModule->size * 100.0f);

		intensitySpinBox_->setValue(profile_->mainLightModule->intensity);
		intensitySlider_->setValue(profile_->mainLightModule->intensity * 10.0f);

		editRotationX_->setValue(rotation.x + 180.0f);
		editRotationY_->setValue(rotation.y + 180.0f);
		editRotationZ_->setValue(rotation.z);

		sliderRotationX_->setValue(rotation.x + 180.0f);
		sliderRotationY_->setValue(rotation.y + 180.0f);
		sliderRotationZ_->setValue(rotation.z);

		colorDialog_->setCurrentColor(QColor::fromRgbF(color.x, color.y, color.z));
	}

	void
	MainLightDock::closeEvent(QCloseEvent* event)
	{
		if (profile_->playerModule->isPlaying)
			event->ignore();
		else
			event->accept();
	}

	bool
	MainLightDock::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() != QEvent::Paint &&
			event->type() != QEvent::Resize)
		{
			if (profile_->playerModule->isPlaying)
			{
				return true;
			}
		}

		return QWidget::eventFilter(watched, event);
	}
}