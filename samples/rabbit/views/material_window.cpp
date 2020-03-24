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

namespace rabbit
{
	MaterialWindow::MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept
		: behaviour_(behaviour)
	{
		this->hide();
		this->setObjectName("materialWindow");
		this->setWindowTitle(u8"²ÄÖÊ");
		this->setFixedWidth(340);
		this->setAcceptDrops(true);

		title_ = std::make_unique<QLabel>();
		title_->setText(u8"²ÄÖÊ");

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"¹Ø±Õ");

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

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addLayout(titleLayout_.get());
		mainLayout_->addWidget(listWidget_.get(), 0, Qt::AlignTop | Qt::AlignCenter);
		mainLayout_->addStretch(500);
		mainLayout_->setContentsMargins(10, 10, 10, 10);

		connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(listWidget_.get(), SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
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
	MaterialWindow::itemSelectionChanged()
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
					auto material = materialComponent->getMaterial(this->currentItem());

					auto meshRenderer = hit.object->getComponent<octoon::MeshRendererComponent>();
					meshRenderer->setMaterial(material, hit.mesh);

					auto offlineMeshRenderer = hit.object->getComponent<octoon::OfflineMeshRendererComponent>();
					offlineMeshRenderer->setMaterial(material, hit.mesh);
				}
			}
		}
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