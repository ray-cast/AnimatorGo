#include "light_dock.h"
#include <qscrollbar.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qapplication.h>
#include <octoon/transform_component.h>
#include <octoon/directional_light_component.h>

namespace unreal
{
	LightListWindow::LightListWindow() noexcept(false)
	{
		this->setResizeMode(QListView::Adjust);
		this->setViewMode(QListView::IconMode);
		this->setMovement(QListView::Static);
		this->setDefaultDropAction(Qt::DropAction::MoveAction);
		this->setSpacing(2);
		this->setStyleSheet("background:transparent;");
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

	LightListWindow::~LightListWindow() noexcept
	{
	}

	void
	LightListWindow::mouseMoveEvent(QMouseEvent *event)
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
						mimeData->setData("object/light", item->data(Qt::UserRole).toByteArray());

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
	LightListWindow::mousePressEvent(QMouseEvent *event)
	{
		if (event->button() == Qt::LeftButton)
			startPos = event->pos();

		QListWidget::mousePressEvent(event);
	}

	LightDock::LightDock(const std::shared_ptr<unreal::UnrealProfile>& profile)
		: profile_(profile)
	{
		this->setObjectName("LightDock");
		this->setWindowTitle(tr("Light"));

		listWidget_ = new LightListWindow;

		std::vector<QString> lights;
		lights.push_back(tr("Point Light"));
		lights.push_back(tr("Spot Light"));
		lights.push_back(tr("Directional Light"));

		for (int i = 0; i < 3; i++)
		{
			QListWidgetItem* item = new QListWidgetItem;
			item->setSizeHint(QSize(150, 130));
			item->setData(Qt::UserRole, i);

			QLabel* imageLabel = new QLabel;
			imageLabel->setPixmap(QPixmap(":res/icons/sun.png").scaled(100, 100));

			QLabel* txtLabel = new QLabel(lights[i]);

			QVBoxLayout* widgetLayout = new QVBoxLayout;
			widgetLayout->setContentsMargins(0, 0, 0, 0);
			widgetLayout->setSpacing(0);
			widgetLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
			widgetLayout->addWidget(txtLabel, 0, Qt::AlignCenter);

			QWidget* widget = new QWidget;
			widget->setLayout(widgetLayout);

			listWidget_->addItem(item);
			listWidget_->setItemWidget(item, widget);
		}

		mainLayout_ = new QVBoxLayout();
		mainLayout_->addWidget(listWidget_);
		mainLayout_->addStretch();
		mainLayout_->setContentsMargins(0, 10, 0, 5);

		mainWidget_ = new QWidget;
		mainWidget_->setLayout(mainLayout_);

		this->setWidget(mainWidget_);
	}

	LightDock::~LightDock()
	{
	}

	void
	LightDock::showEvent(QShowEvent* event)
	{
	}

	void
	LightDock::closeEvent(QCloseEvent* event)
	{
		if (profile_->playerModule->isPlaying)
			event->ignore();
		else
			event->accept();
	}

	void
	LightDock::resizeEvent(QResizeEvent* event)
	{
		QMargins margins = mainLayout_->contentsMargins();
		listWidget_->resize(mainWidget_->width(), mainWidget_->height() - margins.top() - margins.bottom());
	}
}