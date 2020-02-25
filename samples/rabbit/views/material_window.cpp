#include "material_window.h"
#include <qfiledialog.h>
#include <qmessagebox.h>

namespace rabbit
{
	class SpinBox final : public QSpinBox
	{
	public:
		void focusInEvent(QFocusEvent* event) override
		{
			this->grabKeyboard();
			QSpinBox::focusInEvent(event);
		}

		void focusOutEvent(QFocusEvent* event) override
		{
			this->releaseKeyboard();
			QSpinBox::focusOutEvent(event);
		}
	};

	MaterialWindow::MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept
		: behaviour_(behaviour)
		, timer_(std::make_unique<QTimer>(this))
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

		layout_ = std::make_unique<QHBoxLayout>();
		layout_->addWidget(title_.get(), 0, Qt::AlignLeft);
		layout_->addWidget(closeButton_.get(), 0, Qt::AlignRight);

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addLayout(layout_.get());
		mainLayout_->addStretch(500);
		mainLayout_->setContentsMargins(10, 10, 10, 10);

		connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		//connect(timer_.get(), SIGNAL(timeout()), this, SLOT(timeEvent()));
	}

	MaterialWindow::~MaterialWindow() noexcept
	{
		title_.reset();
		closeButton_.reset();
		layout_.reset();
		mainLayout_.reset();
	}

	void
	MaterialWindow::showEvent(QShowEvent* event)
	{
		this->repaint();
	}

	void
	MaterialWindow::closeEvent()
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}
}