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

	MaterialEditWindow::MaterialEditWindow(QWidget* widget, const octoon::GameObjectPtr& behaviour)
		: QWidget(widget)
		, behaviour_(behaviour)
	{
		okButton_ = new QToolButton;
		okButton_->setText(u8"确定");

		auto mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(this->createSummary(), 0, Qt::AlignTop);
		mainLayout->addWidget(this->createAlbedo(), 0,Qt::AlignTop);
		mainLayout->addWidget(this->createNormal(), 0,  Qt::AlignTop);
		mainLayout->addWidget(this->createSmoothness(), 0,  Qt::AlignTop);
		mainLayout->addWidget(this->createMetalness(), 0, Qt::AlignTop);
		mainLayout->addWidget(this->createSheen(), 0, Qt::AlignTop);
		mainLayout->addWidget(this->createClearCoat(), 0, Qt::AlignTop);
		mainLayout->addWidget(this->createSubsurface(), 0, Qt::AlignTop);
		mainLayout->addWidget(this->createEmissive(), 0, Qt::AlignTop);
		mainLayout->addStretch(500);
		mainLayout->addWidget(okButton_, 0, Qt::AlignBottom | Qt::AlignRight);
		mainLayout->setContentsMargins(0, 10, 20, 10);

		connect(albedoColor_, SIGNAL(currentColorChanged(QColor)), this, SLOT(albedoColorChanged(QColor)));
		connect(smoothnessSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(smoothEditEvent(double)));
		connect(smoothnessSlider_, SIGNAL(valueChanged(int)), this, SLOT(smoothSliderEvent(int)));
		connect(metalnessSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(metalEditEvent(double)));
		connect(metalnessSlider_, SIGNAL(valueChanged(int)), this, SLOT(metalSliderEvent(int)));
		connect(anisotropySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(anisotropyEditEvent(double)));
		connect(anisotropySlider_, SIGNAL(valueChanged(int)), this, SLOT(anisotropySliderEvent(int)));
		connect(sheenSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(sheenEditEvent(double)));
		connect(sheenSlider_, SIGNAL(valueChanged(int)), this, SLOT(sheenSliderEvent(int)));
		connect(clearcoatSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(clearcoatEditEvent(double)));
		connect(clearcoatSlider_, SIGNAL(valueChanged(int)), this, SLOT(clearcoatSliderEvent(int)));
		connect(clearcoatRoughnessSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(clearcoatRoughnessEditEvent(double)));
		connect(clearcoatRoughnessSlider_, SIGNAL(valueChanged(int)), this, SLOT(clearcoatRoughnessSliderEvent(int)));
		connect(subsurfaceSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(subsurfaceEditEvent(double)));
		connect(subsurfaceSlider_, SIGNAL(valueChanged(int)), this, SLOT(subsurfaceSliderEvent(int)));
		connect(emissiveColor_, SIGNAL(currentColorChanged(QColor)), this, SLOT(emissiveColorChanged(QColor)));
	}

	MaterialEditWindow::~MaterialEditWindow()
	{
	}

	QWidget*
	MaterialEditWindow::createSummary()
	{
		QPixmap pixmap(":res/icons/material.png");

		imageLabel_ = new QLabel();
		imageLabel_->setFixedSize(QSize(128, 128));

		textLabel_ = new QLabel();
		textLabel_->setText(u8"material");

		QVBoxLayout* summaryLayout = new QVBoxLayout;
		summaryLayout->setMargin(0);
		summaryLayout->setSpacing(0);
		summaryLayout->addWidget(imageLabel_, 0, Qt::AlignCenter);
		summaryLayout->addWidget(textLabel_, 0, Qt::AlignCenter);
		summaryLayout->setContentsMargins(0, 0, 10, 0);

		QWidget* summaryWidget = new QWidget;
		summaryWidget->setLayout(summaryLayout);

		return summaryWidget;
	}

	QWidget*
	MaterialEditWindow::createAlbedo()
	{
		albedoColor_ = new ColorDialog();
		albedoColor_->setMaximumWidth(260);
		albedoColor_->setCurrentColor(QColor(255, 255, 255));

		auto albedoLayout = new QVBoxLayout();
		albedoLayout->addWidget(albedoColor_);
		albedoLayout->setContentsMargins(20, 5, 50, 0);

		auto baseColor = new Spoiler(u8"基本颜色");
		baseColor->setFixedWidth(340);
		baseColor->setContentLayout(*albedoLayout);

		return baseColor;
	}

	QWidget*
	MaterialEditWindow::createNormal()
	{
		QPixmap pixmap(":res/icons/material.png");

		normalLabel_ = new QLabel();
		normalLabel_->setFixedSize(QSize(160, 160));
		normalLabel_->setPixmap(pixmap.scaled(160, 160));

		auto normalLayout = new QVBoxLayout();
		normalLayout->addWidget(normalLabel_);
		normalLayout->setContentsMargins(20, 5, 50, 0);

		auto normal = new Spoiler(u8"法线");
		normal->setFixedWidth(340);
		normal->setContentLayout(*normalLayout);

		return normal;
	}

	QWidget*
	MaterialEditWindow::createSmoothness()
	{
		smoothnessLabel_ = new QLabel;
		smoothnessLabel_->setText(u8"光滑度");

		smoothnessSlider_ = new QSlider(Qt::Horizontal);
		smoothnessSlider_->setObjectName("Value");
		smoothnessSlider_->setMinimum(0);
		smoothnessSlider_->setMaximum(100);
		smoothnessSlider_->setValue(0);
		smoothnessSlider_->setFixedWidth(260);

		smoothnessSpinBox_ = new DoubleSpinBox;
		smoothnessSpinBox_->setFixedWidth(50);
		smoothnessSpinBox_->setMaximum(1.0f);
		smoothnessSpinBox_->setSingleStep(0.03f);
		smoothnessSpinBox_->setAlignment(Qt::AlignRight);
		smoothnessSpinBox_->setValue(0.0f);

		auto smoothnessHLayout = new QHBoxLayout();
		smoothnessHLayout->addWidget(smoothnessLabel_, 0, Qt::AlignLeft);
		smoothnessHLayout->addWidget(smoothnessSpinBox_, 0, Qt::AlignRight);

		auto smoothnessLayout = new QVBoxLayout();
		smoothnessLayout->addLayout(smoothnessHLayout);
		smoothnessLayout->addWidget(smoothnessSlider_);
		smoothnessLayout->setContentsMargins(20, 5, 50, 0);

		auto smoothness = new Spoiler(u8"光滑度");
		smoothness->setFixedWidth(340);
		smoothness->setContentLayout(*smoothnessLayout);

		return smoothness;
	}

	QWidget*
	MaterialEditWindow::createMetalness()
	{
		metalnessLabel_ = new QLabel;
		metalnessLabel_->setText(u8"金属程度");

		metalnessSlider_ = new QSlider(Qt::Horizontal);
		metalnessSlider_->setObjectName("Value");
		metalnessSlider_->setMinimum(0);
		metalnessSlider_->setMaximum(100);
		metalnessSlider_->setValue(0);
		metalnessSlider_->setFixedWidth(260);

		metalnessSpinBox_ = new DoubleSpinBox;
		metalnessSpinBox_->setFixedWidth(50);
		metalnessSpinBox_->setMaximum(1.0f);
		metalnessSpinBox_->setSingleStep(0.03f);
		metalnessSpinBox_->setAlignment(Qt::AlignRight);
		metalnessSpinBox_->setValue(0.0f);

		auto metalnessHLayout = new QHBoxLayout();
		metalnessHLayout->addWidget(metalnessLabel_, 0, Qt::AlignLeft);
		metalnessHLayout->addWidget(metalnessSpinBox_, 0, Qt::AlignRight);

		anisotropyLabel_ = new QLabel;
		anisotropyLabel_->setText(u8"各向异性");

		anisotropySlider_ = new QSlider(Qt::Horizontal);
		anisotropySlider_->setObjectName("Value");
		anisotropySlider_->setMinimum(0);
		anisotropySlider_->setMaximum(100);
		anisotropySlider_->setValue(0);
		anisotropySlider_->setFixedWidth(260);

		anisotropySpinBox_ = new DoubleSpinBox;
		anisotropySpinBox_->setFixedWidth(50);
		anisotropySpinBox_->setMaximum(1.0f);
		anisotropySpinBox_->setSingleStep(0.03f);
		anisotropySpinBox_->setAlignment(Qt::AlignRight);
		anisotropySpinBox_->setValue(0.0f);

		auto anisotropyHLayout = new QHBoxLayout();
		anisotropyHLayout->addWidget(anisotropyLabel_, 0, Qt::AlignLeft);
		anisotropyHLayout->addWidget(anisotropySpinBox_, 0, Qt::AlignRight);

		auto metalnessLayout = new QVBoxLayout();
		metalnessLayout->addLayout(metalnessHLayout);
		metalnessLayout->addWidget(metalnessSlider_);
		metalnessLayout->addLayout(anisotropyHLayout);
		metalnessLayout->addWidget(anisotropySlider_);
		metalnessLayout->setContentsMargins(20, 5, 50, 0);

		auto metalness = new Spoiler(u8"金属程度");
		metalness->setFixedWidth(340);
		metalness->setContentLayout(*metalnessLayout);

		return metalness;
	}


	QWidget*
	MaterialEditWindow::createSheen()
	{
		sheenLabel_ = new QLabel;
		sheenLabel_->setText(u8"光泽度");

		sheenSlider_ = new QSlider(Qt::Horizontal);
		sheenSlider_->setObjectName("Value");
		sheenSlider_->setMinimum(0);
		sheenSlider_->setMaximum(100);
		sheenSlider_->setValue(0);
		sheenSlider_->setFixedWidth(260);

		sheenSpinBox_ = new DoubleSpinBox;
		sheenSpinBox_->setFixedWidth(50);
		sheenSpinBox_->setMaximum(1.0f);
		sheenSpinBox_->setSingleStep(0.03f);
		sheenSpinBox_->setAlignment(Qt::AlignRight);
		sheenSpinBox_->setValue(0.0f);

		auto sheenHLayout = new QHBoxLayout();
		sheenHLayout->addWidget(sheenLabel_, 0, Qt::AlignLeft);
		sheenHLayout->addWidget(sheenSpinBox_, 0, Qt::AlignRight);

		auto sheenLayout = new QVBoxLayout();
		sheenLayout->addLayout(sheenHLayout);
		sheenLayout->addWidget(sheenSlider_);
		sheenLayout->setContentsMargins(20, 5, 50, 0);

		auto sheen = new Spoiler(u8"布料");
		sheen->setFixedWidth(340);
		sheen->setContentLayout(*sheenLayout);

		return sheen;
	}

	QWidget*
	MaterialEditWindow::createClearCoat()
	{
		clearcoatLabel_ = new QLabel;
		clearcoatLabel_->setText(u8"光泽度");

		clearcoatSlider_ = new QSlider(Qt::Horizontal);
		clearcoatSlider_->setObjectName("Value");
		clearcoatSlider_->setMinimum(0);
		clearcoatSlider_->setMaximum(100);
		clearcoatSlider_->setValue(0);
		clearcoatSlider_->setFixedWidth(260);

		clearcoatSpinBox_ = new DoubleSpinBox;
		clearcoatSpinBox_->setFixedWidth(50);
		clearcoatSpinBox_->setMaximum(1.0f);
		clearcoatSpinBox_->setSingleStep(0.03f);
		clearcoatSpinBox_->setAlignment(Qt::AlignRight);
		clearcoatSpinBox_->setValue(0.0f);

		clearcoatRoughnessLabel_ = new QLabel;
		clearcoatRoughnessLabel_->setText(u8"粗糙度");

		clearcoatRoughnessSlider_ = new QSlider(Qt::Horizontal);
		clearcoatRoughnessSlider_->setObjectName("Value");
		clearcoatRoughnessSlider_->setMinimum(0);
		clearcoatRoughnessSlider_->setMaximum(100);
		clearcoatRoughnessSlider_->setValue(0);
		clearcoatRoughnessSlider_->setFixedWidth(260);

		clearcoatRoughnessSpinBox_ = new DoubleSpinBox;
		clearcoatRoughnessSpinBox_->setFixedWidth(50);
		clearcoatRoughnessSpinBox_->setMaximum(1.0f);
		clearcoatRoughnessSpinBox_->setSingleStep(0.03f);
		clearcoatRoughnessSpinBox_->setAlignment(Qt::AlignRight);
		clearcoatRoughnessSpinBox_->setValue(0.0f);

		auto clearcoatHLayout = new QHBoxLayout();
		clearcoatHLayout->addWidget(clearcoatLabel_, 0, Qt::AlignLeft);
		clearcoatHLayout->addWidget(clearcoatSpinBox_, 0, Qt::AlignRight);

		auto clearcoatRoughnessHLayout = new QHBoxLayout();
		clearcoatRoughnessHLayout->addWidget(clearcoatRoughnessLabel_, 0, Qt::AlignLeft);
		clearcoatRoughnessHLayout->addWidget(clearcoatRoughnessSpinBox_, 0, Qt::AlignRight);

		auto clearcoatLayout = new QVBoxLayout();
		clearcoatLayout->addLayout(clearcoatHLayout);
		clearcoatLayout->addWidget(clearcoatSlider_);
		clearcoatLayout->addLayout(clearcoatRoughnessHLayout);
		clearcoatLayout->addWidget(clearcoatRoughnessSlider_);
		clearcoatLayout->setContentsMargins(20, 5, 50, 0);

		auto clearcoat = new Spoiler(u8"清漆");
		clearcoat->setFixedWidth(340);
		clearcoat->setContentLayout(*clearcoatLayout);

		return clearcoat;
	}

	QWidget*
	MaterialEditWindow::createSubsurface()
	{
		subsurfaceLabel_ = new QLabel;
		subsurfaceLabel_->setText(u8"散射程度");

		subsurfaceSlider_ = new QSlider(Qt::Horizontal);
		subsurfaceSlider_->setObjectName("Value");
		subsurfaceSlider_->setMinimum(0);
		subsurfaceSlider_->setMaximum(100);
		subsurfaceSlider_->setValue(0);
		subsurfaceSlider_->setFixedWidth(260);

		subsurfaceSpinBox_ = new DoubleSpinBox;
		subsurfaceSpinBox_->setFixedWidth(50);
		subsurfaceSpinBox_->setMaximum(1.0f);
		subsurfaceSpinBox_->setSingleStep(0.03f);
		subsurfaceSpinBox_->setAlignment(Qt::AlignRight);
		subsurfaceSpinBox_->setValue(0.0f);

		auto subsurfaceHLayout = new QHBoxLayout();
		subsurfaceHLayout->addWidget(subsurfaceLabel_, 0, Qt::AlignLeft);
		subsurfaceHLayout->addWidget(subsurfaceSpinBox_, 0, Qt::AlignRight);

		auto subsurfaceLayout = new QVBoxLayout();
		subsurfaceLayout->addLayout(subsurfaceHLayout);
		subsurfaceLayout->addWidget(subsurfaceSlider_);
		subsurfaceLayout->setContentsMargins(20, 5, 50, 0);

		auto subsurface = new Spoiler(u8"次表面散射");
		subsurface->setFixedWidth(340);
		subsurface->setContentLayout(*subsurfaceLayout);

		return subsurface;
	}

	QWidget*
	MaterialEditWindow::createEmissive()
	{
		emissiveColor_ = new ColorDialog();
		emissiveColor_->setMaximumWidth(260);
		emissiveColor_->setCurrentColor(QColor(255, 255, 255));

		auto emissiveLayout = new QVBoxLayout();
		emissiveLayout->addWidget(emissiveColor_);
		emissiveLayout->setContentsMargins(20, 5, 50, 0);

		auto emissive = new Spoiler(u8"自发光");
		emissive->setFixedWidth(340);
		emissive->setContentLayout(*emissiveLayout);

		return emissive;
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
			auto standard = material->downcast_pointer<octoon::material::MeshStandardMaterial>();

			textLabel_->setText(QString::fromStdString(standard->getName()));
			albedoColor_->setCurrentColor(QColor::fromRgbF(standard->getColor().x, standard->getColor().y, standard->getColor().z));
			smoothnessSpinBox_->setValue(standard->getSmoothness());
			metalnessSpinBox_->setValue(standard->getMetalness());
			anisotropySpinBox_->setValue(standard->getAnisotropy());
			sheenSpinBox_->setValue(standard->getSheen());
			clearcoatSpinBox_->setValue(standard->getClearCoat());
			clearcoatRoughnessSpinBox_->setValue(standard->getClearCoatRoughness());
			subsurfaceSpinBox_->setValue(standard->getSubsurface());
			emissiveColor_->setCurrentColor(QColor::fromRgbF(standard->getEmissive().x, standard->getEmissive().y, standard->getEmissive().z));

			this->material_ = material;
			this->repaint();
		}
	}

	void
	MaterialEditWindow::albedoColorChanged(QColor color)
	{
		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setColor(octoon::math::float3(color.redF(), color.greenF(), color.blueF()));
			this->repaint();
		}
	}

	void
	MaterialEditWindow::emissiveColorChanged(QColor color)
	{
		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setEmissive(octoon::math::float3(color.redF(), color.greenF(), color.blueF()));
			this->repaint();
		}
	}

	void
	MaterialEditWindow::smoothEditEvent(double value)
	{
		smoothnessSlider_->setValue(value * 100.f);

		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setSmoothness(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::smoothSliderEvent(int value)
	{
		smoothnessSpinBox_->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::metalEditEvent(double value)
	{
		metalnessSlider_->setValue(value * 100.f);

		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setMetalness(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::metalSliderEvent(int value)
	{
		metalnessSpinBox_->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::anisotropyEditEvent(double value)
	{
		anisotropySlider_->setValue(value * 100.f);

		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setAnisotropy(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::anisotropySliderEvent(int value)
	{
		anisotropySpinBox_->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::clearcoatEditEvent(double value)
	{
		clearcoatSlider_->setValue(value * 100.f);

		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setClearCoat(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::clearcoatSliderEvent(int value)
	{
		clearcoatSpinBox_->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::clearcoatRoughnessEditEvent(double value)
	{
		clearcoatRoughnessSlider_->setValue(value * 100.f);

		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setClearCoatRoughness(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::clearcoatRoughnessSliderEvent(int value)
	{
		clearcoatRoughnessSpinBox_->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::sheenEditEvent(double value)
	{
		sheenSlider_->setValue(value * 100.f);

		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setSheen(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::sheenSliderEvent(int value)
	{
		sheenSpinBox_->setValue(value / 100.0f);
	}

	void
	MaterialEditWindow::subsurfaceEditEvent(double value)
	{
		subsurfaceSlider_->setValue(value * 100.f);

		if (this->material_)
		{
			auto standard = this->material_->downcast_pointer<octoon::material::MeshStandardMaterial>();
			standard->setSubsurface(value);
			this->repaint();
		}
	}

	void
	MaterialEditWindow::subsurfaceSliderEvent(int value)
	{
		subsurfaceSpinBox_->setValue(value / 100.0f);
	}

	void 
	MaterialEditWindow::closeEvent()
	{
		this->hide();
	}

	MaterialListWindow::MaterialListWindow() noexcept(false)
	{
	}

	MaterialListWindow::~MaterialListWindow() noexcept
	{
	}

	void
	MaterialListWindow::mouseMoveEvent(QMouseEvent *event)
	{
		if (event->button() == Qt::LeftButton)
			startPos = event->pos();

		QListWidget::mousePressEvent(event);
	}

	void
	MaterialListWindow::mousePressEvent(QMouseEvent *event)
	{
		QListWidget::mousePressEvent(event);

		QListWidgetItem* item = this->itemAt(event->pos());
		if (item)
		{
			auto widget = this->itemWidget(item);
			auto layout = widget->layout();
			auto label = dynamic_cast<QLabel*>(layout->itemAt(0)->widget());
			if (label)
			{
				auto mimeData = new QMimeData;
				mimeData->setData("object/material", item->data(Qt::UserRole).toByteArray());

				QDrag *drag = new QDrag(this);
				drag->setMimeData(mimeData);
				drag->setPixmap(label->pixmap(Qt::ReturnByValue));
				drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));

				drag->exec(Qt::MoveAction);
			}
		}
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
		listWidget_->setResizeMode(QListView::Adjust);
		listWidget_->setViewMode(QListView::IconMode);
		listWidget_->setMovement(QListView::Static);
		listWidget_->setDragEnabled(true);
		listWidget_->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		listWidget_->setDragDropOverwriteMode(false);
		listWidget_->setDefaultDropAction(Qt::DropAction::MoveAction);
		listWidget_->setSpacing(10);
		listWidget_->setMinimumHeight(this->height());
		listWidget_->setMinimumWidth(this->width());
		listWidget_->setStyleSheet("background:transparent;");
		listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
		titleLayout_ = new QHBoxLayout();
		titleLayout_->addWidget(title_, 0, Qt::AlignLeft);
		titleLayout_->addWidget(closeButton_, 0, Qt::AlignRight);

		modifyWidget_ = new MaterialEditWindow(this, behaviour);
		modifyWidget_->setFixedWidth(340);

		modifyMaterialArea_ = new QScrollArea();
		modifyMaterialArea_->setFixedHeight(700);
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
		connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		connect(listWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

		behaviour->addMessageListener("editor:material:change", [this](const std::any&) {
			if (this->isVisible())
				this->updateList();
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