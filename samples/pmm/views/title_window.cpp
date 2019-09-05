#include "title_window.h"

TitleBar::TitleBar(QWidget* parent)
	: maxNormal(false)
{
	maxPix = QIcon::fromTheme("res", QIcon(":res/icons/maximize.png"));
	restorePix = QIcon::fromTheme("res", QIcon(":res/icons/restore.png"));

	minimizeButton = new QToolButton(this);
	minimizeButton->setIcon(QIcon::fromTheme("res", QIcon(":res/icons/minimize.png")));
	minimizeButton->setToolTip(u8"最小化");

	maximizeButton = new QToolButton(this);
	maximizeButton->setIcon(maxPix);
	maximizeButton->setToolTip(u8"最大化");

	closeButton = new QToolButton(this);
	closeButton->setIcon(QIcon::fromTheme("res", QIcon(":res/icons/close.png")));
	closeButton->setToolTip(u8"关闭");

	settingButton = new QToolButton(this);
	settingButton->setIcon(QIcon::fromTheme("res", QIcon(":res/icons/setting.png")));
	settingButton->setToolTip(u8"全局设置");

	logoButton = new QPushButton(this);
	logoButton->setIcon(QIcon::fromTheme("res", QIcon(":res/icons/logo.png")));

	QLabel* titleLabel = new QLabel(this);
	titleLabel->setText(u8"兔纸渲染器");
	titleLabel->setFont(QFont("Microsoft YaHei", 7, QFont::Normal));

	QHBoxLayout* hbox = new QHBoxLayout(this);
	hbox->addWidget(logoButton);
	hbox->addWidget(titleLabel);
	hbox->addWidget(settingButton);
	hbox->addWidget(minimizeButton);
	hbox->addWidget(maximizeButton);
	hbox->addWidget(closeButton);

	hbox->setSpacing(0);
	hbox->insertStretch(2, 500);
	hbox->setContentsMargins(10, 0, 10, 0);

	this->connect(closeButton, SIGNAL(clicked()), parent, SLOT(close()));
	this->connect(minimizeButton, SIGNAL(clicked()), this, SLOT(showSmall()));
	this->connect(maximizeButton, SIGNAL(clicked()), this, SLOT(showMaxRestore()));

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
	if (maxNormal)
	{
		parentWidget()->setMinimumSize(size);
		parentWidget()->showNormal();
		parentWidget()->showMaximized();
		parentWidget()->showNormal();
		maxNormal = !maxNormal;
		maximizeButton->setIcon(maxPix);
	}
	else
	{
		size = parentWidget()->size();
		parentWidget()->showMaximized();
		maxNormal = !maxNormal;
		maximizeButton->setIcon(restorePix);
	}
}

void
TitleBar::mousePressEvent(QMouseEvent* me)
{
	allowMove = true;
	startPos = me->globalPos();
	clickPos = mapToParent(me->pos());
}

void
TitleBar::mouseReleaseEvent(QMouseEvent* me)
{
	allowMove = false;
}

void
TitleBar::mouseMoveEvent(QMouseEvent* me)
{
	if (allowMove && !maxNormal)
		parentWidget()->move(me->globalPos() - clickPos);
}