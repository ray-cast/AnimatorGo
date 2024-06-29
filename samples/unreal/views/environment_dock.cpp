#include "environment_dock.h"
#include "../utils/asset_library.h"
#include <octoon/asset_database.h>
#include <octoon/asset_importer.h>

#include <qapplication.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qmimedata.h>
#include <qpainter.h>
#include <qprogressdialog.h>
#include <qgraphicseffect.h>

namespace unreal
{
	EnvironmentDock::EnvironmentDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile)
		: behaviour_(behaviour)
		, profile_(profile)
		, environmentListDialog_(nullptr)
	{
		this->setWindowTitle(tr("Environment Light"));
		this->setObjectName("EnvironmentDock");
		this->installEventFilter(this);

		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		this->title_ = new QLabel;
		this->title_->setObjectName("title");
		this->title_->setText(tr("Environment Editor"));

		this->headerLine_ = new QFrame;
		this->headerLine_->setObjectName("HLine");
		this->headerLine_->setFixedHeight(1);
		this->headerLine_->setFrameShape(QFrame::NoFrame);
		this->headerLine_->setFrameShadow(QFrame::Plain);
		this->headerLine_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

		auto titleLayout_ = new QVBoxLayout();
		titleLayout_->addWidget(title_, 0, Qt::AlignLeft);
		titleLayout_->addSpacing(8);
		titleLayout_->addWidget(headerLine_);
		titleLayout_->setSpacing(0);
		titleLayout_->setContentsMargins(4, 0, 4, 0);

		this->previewButton_ = new QToolButton();
		this->previewButton_->setObjectName("Preview");
		this->previewButton_->setIconSize(QSize(200, 100));
		this->previewButton_->setToolTip(tr("Click the select a Preview button to locate each HDRi on your computer"));
		this->previewButton_->installEventFilter(this);

		this->previewName_ = new QLabel;
		this->previewName_->setObjectName("PreviewName");
		this->previewName_->setText(tr("Untitled"));
		this->previewName_->setAlignment(Qt::AlignCenter);
		this->previewName_->setMinimumWidth(210);

		auto shadowEffect = std::make_unique<QGraphicsDropShadowEffect>();
		shadowEffect->setOffset(4, 4);
		shadowEffect->setColor(Qt::lightGray);
		shadowEffect->setBlurRadius(8);

		this->colorButton = new QToolButton;
		this->colorButton->setObjectName("Color");
		this->colorButton->setIconSize(QSize(40, 24));
		this->colorButton->installEventFilter(this);
		this->colorButton->setGraphicsEffect(shadowEffect.release());

		this->thumbnail = new QToolButton;
		this->thumbnail->setObjectName("Thumbnail ");
		this->thumbnail->setIcon(QIcon::fromTheme(":res/icons/append2.png"));
		this->thumbnail->setIconSize(QSize(38, 38));
		this->thumbnail->setToolTip(tr("Open"));
		this->thumbnail->installEventFilter(this);

		this->thumbnailToggle = new QCheckBox;
		this->thumbnailToggle->setText(tr("Thumbnail"));
		this->thumbnailToggle->installEventFilter(this);

		this->backgroundToggle = new QCheckBox;
		this->backgroundToggle->setText(tr("Toggle Background"));
		this->backgroundToggle->setChecked(true);
		this->backgroundToggle->installEventFilter(this);

		this->thumbnailPath = new QLabel;
		this->thumbnailPath->setMinimumSize(QSize(128, 16));

		this->intensityLabel_ = new QLabel;
		this->intensityLabel_->setText(tr("Intensity"));

		this->intensitySlider = new QSlider(Qt::Horizontal);
		this->intensitySlider->setMinimum(0);
		this->intensitySlider->setMaximum(100);
		this->intensitySlider->setValue(0);
		this->intensitySlider->setMinimumWidth(215);
		this->intensitySlider->installEventFilter(this);

		this->intensitySpinBox = new UDoubleSpinBox;
		this->intensitySpinBox->setMaximum(20.0f);
		this->intensitySpinBox->setSingleStep(0.1f);
		this->intensitySpinBox->setAlignment(Qt::AlignRight);
		this->intensitySpinBox->setValue(0.0f);
		this->intensitySpinBox->setDecimals(1);
		this->intensitySpinBox->installEventFilter(this);
		
		this->horizontalRotationLabel_ = new QLabel;
		this->horizontalRotationLabel_->setText(tr("Horizontal Rotation"));

		this->horizontalRotationSlider = new QSlider(Qt::Horizontal);
		this->horizontalRotationSlider->setMinimum(-100);
		this->horizontalRotationSlider->setMaximum(100);
		this->horizontalRotationSlider->setValue(0);
		this->horizontalRotationSlider->setMinimumWidth(215);
		this->horizontalRotationSlider->installEventFilter(this);
		
		this->horizontalRotationSpinBox = new UDoubleSpinBox;
		this->horizontalRotationSpinBox->setMinimum(-1.0f);
		this->horizontalRotationSpinBox->setMaximum(1.0f);
		this->horizontalRotationSpinBox->setSingleStep(0.03f);
		this->horizontalRotationSpinBox->setAlignment(Qt::AlignRight);
		this->horizontalRotationSpinBox->setValue(0.0f);
		this->horizontalRotationSpinBox->installEventFilter(this);

		this->verticalRotationLabel_ = new QLabel;
		this->verticalRotationLabel_->setText(tr("Vertical Rotation"));

		this->verticalRotationSlider = new QSlider(Qt::Horizontal);
		this->verticalRotationSlider->setMinimum(-100);
		this->verticalRotationSlider->setMaximum(100);
		this->verticalRotationSlider->setValue(0);
		this->verticalRotationSlider->setMinimumWidth(215);
		this->verticalRotationSlider->installEventFilter(this);

		this->verticalRotationSpinBox = new UDoubleSpinBox;
		this->verticalRotationSpinBox->setMinimum(-1.0f);
		this->verticalRotationSpinBox->setMaximum(1.0f);
		this->verticalRotationSpinBox->setSingleStep(0.03f);
		this->verticalRotationSpinBox->setAlignment(Qt::AlignRight);
		this->verticalRotationSpinBox->setValue(0.0f);
		this->verticalRotationSpinBox->installEventFilter(this);

		this->resetButton_ = new QToolButton();
		this->resetButton_->setObjectName("Reset");
		this->resetButton_->setText(tr("Reset"));
		this->resetButton_->installEventFilter(this);

		this->colorSelector_ = new QColorDialog;
	
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
		spoilerLayout->setContentsMargins(15, 0, 15, 0);

		this->spoiler = new Spoiler(tr("Attribute"));
		this->spoiler->setContentLayout(*spoilerLayout);
		this->spoiler->toggleButton.click();

		auto contextLayout_ = new QVBoxLayout;
		contextLayout_->addWidget(previewButton_, 0, Qt::AlignCenter);
		contextLayout_->addWidget(previewName_, 0, Qt::AlignCenter);
		contextLayout_->addWidget(spoiler);
		contextLayout_->addStretch();
		contextLayout_->addWidget(resetButton_, 0, Qt::AlignBottom | Qt::AlignRight);
		contextLayout_->setContentsMargins(8, 4, 8, 0);

		auto mainLayout = new QVBoxLayout();
		mainLayout->addLayout(titleLayout_);
		mainLayout->addLayout(contextLayout_);
		mainLayout->setContentsMargins(0, 12, 0, 8);

		auto mainWidget = new QWidget();
		mainWidget->setObjectName("EnvironmentWidget");
		mainWidget->setLayout(mainLayout);
		
		this->setWidget(mainWidget);

		this->profile_->environmentLightModule->offset += [this](const octoon::math::float2& value) {
			horizontalRotationSlider->blockSignals(true);
			horizontalRotationSlider->setValue(value.x * 100.0f);
			horizontalRotationSlider->blockSignals(false);

			horizontalRotationSpinBox->blockSignals(true);
			horizontalRotationSpinBox->setValue(value.x);
			horizontalRotationSpinBox->blockSignals(false);

			verticalRotationSlider->blockSignals(true);
			verticalRotationSlider->setValue(value.y * 100.0f);
			verticalRotationSlider->blockSignals(false);

			verticalRotationSpinBox->blockSignals(true);
			verticalRotationSpinBox->setValue(value.y);
			verticalRotationSpinBox->blockSignals(false);

			this->updatePreviewImage();
		};

		this->profile_->environmentLightModule->color += [this](const octoon::math::float3& value) {
			this->setColor(QColor::fromRgbF(value.x, value.y, value.z));
			this->updatePreviewImage();
		};

		this->profile_->environmentLightModule->intensity += [this](float value) {
			intensitySlider->blockSignals(true);
			intensitySlider->setValue(value * 5.0f);
			intensitySlider->blockSignals(false);

			intensitySpinBox->blockSignals(true);
			intensitySpinBox->setValue(value);
			intensitySpinBox->blockSignals(false);
		};

		this->profile_->environmentLightModule->useTexture += [this](bool value) {
			thumbnailToggle->blockSignals(true);
			thumbnailToggle->setChecked(value);
			thumbnailToggle->blockSignals(false);

			updatePreviewImage();
		};

		this->profile_->environmentLightModule->texture += [this](const std::shared_ptr<octoon::Texture>& texture)
		{
			if (texture && this->isVisible())
			{
				auto package = AssetLibrary::instance()->getPackage(texture);
				if (package.is_object())
				{
					auto name = QString::fromUtf8(package["name"].get<nlohmann::json::string_t>());
					if (thumbnailPath->toolTip() != name)
					{
						auto previewImage = std::make_shared<QImage>();
						if (previewImage->load(QString::fromStdWString(AssetLibrary::instance()->getAssetPath(package["preview"].get<std::string>(), true).wstring())))
						{
							this->setPreviewImage(QFileInfo(name).fileName(), previewImage);
							this->setThumbnailImage(name, *previewImage);
							this->updatePreviewImage();
						}
						else
						{
							throw std::runtime_error("Cannot generate image for preview");
						}
					}
				}
				else
				{
					auto name = texture->getName();
					auto width = texture->width();
					auto height = texture->height();
					auto data_ = (float*)texture->data();
					auto bytesPerLine = (width * 24 + 31) / 32 * 4;
					auto pixels = std::make_unique<std::uint8_t[]>(height * bytesPerLine);

					for (std::size_t y = 0; y < height; y++)
					{
						for (std::size_t x = 0; x < width; x++)
						{
							auto src = (y * width + x) * 3;
							auto dest = y * bytesPerLine + x * 3;
							pixels[dest] = std::clamp<float>(std::pow(data_[src], 1.0f / 2.2f) * 255.0f, 0, 255);
							pixels[dest + 1] = std::clamp<float>(std::pow(data_[src + 1], 1.0f / 2.2f) * 255.0f, 0, 255);
							pixels[dest + 2] = std::clamp<float>(std::pow(data_[src + 2], 1.0f / 2.2f) * 255.0f, 0, 255);
						}
					}

					QImage qimage(pixels.get(), width, height, QImage::Format::Format_RGB888);
					auto dpi = QApplication::primaryScreen()->devicePixelRatio();
					auto iconSize = previewButton_->iconSize() * dpi;
					auto previewImage = std::make_shared<QImage>(qimage.scaled(iconSize));

					this->setPreviewImage(QFileInfo(QString::fromStdString(name)).fileName(), previewImage);
					this->setThumbnailImage(QString::fromStdString(name), *previewImage);
					this->updatePreviewImage();
				}
			}
			else
			{
				this->previewName_->setText(tr("Untitled"));
				this->thumbnailPath->clear();
				this->thumbnailPath->setToolTip(QString());
				this->thumbnail->setIcon(QIcon::fromTheme(":res/icons/append2.png"));
				this->updatePreviewImage();
			}
		};

		connect(previewButton_, SIGNAL(clicked(bool)), this, SLOT(previewClickEvent(bool)));
		connect(thumbnail, SIGNAL(clicked()), this, SLOT(thumbnailClickEvent()));
		connect(thumbnailToggle, SIGNAL(stateChanged(int)), this, SLOT(thumbnailToggleEvent(int)));
		connect(backgroundToggle, SIGNAL(stateChanged(int)), this, SLOT(backgroundMapCheckEvent(int)));
		connect(colorButton, SIGNAL(clicked()), this, SLOT(colorClickEvent()));
		connect(intensitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(intensityEditEvent(double)));
		connect(intensitySlider, SIGNAL(valueChanged(int)), this, SLOT(intensitySliderEvent(int)));
		connect(horizontalRotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(horizontalRotationEditEvent(double)));
		connect(horizontalRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(horizontalRotationSliderEvent(int)));
		connect(verticalRotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(verticalRotationEditEvent(double)));
		connect(verticalRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(verticalRotationSliderEvent(int)));
		connect(colorSelector_, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(colorChangeEvent(const QColor&)));
		connect(resetButton_, SIGNAL(clicked()), this, SLOT(resetEvent()));
	}

	EnvironmentDock::~EnvironmentDock()
	{
		this->previewImage_.reset();
	}

	void
	EnvironmentDock::setColor(const QColor& c, int w, int h)
	{
		QPixmap pixmap(w, h);
		QPainter painter(&pixmap);
		painter.setPen(Qt::NoPen);
		painter.fillRect(QRect(0, 0, w, h), c);
		this->colorButton->setIcon(QIcon(pixmap));
	}

	void
	EnvironmentDock::setPreviewImage(QString name, std::shared_ptr<QImage> image)
	{
		QFontMetrics previewMetrics(this->previewName_->font());

		this->previewImage_ = image;
		this->previewName_->setText(previewMetrics.elidedText(name, Qt::ElideRight, this->previewName_->width()));
	}

	void
	EnvironmentDock::setThumbnailImage(QString name, const QImage& image)
	{
		QFontMetrics thumbnailMetrics(this->thumbnailPath->font());

		this->thumbnailPath->setToolTip(name);
		this->thumbnailPath->setText(thumbnailMetrics.elidedText(this->thumbnailPath->toolTip(), Qt::ElideRight, this->thumbnailPath->width()));
		this->thumbnail->setIcon(QIcon(QPixmap::fromImage(image.scaled(QSize(38, 24)))));
	}

	void
	EnvironmentDock::updatePreviewImage()
	{
		auto w = this->previewButton_->width();
		auto h = this->previewButton_->height();
		auto color = this->profile_->environmentLightModule->color.getValue();
		auto c = QColor::fromRgbF(color.x, color.y, color.z);

		if (this->previewImage_ && this->profile_->environmentLightModule->useTexture && this->profile_->environmentLightModule->texture.getValue())
		{
			auto srcWidth = this->previewImage_->width();
			auto srcHeight = this->previewImage_->height();
			auto bytesPerLine = (srcWidth * 24 + 31) / 32 * 4;
			auto offset = this->profile_->environmentLightModule->offset.getValue();
			auto pixels = std::make_unique<std::uint8_t[]>(bytesPerLine * srcHeight);

			for (std::size_t y = 0; y < srcHeight; y++)
			{
				auto v = y / float(srcHeight - 1) - offset.y;
				v -= std::floor(v);
				auto vi = int(v * srcHeight);

				for (std::size_t x = 0; x < srcWidth; x++)
				{
					auto u = x / float(srcWidth - 1) - offset.x;
					u -= std::floor(u);
					auto ui = int(u * srcWidth);

					auto dst = y * bytesPerLine + x * 3;
					auto pixel = this->previewImage_->pixelColor(ui, vi);

					pixels[dst] = std::clamp<float>(pixel.redF() * c.red(), 0, 255);
					pixels[dst + 1] = std::clamp<float>(pixel.greenF() * c.green(), 0, 255);
					pixels[dst + 2] = std::clamp<float>(pixel.blueF() * c.blue(), 0, 255);
				}
			}

			previewButton_->setIcon(QPixmap::fromImage(QImage(pixels.get(), srcWidth, srcHeight, QImage::Format::Format_RGB888)));
		}
		else
		{
			QPixmap pixmap(w, h);
			QPainter painter(&pixmap);
			painter.setPen(Qt::NoPen);
			painter.fillRect(QRect(0, 0, w, h), c);
			previewButton_->setIcon(pixmap);
		}
	}

	void
	EnvironmentDock::previewClickEvent(bool checked)
	{
		if (!environmentListDialog_)
		{
			environmentListDialog_ = std::make_unique<EnvironmentListDialog>(this, behaviour_, profile_);
			connect(environmentListDialog_.get(), SIGNAL(itemSelected(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
		}

		if (environmentListDialog_->isHidden())
			environmentListDialog_->show();
		else
			environmentListDialog_->close();
	}

	void
	EnvironmentDock::itemSelected(QListWidgetItem* item)
	{
		try
		{
			auto uuid = item->data(Qt::UserRole).toString().toStdString();
			if (!uuid.empty())
			{
				this->profile_->environmentLightModule->color = octoon::math::float3(1, 1, 1);
				this->profile_->environmentLightModule->texture = AssetLibrary::instance()->loadAsset<octoon::Texture>(uuid);
			}
			else
			{
				this->profile_->environmentLightModule->texture = nullptr;
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(tr("Error"));
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void
	EnvironmentDock::thumbnailClickEvent()
	{
		try
		{
			QString filepath = QFileDialog::getOpenFileName(this, tr("Import Image"), "", tr("HDRi Files (*.hdr)"));
			if (!filepath.isEmpty())
			{
				auto texture = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Texture>(filepath.toStdWString());
				if (texture)
				{
					texture->setMipLevel(8);
					texture->apply();
				}

				this->profile_->environmentLightModule->color = octoon::math::float3(1, 1, 1);
				this->profile_->environmentLightModule->texture = texture;
			}
		}
		catch (const std::exception& e)
		{
			QCoreApplication::processEvents();

			QMessageBox msg(this);
			msg.setWindowTitle(tr("Error"));
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void
	EnvironmentDock::thumbnailToggleEvent(int state)
	{
		if (state == Qt::Checked)
			this->profile_->environmentLightModule->useTexture = true;
		else
			this->profile_->environmentLightModule->useTexture = false;
	}

	void
	EnvironmentDock::backgroundMapCheckEvent(int state)
	{
		if (state == Qt::Checked)
			this->profile_->environmentLightModule->showBackground = true;
		else
			this->profile_->environmentLightModule->showBackground = false;
	}

	void
	EnvironmentDock::colorClickEvent()
	{
		auto color = this->profile_->environmentLightModule->color.getValue();
		colorSelector_->setCurrentColor(QColor::fromRgbF(color.x, color.y, color.z));
		colorSelector_->show();
	}

	void
	EnvironmentDock::colorChangeEvent(const QColor& c)
	{
		this->profile_->environmentLightModule->color = octoon::math::float3(c.redF(), c.greenF(), c.blueF());
	}

	void
	EnvironmentDock::intensitySliderEvent(int value)
	{
		this->profile_->environmentLightModule->intensity = value / 5.0f;
	}

	void
	EnvironmentDock::intensityEditEvent(double value)
	{
		this->profile_->environmentLightModule->intensity = value;
	}

	void
	EnvironmentDock::horizontalRotationSliderEvent(int value)
	{
		this->profile_->environmentLightModule->offset = octoon::math::float2(value / 100.0f, this->profile_->environmentLightModule->offset.getValue().y);
	}

	void
	EnvironmentDock::horizontalRotationEditEvent(double value)
	{
		this->profile_->environmentLightModule->offset = octoon::math::float2(value, this->profile_->environmentLightModule->offset.getValue().y);
	}

	void
	EnvironmentDock::verticalRotationSliderEvent(int value)
	{
		this->profile_->environmentLightModule->offset = octoon::math::float2(this->profile_->environmentLightModule->offset.getValue().x, value / 100.0f);
	}

	void
	EnvironmentDock::verticalRotationEditEvent(double value)
	{
		this->profile_->environmentLightModule->offset = octoon::math::float2(this->profile_->environmentLightModule->offset.getValue().x, value);
	}

	void
	EnvironmentDock::resetEvent()
	{
		this->profile_->environmentLightModule->reset();
	}

	void
	EnvironmentDock::showEvent(QShowEvent * event)
	{
		auto color = profile_->environmentLightModule->color.getValue();
		this->setColor(QColor::fromRgbF(color.x, color.y, color.z));

		this->intensitySpinBox->setValue(profile_->environmentLightModule->intensity);
		this->intensitySlider->setValue(profile_->environmentLightModule->intensity * 5.0f);

		this->horizontalRotationSpinBox->setValue(profile_->environmentLightModule->offset.getValue().x);
		this->horizontalRotationSlider->setValue(profile_->environmentLightModule->offset.getValue().x * 100.0f);

		this->verticalRotationSpinBox->setValue(profile_->environmentLightModule->offset.getValue().y);
		this->verticalRotationSlider->setValue(profile_->environmentLightModule->offset.getValue().y * 100.0f);

		this->backgroundToggle->setChecked(profile_->environmentLightModule->showBackground);
		this->thumbnailToggle->setChecked(profile_->environmentLightModule->useTexture);

		this->profile_->environmentLightModule->texture.submit();
	}

	void
	EnvironmentDock::closeEvent(QCloseEvent * event)
	{
		if (profile_->playerModule->isPlaying)
			event->ignore();
		else
			event->accept();
	}

	bool
	EnvironmentDock::eventFilter(QObject * watched, QEvent * event)
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