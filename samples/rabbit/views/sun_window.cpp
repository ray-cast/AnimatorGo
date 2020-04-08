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

		title_ = std::make_unique<QLabel>();
		title_->setText(u8"主光源");

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		scrollWidget_ = std::make_unique<QWidget>(this);
		scrollWidget_->setMinimumWidth(340);

		colorDialog_ = std::make_unique<ColorDialog>();
		colorDialog_->setCurrentColor(QColor(profile->sunModule->color.x * 255.0f, profile->sunModule->color.y * 255.0f, profile->sunModule->color.z * 255.0f));

		okButton_ = std::make_unique<QToolButton>();
		okButton_->setText(u8"确定");

		labelIntensity_ = std::make_unique<QLabel>();
		labelIntensity_->setText(u8"光强");

		editIntensity_ = std::make_unique<DoubleSpinBox>();
		editIntensity_->setFixedWidth(50);
		editIntensity_->setMaximum(10.0f);
		editIntensity_->setSingleStep(0.1f);
		editIntensity_->setAlignment(Qt::AlignRight);
		editIntensity_->setValue(profile->sunModule->intensity);

		sliderIntensity_ = std::make_unique<QSlider>();
		sliderIntensity_->setObjectName("Intensity");
		sliderIntensity_->setOrientation(Qt::Horizontal);
		sliderIntensity_->setMinimum(0);
		sliderIntensity_->setMaximum(100);
		sliderIntensity_->setValue(profile->sunModule->intensity * 10.0f);
		sliderIntensity_->setFixedWidth(240);

		layoutIntensity_ = std::make_unique<QHBoxLayout>();
		layoutIntensity_->addWidget(labelIntensity_.get(), 0, Qt::AlignLeft);
		layoutIntensity_->addWidget(editIntensity_.get(), 0, Qt::AlignRight);
		layoutIntensity_->setContentsMargins(40, 5, 35, 0);

		labelRotationX_ = std::make_unique<QLabel>();
		labelRotationX_->setText(u8"旋转 X");

		editRotationX_ = std::make_unique<DoubleSpinBox>();
		editRotationX_->setFixedWidth(50);
		editRotationX_->setMaximum(360.0f);
		editRotationX_->setSingleStep(1.0f);
		editRotationX_->setAlignment(Qt::AlignRight);
		editRotationX_->setValue(profile->sunModule->rotation.x);

		sliderRotationX_ = std::make_unique<QSlider>();
		sliderRotationX_->setObjectName("RotationX");
		sliderRotationX_->setOrientation(Qt::Horizontal);
		sliderRotationX_->setMinimum(0);
		sliderRotationX_->setMaximum(360);
		sliderRotationX_->setValue(profile->sunModule->rotation.x);
		sliderRotationX_->setFixedWidth(240);

		layoutRotationX_ = std::make_unique<QHBoxLayout>();
		layoutRotationX_->addWidget(labelRotationX_.get(), 0, Qt::AlignLeft);
		layoutRotationX_->addWidget(editRotationX_.get(), 0, Qt::AlignRight);
		layoutRotationX_->setContentsMargins(40, 5, 35, 0);

		labelRotationY_ = std::make_unique<QLabel>();
		labelRotationY_->setText(u8"旋转 Y");

		editRotationY_ = std::make_unique<DoubleSpinBox>();
		editRotationY_->setFixedWidth(50);
		editRotationY_->setMaximum(360.0f);
		editRotationY_->setSingleStep(1.0f);
		editRotationY_->setAlignment(Qt::AlignRight);
		editRotationY_->setValue(profile->sunModule->rotation.y);

		sliderRotationY_ = std::make_unique<QSlider>();
		sliderRotationY_->setObjectName("RotationY");
		sliderRotationY_->setOrientation(Qt::Horizontal);
		sliderRotationY_->setMinimum(0);
		sliderRotationY_->setMaximum(360);
		sliderRotationY_->setValue(profile->sunModule->rotation.x);
		sliderRotationY_->setFixedWidth(240);

		layoutRotationY_ = std::make_unique<QHBoxLayout>();
		layoutRotationY_->addWidget(labelRotationY_.get(), 0, Qt::AlignLeft);
		layoutRotationY_->addWidget(editRotationY_.get(), 0, Qt::AlignRight);
		layoutRotationY_->setContentsMargins(40, 5, 35, 0);

		labelRotationZ_ = std::make_unique<QLabel>();
		labelRotationZ_->setText(u8"旋转 Z");

		editRotationZ_ = std::make_unique<DoubleSpinBox>();
		editRotationZ_->setFixedWidth(50);
		editRotationZ_->setMaximum(360.0f);
		editRotationZ_->setSingleStep(1.0f);
		editRotationZ_->setAlignment(Qt::AlignRight);
		editRotationZ_->setValue(profile->sunModule->rotation.x);

		sliderRotationZ_ = std::make_unique<QSlider>();
		sliderRotationZ_->setObjectName("RotationZ");
		sliderRotationZ_->setOrientation(Qt::Horizontal);
		sliderRotationZ_->setMinimum(0);
		sliderRotationZ_->setMaximum(360);
		sliderRotationZ_->setValue(profile->sunModule->rotation.x);
		sliderRotationZ_->setFixedWidth(240);

		layoutRotationZ_ = std::make_unique<QHBoxLayout>();
		layoutRotationZ_->addWidget(labelRotationZ_.get(), 0, Qt::AlignLeft);
		layoutRotationZ_->addWidget(editRotationZ_.get(), 0, Qt::AlignRight);
		layoutRotationZ_->setContentsMargins(40, 5, 35, 0);

		scrollLayout_ = std::make_unique<QVBoxLayout>(scrollWidget_.get());
		scrollLayout_->addWidget(colorDialog_.get(), 0, Qt::AlignHCenter | Qt::AlignTop);
		scrollLayout_->addLayout(layoutIntensity_.get(), 0);
		scrollLayout_->addWidget(sliderIntensity_.get(), 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationX_.get(), 0);
		scrollLayout_->addWidget(sliderRotationX_.get(), 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationY_.get(), 0);
		scrollLayout_->addWidget(sliderRotationY_.get(), 0, Qt::AlignHCenter);
		scrollLayout_->addLayout(layoutRotationZ_.get(), 0);
		scrollLayout_->addWidget(sliderRotationZ_.get(), 0, Qt::AlignHCenter);
		scrollLayout_->addStretch(100);

		scrollArea_ = std::make_unique<QScrollArea>();
		scrollArea_->setWidget(scrollWidget_.get());
		scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea_->setWidgetResizable(true);

		layout_ = std::make_unique<QHBoxLayout>();
		layout_->addWidget(title_.get(), 0, Qt::AlignLeft);
		layout_->addWidget(closeButton_.get(), 0, Qt::AlignRight);

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addLayout(layout_.get());
		mainLayout_->addWidget(scrollArea_.get());
		mainLayout_->addStretch(100);
		mainLayout_->addWidget(okButton_.get(), 0, Qt::AlignBottom | Qt::AlignRight);
		mainLayout_->setContentsMargins(0, 10, 10, 10);

		connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(rejected()));
		connect(okButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(editIntensity_.get(), SIGNAL(valueChanged(double)), this, SLOT(editIntensityEvent(double)));
		connect(sliderIntensity_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderIntensityEvent(int)));
		connect(editRotationX_.get(), SIGNAL(valueChanged(double)), this, SLOT(editRotationXEvent(double)));
		connect(sliderRotationX_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderRotationXEvent(int)));
		connect(editRotationY_.get(), SIGNAL(valueChanged(double)), this, SLOT(editRotationYEvent(double)));
		connect(sliderRotationY_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderRotationYEvent(int)));
		connect(editRotationZ_.get(), SIGNAL(valueChanged(double)), this, SLOT(editRotationZEvent(double)));
		connect(sliderRotationZ_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderRotationZEvent(int)));
		connect(colorDialog_.get(), SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChanged(QColor)));
		connect(colorDialog_.get(), SIGNAL(colorSelected(QColor)), this, SLOT(colorSelected(QColor)));
	}

	SunWindow::~SunWindow()
	{
		title_.reset();
		closeButton_.reset();
		layout_.reset();
		colorDialog_.reset();
		sliderIntensity_.reset();
		labelIntensity_.reset();
		editIntensity_.reset();
		layoutIntensity_.reset();
		labelRotationX_.reset();
		editRotationX_.reset();
		layoutRotationX_.reset();
		sliderRotationX_.reset();
		labelRotationY_.reset();
		editRotationY_.reset();
		sliderRotationY_.reset();
		layoutRotationY_.reset();
		labelRotationZ_.reset();
		editRotationZ_.reset();
		sliderRotationZ_.reset();
		layoutRotationZ_.reset();
		scrollLayout_.reset();
		scrollWidget_.reset();
		scrollArea_.reset();
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
		scrollArea_->setFixedHeight(event->size().height() - okButton_->height() - title_->height() - (top + bottom) * 2);
	}

	void
	SunWindow::closeEvent(QCloseEvent* event)
	{
		auto sunLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
		if (sunLight)
			sunLight->setColor(octoon::math::srgb2linear(profile_->sunModule->color));
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
	SunWindow::rejected()
	{
		auto sunLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
		if (sunLight)
			sunLight->setColor(octoon::math::srgb2linear(profile_->sunModule->color));
		auto transform = profile_->entitiesModule->sunLight->getComponent<octoon::TransformComponent>();
		if (transform)
		{
			transform->setQuaternion(octoon::math::Quaternion(octoon::math::radians(profile_->sunModule->rotation)));
			transform->setTranslate(-octoon::math::rotate(transform->getQuaternion(), octoon::math::float3::UnitZ) * 60);
		}
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}

	void
	SunWindow::colorSelected(QColor color)
	{
		if (color.isValid())
		{
			auto sunLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
			if (sunLight)
			{
				profile_->sunModule->color = octoon::math::float3(color.redF(), color.greenF(), color.blueF());
				sunLight->setColor(octoon::math::srgb2linear(profile_->sunModule->color));
			}
		}

		auto x = editRotationX_->value() - 180.0f;
		auto y = editRotationY_->value() - 180.0f;
		auto z = editRotationZ_->value();
		profile_->sunModule->rotation = octoon::math::float3(x, y, z);

		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}

	void
	SunWindow::closeEvent()
	{
		this->colorSelected(colorDialog_->getCurrentColor());
	}

	void
	SunWindow::sliderIntensityEvent(int value)
	{
		auto envLight = profile_->entitiesModule->sunLight->getComponent<octoon::DirectionalLightComponent>();
		if (envLight)
			envLight->setIntensity(value / 10.0f);
		editIntensity_->setValue(value / 10.0f);
	}

	void
	SunWindow::editIntensityEvent(double value)
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