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
			this->spinBox->setMaximum(10.0f);
			this->spinBox->setSingleStep(0.03f);
			this->spinBox->setAlignment(Qt::AlignRight);
			this->spinBox->setValue(0.0f);

			this->rotationLabel_ = new QLabel;
			this->rotationLabel_->setText(u8"旋转");

			this->rotationSlider = new QSlider(Qt::Horizontal);
			this->rotationSlider->setObjectName("Value");
			this->rotationSlider->setMinimum(0);
			this->rotationSlider->setMaximum(100);
			this->rotationSlider->setValue(0);
			this->rotationSlider->setFixedWidth(270);

			this->rotationSpinBox = new DoubleSpinBox;
			this->rotationSpinBox->setFixedWidth(50);
			this->rotationSpinBox->setMaximum(1.0f);
			this->rotationSpinBox->setSingleStep(0.03f);
			this->rotationSpinBox->setAlignment(Qt::AlignRight);
			this->rotationSpinBox->setValue(0.0f);

			auto HLayout = new QHBoxLayout();
			HLayout->addWidget(this->label_, 0, Qt::AlignLeft);
			HLayout->addWidget(this->spinBox, 0, Qt::AlignRight);

			auto rotationLayout = new QHBoxLayout();
			rotationLayout->addWidget(this->rotationLabel_, 0, Qt::AlignLeft);
			rotationLayout->addWidget(this->rotationSpinBox, 0, Qt::AlignRight);

			auto layout = new QVBoxLayout();
			layout->addLayout(this->mapLayout);
			layout->addLayout(HLayout);
			layout->addWidget(this->slider);
			layout->addLayout(rotationLayout);
			layout->addWidget(this->rotationSlider);
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
		this->imageLabel_->setFixedSize(QSize(256, 144));

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
		connect(colorMap_.rotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(rotationEditEvent(double)));
		connect(colorMap_.rotationSlider, SIGNAL(valueChanged(int)), this, SLOT(rotationSliderEvent(int)));
		connect(&mapColor_, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(colorChangeEvent(const QColor&)));
	}

	EnvironmentWindow::~EnvironmentWindow()
	{
		this->image_.reset();
	}

	void
	EnvironmentWindow::repaint()
	{
		auto w = this->imageLabel_->width();
		auto h = this->imageLabel_->height();
		auto c = QColor::fromRgbF(profile_->environmentModule->color.x, profile_->environmentModule->color.y, profile_->environmentModule->color.z);
		auto offset = this->profile_->environmentModule->offset;

		if (this->image_)
		{
			float* data_ = nullptr;
			auto srcWidth = this->image_->width();
			auto srcHeight = this->image_->height();
			auto pixels = std::make_unique<std::uint8_t[]>(w * h * 3);

			for (std::size_t y = 0; y < h; y++)
			{
				for (std::size_t x = 0; x < w; x++)
				{
					auto u = x / float(w) - offset.x;
					auto v = y / float(h) - offset.y;
					u -= std::floor(u);
					v -= std::floor(v);
					auto ui = int(u * srcWidth);
					auto vi = int(v * srcHeight);

					auto src = (vi * srcWidth + ui) * 3;
					auto dst = (y * w + x) * 3;
					auto color = this->image_->pixelColor(ui, vi);

					pixels[dst] = std::clamp<float>(color.redF() * c.red(), 0, 255);
					pixels[dst + 1] = std::clamp<float>(color.greenF() * c.green(), 0, 255);
					pixels[dst + 2] = std::clamp<float>(color.blueF() * c.blue(), 0, 255);
				}
			}

			imageLabel_->setPixmap(QPixmap::fromImage(QImage(pixels.get(), w, h, QImage::Format::Format_RGB888)));
		}
		else
		{
			QPixmap pixmap(w, h);
			QPainter painter(&pixmap);
			painter.setPen(Qt::NoPen);
			painter.fillRect(QRect(0, 0, w, h), c);
			imageLabel_->setPixmap(pixmap);
		}
	}

	void
	EnvironmentWindow::showEvent(QShowEvent* event)
	{
		this->colorMap_.spinBox->setValue(profile_->environmentModule->intensity);
		this->colorMap_.rotationSpinBox->setValue(profile_->environmentModule->offset.x);
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
					QString filepath = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("HDRi Files (*.hdr)"));
					if (!filepath.isEmpty())
					{
						auto texture = octoon::TextureLoader::load(filepath.toStdString());
						if (texture)
						{
							auto width = texture->getTextureDesc().getWidth();
							auto height = texture->getTextureDesc().getHeight();
							float* data_ = nullptr;

							if (texture->map(0, 0, width, height, 0, (void**)&data_))
							{
								auto size = width * height * 3;
								auto pixels = std::make_unique<std::uint8_t[]>(size);

								for (std::size_t i = 0; i < size; i += 3) {
									pixels[i] = std::clamp<float>(std::pow(data_[i], 1 / 2.2) * 255.0f, 0, 255);
									pixels[i + 1] = std::clamp<float>(std::pow(data_[i + 1], 1 / 2.2) * 255.0f, 0, 255);
									pixels[i + 2] = std::clamp<float>(std::pow(data_[i + 2], 1 / 2.2) * 255.0f, 0, 255);
								}

								texture->unmap();

								QImage qimage(pixels.get(), width, height, QImage::Format::Format_RGB888);

								QFontMetrics metrics(this->colorMap_.path->font());
								auto name = metrics.elidedText(QFileInfo(filepath).fileName(), Qt::ElideRight, this->colorMap_.path->width());

								this->colorMap_.path->setText(name);
								this->colorMap_.check->setCheckState(Qt::CheckState::Checked);
								this->colorMap_.image->setIcon(QIcon(QPixmap::fromImage(qimage.scaled(QSize(48, 30)))));
								this->colorMap_.texture = texture;
								this->image_ = std::make_shared<QImage>(qimage.scaled(imageLabel_->size()));
								behaviour->loadHDRi(texture);
								this->colorChangeEvent(QColor::fromRgbF(1, 1, 1));
							}
						}
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
				else if (this->colorMap_.texture)
				{
					behaviour->loadHDRi(this->colorMap_.texture);
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
			this->profile_->environmentModule->color = octoon::math::float3(color.redF(), color.greenF(), color.blueF());

			auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
			if (environmentLight)
				environmentLight->setColor(octoon::math::srgb2linear(profile_->environmentModule->color));

			auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
			if (meshRenderer)
				meshRenderer->getMaterial()->set("diffuse", octoon::math::srgb2linear(profile_->environmentModule->color));

			this->colorMap_.setColor(color);
			this->repaint();
		}
	}

	void
	EnvironmentWindow::intensitySliderEvent(int value)
	{
		this->colorMap_.spinBox->setValue(value / 10.0f);
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
	EnvironmentWindow::rotationSliderEvent(int value)
	{
		this->colorMap_.rotationSpinBox->setValue(value / 100.0f);
	}
	
	void
	EnvironmentWindow::rotationEditEvent(double value)
	{
		auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
		if (meshRenderer)
		{
			auto material = meshRenderer->getMaterial();
			if (material->isInstanceOf<octoon::material::MeshBasicMaterial>())
			{
				auto basicMaterial = material->downcast<octoon::material::MeshBasicMaterial>();
				basicMaterial->setOffset(octoon::math::float2(value, 0));
			}
		}

		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setOffset(octoon::math::float2(value, 0));

		this->profile_->environmentModule->offset = octoon::math::float2(value, 0);
		this->colorMap_.rotationSlider->setValue(value * 100.0f);
		this->repaint();
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

		this->colorMap_.setColor(QColor::fromRgbF(profile_->environmentModule->color.x, profile_->environmentModule->color.y, profile_->environmentModule->color.z));
		this->repaint();
	}
}