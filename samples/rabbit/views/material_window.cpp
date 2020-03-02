#include "material_window.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qevent.h>
#include <qscrollbar.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qapplication.h>

namespace rabbit
{
	ListDragWidget::ListDragWidget()
	{
		this->setAcceptDrops(true);
	}

	ListDragWidget::ListDragWidget(QWidget* parent)
		: QListWidget(parent)
	{
		this->setAcceptDrops(true);
	}

	ListDragWidget::~ListDragWidget()
	{

	}
	
	void
	ListDragWidget::mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
			dragPoint_ = event->pos();

		QListWidget::mousePressEvent(event);
	}

	void
	ListDragWidget::mouseMoveEvent(QMouseEvent* event)
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

	void
	ListDragWidget::dragEnterEvent(QDragEnterEvent* event)
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}

	void
	ListDragWidget::dragMoveEvent(QDragMoveEvent* event)
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}

	void
	ListDragWidget::dropEvent(QDropEvent* event)
	{
		QString str = event->mimeData()->text();
		if (!str.isEmpty())
		{
			auto item = this->itemAt(event->pos());
			if (!item)
				this->addItem(str);
			else
				this->insertItem(this->row(item), str);

			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
	}

	MaterialWindow::MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept
		: behaviour_(behaviour)
	{
		this->hide();
		this->setObjectName("materialWindow");
		this->setWindowTitle(u8"²ÄÖÊ");
		this->setFixedWidth(340);

		title_ = std::make_unique<QLabel>();
		title_->setText(u8"²ÄÖÊ");

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"¹Ø±Õ");

		listWidget_ = std::make_unique<ListDragWidget>();
		listWidget_->setIconSize(QSize(210, 210));
		listWidget_->setResizeMode(QListView::Adjust);
		listWidget_->setViewMode(QListView::IconMode);
		listWidget_->setMovement(QListView::Static);
		listWidget_->setSpacing(10);
		listWidget_->setMinimumHeight(this->height());
		listWidget_->setMinimumWidth(this->width());
		listWidget_->setStyleSheet("background:transparent;");
		listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		
		for (std::size_t i = 0; i < 22; i++)
		{
			QListWidgetItem* item = new QListWidgetItem;
			item->setText(tr(u8"Í¼Æ¬%1").arg(i));
			item->setSizeHint(QSize(this->width() / 2 - 28, 158));

			QLabel* imageLabel = new QLabel;
			QPixmap pixmap(":res/icons/material.png");
			imageLabel->setPixmap(pixmap);

			QLabel* txtLabel = new QLabel(tr(u8"Í¼Æ¬%1").arg(i));
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

		titleLayout_ = std::make_unique<QHBoxLayout>();
		titleLayout_->addWidget(title_.get(), 0, Qt::AlignLeft);
		titleLayout_->addWidget(closeButton_.get(), 0, Qt::AlignRight);

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addLayout(titleLayout_.get());
		mainLayout_->addWidget(listWidget_.get(), 0, Qt::AlignTop);
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
	MaterialWindow::showEvent(QShowEvent* event)
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
}