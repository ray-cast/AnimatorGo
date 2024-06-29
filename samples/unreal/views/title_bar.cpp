#include "title_bar.h"

namespace unreal
{
	TitleBar::TitleBar(QWidget* parent) noexcept
		: allowMove_(true)
	{
		settingButton_ = std::make_unique<QPushButton>(this);
		settingButton_->setObjectName("setting");

		titleLabel_ = std::make_unique<QLabel>(this);
		titleLabel_->setObjectName("title");
		titleLabel_->setText(tr("Settings"));

		closeButton_ = std::make_unique<QToolButton>(this);
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(tr("Close"));

		layout_ = std::make_unique<QHBoxLayout>(this);
		layout_->setObjectName("titleLayout");
		layout_->addWidget(settingButton_.get());
		layout_->addWidget(titleLabel_.get());
		layout_->addWidget(closeButton_.get());

		layout_->setSpacing(0);
		layout_->insertStretch(2, 440);
		layout_->setContentsMargins(10, 0, 0, 0);
		
		this->connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));

		this->setObjectName("settingTitle");
		this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		
		this->installEventFilter(this);
	}

	TitleBar::~TitleBar() noexcept
	{
	}

	void
	TitleBar::closeEvent()
	{
		emit closeSignal();
	}

	void
	TitleBar::mousePressEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = true;
		startPos_ = e->globalPos();
		clickPos_ = mapToParent(e->pos());
	}

	void
	TitleBar::mouseReleaseEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = false;
	}

	void
	TitleBar::mouseMoveEvent(QMouseEvent* e) noexcept
	{
		if (allowMove_)
			parentWidget()->move(e->globalPos() - clickPos_);
	}
	bool
	TitleBar::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() == QEvent::LanguageChange)
		{
			retranslate();
		}

		return QWidget::eventFilter(watched, event);
	}
	void
	TitleBar::retranslate()
	{
		titleLabel_->setText(tr("Settings"));
		closeButton_->setToolTip(tr("Close"));
	}
}