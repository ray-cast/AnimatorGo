#include "title_window.h"

TitleBar::TitleBar(QWidget* parent)
	: maxNormal_(false)
{
	maxPix_ = QIcon::fromTheme("res", QIcon(":res/icons/maximize.png"));
	restorePix_ = QIcon::fromTheme("res", QIcon(":res/icons/restore.png"));

	minimizeButton_ = std::make_unique<QToolButton>(this);
	minimizeButton_->setObjectName("minimize");
	minimizeButton_->setToolTip(u8"最小化");

	maximizeButton_ = std::make_unique<QToolButton>(this);
	maximizeButton_->setObjectName("maximize");
	maximizeButton_->setIcon(maxPix_);
	maximizeButton_->setToolTip(u8"最大化");

	closeButton_ = std::make_unique<QToolButton>(this);
	closeButton_->setObjectName("close");
	closeButton_->setToolTip(u8"关闭");

	settingButton_ = std::make_unique<QToolButton>(this);
	settingButton_->setObjectName("setting");
	settingButton_->setToolTip(u8"全局设置");

	logoButton_ = std::make_unique<QPushButton>(this);
	logoButton_->setObjectName("logo");

	titleLabel_ = std::make_unique<QLabel>(this);
	titleLabel_->setObjectName("title");
	titleLabel_->setText(u8"兔纸渲染器");
	titleLabel_->setFont(QFont("Microsoft YaHei", 7, QFont::Normal));

	layout_ = std::make_unique<QHBoxLayout>(this);
	layout_->addWidget(logoButton_.get());
	layout_->addWidget(titleLabel_.get());
	layout_->addWidget(settingButton_.get());
	layout_->addWidget(minimizeButton_.get());
	layout_->addWidget(maximizeButton_.get());
	layout_->addWidget(closeButton_.get());

	layout_->setSpacing(0);
	layout_->insertStretch(2, 500);
	layout_->setContentsMargins(10, 0, 10, 0);

	this->connect(closeButton_.get(), SIGNAL(clicked()), parent, SLOT(close()));
	this->connect(minimizeButton_.get(), SIGNAL(clicked()), this, SLOT(showSmall()));
	this->connect(maximizeButton_.get(), SIGNAL(clicked()), this, SLOT(showMaxRestore()));

	this->setFixedHeight(32);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

TitleBar::~TitleBar()
{
}

void
TitleBar::showSmall()
{
	parentWidget()->showMinimized();
}

void
TitleBar::showMaxRestore()
{
	if (maxNormal_)
	{
		parentWidget()->setMinimumSize(size_);
		parentWidget()->showNormal();
		parentWidget()->showMaximized();
		parentWidget()->showNormal();
		maxNormal_ = !maxNormal_;
		maximizeButton_->setIcon(maxPix_);
	}
	else
	{
		size_ = parentWidget()->size();
		parentWidget()->showMaximized();
		maxNormal_ = !maxNormal_;
		maximizeButton_->setIcon(restorePix_);
	}
}

void
TitleBar::mousePressEvent(QMouseEvent* me)
{
	allowMove_ = true;
	startPos_ = me->globalPos();
	clickPos_ = mapToParent(me->pos());
}

void
TitleBar::mouseReleaseEvent(QMouseEvent* me)
{
	allowMove_ = false;
}

void
TitleBar::mouseMoveEvent(QMouseEvent* me)
{
	if (allowMove_ && !maxNormal_)
		parentWidget()->move(me->globalPos() - clickPos_);
}