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
#include <qpropertyanimation.h>

namespace rabbit
{
	Spoiler::Spoiler(const QString& title, const int animationDuration, QWidget* parent) : QWidget(parent), animationDuration(animationDuration) {
		toggleButton.setStyleSheet("QToolButton { border: none; }");
		toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		toggleButton.setArrowType(Qt::ArrowType::RightArrow);
		toggleButton.setText(title);
		toggleButton.setCheckable(true);
		toggleButton.setChecked(false);

		headerLine.setFrameShape(QFrame::HLine);
		headerLine.setFrameShadow(QFrame::Sunken);
		headerLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

		contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		// start out collapsed
		contentArea.setMaximumHeight(0);
		contentArea.setMinimumHeight(0);
		// let the entire widget grow and shrink with its content
		toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
		toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
		toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));
		// don't waste space
		mainLayout.setVerticalSpacing(0);
		mainLayout.setContentsMargins(0, 0, 0, 0);
		int row = 0;
		mainLayout.addWidget(&toggleButton, row, 0, 1, 1, Qt::AlignLeft);
		mainLayout.addWidget(&headerLine, row++, 2, 1, 1);
		mainLayout.addWidget(&contentArea, row, 0, 1, 3);
		setLayout(&mainLayout);
		QObject::connect(&toggleButton, &QToolButton::clicked, [this](const bool checked) {
			toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
			toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
			toggleAnimation.start();
		});
	}

	void Spoiler::setContentLayout(QLayout& contentLayout) {
		delete contentArea.layout();
		contentArea.setLayout(&contentLayout);
		const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
		auto contentHeight = contentLayout.sizeHint().height();
		for (int i = 0; i < toggleAnimation.animationCount() - 1; ++i) {
			QPropertyAnimation* spoilerAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(i));
			spoilerAnimation->setDuration(animationDuration);
			spoilerAnimation->setStartValue(collapsedHeight);
			spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
		}
		QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
		contentAnimation->setDuration(animationDuration);
		contentAnimation->setStartValue(0);
		contentAnimation->setEndValue(contentHeight);
	}

	MaterialModifyWindow::MaterialModifyWindow(QWidget* widget)
		: QWidget(widget)
	{
		QPixmap pixmap(":res/icons/material.png");

		imageLabel_ = new QLabel();
		imageLabel_->setFixedSize(QSize(160, 160));
		imageLabel_->setPixmap(pixmap.scaled(160, 160));

		textLabel_ = new QLabel();
		textLabel_->setText(u8"material");

		QVBoxLayout* summaryLayout = new QVBoxLayout;
		summaryLayout->setMargin(0);
		summaryLayout->setSpacing(0);
		summaryLayout->addWidget(imageLabel_, 0, Qt::AlignCenter);
		summaryLayout->addWidget(textLabel_, 0, Qt::AlignCenter);
		summaryLayout->setContentsMargins(0, 0, 50, 0);

		QWidget* summaryWidget = new QWidget;
		summaryWidget->setLayout(summaryLayout);

		albedoColor_ = new ColorDialog();
		albedoColor_->setMaximumWidth(260);
		albedoColor_->setCurrentColor(QColor(255, 255, 255));

		normalColor_ = new ColorDialog();
		normalColor_->setMaximumWidth(260);
		normalColor_->setCurrentColor(QColor(255, 255, 255));

		smoothnessColor_ = new ColorDialog();
		smoothnessColor_->setMaximumWidth(260);
		smoothnessColor_->setCurrentColor(QColor(255, 255, 255));

		metalnessColor_ = new ColorDialog();
		metalnessColor_->setMaximumWidth(260);
		metalnessColor_->setCurrentColor(QColor(255, 255, 255));

		emissiveColor_ = new ColorDialog();
		emissiveColor_->setMaximumWidth(260);
		emissiveColor_->setCurrentColor(QColor(255, 255, 255));

		auto albedoLayout = new QVBoxLayout();
		albedoLayout->addWidget(albedoColor_);

		auto normalLayout = new QVBoxLayout();
		normalLayout->addWidget(normalColor_);

		auto smoothnessLayout = new QVBoxLayout();
		smoothnessLayout->addWidget(smoothnessColor_);

		auto metalnessLayout = new QVBoxLayout();
		metalnessLayout->addWidget(metalnessColor_);

		auto emissiveLayout = new QVBoxLayout();
		emissiveLayout->addWidget(emissiveColor_);
		
		auto baseColor = new Spoiler(u8"基本颜色");
		baseColor->setFixedWidth(340);
		baseColor->setContentLayout(*albedoLayout);

		auto normal = new Spoiler(u8"法线");
		normal->setFixedWidth(340);
		normal->setContentLayout(*normalLayout);

		auto smoothness = new Spoiler(u8"光滑度");
		smoothness->setFixedWidth(340);
		smoothness->setContentLayout(*smoothnessLayout);

		auto metalness = new Spoiler(u8"金属程度");
		metalness->setFixedWidth(340);
		metalness->setContentLayout(*metalnessLayout);

		auto emissive = new Spoiler(u8"自发光");
		emissive->setFixedWidth(340);
		emissive->setContentLayout(*metalnessLayout);

		auto mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addWidget(summaryWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addWidget(baseColor, 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addWidget(normal, 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addWidget(smoothness, 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addWidget(metalness, 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addWidget(emissive, 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addStretch(500);
		mainLayout_->setContentsMargins(0, 10, 10, 10);

		connect(albedoColor_, SIGNAL(colorSelected(QColor)), this, SLOT(colorSelected(QColor)));
	}

	MaterialModifyWindow::~MaterialModifyWindow()
	{
	}

	void
	MaterialModifyWindow::repaint()
	{
		albedoColor_->setCurrentColor(QColor(255,255,255));
	}

	void
	MaterialModifyWindow::showEvent(QShowEvent* event)
	{
		this->repaint();
	}

	void
	MaterialModifyWindow::colorSelected(QColor color)
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}

	MaterialWindow::MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept
		: behaviour_(behaviour)
	{
		this->hide();
		this->setObjectName("materialWindow");
		this->setWindowTitle(u8"材质");
		this->setFixedWidth(340);
		this->setAcceptDrops(true);

		title_ = std::make_unique<QLabel>();
		title_->setText(u8"材质");

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		listWidget_ = std::make_unique<QListWidget>();
		listWidget_->setIconSize(QSize(210, 210));
		listWidget_->setResizeMode(QListView::Adjust);
		listWidget_->setViewMode(QListView::IconMode);
		listWidget_->setMovement(QListView::Static);
		listWidget_->setSpacing(10);
		listWidget_->setMinimumHeight(this->height());
		listWidget_->setMinimumWidth(this->width());
		listWidget_->setStyleSheet("background:transparent;");
		listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
		titleLayout_ = std::make_unique<QHBoxLayout>();
		titleLayout_->addWidget(title_.get(), 0, Qt::AlignLeft);
		titleLayout_->addWidget(closeButton_.get(), 0, Qt::AlignRight);

		modifyWidget_ = std::make_unique<MaterialModifyWindow>(this);

		modifyMaterialArea_ = new QScrollArea();
		modifyMaterialArea_->setFixedHeight(700);
		modifyMaterialArea_->setWidget(modifyWidget_.get());
		modifyMaterialArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		modifyMaterialArea_->setWidgetResizable(true);
		modifyMaterialArea_->hide();

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addLayout(titleLayout_.get());
		mainLayout_->addWidget(listWidget_.get(), 0, Qt::AlignTop | Qt::AlignCenter);
		mainLayout_->addWidget(modifyMaterialArea_, 0, Qt::AlignTop | Qt::AlignCenter);
		mainLayout_->addStretch(500);
		mainLayout_->setContentsMargins(10, 10, 10, 10);

		connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(listWidget_.get(), SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		connect(listWidget_.get(), SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
	}

	MaterialWindow::~MaterialWindow() noexcept
	{
		title_.reset();
		closeButton_.reset();
		titleLayout_.reset();
		mainLayout_.reset();
	}

	void
	MaterialWindow::showEvent(QShowEvent* event) noexcept
	{
		this->repaint();
		this->updateList();
		QMargins margins = mainLayout_->contentsMargins();
		listWidget_->setMinimumHeight(this->height() - title_->height() * 2 - margins.top() - margins.bottom());
	}

	void
	MaterialWindow::closeEvent()
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
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
					auto material = materialComponent->getMaterial(item->text().toStdString());

					auto meshRenderer = hit.object->getComponent<octoon::MeshRendererComponent>();
					meshRenderer->setMaterial(material, hit.mesh);

					auto offlineMeshRenderer = hit.object->getComponent<octoon::OfflineMeshRendererComponent>();
					offlineMeshRenderer->setMaterial(material, hit.mesh);
				}
			}
		}
	}

	void
	MaterialWindow::itemDoubleClicked(QListWidgetItem* item)
	{
		//listWidget_->hide();
		//modifyMaterialArea_->show();
	}

	void
	MaterialWindow::dragEnterEvent(QDragEnterEvent* event) noexcept
	{
		event->acceptProposedAction();
		event->accept();
	}

	void
	MaterialWindow::dropEvent(QDropEvent* event) noexcept
	{
		auto urls = event->mimeData()->urls();
		if (!urls.isEmpty())
		{
			std::vector<std::wstring> paths;
			for (auto& it : urls)
			{
				auto path = it.toString().toStdWString();
				if (path.find(L"file:///") == 0)
					path = path.substr(8);
			}

			event->accept();
		}
	}

	std::string
	MaterialWindow::currentItem() const noexcept
	{
		return this->listWidget_->currentItem()->text().toStdString();
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

				octoon::math::float3 base(mat["color"][0], mat["color"][1], mat["color"][2]);
				octoon::math::float3 specular = octoon::math::float3::One;
				octoon::math::float3 ambient = octoon::math::float3::Zero;
				octoon::math::float4 edgeColor = octoon::math::float4::Zero;

				QListWidgetItem* item = new QListWidgetItem;
				item->setText(QString::fromStdString(mat["uuid"]));
				item->setSizeHint(QSize(130, 160));

				QLabel* imageLabel = new QLabel;

				if (textureName.empty())
				{
					QPixmap pixmap(":res/icons/material.png");
					imageLabel->setPixmap(pixmap);
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