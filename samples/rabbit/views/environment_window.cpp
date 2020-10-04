#include "environment_window.h"
#include <octoon/environment_light_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/image/image.h>

#include <qpainter.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfileinfo.h>

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

	void
	EnvironmentWindow::MaterialUi::init(const QString& name, std::uint32_t flags)
	{
		this->image = new QToolButton;
		this->image->setIcon(QIcon::fromTheme(":res/icons/append2.png"));
		this->image->setIconSize(QSize(48, 48));

		this->check = new QCheckBox;

		this->title = new QLabel;
		this->title->setText(name + u8"贴图");

		this->path = new QLabel;
		this->path->setMinimumSize(QSize(160, 20));

		this->titleLayout = new QHBoxLayout;
		this->titleLayout->addWidget(check, 0, Qt::AlignLeft);
		this->titleLayout->addWidget(title, 0, Qt::AlignLeft);
		this->titleLayout->addStretch();
		this->titleLayout->setSpacing(0);
		this->titleLayout->setContentsMargins(0, 2, 0, 0);

		auto textLayout = new QHBoxLayout;
		textLayout->setSpacing(2);
		textLayout->setContentsMargins(0, 2, 0, 0);
		textLayout->addWidget(this->path, 0, Qt::AlignLeft | Qt::AlignCenter);
		textLayout->addStretch();

		if (flags & CreateFlags::ColorBit)
		{
			this->color = new QToolButton;
			this->color->setIconSize(QSize(50, 30));

			textLayout->addWidget(this->color, 0, Qt::AlignRight);
		}
		else
		{
			this->color = nullptr;
		}		

		this->rightLayout = new QVBoxLayout;
		this->rightLayout->setSpacing(0);
		this->rightLayout->setContentsMargins(0, 0, 0, 0);
		this->rightLayout->addLayout(this->titleLayout);
		this->rightLayout->addLayout(textLayout);
		this->rightLayout->addStretch();

		this->mapLayout = new QHBoxLayout;
		this->mapLayout->addWidget(image);
		this->mapLayout->addLayout(rightLayout);

		if (flags & CreateFlags::ValueBit)
		{
			this->label_ = new QLabel;
			this->label_->setText(name);

			this->slider = new QSlider(Qt::Horizontal);
			this->slider->setObjectName("Value");
			this->slider->setMinimum(0);
			this->slider->setMaximum(100);
			this->slider->setValue(0);
			this->slider->setFixedWidth(270);

			this->spinBox = new DoubleSpinBox;
			this->spinBox->setFixedWidth(50);
			this->spinBox->setMaximum(1.0f);
			this->spinBox->setSingleStep(0.03f);
			this->spinBox->setAlignment(Qt::AlignRight);
			this->spinBox->setValue(0.0f);

			auto HLayout = new QHBoxLayout();
			HLayout->addWidget(this->label_, 0, Qt::AlignLeft);
			HLayout->addWidget(this->spinBox, 0, Qt::AlignRight);

			auto layout = new QVBoxLayout();
			layout->addLayout(this->mapLayout);
			layout->addLayout(HLayout);
			layout->addWidget(this->slider);
			layout->setContentsMargins(20, 5, 50, 0);
			this->mainLayout = layout;

			if (flags & CreateFlags::SpoilerBit)
			{
				this->spoiler = new Spoiler(name);
				this->spoiler->setFixedWidth(340);
				this->spoiler->setContentLayout(*this->mainLayout);
			}
			else
			{
				this->spoiler = nullptr;
			}
		}
		else
		{
			this->label_ = nullptr;
			this->slider = nullptr;
			this->spinBox = nullptr;
			this->mapLayout->setContentsMargins(20, 5, 50, 0);

			if (flags & CreateFlags::SpoilerBit)
			{
				this->spoiler = new Spoiler(name);
				this->spoiler->setFixedWidth(340);
				this->spoiler->setContentLayout(*mapLayout);
			}
			else
			{
				this->spoiler = nullptr;
			}
		}
	}

	void
	EnvironmentWindow::MaterialUi::resetState()
	{
		this->texture = nullptr;
		if (this->path) this->path->clear();
		if (this->image) this->image->setIcon(QIcon::fromTheme(":res/icons/append2.png"));
	}

	void
	EnvironmentWindow::MaterialUi::setColor(const QColor& c, int w, int h)
	{
		QPixmap pixmap(w, h);
		QPainter painter(&pixmap);
		painter.setPen(Qt::NoPen);
		painter.fillRect(QRect(0, 0, w, h), c);
		this->color->setIcon(QIcon(pixmap));
	}

	EnvironmentWindow::EnvironmentWindow(QWidget* widget, const octoon::GameObjectPtr& behaviour, const std::shared_ptr<rabbit::RabbitProfile>& profile)
		: QWidget(widget)
		, profile_(profile)
		, behaviour_(behaviour)
	{
		this->setObjectName("environmentWindow");
		this->setMinimumWidth(340);
		this->hide();

		this->title_ = new QLabel();
		this->title_->setText(u8"环境光");

		this->closeButton_ = new QToolButton();
		this->closeButton_->setObjectName("close");
		this->closeButton_->setToolTip(u8"关闭");

		this->imageLabel_ = new QLabel();
		this->imageLabel_->setFixedSize(QSize(128, 128));

		this->path = new QLabel;
		this->path->setText(u8"未命名");

		resetButton_ = new QToolButton();
		resetButton_->setText(u8"重置");

		auto layout = new QHBoxLayout();
		layout->addSpacing(this->closeButton_->iconSize().width());
		layout->addStretch();
		layout->addWidget(title_, 0, Qt::AlignCenter);
		layout->addStretch();
		layout->addWidget(closeButton_, 0, Qt::AlignRight);

		auto imageLayout = new QHBoxLayout();
		imageLayout->addStretch();
		imageLayout->addWidget(imageLabel_, 0, Qt::AlignCenter);
		imageLayout->addStretch();

		this->colorMap_.init(u8"环境", CreateFlags::SpoilerBit | CreateFlags::ColorBit | CreateFlags::ValueBit);
		this->colorMap_.setColor(QColor::fromRgbF(profile_->environmentModule->color.x, profile_->environmentModule->color.y, profile_->environmentModule->color.z));
		this->colorMap_.spinBox->setMaximum(10.0f);
		this->colorMap_.spinBox->setSingleStep(0.1f);
		this->colorMap_.spinBox->setAlignment(Qt::AlignRight);
		this->colorMap_.spinBox->setValue(profile->environmentModule->intensity);
		this->colorMap_.slider->setMinimum(0);
		this->colorMap_.slider->setMaximum(100);
		this->colorMap_.slider->setValue(profile->environmentModule->intensity * 10.0f);
		this->colorMap_.spoiler->toggleButton.click();

		mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addLayout(layout);
		mainLayout_->addLayout(imageLayout);
		mainLayout_->addWidget(path, 0, Qt::AlignCenter);
		mainLayout_->addWidget(colorMap_.spoiler);
		mainLayout_->addStretch();
		mainLayout_->addWidget(resetButton_, 0, Qt::AlignBottom | Qt::AlignRight);
		mainLayout_->setContentsMargins(10, 10, 10, 10);

		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(resetButton_, SIGNAL(clicked()), this, SLOT(resetEvent()));
		connect(colorMap_.image, SIGNAL(clicked()), this, SLOT(colorMapClickEvent()));
		connect(colorMap_.check, SIGNAL(stateChanged(int)), this, SLOT(colorMapCheckEvent(int)));
		connect(colorMap_.color, SIGNAL(clicked()), this, SLOT(colorClickEvent()));
		connect(colorMap_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(intensityEditEvent(double)));
		connect(colorMap_.slider, SIGNAL(valueChanged(int)), this, SLOT(intensitySliderEvent(int)));
		connect(&mapColor_, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(colorChangeEvent(const QColor&)));
	}

	EnvironmentWindow::~EnvironmentWindow()
	{
	}

	void
	EnvironmentWindow::repaint()
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
			if (meshRenderer)
			{
				auto material = meshRenderer->getMaterial();

				auto materialComponent = behaviour->getComponent<MaterialComponent>();
				QPixmap pixmap;
				materialComponent->repaintMaterial(material, pixmap, imageLabel_->width(), imageLabel_->height());
				material->setDirty(true);
				imageLabel_->setPixmap(pixmap);
			}
		}
	}

	void
	EnvironmentWindow::showEvent(QShowEvent* event)
	{
		this->colorMap_.spinBox->setValue(profile_->environmentModule->intensity);
		this->colorMap_.setColor(QColor::fromRgbF(profile_->environmentModule->color.x, profile_->environmentModule->color.y, profile_->environmentModule->color.z));

		this->repaint();
	}

	void
	EnvironmentWindow::closeEvent(QCloseEvent* event)
	{
	}

	void
	EnvironmentWindow::colorMapClickEvent()
	{
		try
		{
			if (behaviour_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					QString fileName = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("HDRi Files (*.hdr)"));
					if (!fileName.isEmpty())
					{
						QFontMetrics metrics(this->colorMap_.path->font());
						auto name = metrics.elidedText(QFileInfo(fileName).fileName(), Qt::ElideRight, this->colorMap_.path->width());

						this->colorMap_.path->setText(name);
						this->colorMap_.check->setCheckState(Qt::CheckState::Checked);
						this->colorMap_.image->setIcon(QIcon(fileName));
						this->colorMap_.texture = fileName;
						behaviour->loadHDRi(fileName.toUtf8().data());
						this->colorChangeEvent(QColor::fromRgbF(1, 1, 1));
						this->repaint();
					}
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
				}
			}
		}
		catch (const std::exception & e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void
	EnvironmentWindow::colorMapCheckEvent(int state)
	{
		if (behaviour_)
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour->isOpen())
			{
				if (state == Qt::Unchecked)
				{
					behaviour->clearHDRi();
					this->repaint();
				}
				else if (!this->colorMap_.texture.isEmpty())
				{
					behaviour->loadHDRi(this->colorMap_.texture.toUtf8().data());
					this->repaint();
				}
			}
		}
	}

	void
	EnvironmentWindow::colorClickEvent()
	{
		mapColor_.setCurrentColor(QColor::fromRgbF(this->profile_->environmentModule->color.x, this->profile_->environmentModule->color.y, this->profile_->environmentModule->color.z));
		mapColor_.show();
	}

	void 
	EnvironmentWindow::colorChangeEvent(const QColor& color)
	{
		if (color.isValid())
		{
			auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
			auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
			this->profile_->environmentModule->color = octoon::math::float3(color.redF(), color.greenF(), color.blueF());

			if (environmentLight)
				environmentLight->setColor(octoon::math::srgb2linear(profile_->environmentModule->color));

			if (meshRenderer)
				meshRenderer->getMaterial()->set("diffuse", octoon::math::srgb2linear(profile_->environmentModule->color));

			this->colorMap_.setColor(color);
			this->repaint();
		}
	}

	void
	EnvironmentWindow::intensitySliderEvent(int value)
	{
		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setIntensity(value / 10.0f);
		this->colorMap_.spinBox->setValue(value / 10.0f);
		this->profile_->environmentModule->intensity = value / 10.0f;
	}

	void
	EnvironmentWindow::intensityEditEvent(double value)
	{
		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setIntensity(value);
		this->colorMap_.slider->setValue(value * 10.0f);
		this->profile_->environmentModule->intensity = value * 10.0f;
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

		profile_->environmentModule->intensity = 1.f;
		profile_->environmentModule->color = octoon::math::float3(229.f, 229.f, 235.f) / 255.f;

		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
		{
			environmentLight->setColor(octoon::math::srgb2linear(profile_->environmentModule->color));
			environmentLight->setIntensity(profile_->environmentModule->intensity);
		}

		auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
		if (meshRenderer)
			meshRenderer->getMaterial()->set("diffuse", octoon::math::srgb2linear(profile_->environmentModule->color));

		this->repaint();
	}
}