#include "sun_window.h"
#include <qscrollbar.h>
#include <octoon/transform_component.h>
#include <octoon/directional_light_component.h>

namespace rabbit
{
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

	SunWindow::SunWindow(const std::shared_ptr<rabbit::RabbitProfile>& profile)
		: profile_(profile)
	{
		this->setObjectName("sunWindow");
		this->setMinimumWidth(340);
		this->grabKeyboard();
		this->hide();

		title_ = new QLabel();
		title_->setText(u8"主光源");

		closeButton_ = new QToolButton();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		scrollWidget_ = new QWidget(this);
		scrollWidget_->setMinimumWidth(340);

		colorDialog_ = new ColorDialog();
		colorDialog_->setCurrentColor(QColor::fromRgbF(profile->sunModule->color.x, profile->sunModule->color.y, profile->sunModule->color.z));

		resetButton_ = new QToolButton();
		resetButton_->setText(u8"重置");

		labelIntensity_ = new QLabel();
		labelIntensity_->setText(u8"光强");

		editIntensity_ = new DoubleSpinBox();
		editIntensity_->setFixedWidth(50);
		editIntensity_->setMaximum(10.0f);
		editIntensity_->setSingleStep(0.1f);
		editIntensity_->setAlignment(Qt::AlignRight);
		editIntensity_->setValue(profile->sunModule->intensity);

		sliderIntensity_ = new QSlider();
		sliderIntensity_->setObjectName("Intensity");
		sliderIntensity_->setOrientation(Qt::Horizontal);
		sliderIntensity_->setMinimum(0);
		sliderIntensity_->setMaximum(100);
		sliderIntensity_->setValue(profile->sunModule->intensity * 10.0f);
		sliderIntensity_->setFixedWidth(250);

		layoutIntensity_ = new QHBoxLayout();
		layoutIntensity_->addWidget(labelIntensity_, 0, Qt::AlignLeft);
		layoutIntensity_->addWidget(editIntensity_, 0, Qt::AlignRight);
		layoutIntensity_->setContentsMargins(40, 5, 35, 0);

		labelRotationX_ = new QLabel();
		labelRotationX_->setText(u8"旋转 X");

		editRotationX_ = new DoubleSpinBox();
		editRotationX_->setFixedWidth(50);
		editRotationX_->setMaximum(360.0f);
		editRotationX_->setSingleStep(1.0f);
		editRotationX_->setAlignment(Qt::AlignRight);
		editRotationX_->setValue(profile->sunModule->rotation.x);

		sliderRotationX_ = new QSlider();
		sliderRotationX_->setObjectName("RotationX");
		sliderRotationX_->setOrientation(Qt::Horizontal);
		sliderRotationX_->setMinimum(0);
		sliderRotationX_->setMaximum(360);
		sliderRotationX_->setValue(profile->sunModule->rotation.x);
		sliderRotationX_->setFixedWidth(250);

		layoutRotationX_ = new QHBoxLayout();
		layoutRotationX_->addWidget(labelRotationX_, 0, Qt::AlignLeft);
		layoutRotationX_->addWidget(editRotationX_, 0, Qt::AlignRight);
		layoutRotationX_->setContentsMargins(40, 5, 35, 0);

		labelRotationY_ = new QLabel();
		labelRotationY_->setText(u8"旋转 Y");

		editRotationY_ = new DoubleSpinBox();
		editRotationY_->setFixedWidth(50);
		editRotationY_->setMaximum(360.0f);
		editRotationY_->setSingleStep(1.0f);
		editRotationY_->setAlignment(Qt::AlignRight);
		editRotationY_->setValue(profile->sunModule->rotation.y);

		sliderRotationY_ = new QSlider();
		sliderRotationY_->setObjectName("RotationY");
		sliderRotationY_->setOrientation(Qt::Horizontal);
		sliderRotationY_->setMinimum(0);
		sliderRotationY_->setMaximum(360);
		sliderRotationY_->setValue(profile->sunModule->rotation.x);
		sliderRotationY_->setFixedWidth(250);

		layoutRotationY_ = new QHBoxLayout();
		layoutRotationY_->addWidget(labelRotationY_, 0, Qt::AlignLeft);
		layoutRotationY_->addWidget(editRotationY_, 0, Qt::AlignRight);
		layoutRotationY_->setContentsMargins(40, 5, 35, 0);

		labelRotationZ_ = new QLabel();
		labelRotationZ_->setText(u8"旋转 Z");

		editRotationZ_ = new DoubleSpinBox();
		editRotationZ_->setFixedWidth(50);
		editRotationZ_->setMaximum(360.0f);
		editRotationZ_->setSingleStep(1.0f);
		editRotationZ_->setAlignment(Qt::AlignRight);
		editRotationZ_->setValue(profile->sunModule->rotation.x);

		sliderRotationZ_ = new QSlider();
		sliderRotationZ_->setObjectName("RotationZ");
		sliderRotationZ_->setOrientation(Qt::Horizontal);
		sliderRotationZ_->setMinimum(0);
		sliderRotationZ_->setMaximum(360);
		sliderRotationZ_->setValue(profile->sunModule->rotation.x);
		sliderRotationZ_->setFixedWidth(250);

		layoutRotationZ_ = new QHBoxLayout();
		layoutRotationZ_->addWidget(labelRotationZ_, 0, Qt::AlignLeft);
		layoutRotationZ_->addWidget(editRotationZ_, 0, Qt::AlignRight);
		layoutRotationZ_->setContentsMargins(40, 5, 35, 0);

		scrollLayout_ = new QVBoxLayout(scrollWidget_);
		scrollLayout_->addWidget(colorDialog_, 0, Qt::AlignHCenter | Qt::AlignTop);
		scrollLayout_->addLayout(layoutIntensity_, 0);
		scrollLayout_->addWidget(sliderIntensity_, 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationX_, 0);
		scrollLayout_->addWidget(sliderRotationX_, 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationY_, 0);
		scrollLayout_->addWidget(sliderRotationY_, 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationZ_, 0);
		scrollLayout_->addWidget(sliderRotationZ_, 0, Qt::AlignHCenter);
		scrollLayout_->addStretch();

		scrollArea_ = new QScrollArea();
		scrollArea_->setWidget(scrollWidget_);
		scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea_->setWidgetResizable(true);

		layout_ = new QHBoxLayout();
		layout_->addSpacing(this->closeButton_->iconSize().width());
		layout_->addStretch();
		layout_->addWidget(title_, 0, Qt::AlignLeft);
		layout_->addStretch();
		layout_->addWidget(closeButton_, 0, Qt::AlignRight);

		mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addLayout(layout_);
		mainLayout_->addWidget(scrollArea_);
		mainLayout_->addStretch();
		mainLayout_->addWidget(resetButton_, 0, Qt::AlignBottom | Qt::AlignRight);
		mainLayout_->setContentsMargins(0, 10, 2, 10);

		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(resetButton_, SIGNAL(clicked()), this, SLOT(resetEvent()));
		connect(editIntensity_, SIGNAL(valueChanged(double)), this, SLOT(intensityEditEvent(double)));
		connect(sliderIntensity_, SIGNAL(valueChanged(int)), this, SLOT(intensitySliderEvent(int)));
		connect(editRotationX_, SIGNAL(valueChanged(double)), this, SLOT(editRotationXEvent(double)));
		connect(sliderRotationX_, SIGNAL(valueChanged(int)), this, SLOT(sliderRotationXEvent(int)));
		connect(editRotationY_, SIGNAL(valueChanged(double)), this, SLOT(editRotationYEvent(double)));
		connect(sliderRotationY_, SIGNAL(valueChanged(int)), this, SLOT(sliderRotationYEvent(int)));
		connect(editRotationZ_, SIGNAL(valueChanged(double)), this, SLOT(editRotationZEvent(double)));
		connect(sliderRotationZ_, SIGNAL(valueChanged(int)), this, SLOT(sliderRotationZEvent(int)));
		connect(colorDialog_, SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChanged(QColor)));
	}

	SunWindow::~SunWindow()
	{
	}

	void
	SunWindow::repaint()
	{
		editRotationX_->setValue(profile_->sunModule->rotation.x + 180.0f);
		editRotationY_->setValue(profile_->sunModule->rotation.y + 180.0f);
		editRotationZ_->setValue(profile_->sunModule->rotation.z);
		editIntensity_->setValue(profile_->sunModule->intensity);
		colorDialog_->setCurrentColor(QColor(profile_->sunModule->color.x * 255.0f, profile_->sunModule->color.y * 255.0f, profile_->sunModule->color.z * 255.0f));
	}

	void
	SunWindow::showEvent(QShowEvent* event)
	{
		this->repaint();
	}

	void
	SunWindow::resizeEvent(QResizeEvent* event)
	{
		int left, top, bottom, right;
		mainLayout_->getContentsMargins(&left, &top, &right, &bottom);
		scrollArea_->setFixedHeight(event->size().height() - resetButton_->height() - title_->height() - (top + bottom) * 2);
	}

	void
	SunWindow::closeEvent(QCloseEvent* event)
	{
		auto x = editRotationX_->value() - 180.0f;
		auto y = editRotationY_->value() - 180.0f;
		auto z = editRotationZ_->value();
		auto color = colorDialog_->getCurrentColor();
		profile_->sunModule->intensity = editIntensity_->value();
		profile_->sunModule->color = octoon::math::float3(color.redF(), color.greenF(), color.blueF());
		profile_->sunModule->rotation = octoon::math::float3(x, y, z);
	}

	void 
	SunWindow::currentColorChanged(QColor color)
	{
		if (color.isValid())
		{
			auto sunLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
			if (sunLight)
				sunLight->setColor(octoon::math::srgb2linear(octoon::math::float3(color.redF(), color.greenF(), color.blueF())));
		}
	}

	void
	SunWindow::resetEvent()
	{
		this->repaint();

		auto sunLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
		if (sunLight)
		{
			sunLight->setIntensity(profile_->sunModule->intensity);
			sunLight->setColor(octoon::math::srgb2linear(profile_->sunModule->color));
		}

		auto transform = profile_->entitiesModule->sunLight->getComponent<octoon::TransformComponent>();
		if (transform)
		{
			transform->setQuaternion(octoon::math::Quaternion(octoon::math::radians(profile_->sunModule->rotation)));
			transform->setTranslate(-octoon::math::rotate(transform->getQuaternion(), octoon::math::float3::UnitZ) * 60);
		}
	}

	void
	SunWindow::closeEvent()
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}

	void
	SunWindow::intensitySliderEvent(int value)
	{
		auto envLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
		if (envLight)
			envLight->setIntensity(value / 10.0f);
		editIntensity_->setValue(value / 10.0f);
	}

	void
	SunWindow::intensityEditEvent(double value)
	{
		auto envLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
		if (envLight)
			envLight->setIntensity(value);
		sliderIntensity_->setValue(value * 10.0f);
	}

	void
	SunWindow::sliderRotationXEvent(int value)
	{
		editRotationX_->setValue(value);
	}

	void
	SunWindow::editRotationXEvent(double value)
	{
		auto y = octoon::math::radians(editRotationY_->value() - 180.0f);
		auto z = octoon::math::radians(editRotationZ_->value());

		auto transform = profile_->entitiesModule->sunLight->getComponent<octoon::TransformComponent>();
		if (transform)
		{
			transform->setQuaternion(octoon::math::Quaternion(octoon::math::float3(octoon::math::radians(value - 180.0f), y, z)));
			transform->setTranslate(-octoon::math::rotate(transform->getQuaternion(), octoon::math::float3::UnitZ) * 60);
		}

		sliderRotationX_->setValue(value);
	}

	void
	SunWindow::sliderRotationYEvent(int value)
	{
		editRotationY_->setValue(value);
	}

	void
	SunWindow::editRotationYEvent(double value)
	{
		auto x = octoon::math::radians(editRotationX_->value() - 180.0f);
		auto z = octoon::math::radians(editRotationZ_->value());

		auto transform = profile_->entitiesModule->sunLight->getComponent<octoon::TransformComponent>();
		if (transform)
		{
			transform->setQuaternion(octoon::math::Quaternion(octoon::math::float3(x, octoon::math::radians(value - 180.0f), z)));
			transform->setTranslate(-octoon::math::rotate(transform->getQuaternion(), octoon::math::float3::UnitZ) * 60);
		}

		sliderRotationY_->setValue(value);
	}

	void
	SunWindow::sliderRotationZEvent(int value)
	{
		editRotationZ_->setValue(value);
	}

	void
	SunWindow::editRotationZEvent(double value)
	{
		auto x = octoon::math::radians(editRotationX_->value() - 180.0f);
		auto y = octoon::math::radians(editRotationY_->value() - 180.0f);

		auto transform = profile_->entitiesModule->sunLight->getComponent<octoon::TransformComponent>();
		if (transform)
		{
			transform->setQuaternion(octoon::math::Quaternion(octoon::math::float3(x, y, octoon::math::radians(value))));
			transform->setTranslate(-octoon::math::rotate(transform->getQuaternion(), octoon::math::float3::UnitZ) * 60);
		}

		sliderRotationZ_->setValue(value);
	}
}