#include "environment_window.h"
#include <octoon/environment_light_component.h>
#include <octoon/mesh_renderer_component.h>

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

	EnvironmentWindow::EnvironmentWindow(QWidget* widget, const std::shared_ptr<rabbit::RabbitProfile>& profile)
		: QWidget(widget)
		, profile_(profile)
	{
		this->setObjectName("environmentWindow");
		this->setMinimumWidth(340);
		this->hide();

		title_ = std::make_unique<QLabel>();
		title_->setText(u8"环境光");

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		colorDialog_ = std::make_unique<ColorDialog>();
		colorDialog_->setCurrentColor(QColor(profile->environmentModule->color.x * 255.0f, profile->environmentModule->color.y * 255.0f, profile->environmentModule->color.z * 255.0f));

		resetButton_ = std::make_unique<QToolButton>();
		resetButton_->setText(u8"重置");

		labelIntensity_ = std::make_unique<QLabel>();
		labelIntensity_->setText(u8"光强");

		editIntensity_ = std::make_unique<DoubleSpinBox>();
		editIntensity_->setFixedWidth(50);
		editIntensity_->setMaximum(10.0f);
		editIntensity_->setSingleStep(0.1f);
		editIntensity_->setAlignment(Qt::AlignRight);
		editIntensity_->setValue(profile->environmentModule->intensity);

		sliderIntensity_ = std::make_unique<QSlider>();
		sliderIntensity_->setObjectName("Value");
		sliderIntensity_->setOrientation(Qt::Horizontal);
		sliderIntensity_->setMinimum(0);
		sliderIntensity_->setMaximum(100);
		sliderIntensity_->setValue(profile->environmentModule->intensity * 10.0f);
		sliderIntensity_->setFixedWidth(240);

		layout_ = std::make_unique<QHBoxLayout>();
		layout_->addWidget(title_.get(), 0, Qt::AlignLeft);
		layout_->addWidget(closeButton_.get(), 0, Qt::AlignRight);

		layoutIntensity_ = std::make_unique<QHBoxLayout>();
		layoutIntensity_->addWidget(labelIntensity_.get(), 0, Qt::AlignLeft);
		layoutIntensity_->addWidget(editIntensity_.get(), 0, Qt::AlignRight);
		layoutIntensity_->setContentsMargins(40, 5, 40, 0);

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addLayout(layout_.get());
		mainLayout_->addWidget(colorDialog_.get(), 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addLayout(layoutIntensity_.get(), 0);
		mainLayout_->addWidget(sliderIntensity_.get(), 0, Qt::AlignHCenter);
		mainLayout_->addStretch();
		mainLayout_->addWidget(resetButton_.get(), 0, Qt::AlignBottom | Qt::AlignRight);
		mainLayout_->setContentsMargins(0, 10, 10, 10);

		connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(resetButton_.get(), SIGNAL(clicked()), this, SLOT(resetEvent()));
		connect(editIntensity_.get(), SIGNAL(valueChanged(double)), this, SLOT(editIntensityEvent(double)));
		connect(sliderIntensity_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderIntensityEvent(int)));
		connect(colorDialog_.get(), SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChanged(QColor)));
	}

	EnvironmentWindow::~EnvironmentWindow()
	{
		title_.reset();
		closeButton_.reset();
		layout_.reset();
		labelIntensity_.reset();
		editIntensity_.reset();
		layoutIntensity_.reset();
	}

	void
	EnvironmentWindow::repaint()
	{
		editIntensity_->setValue(profile_->environmentModule->intensity);
		colorDialog_->setCurrentColor(QColor(profile_->environmentModule->color.x * 255.0f, profile_->environmentModule->color.y * 255.0f, profile_->environmentModule->color.z * 255.0f));
	}

	void
	EnvironmentWindow::showEvent(QShowEvent* event)
	{
		this->repaint();
	}

	void
	EnvironmentWindow::closeEvent(QCloseEvent* event)
	{
		auto color = colorDialog_->getCurrentColor();
		profile_->environmentModule->intensity = editIntensity_->value();
		profile_->environmentModule->color = octoon::math::float3(color.redF(), color.greenF(), color.blueF());
	}

	void 
	EnvironmentWindow::currentColorChanged(QColor color)
	{
		if (color.isValid())
		{
			auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
			auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();

			if (environmentLight)
				environmentLight->setColor(octoon::math::srgb2linear(octoon::math::float3(color.redF(), color.greenF(), color.blueF())));

			if (meshRenderer)
				meshRenderer->getMaterial()->set("diffuse", octoon::math::srgb2linear(octoon::math::float3(color.redF(), color.greenF(), color.blueF())));
		}
	}

	void
	EnvironmentWindow::closeEvent()
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}

	void
	EnvironmentWindow::resetEvent()
	{
		this->repaint();

		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
		{
			environmentLight->setColor(octoon::math::srgb2linear(profile_->environmentModule->color));
			environmentLight->setIntensity(profile_->environmentModule->intensity);
		}

		auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
		if (meshRenderer)
			meshRenderer->getMaterial()->set("diffuse", octoon::math::srgb2linear(profile_->environmentModule->color));
	}

	void
	EnvironmentWindow::sliderIntensityEvent(int value)
	{
		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setIntensity(value / 10.0f);
		editIntensity_->setValue(value / 10.0f);
	}

	void
	EnvironmentWindow::editIntensityEvent(double value)
	{
		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setIntensity(value);
		sliderIntensity_->setValue(value * 10.0f);
	}
}