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
	MaterialListWidget::MaterialListWidget()
	{
		this->setAcceptDrops(true);
	}

	MaterialListWidget::MaterialListWidget(QWidget* parent)
		: QListWidget(parent)
	{
		this->setAcceptDrops(true);
	}

	MaterialListWidget::~MaterialListWidget()
	{

	}
	
	void
	MaterialListWidget::mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
			dragPoint_ = event->pos();

		QListWidget::mousePressEvent(event);
	}

	void
	MaterialListWidget::mouseMoveEvent(QMouseEvent* event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			QPoint temp = event->pos() - dragPoint_;

			if (temp.manhattanLength() > QApplication::startDragDistance())
			{
				auto dragItem = this->itemAt(event->pos());
				if (dragItem)
				{
					QMimeData* mimeData = new QMimeData;
					mimeData->setText(dragItem->text());

					QDrag* drag = new QDrag(this);
					drag->setMimeData(mimeData);
					drag->exec(Qt::CopyAction | Qt::MoveAction);
				}
			}
		}

		QListWidget::mouseMoveEvent(event);
	}

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

		listWidget_ = std::make_unique<MaterialListWidget>();
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
	}

	MaterialWindow::~MaterialWindow() noexcept
	{
		title_.reset();
		closeButton_.reset();
		titleLayout_.reset();
		mainLayout_.reset();
	}

	void
	MaterialWindow::import(std::wstring& filepath) noexcept
	{
		std::ifstream stream(filepath);
		if (stream)
		{
			std::map<std::string, int> materialMap;

			std::vector<tinyobj::material_t> materials;
			auto err = tinyobj::LoadMtl(materialMap, materials, stream);
			if (err.empty())
			{
				std::wstring dirpath = filepath.substr(0, filepath.find_last_of(L"/") + 1);

				this->initMaterials(materials, QString::fromStdWString(dirpath).toStdString());

				std::map<QString, std::shared_ptr<QPixmap>> imageTable;

				for (std::size_t i = 0; i < materials.size(); i++)
				{
					auto& material = materials[i];

					QListWidgetItem* item = new QListWidgetItem;
					item->setText(QString::fromStdString(material.name));
					item->setSizeHint(QSize(130, 160));

					QLabel* imageLabel = new QLabel;

					if (material.diffuse_texname.empty())
					{
						QPixmap pixmap(":res/icons/material.png");
						imageLabel->setPixmap(pixmap);
					}
					else
					{
						auto texpath = QString::fromStdWString(dirpath) + QString::fromStdString(material.diffuse_texname);
						if (!imageTable[texpath])
						{
							QImage image(texpath);
							image.convertTo(QImage::Format::Format_RGB888);
							imageTable[texpath] = std::make_shared<QPixmap>(QPixmap::fromImage(image.scaled(QSize(130, 130))));
						}

						imageLabel->setPixmap(*imageTable[texpath]);
					}

					QLabel* txtLabel = new QLabel(QString::fromStdString(material.name));
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

	void
	MaterialWindow::showEvent(QShowEvent* event) noexcept
	{
		this->repaint();

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

				this->import(path);
			}

			event->accept();
		}
	}

	octoon::model::MaterialPtr
	MaterialWindow::getMaterial(const std::string& name) const noexcept
	{
		return this->materials_.front();
	}

	void
	MaterialWindow::initMaterials(const std::vector<tinyobj::material_t>& materials, const std::string& rootPath)
	{
		for (auto& it : materials)
		{
			auto material = std::make_shared<octoon::model::Material>();
			material->set(MATKEY_NAME, it.name);
			material->set(MATKEY_PATH, rootPath);
			material->set(MATKEY_COLOR_DIFFUSE, octoon::math::float3::One);
			material->set(MATKEY_TEXTURE_DIFFUSE, it.diffuse_texname);

			this->materials_.emplace_back(std::move(material));
		}
	}
}