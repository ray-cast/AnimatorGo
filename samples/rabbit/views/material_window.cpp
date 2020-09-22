#include "material_window.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qevent.h>
#include <qscrollbar.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qapplication.h>
#include <fstream>
#include <codecvt>
#include <qpainter.h>
#include <qcolordialog.h>
#include <qtreewidget.h>

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

	QIcon createColorIcon(QColor color, int w = 50, int h = 22)
	{
		QPixmap pixmap(w, h);
		QPainter painter(&pixmap);
		painter.setPen(Qt::NoPen);
		painter.fillRect(QRect(0, 0, w, h), color);
		return QIcon(pixmap);
	}

	void
	MaterialEditWindow::MaterialUi::init(const QString& name, std::uint32_t flags)
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
		this->titleLayout->addStretch(300);
		this->titleLayout->setSpacing(0);
		this->titleLayout->setContentsMargins(0, 2, 0, 0);

		auto textLayout = new QHBoxLayout;
		textLayout->setSpacing(0);
		textLayout->setContentsMargins(0, 2, 0, 0);
		textLayout->addWidget(this->path, 0, Qt::AlignLeft | Qt::AlignCenter);
		textLayout->addStretch(300);

		if (flags & CreateFlags::ColorBit)
		{
			this->color = new QPushButton;
			this->color->setIconSize(QSize(50, 22));

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
		this->rightLayout->addStretch(100);

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
			this->slider->setFixedWidth(260);

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
	MaterialEditWindow::MaterialUi::resetState()
	{
		this->texture = nullptr;
		if (this->path) this->path->clear();
		if (this->image) this->image->setIcon(QIcon::fromTheme(":res/icons/append2.png"));
	}

	MaterialEditWindow::MaterialEditWindow(QWidget* widget, const octoon::GameObjectPtr& behaviour)
		: QWidget(widget)
		, behaviour_(behaviour)
	{
		okButton_ = new QToolButton;
		okButton_->setText(u8"返回");

		this->albedo_.init(u8"基本颜色", CreateFlags::SpoilerBit | CreateFlags::ColorBit);
		this->opacity_.init(u8"不透明度", CreateFlags::SpoilerBit | CreateFlags::ValueBit);
		this->normal_.init(u8"法线", CreateFlags::SpoilerBit);
		this->roughness_.init(u8"粗糙度", CreateFlags::SpoilerBit | CreateFlags::ValueBit);
		this->metalness_.init(u8"金属", CreateFlags::SpoilerBit | CreateFlags::ValueBit);
		this->specular_.init(u8"反射", CreateFlags::SpoilerBit | CreateFlags::ValueBit);
		this->anisotropy_.init(u8"各向异性", CreateFlags::SpoilerBit | CreateFlags::ValueBit);
		this->sheen_.init(u8"布料", CreateFlags::SpoilerBit | CreateFlags::ValueBit);
		this->clearcoat_.init(u8"清漆", CreateFlags::ValueBit);
		this->clearcoatRoughness_.init(u8"清漆粗糙度", CreateFlags::ValueBit);
		this->subsurface_.init(u8"次表面散射", CreateFlags::SpoilerBit | CreateFlags::ValueBit);
		this->emissive_.init(u8"自发光", CreateFlags::SpoilerBit | CreateFlags::ValueBit | CreateFlags::ColorBit);

		this->clearcoat_.mainLayout->setContentsMargins(0, 0, 0, 0);
		this->clearcoatRoughness_.mainLayout->setContentsMargins(0, 0, 0, 0);

		auto clearlayout = new QVBoxLayout();
		clearlayout->addLayout(this->clearcoat_.mainLayout);
		clearlayout->addLayout(this->clearcoatRoughness_.mainLayout);
		clearlayout->setContentsMargins(20, 5, 50, 0);

		this->clearCoatSpoiler_ = new Spoiler(u8"清漆");
		this->clearCoatSpoiler_->setContentLayout(*clearlayout);

		this->emissive_.spinBox->setMaximum(100.f);
		this->albedoColor_.setWindowModality(Qt::ApplicationModal);
		this->emissiveColor_.setWindowModality(Qt::ApplicationModal);

		auto mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(this->createSummary(), 0, Qt::AlignTop);
		mainLayout->addWidget(this->albedo_.spoiler, 0,Qt::AlignTop);
		mainLayout->addWidget(this->opacity_.spoiler, 0, Qt::AlignTop);
		mainLayout->addWidget(this->normal_.spoiler, 0,  Qt::AlignTop);
		mainLayout->addWidget(this->roughness_.spoiler, 0,  Qt::AlignTop);
		mainLayout->addWidget(this->specular_.spoiler, 0, Qt::AlignTop);
		mainLayout->addWidget(this->metalness_.spoiler, 0, Qt::AlignTop);
		mainLayout->addWidget(this->anisotropy_.spoiler, 0, Qt::AlignTop);
		mainLayout->addWidget(this->sheen_.spoiler, 0, Qt::AlignTop);
		mainLayout->addWidget(this->clearCoatSpoiler_, 0, Qt::AlignTop);
		mainLayout->addWidget(this->subsurface_.spoiler, 0, Qt::AlignTop);
		mainLayout->addWidget(this->emissive_.spoiler, 0, Qt::AlignTop);
		mainLayout->addStretch(500);
		mainLayout->addWidget(okButton_, 0, Qt::AlignBottom | Qt::AlignRight);
		mainLayout->setContentsMargins(0, 10, 20, 10);

		connect(albedo_.image, SIGNAL(clicked()), this, SLOT(colorMapClickEvent()));
		connect(albedo_.check, SIGNAL(stateChanged(int)), this, SLOT(colorMapCheckEvent(int)));
		connect(albedo_.color, SIGNAL(clicked()), this, SLOT(colorClickEvent()));
		connect(&albedoColor_, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(colorChangeEvent(const QColor&)));
		connect(&emissiveColor_, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(emissiveChangeEvent(const QColor&)));
		connect(opacity_.image, SIGNAL(clicked()), this, SLOT(opacityMapClickEvent()));
		connect(opacity_.check, SIGNAL(stateChanged(int)), this, SLOT(opacityMapCheckEvent(int)));
		connect(opacity_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(opacityEditEvent(double)));
		connect(opacity_.slider, SIGNAL(valueChanged(int)), this, SLOT(opacitySliderEvent(int)));
		connect(normal_.image, SIGNAL(clicked()), this, SLOT(normalMapClickEvent()));
		connect(roughness_.image, SIGNAL(clicked()), this, SLOT(smoothnessMapClickEvent()));
		connect(roughness_.check, SIGNAL(stateChanged(int)), this, SLOT(smoothnessMapCheckEvent(int)));
		connect(roughness_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(roughnessEditEvent(double)));
		connect(roughness_.slider, SIGNAL(valueChanged(int)), this, SLOT(roughnessSliderEvent(int)));
		connect(specular_.image, SIGNAL(clicked()), this, SLOT(specularMapClickEvent()));
		connect(specular_.check, SIGNAL(stateChanged(int)), this, SLOT(specularMapCheckEvent(int)));
		connect(specular_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(specularEditEvent(double)));
		connect(specular_.slider, SIGNAL(valueChanged(int)), this, SLOT(specularSliderEvent(int)));
		connect(metalness_.image, SIGNAL(clicked()), this, SLOT(metalnessMapClickEvent()));
		connect(metalness_.check, SIGNAL(stateChanged(int)), this, SLOT(metalnessMapCheckEvent(int)));
		connect(metalness_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(metalEditEvent(double)));
		connect(metalness_.slider, SIGNAL(valueChanged(int)), this, SLOT(metalSliderEvent(int)));
		connect(anisotropy_.image, SIGNAL(clicked()), this, SLOT(anisotropyMapClickEvent()));
		connect(anisotropy_.check, SIGNAL(stateChanged(int)), this, SLOT(anisotropyMapCheckEvent(int)));
		connect(anisotropy_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(anisotropyEditEvent(double)));
		connect(anisotropy_.slider, SIGNAL(valueChanged(int)), this, SLOT(anisotropySliderEvent(int)));
		connect(sheen_.image, SIGNAL(clicked()), this, SLOT(sheenMapClickEvent()));
		connect(sheen_.check, SIGNAL(stateChanged(int)), this, SLOT(sheenMapCheckEvent(int)));
		connect(sheen_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(sheenEditEvent(double)));
		connect(sheen_.slider, SIGNAL(valueChanged(int)), this, SLOT(sheenSliderEvent(int)));
		connect(clearcoat_.image, SIGNAL(clicked()), this, SLOT(clearcoatMapClickEvent()));
		connect(clearcoat_.check, SIGNAL(stateChanged(int)), this, SLOT(clearcoatMapCheckEvent(int)));
		connect(clearcoat_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(clearcoatEditEvent(double)));
		connect(clearcoat_.slider, SIGNAL(valueChanged(int)), this, SLOT(clearcoatSliderEvent(int)));
		connect(clearcoatRoughness_.image, SIGNAL(clicked()), this, SLOT(clearcoatRoughnessMapClickEvent()));
		connect(clearcoatRoughness_.check, SIGNAL(stateChanged(int)), this, SLOT(clearcoatRoughnessMapCheckEvent(int)));
		connect(clearcoatRoughness_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(clearcoatRoughnessEditEvent(double)));
		connect(clearcoatRoughness_.slider, SIGNAL(valueChanged(int)), this, SLOT(clearcoatRoughnessSliderEvent(int)));
		connect(subsurface_.image, SIGNAL(clicked()), this, SLOT(subsurfaceMapClickEvent()));
		connect(subsurface_.check, SIGNAL(stateChanged(int)), this, SLOT(subsurfaceMapCheckEvent(int)));
		connect(subsurface_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(subsurfaceEditEvent(double)));
		connect(subsurface_.slider, SIGNAL(valueChanged(int)), this, SLOT(subsurfaceSliderEvent(int)));
		connect(emissive_.image, SIGNAL(clicked()), this, SLOT(emissiveMapClickEvent()));
		connect(emissive_.color, SIGNAL(clicked()), this, SLOT(emissiveClickEvent()));
		connect(emissive_.check, SIGNAL(stateChanged(int)), this, SLOT(emissiveMapCheckEvent(int)));
		connect(emissive_.spinBox, SIGNAL(valueChanged(double)), this, SLOT(emissiveEditEvent(double)));
		connect(emissive_.slider, SIGNAL(valueChanged(int)), this, SLOT(emissiveSliderEvent(int)));
	}

	MaterialEditWindow::~MaterialEditWindow()
	{
	}

	void
	MaterialEditWindow::setAlbedoMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->albedo_.path->setText(name);
			this->albedo_.check->setCheckState(Qt::CheckState::Checked);
			this->albedo_.image->setIcon(QIcon(path));
			this->albedo_.texture = octoon::TextureLoader::load(path.toStdString());
			this->material_->setColorMap(this->albedo_.texture);
		}
		else
		{
			this->material_->setColorMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setNormalMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->normal_.path->setText(name);
			this->normal_.check->setCheckState(Qt::CheckState::Checked);
			this->normal_.image->setIcon(QIcon(path));
			this->normal_.texture = octoon::TextureLoader::load(path.toStdString());
			this->material_->setNormalMap(this->normal_.texture);
		}
		else
		{
			this->material_->setNormalMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setOpacityMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->opacity_.path->setText(name);
			this->opacity_.check->setCheckState(Qt::CheckState::Checked);
			this->opacity_.image->setIcon(QIcon(path));
			this->opacity_.texture = octoon::TextureLoader::load(path.toStdString());
			this->opacity_.spinBox->setValue(1.0f);
			this->material_->setOpacity(1.0f);
			this->material_->setOpacityMap(this->opacity_.texture);
		}
		else
		{
			this->material_->setOpacityMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setRoughnessMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->roughness_.path->setText(name);
			this->roughness_.check->setCheckState(Qt::CheckState::Checked);
			this->roughness_.image->setIcon(QIcon(path));
			this->roughness_.texture = octoon::TextureLoader::load(path.toStdString());
			this->roughness_.spinBox->setValue(1.0f);
			this->material_->setRoughness(1.0f);
			this->material_->setRoughnessMap(this->roughness_.texture);
		}
		else
		{
			this->material_->setRoughnessMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setSpecularMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->specular_.path->setText(name);
			this->specular_.check->setCheckState(Qt::CheckState::Checked);
			this->specular_.image->setIcon(QIcon(path));
			this->specular_.texture = octoon::TextureLoader::load(path.toStdString());
			this->specular_.spinBox->setValue(1.0f);
			this->material_->setSpecular(1.0f);
			this->material_->setSpecularMap(this->specular_.texture);
		}
		else
		{
			this->material_->setSpecularMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setMetalnessMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->metalness_.path->setText(name);
			this->metalness_.check->setCheckState(Qt::CheckState::Checked);
			this->metalness_.image->setIcon(QIcon(path));
			this->metalness_.texture = octoon::TextureLoader::load(path.toStdString());
			this->metalness_.spinBox->setValue(1.0f);
			this->material_->setMetalness(1.0f);
			this->material_->setMetalnessMap(this->metalness_.texture);
		}
		else
		{
			this->material_->setMetalnessMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setAnisotropyMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->anisotropy_.path->setText(name);
			this->anisotropy_.check->setCheckState(Qt::CheckState::Checked);
			this->anisotropy_.image->setIcon(QIcon(path));
			this->anisotropy_.texture = octoon::TextureLoader::load(path.toStdString());
			this->anisotropy_.spinBox->setValue(1.0f);
			this->material_->setAnisotropy(1.0f);
			this->material_->setAnisotropyMap(this->anisotropy_.texture);
		}
		else
		{
			this->material_->setAnisotropyMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setSheenMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->sheen_.path->setText(name);
			this->sheen_.check->setCheckState(Qt::CheckState::Checked);
			this->sheen_.image->setIcon(QIcon(path));
			this->sheen_.texture = octoon::TextureLoader::load(path.toStdString());
			this->sheen_.spinBox->setValue(1.0f);
			this->material_->setSheen(1.0f);
			this->material_->setSheenMap(this->sheen_.texture);
		}
		else
		{
			this->material_->setSheenMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setClearCoatMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->clearcoat_.path->setText(name);
			this->clearcoat_.check->setCheckState(Qt::CheckState::Checked);
			this->clearcoat_.image->setIcon(QIcon(path));
			this->clearcoat_.texture = octoon::TextureLoader::load(path.toStdString());
			this->clearcoat_.spinBox->setValue(1.0f);
			this->material_->setClearCoat(1.0f);
			this->material_->setClearCoatMap(this->clearcoat_.texture);
		}
		else
		{
			this->material_->setClearCoatMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setClearCoatRoughnessMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->clearcoatRoughness_.path->setText(name);
			this->clearcoatRoughness_.check->setCheckState(Qt::CheckState::Checked);
			this->clearcoatRoughness_.image->setIcon(QIcon(path));
			this->clearcoatRoughness_.texture = octoon::TextureLoader::load(path.toStdString());
			this->clearcoatRoughness_.spinBox->setValue(1.0f);
			this->material_->setClearCoatRoughness(1.0f);
			this->material_->setClearCoatRoughnessMap(this->clearcoatRoughness_.texture);
		}
		else
		{
			this->material_->setClearCoatRoughnessMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::setSubsurfaceMap(const QString& path)
	{
	}

	void
	MaterialEditWindow::setEmissiveMap(const QString& path)
	{
		if (!path.isEmpty())
		{
			QFontMetrics metrics(this->albedo_.path->font());
			auto name = metrics.elidedText(QFileInfo(path).fileName(), Qt::ElideRight, this->albedo_.path->width());

			this->emissive_.path->setText(name);
			this->emissive_.check->setCheckState(Qt::CheckState::Checked);
			this->emissive_.image->setIcon(QIcon(path));
			this->emissive_.texture = octoon::TextureLoader::load(path.toStdString());
			this->material_->setEmissiveMap(this->emissive_.texture);
		}
		else
		{
			this->material_->setEmissiveMap(nullptr);
		}

		this->repaint();
	}

	void
	MaterialEditWindow::colorClickEvent()
	{
		albedoColor_.show();
	}

	void
	MaterialEditWindow::colorChangeEvent(const QColor &color)
	{
		this->albedo_.color->setIcon(createColorIcon(color));
		this->material_->setColor(octoon::math::srgb2linear(octoon::math::float3(color.redF(), color.greenF(), color.blueF())));
		this->repaint();
	}

	void
	MaterialEditWindow::emissiveClickEvent()
	{
		emissiveColor_.show();
	}

	void
	MaterialEditWindow::emissiveChangeEvent(const QColor &color)
	{
		this->emissive_.color->setIcon(createColorIcon(color));
		this->material_->setEmissive(octoon::math::srgb2linear(octoon::math::float3(color.redF(), color.greenF(), color.blueF())));
		this->repaint();
	}

	void
	MaterialEditWindow::colorMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setAlbedoMap(path);
	}

	void
	MaterialEditWindow::opacityMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setOpacityMap(path);
	}

	void
	MaterialEditWindow::normalMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setNormalMap(path);
	}

	void
	MaterialEditWindow::specularMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setSpecularMap(path);
	}

	void
	MaterialEditWindow::smoothnessMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setRoughnessMap(path);
	}

	void
	MaterialEditWindow::metalnessMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setMetalnessMap(path);
	}

	void
	MaterialEditWindow::anisotropyMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setAnisotropyMap(path);
	}

	void
	MaterialEditWindow::sheenMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setSheenMap(path);
	}

	void
	MaterialEditWindow::clearcoatMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setClearCoatMap(path);
	}

	void
	MaterialEditWindow::clearcoatRoughnessMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setClearCoatRoughnessMap(path);
	}

	void
	MaterialEditWindow::subsurfaceMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setSubsurfaceMap(path);
	}

	void
	MaterialEditWindow::emissiveMapClickEvent()
	{
		QString path = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("All Files(*.jpeg *.jpg *.png)"));
		if (!path.isEmpty())
			this->setEmissiveMap(path);
	}

	void
	MaterialEditWindow::colorMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setAlbedoMap("");
		}
		else if (this->albedo_.texture)
		{
			this->material_->setColorMap(this->albedo_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::opacityMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setOpacityMap("");
		}
		else if (this->opacity_.texture)
		{
			this->material_->setOpacityMap(this->opacity_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::normalMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setNormalMap("");
		}
		else if (this->normal_.texture)
		{
			this->material_->setNormalMap(this->normal_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::smoothnessMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setRoughnessMap("");
		}
		else if (this->roughness_.texture)
		{
			this->material_->setRoughnessMap(this->roughness_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::specularMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setSpecularMap("");
		}
		else if (this->specular_.texture)
		{
			this->material_->setSpecularMap(this->specular_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::metalnessMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setMetalnessMap("");
		}
		else if (this->metalness_.texture)
		{
			this->material_->setMetalnessMap(this->metalness_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::anisotropyMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setAnisotropyMap("");
		}
		else if (this->anisotropy_.texture)
		{
			this->material_->setAnisotropyMap(this->anisotropy_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::sheenMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setSheenMap("");
		}
		else if (this->sheen_.texture)
		{
			this->material_->setSheenMap(this->sheen_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::clearcoatMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setClearCoatMap("");
		}
		else if (this->clearcoat_.texture)
		{
			this->material_->setClearCoatMap(this->clearcoat_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::clearcoatRoughnessMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setClearCoatRoughnessMap("");
		}
		else if (this->clearcoatRoughness_.texture)
		{
			this->material_->setClearCoatRoughnessMap(this->clearcoatRoughness_.texture);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::subsurfaceMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setSubsurfaceMap("");
		}
		else if (this->subsurface_.texture)
		{
		}
	}

	void
	MaterialEditWindow::emissiveMapCheckEvent(int state)
	{
		if (state == Qt::Unchecked)
		{
			this->setEmissiveMap("");
		}
		else if (this->emissive_.texture)
		{
			this->material_->setEmissiveMap(this->emissive_.texture);
			this->repaint();
		}
	}

	QWidget*
	MaterialEditWindow::createSummary()
	{
		imageLabel_ = new QLabel();
		imageLabel_->setFixedSize(QSize(128, 128));

		textLabel_ = new QLabel();
		textLabel_->setText(u8"material");

		QVBoxLayout* summaryLayout = new QVBoxLayout;
		summaryLayout->setMargin(0);
		summaryLayout->setSpacing(2);
		summaryLayout->addWidget(imageLabel_, 0, Qt::AlignCenter);
		summaryLayout->addWidget(textLabel_, 0, Qt::AlignCenter);
		summaryLayout->setContentsMargins(0, 0, 10, 0);

		QWidget* summaryWidget = new QWidget;
		summaryWidget->setLayout(summaryLayout);

		return summaryWidget;
	}

	void
	MaterialEditWindow::repaint()
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour && this->material_)
		{
			auto materialComponent = behaviour->getComponent<MaterialComponent>();
			QPixmap pixmap;
			materialComponent->repaintMaterial(this->material_, pixmap);
			this->material_->setDirty(true);
			imageLabel_->setPixmap(pixmap);
		}
	}

	void
	MaterialEditWindow::setMaterial(const std::shared_ptr<octoon::material::Material>& material)
	{
		if (this->material_ != material)
		{
			this->material_ = material->downcast_pointer<octoon::material::MeshStandardMaterial>();

			this->opacity_.resetState();
			this->normal_.resetState();
			this->roughness_.resetState();
			this->specular_.resetState();
			this->metalness_.resetState();
			this->anisotropy_.resetState();
			this->sheen_.resetState();
			this->clearcoat_.resetState();
			this->clearcoatRoughness_.resetState();
			this->subsurface_.resetState();
			this->emissive_.resetState();

			auto albedoColor = octoon::math::linear2srgb(material_->getColor());
			auto emissiveColor = octoon::math::linear2srgb(material_->getEmissive());

			this->textLabel_->setText(QString::fromStdString(material_->getName()));
			this->albedo_.color->setIcon(createColorIcon(QColor::fromRgbF(albedoColor.x, albedoColor.y, albedoColor.z)));
			this->opacity_.spinBox->setValue(material_->getOpacity());
			this->roughness_.spinBox->setValue(material_->getRoughness());
			this->specular_.spinBox->setValue(material_->getSpecular());
			this->metalness_.spinBox->setValue(material_->getMetalness());
			this->anisotropy_.spinBox->setValue(material_->getAnisotropy());
			this->sheen_.spinBox->setValue(material_->getSheen());
			this->clearcoat_.spinBox->setValue(material_->getClearCoat());
			this->clearcoatRoughness_.spinBox->setValue(material_->getClearCoatRoughness());
			this->subsurface_.spinBox->setValue(material_->getSubsurface());
			this->emissive_.color->setIcon(createColorIcon(QColor::fromRgbF(emissiveColor.x, emissiveColor.y, emissiveColor.z)));
			this->emissive_.spinBox->setValue(material_->getEmissiveIntensity());

			auto colorMap = material_->getColorMap();
			if (colorMap)
				this->setAlbedoMap(QString::fromStdString(colorMap->getTextureDesc().getName()));

			auto opacityMap = material_->getOpacityMap();
			if (opacityMap)
				this->setOpacityMap(QString::fromStdString(opacityMap->getTextureDesc().getName()));

			auto normalMap = material_->getNormalMap();
			if (normalMap)
				this->setNormalMap(QString::fromStdString(normalMap->getTextureDesc().getName()));

			auto roughnessMap = material_->getRoughnessMap();
			if (roughnessMap)
				this->setRoughnessMap(QString::fromStdString(roughnessMap->getTextureDesc().getName()));

			auto metalnessMap = material_->getMetalnessMap();
			if (metalnessMap)
				this->setMetalnessMap(QString::fromStdString(metalnessMap->getTextureDesc().getName()));

			auto sheenMap = material_->getSheenMap();
			if (sheenMap)
				this->setSheenMap(QString::fromStdString(sheenMap->getTextureDesc().getName()));

			auto clearcoatMap = material_->getClearCoatMap();
			if (clearcoatMap)
				this->setClearCoatMap(QString::fromStdString(clearcoatMap->getTextureDesc().getName()));

			auto clearcoatRoughnessMap = material_->getClearCoatRoughnessMap();
			if (clearcoatRoughnessMap)
				this->setClearCoatRoughnessMap(QString::fromStdString(clearcoatRoughnessMap->getTextureDesc().getName()));

			this->repaint();
		}
	}

	void
	MaterialEditWindow::emissiveSliderEvent(int value)
	{
		this->emissive_.spinBox->setValue(value);
	}

	void
	MaterialEditWindow::emissiveEditEvent(double value)
	{
		this->emissive_.slider->setValue(value);

		if (this->material_)
		{
			material_->setEmissiveIntensity(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::opacitySliderEvent(int value)
	{
		this->opacity_.spinBox->setValue(value / 100.f);
	}

	void
	MaterialEditWindow::opacityEditEvent(double value)
	{
		this->opacity_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setOpacity(value);
			if (value < 1.0f)
			{
				octoon::hal::GraphicsColorBlend blend;
				blend.setBlendEnable(true);
				blend.setBlendSrc(octoon::hal::GraphicsBlendFactor::SrcAlpha);
				blend.setBlendDest(octoon::hal::GraphicsBlendFactor::OneMinusSrcAlpha);

				std::vector<octoon::hal::GraphicsColorBlend> blends;
				blends.push_back(blend);

				material_->setColorBlends(std::move(blends));
			}
			else
			{
				material_->getColorBlends().shrink_to_fit();
			}

			this->repaint();
		}
	}

	void
	MaterialEditWindow::roughnessEditEvent(double value)
	{
		this->roughness_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setRoughness(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::roughnessSliderEvent(int value)
	{
		this->roughness_.spinBox->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::metalEditEvent(double value)
	{
		this->metalness_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setMetalness(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::metalSliderEvent(int value)
	{
		this->metalness_.spinBox->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::specularEditEvent(double value)
	{
		this->specular_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setSpecular(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::specularSliderEvent(int value)
	{
		this->specular_.spinBox->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::anisotropyEditEvent(double value)
	{
		this->anisotropy_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setAnisotropy(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::anisotropySliderEvent(int value)
	{
		this->anisotropy_.spinBox->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::clearcoatEditEvent(double value)
	{
		this->clearcoat_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setClearCoat(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::clearcoatSliderEvent(int value)
	{
		this->clearcoat_.spinBox->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::clearcoatRoughnessEditEvent(double value)
	{
		this->clearcoatRoughness_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setClearCoatRoughness(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::clearcoatRoughnessSliderEvent(int value)
	{
		this->clearcoatRoughness_.spinBox->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::sheenEditEvent(double value)
	{
		this->sheen_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setSheen(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::sheenSliderEvent(int value)
	{
		this->sheen_.spinBox->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::subsurfaceEditEvent(double value)
	{
		this->subsurface_.slider->setValue(value * 100.f);

		if (this->material_)
		{
			material_->setSubsurface(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::subsurfaceSliderEvent(int value)
	{
		this->subsurface_.spinBox->setValue(value / 100.0f);
	}

	void 
	MaterialEditWindow::closeEvent()
	{
		this->hide();
	}

	MaterialListWindow::MaterialListWindow() noexcept(false)
	{
		this->setResizeMode(QListView::Adjust);
		this->setViewMode(QListView::IconMode);
		this->setMovement(QListView::Static);
		this->setDefaultDropAction(Qt::DropAction::MoveAction);
		this->setSpacing(10);
		this->setStyleSheet("background:transparent;");
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

	MaterialListWindow::~MaterialListWindow() noexcept
	{
	}

	void
	MaterialListWindow::mouseMoveEvent(QMouseEvent *event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			QPoint length = event->pos() - startPos;
			if (length.manhattanLength() > QApplication::startDragDistance())
			{
				QListWidgetItem* item = this->itemAt(this->startPos);
				if (item)
				{
					auto widget = this->itemWidget(item);
					auto layout = widget->layout();
					auto label = dynamic_cast<QLabel*>(layout->itemAt(0)->widget());
					if (label)
					{
						auto mimeData = new QMimeData;
						mimeData->setData("object/material", item->data(Qt::UserRole).toByteArray());

						auto drag = new QDrag(this);
						drag->setMimeData(mimeData);
						drag->setPixmap(label->pixmap(Qt::ReturnByValue));
						drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
						drag->exec(Qt::MoveAction);
					}
				}
			}
		}

		QListWidget::mouseMoveEvent(event);
	}

	void
	MaterialListWindow::mousePressEvent(QMouseEvent *event)
	{
		if (event->button() == Qt::LeftButton)
			startPos = event->pos();

		QListWidget::mousePressEvent(event);
	}

	MaterialWindow::MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept(false)
		: behaviour_(behaviour)
	{
		this->hide();
		this->setObjectName("materialWindow");
		this->setWindowTitle(u8"材质");
		this->setFixedWidth(340);
		this->setMouseTracking(true);

		title_ = new QLabel();
		title_->setText(u8"材质");

		closeButton_ = new QToolButton();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		listWidget_ = new MaterialListWindow();
		listWidget_->setIconSize(QSize(210, 210));
		listWidget_->setMinimumSize(QSize(this->width(), this->height()));

		titleLayout_ = new QHBoxLayout();
		titleLayout_->addWidget(title_, 0, Qt::AlignLeft);
		titleLayout_->addWidget(closeButton_, 0, Qt::AlignRight);

		modifyWidget_ = new MaterialEditWindow(this, behaviour);
		modifyWidget_->setFixedWidth(340);

		modifyMaterialArea_ = new QScrollArea();
		modifyMaterialArea_->setWidget(modifyWidget_);
		modifyMaterialArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		modifyMaterialArea_->setWidgetResizable(true);
		modifyMaterialArea_->hide();

		mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addLayout(titleLayout_);
		mainLayout_->addWidget(listWidget_, 0, Qt::AlignTop | Qt::AlignCenter);
		mainLayout_->addWidget(modifyMaterialArea_, 0, Qt::AlignTop | Qt::AlignCenter);
		mainLayout_->addStretch(500);
		mainLayout_->setContentsMargins(10, 10, 10, 10);

		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(modifyWidget_->okButton_, SIGNAL(clicked()), this, SLOT(okEvent()));
		//connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		connect(listWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

		behaviour->addMessageListener("editor:material:change", [this](const std::any&) {
			if (this->isVisible())
				this->updateList();
		});

		behaviour->addMessageListener("editor:material:selected", [this](const std::any& any_data) {
			if (this->isVisible())
			{
				auto uuid = QString::fromStdString(std::any_cast<std::string>(any_data));
				auto count = this->listWidget_->count();
				for (int i = 0; i < count; i++)
				{
					auto item = this->listWidget_->item(i);
					if (item->data(Qt::UserRole).toString() == uuid)
					{
						this->listWidget_->setCurrentItem(item);
						break;
					}
				}
			}
		});
	}

	MaterialWindow::~MaterialWindow() noexcept
	{
	}

	void
	MaterialWindow::showEvent(QShowEvent* event) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins();
		modifyMaterialArea_->hide();
		modifyMaterialArea_->setMinimumHeight(this->height() - title_->height() * 2 - margins.top() - margins.bottom());
		listWidget_->setMinimumHeight(this->height() - title_->height() * 2 - margins.top() - margins.bottom());
		listWidget_->show();
		this->updateList();
	}

	void
	MaterialWindow::closeEvent()
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}

	void
	MaterialWindow::okEvent()
	{
		modifyMaterialArea_->hide();
		listWidget_->show();
	}

	void
	MaterialWindow::itemClicked(QListWidgetItem* item)
	{
		if (behaviour_)
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour->isOpen())
			{
				auto selectedItem = behaviour->getComponent<DragComponent>()->getSelectedItem();
				if (selectedItem)
				{
					auto hit = selectedItem.value();
					auto materialComponent = behaviour->getComponent<MaterialComponent>();
					auto material = materialComponent->getMaterial(item->data(Qt::UserRole).toString().toStdString());

					auto meshRenderer = hit.object->getComponent<octoon::MeshRendererComponent>();
					if (meshRenderer)
						meshRenderer->setMaterial(material, hit.mesh);
				}
			}
		}
	}

	void
	MaterialWindow::itemDoubleClicked(QListWidgetItem* item)
	{
		if (behaviour_)
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour->isOpen())
			{
				auto materialComponent = behaviour->getComponent<MaterialComponent>();
				auto material = materialComponent->getMaterial(item->data(Qt::UserRole).toString().toStdString());
				if (material)
				{
					listWidget_->hide();
					modifyWidget_->setMaterial(material);
					modifyMaterialArea_->show();
				}
			}
		}
	}

	void
	MaterialWindow::updateList()
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			auto materialComponent = behaviour->getComponent<MaterialComponent>();
			auto& materials = materialComponent->getMaterialList();

			listWidget_->clear();

			std::map<QString, std::shared_ptr<QPixmap>> imageTable;

			for (auto& it : materials)
			{
				std::string path;
				std::string normalName;
				std::string textureName;

				auto mat = it.second;
				if (mat.find("preview") != mat.end())
					textureName = mat["preview"].get<nlohmann::json::string_t>();

				QListWidgetItem* item = new QListWidgetItem;
				item->setData(Qt::UserRole, QString::fromStdString(mat["uuid"]));
				item->setSizeHint(QSize(130, 160));

				QLabel* imageLabel = new QLabel;
				if (textureName.empty())
				{
					auto material = materialComponent->getMaterial(mat["uuid"].get<nlohmann::json::string_t>());
					if (material)
					{
						QPixmap pixmap;
						materialComponent->repaintMaterial(material, pixmap);
						imageLabel->setPixmap(pixmap);
					}
				}
				else
				{
					auto texpath = QString::fromStdString(path + textureName);
					if (!imageTable[texpath])
						imageTable[texpath] = std::make_shared<QPixmap>(texpath);

					imageLabel->setPixmap(*imageTable[texpath]);
				}

				QLabel* txtLabel = new QLabel(QString::fromStdString(mat["name"]));
				txtLabel->setFixedHeight(30);

				QVBoxLayout* widgetLayout = new QVBoxLayout;
				widgetLayout->setMargin(0);
				widgetLayout->setSpacing(0);
				widgetLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
				widgetLayout->addWidget(txtLabel, 0, Qt::AlignCenter);

				QWidget* widget = new QWidget;
				widget->setLayout(widgetLayout);

				listWidget_->addItem(item);
				listWidget_->setItemWidget(item, widget);
			}
		}
	}
}