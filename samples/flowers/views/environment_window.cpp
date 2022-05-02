#include "environment_window.h"
#include <octoon/environment_light_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/image/image.h>
#include <octoon/PMREM_loader.h>

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

		this->previewButton_ = new QToolButton();
		this->previewButton_->setFixedSize(QSize(256, 144));

		this->previewName_ = new QLabel;
		this->previewName_->setText(u8"未命名");

		this->colorButton = new QToolButton;
		this->colorButton->setIconSize(QSize(50, 30));

		this->thumbnail = new QToolButton;
		this->thumbnail->setIcon(QIcon::fromTheme(":res/icons/append2.png"));
		this->thumbnail->setIconSize(QSize(48, 48));

		this->thumbnailToggle = new QCheckBox;
		this->thumbnailToggle->setText(u8"环境贴图");

		this->backgroundToggle = new QCheckBox;
		this->backgroundToggle->setText(u8"显示背景");
		this->backgroundToggle->setChecked(true);

		this->thumbnailPath = new QLabel;
		this->thumbnailPath->setMinimumSize(QSize(160, 20));

		this->intensityLabel_ = new QLabel;
		this->intensityLabel_->setText(u8"光强");

		this->intensitySlider = new QSlider(Qt::Horizontal);
		this->intensitySlider->setObjectName("Value");
		this->intensitySlider->setMinimum(0);
		this->intensitySlider->setMaximum(100);
		this->intensitySlider->setValue(0);
		this->intensitySlider->setFixedWidth(270);

		this->intensitySpinBox = new DoubleSpinBox;
		this->intensitySpinBox->setFixedWidth(50);
		this->intensitySpinBox->setMaximum(10.0f);
		this->intensitySpinBox->setSingleStep(0.1f);
		this->intensitySpinBox->setAlignment(Qt::AlignRight);
		this->intensitySpinBox->setValue(0.0f);
		this->intensitySpinBox->setDecimals(1);

		this->horizontalRotationLabel_ = new QLabel;
		this->horizontalRotationLabel_->setText(u8"水平旋转");

		this->horizontalRotationSlider = new QSlider(Qt::Horizontal);
		this->horizontalRotationSlider->setObjectName("Value");
		this->horizontalRotationSlider->setMinimum(-100);
		this->horizontalRotationSlider->setMaximum(100);
		this->horizontalRotationSlider->setValue(0);
		this->horizontalRotationSlider->setFixedWidth(270);

		this->horizontalRotationSpinBox = new DoubleSpinBox;
		this->horizontalRotationSpinBox->setFixedWidth(50);
		this->horizontalRotationSpinBox->setMinimum(-1.0f);
		this->horizontalRotationSpinBox->setMaximum(1.0f);
		this->horizontalRotationSpinBox->setSingleStep(0.03f);
		this->horizontalRotationSpinBox->setAlignment(Qt::AlignRight);
		this->horizontalRotationSpinBox->setValue(0.0f);

		this->verticalRotationLabel_ = new QLabel;
		this->verticalRotationLabel_->setText(u8"垂直旋转");

		this->verticalRotationSlider = new QSlider(Qt::Horizontal);
		this->verticalRotationSlider->setObjectName("Value");
		this->verticalRotationSlider->setMinimum(-100);
		this->verticalRotationSlider->setMaximum(100);
		this->verticalRotationSlider->setValue(0);
		this->verticalRotationSlider->setFixedWidth(270);

		this->verticalRotationSpinBox = new DoubleSpinBox;
		this->verticalRotationSpinBox->setFixedWidth(50);
		this->verticalRotationSpinBox->setMinimum(-1.0f);
		this->verticalRotationSpinBox->setMaximum(1.0f);
		this->verticalRotationSpinBox->setSingleStep(0.03f);
		this->verticalRotationSpinBox->setAlignment(Qt::AlignRight);
		this->verticalRotationSpinBox->setValue(0.0f);

		this->resetButton_ = new QToolButton();
		this->resetButton_->setText(u8"重置");

		auto thumbnailTitleLayout = new QHBoxLayout();
		thumbnailTitleLayout->addWidget(thumbnailToggle, 0, Qt::AlignLeft);
		thumbnailTitleLayout->addSpacing(4);
		thumbnailTitleLayout->addWidget(backgroundToggle, 0, Qt::AlignLeft);
		thumbnailTitleLayout->addStretch();
		thumbnailTitleLayout->setSpacing(0);
		thumbnailTitleLayout->setContentsMargins(0, 2, 0, 0);

		auto thumbnailTextLayout = new QHBoxLayout;
		thumbnailTextLayout->setSpacing(2);
		thumbnailTextLayout->setContentsMargins(0, 2, 0, 0);
		thumbnailTextLayout->addWidget(this->thumbnailPath, 0, Qt::AlignLeft | Qt::AlignCenter);
		thumbnailTextLayout->addStretch();
		thumbnailTextLayout->addWidget(this->colorButton, 0, Qt::AlignRight);

		auto thumbnailRightLayout = new QVBoxLayout;
		thumbnailRightLayout->setSpacing(0);
		thumbnailRightLayout->setContentsMargins(0, 0, 0, 0);
		thumbnailRightLayout->addLayout(thumbnailTitleLayout);
		thumbnailRightLayout->addLayout(thumbnailTextLayout);
		thumbnailRightLayout->addStretch();

		auto thumbnailLayout = new QHBoxLayout;
		thumbnailLayout->addWidget(thumbnail);
		thumbnailLayout->addLayout(thumbnailRightLayout);

		auto intensityLayout = new QHBoxLayout();
		intensityLayout->addWidget(this->intensityLabel_, 0, Qt::AlignLeft);
		intensityLayout->addWidget(this->intensitySpinBox, 0, Qt::AlignRight);

		auto horizontalRotationLayout = new QHBoxLayout();
		horizontalRotationLayout->addWidget(this->horizontalRotationLabel_, 0, Qt::AlignLeft);
		horizontalRotationLayout->addWidget(this->horizontalRotationSpinBox, 0, Qt::AlignRight);

		auto verticalRotationLayout = new QHBoxLayout();
		verticalRotationLayout->addWidget(this->verticalRotationLabel_, 0, Qt::AlignLeft);
		verticalRotationLayout->addWidget(this->verticalRotationSpinBox, 0, Qt::AlignRight);

		auto spoilerLayout = new QVBoxLayout();
		spoilerLayout->addLayout(thumbnailLayout);
		spoilerLayout->addLayout(intensityLayout);
		spoilerLayout->addWidget(this->intensitySlider);
		spoilerLayout->addLayout(horizontalRotationLayout);
		spoilerLayout->addWidget(this->horizontalRotationSlider);
		spoilerLayout->addLayout(verticalRotationLayout);
		spoilerLayout->addWidget(this->verticalRotationSlider);
		spoilerLayout->setContentsMargins(20, 5, 50, 0);

		this->spoiler = new Spoiler(u8"环境属性");
		this->spoiler->setFixedWidth(340);
		this->spoiler->setContentLayout(*spoilerLayout);
		this->spoiler->toggleButton.click();

		auto titleLayout = new QHBoxLayout();
		titleLayout->addSpacing(this->closeButton_->iconSize().width());
		titleLayout->addStretch();
		titleLayout->addWidget(title_, 0, Qt::AlignCenter);
		titleLayout->addStretch();
		titleLayout->addWidget(closeButton_, 0, Qt::AlignRight);

		auto imageLayout = new QHBoxLayout();
		imageLayout->addStretch();
		imageLayout->addWidget(previewButton_, 0, Qt::AlignCenter);
		imageLayout->addStretch();

		auto mainLayout = new QVBoxLayout(this);
		mainLayout->addLayout(titleLayout);
		mainLayout->addLayout(imageLayout);
		mainLayout->addWidget(previewName_, 0, Qt::AlignCenter);
		mainLayout->addWidget(spoiler);
		mainLayout->addStretch();
		mainLayout->addWidget(resetButton_, 0, Qt::AlignBottom | Qt::AlignRight);
		mainLayout->setContentsMargins(10, 10, 10, 10);

		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(resetButton_, SIGNAL(clicked()), this, SLOT(resetEvent()));
		connect(thumbnail, SIGNAL(clicked()), this, SLOT(colorMapClickEvent()));
		connect(thumbnailToggle, SIGNAL(stateChanged(int)), this, SLOT(colorMapCheckEvent(int)));
		connect(backgroundToggle, SIGNAL(stateChanged(int)), this, SLOT(backgroundMapCheckEvent(int)));
		connect(colorButton, SIGNAL(clicked()), this, SLOT(colorClickEvent()));
		connect(intensitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(intensityEditEvent(double)));
		connect(intensitySlider, SIGNAL(valueChanged(int)), this, SLOT(intensitySliderEvent(int)));
		connect(horizontalRotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(horizontalRotationEditEvent(double)));
		connect(horizontalRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(horizontalRotationSliderEvent(int)));
		connect(verticalRotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(verticalRotationEditEvent(double)));
		connect(verticalRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(verticalRotationSliderEvent(int)));
		connect(&colorSelector_, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(colorChangeEvent(const QColor&)));
	}

	EnvironmentWindow::~EnvironmentWindow()
	{
		this->image_.reset();
	}

	void
	EnvironmentWindow::setColor(const QColor& c, int w, int h)
	{
		this->profile_->environmentModule->color = octoon::math::float3(c.redF(), c.greenF(), c.blueF());

		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setColor(octoon::math::srgb2linear(profile_->environmentModule->color));

		auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
		if (meshRenderer)
			meshRenderer->getMaterial()->set("diffuse", octoon::math::srgb2linear(profile_->environmentModule->color));

		QPixmap pixmap(w, h);
		QPainter painter(&pixmap);
		painter.setPen(Qt::NoPen);
		painter.fillRect(QRect(0, 0, w, h), c);
		this->colorButton->setIcon(QIcon(pixmap));
	}

	void
	EnvironmentWindow::repaint()
	{
		auto w = this->previewButton_->width();
		auto h = this->previewButton_->height();
		auto c = QColor::fromRgbF(profile_->environmentModule->color.x, profile_->environmentModule->color.y, profile_->environmentModule->color.z);
		auto offset = this->profile_->environmentModule->offset;

		if (this->image_ && this->thumbnailToggle->isChecked())
		{
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

					auto dst = (y * w + x) * 3;
					auto color = this->image_->pixelColor(ui, vi);

					pixels[dst] = std::clamp<float>(color.redF() * c.red(), 0, 255);
					pixels[dst + 1] = std::clamp<float>(color.greenF() * c.green(), 0, 255);
					pixels[dst + 2] = std::clamp<float>(color.blueF() * c.blue(), 0, 255);
				}
			}

			previewButton_->setIcon(QPixmap::fromImage(QImage(pixels.get(), w, h, QImage::Format::Format_RGB888)));
			previewButton_->setIconSize(QSize(w, h));
		}
		else
		{
			QPixmap pixmap(w, h);
			QPainter painter(&pixmap);
			painter.setPen(Qt::NoPen);
			painter.fillRect(QRect(0, 0, w, h), c);
			previewButton_->setIcon(pixmap);
			previewButton_->setIconSize(QSize(w, h));
		}
	}

	void
	EnvironmentWindow::showEvent(QShowEvent* event)
	{
		this->intensitySpinBox->setValue(profile_->environmentModule->intensity);
		this->horizontalRotationSpinBox->setValue(profile_->environmentModule->offset.x);
		this->verticalRotationSpinBox->setValue(profile_->environmentModule->offset.y);
		this->setColor(QColor::fromRgbF(profile_->environmentModule->color.x, profile_->environmentModule->color.y, profile_->environmentModule->color.z));

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
				QString filepath = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("HDRi Files (*.hdr)"));
				if (!filepath.isEmpty())
				{
					auto texel = octoon::TextureLoader::load(filepath.toStdString(), true);
					if (texel)
					{
						auto width = texel->getTextureDesc().getWidth();
						auto height = texel->getTextureDesc().getHeight();
						float* data_ = nullptr;

						if (texel->map(0, 0, width, height, 0, (void**)&data_))
						{
							auto size = width * height * 3;
							auto pixels = std::make_unique<std::uint8_t[]>(size);

							for (std::size_t i = 0; i < size; i += 3)
							{
								pixels[i] = std::clamp<float>(std::pow(data_[i], 1 / 2.2) * 255.0f, 0, 255);
								pixels[i + 1] = std::clamp<float>(std::pow(data_[i + 1], 1 / 2.2) * 255.0f, 0, 255);
								pixels[i + 2] = std::clamp<float>(std::pow(data_[i + 2], 1 / 2.2) * 255.0f, 0, 255);
							}

							texel->unmap();

							QImage qimage(pixels.get(), width, height, QImage::Format::Format_RGB888);

							QFontMetrics metrics(this->thumbnailPath->font());
							auto name = metrics.elidedText(QFileInfo(filepath).fileName(), Qt::ElideRight, this->thumbnailPath->width());

							this->thumbnailPath->setText(name);
							this->thumbnailToggle->setChecked(false);
							this->thumbnail->setIcon(QIcon(QPixmap::fromImage(qimage.scaled(QSize(48, 30)))));
							this->texture = texel;
							this->image_ = std::make_shared<QImage>(qimage.scaled(previewButton_->size()));
							this->setColor(QColor::fromRgbF(1, 1, 1));
							this->thumbnailToggle->setChecked(true);
						}
					}
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
		auto& environmentLight = this->profile_->entitiesModule->enviromentLight;
		if (environmentLight)
		{
			auto envLight = environmentLight->getComponent<octoon::EnvironmentLightComponent>();
			if (envLight)
			{
				envLight->setBackgroundMap(state == Qt::Checked ? this->texture : nullptr);
				envLight->setEnvironmentMap(state == Qt::Checked ? octoon::PMREMLoader::load(this->texture) : nullptr);
			}

			auto material = environmentLight->getComponent<octoon::MeshRendererComponent>()->getMaterial()->downcast<octoon::MeshBasicMaterial>();
			material->setColorMap(state == Qt::Checked && backgroundToggle->isChecked() ? this->texture : nullptr);
		}

		this->repaint();
	}

	void
	EnvironmentWindow::backgroundMapCheckEvent(int state)
	{
		auto& environmentLight = this->profile_->entitiesModule->enviromentLight;
		if (environmentLight)
		{
			auto envLight = environmentLight->getComponent<octoon::EnvironmentLightComponent>();
			if (envLight)
				envLight->setShowBackground(state == Qt::Checked);

			environmentLight->getComponent<octoon::MeshRendererComponent>()->setActive(state == Qt::Checked);
		}
	}

	void
	EnvironmentWindow::colorClickEvent()
	{
		colorSelector_.setCurrentColor(QColor::fromRgbF(this->profile_->environmentModule->color.x, this->profile_->environmentModule->color.y, this->profile_->environmentModule->color.z));
		colorSelector_.show();
	}

	void 
	EnvironmentWindow::colorChangeEvent(const QColor& color)
	{
		this->setColor(color);
		this->repaint();
	}

	void
	EnvironmentWindow::intensitySliderEvent(int value)
	{
		this->intensitySpinBox->setValue(value / 10.0f);
	}

	void
	EnvironmentWindow::intensityEditEvent(double value)
	{
		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setIntensity(value);
		this->intensitySlider->setValue(value * 10.0f);
		this->profile_->environmentModule->intensity = value;
	}

	void
	EnvironmentWindow::horizontalRotationSliderEvent(int value)
	{
		this->horizontalRotationSpinBox->setValue(value / 100.0f);
	}
	
	void
	EnvironmentWindow::horizontalRotationEditEvent(double value)
	{
		this->profile_->environmentModule->offset = octoon::math::float2(value, this->profile_->environmentModule->offset.y);

		auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
		if (meshRenderer)
		{
			auto material = meshRenderer->getMaterial();
			if (material->isInstanceOf<octoon::MeshBasicMaterial>())
			{
				auto basicMaterial = material->downcast<octoon::MeshBasicMaterial>();
				basicMaterial->setOffset(this->profile_->environmentModule->offset);
			}
		}

		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setOffset(this->profile_->environmentModule->offset);

		this->horizontalRotationSlider->setValue(value * 100.0f);
		this->repaint();
	}

	void
	EnvironmentWindow::verticalRotationSliderEvent(int value)
	{
		this->verticalRotationSpinBox->setValue(value / 100.0f);
	}
	
	void
	EnvironmentWindow::verticalRotationEditEvent(double value)
	{
		this->profile_->environmentModule->offset = octoon::math::float2(this->profile_->environmentModule->offset.x, value);

		auto meshRenderer = profile_->entitiesModule->enviromentLight->getComponent<octoon::MeshRendererComponent>();
		if (meshRenderer)
		{
			auto material = meshRenderer->getMaterial();
			if (material->isInstanceOf<octoon::MeshBasicMaterial>())
			{
				auto basicMaterial = material->downcast<octoon::MeshBasicMaterial>();
				basicMaterial->setOffset(this->profile_->environmentModule->offset);
			}
		}

		auto environmentLight = profile_->entitiesModule->enviromentLight->getComponent<octoon::EnvironmentLightComponent>();
		if (environmentLight)
			environmentLight->setOffset(this->profile_->environmentModule->offset);
		
		this->verticalRotationSlider->setValue(value * 100.0f);
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
		this->intensitySpinBox->setValue(1.0f);
		this->thumbnailToggle->setChecked(false);
		this->horizontalRotationSpinBox->setValue(0.0f);
		this->verticalRotationSpinBox->setValue(0.0f);
		this->setColor(QColor::fromRgb(229, 229, 235));
		this->repaint();
	}
}