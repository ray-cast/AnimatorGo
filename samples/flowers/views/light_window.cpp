#include "light_window.h"
#include <qscrollbar.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qapplication.h>
#include <octoon/transform_component.h>
#include <octoon/directional_light_component.h>

namespace flower
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

	LightWindow::LightWindow(const std::shared_ptr<flower::FlowerProfile>& profile)
	{
		this->setObjectName("LightWindow");
		this->setMinimumWidth(340);
		this->grabKeyboard();
		this->hide();

		title_ = new QLabel();
		title_->setText(u8"光源");

		closeButton_ = new QToolButton();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		titleLayout_ = new QHBoxLayout();
		titleLayout_->addStretch();
		titleLayout_->addWidget(title_, 0, Qt::AlignCenter);
		titleLayout_->addStretch();
		titleLayout_->addWidget(closeButton_, 0, Qt::AlignRight);
		titleLayout_->setContentsMargins(10, 0, 10, 0);

		listWidget_ = new LightListWindow;

		std::vector<const char*> lights;
		lights.push_back(u8"点光源");
		lights.push_back(u8"聚光灯");
		lights.push_back(u8"平行光");

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

		mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addLayout(titleLayout_);
		mainLayout_->addWidget(listWidget_);
		mainLayout_->addStretch();
		mainLayout_->setContentsMargins(0, 10, 0, 5);

		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeEvent()));
	}

	LightWindow::~LightWindow()
	{
	}

	void
	LightWindow::repaint()
	{
	}

	void
	LightWindow::showEvent(QShowEvent* event)
	{
		this->repaint();
	}

	void
	LightWindow::resizeEvent(QResizeEvent* event)
	{
		QMargins margins = mainLayout_->contentsMargins();
		listWidget_->setMinimumHeight(this->height() - title_->height() * 2 - margins.top() - margins.bottom());
	}

	void
	LightWindow::closeEvent()
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}
}