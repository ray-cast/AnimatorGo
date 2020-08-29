#include "tool_window.h"

namespace rabbit
{
	ToolWindow::ToolWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept
		: QWidget(parent)
		, gpuEnable_(false)
		, playEnable_(false)
		, recordEnable_(false)
		, hdrEnable_(false)
		, sunEnable_(false)
		, environmentEnable_(false)
		, behaviour_(behaviour)
		, playIcon_(QIcon::fromTheme("res", QIcon(":res/icons/play.png")))
		, playOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/play-on.png")))
		, gpuIcon_(QIcon::fromTheme("res", QIcon(":res/icons/gpu.png")))
		, gpuOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/gpu-on.png")))
		, recordIcon_(QIcon::fromTheme("res", QIcon(":res/icons/record.png")))
		, recordOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/record-on.png")))
		, hdrIcon_(QIcon::fromTheme("res", QIcon(":res/icons/hdr.png")))
		, hdrOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/hdr-on.png")))
		, sunIcon_(QIcon::fromTheme("res", QIcon(":res/icons/sun.png")))
		, sunOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/sun-on.png")))
		, environmentIcon_(QIcon::fromTheme("res", QIcon(":res/icons/environment.png")))
		, environmentOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/environment-on.png")))
	{
		this->setObjectName("ToolWindow");
		this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

		hideButton = new QToolButton(this);
		hideButton->setObjectName("hide");
		hideButton->setText(u8"隐藏");
		hideButton->setToolTip(u8"隐藏工具栏");
		hideButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		importButton = new QToolButton(this);
		importButton->setObjectName("import");
		importButton->setText(u8"打开");
		importButton->setToolTip(u8"加载文件");
		importButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		playButton = new QToolButton(this);
		playButton->setObjectName("play");
		playButton->setText(u8"播放");
		playButton->setToolTip(u8"播放");
		playButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		resetButton = new QToolButton(this);
		resetButton->setObjectName("reset");
		resetButton->setText(u8"重置");
		resetButton->setToolTip(u8"重置");
		resetButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		leftButton = new QToolButton(this);
		leftButton->setObjectName("left");
		leftButton->setText(u8"后退");
		leftButton->setToolTip(u8"快退");
		leftButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		rightButton = new QToolButton(this);
		rightButton->setObjectName("right");
		rightButton->setText(u8"快进");
		rightButton->setToolTip(u8"快进");
		rightButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		recordButton = new QToolButton(this);
		recordButton->setObjectName("record");
		recordButton->setText(u8"录制");
		recordButton->setToolTip(u8"录制视频");
		recordButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		shotButton = new QToolButton(this);
		shotButton->setObjectName("shot");
		shotButton->setText(u8"截图");
		shotButton->setToolTip(u8"降噪截图");
		shotButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		gpuButton = new QToolButton(this);
		gpuButton->setObjectName("gpu");
		gpuButton->setText(u8"渲染");
		gpuButton->setToolTip(u8"开启高质量渲染模式");
		gpuButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		cleanupButton = new QToolButton(this);
		cleanupButton->setObjectName("cleanup");
		cleanupButton->setText(u8"清空");
		cleanupButton->setToolTip(u8"清空项目");
		cleanupButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		hdrButton = new QToolButton(this);
		hdrButton->setObjectName("hdr");
		hdrButton->setText(u8"全局光");
		hdrButton->setToolTip(u8"加载高动态图像");
		hdrButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		sunButton = new QToolButton(this);
		sunButton->setObjectName("sun");
		sunButton->setText(u8"主光源");
		sunButton->setToolTip(u8"主光源");
		sunButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		environmentButton = new QToolButton(this);
		environmentButton->setObjectName("environment");
		environmentButton->setText(u8"环境光");
		environmentButton->setToolTip(u8"环境光");
		environmentButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		materialButton = new QToolButton(this);
		materialButton->setObjectName("material");
		materialButton->setText(u8"材质");
		materialButton->setToolTip(u8"材质");
		materialButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		layout = new QVBoxLayout(this);
		layout->addSpacing(1);
		layout->addWidget(hideButton, 0, Qt::AlignCenter);
		layout->addWidget(importButton, 0, Qt::AlignCenter);
		layout->addWidget(playButton, 0, Qt::AlignCenter);
		layout->addWidget(resetButton, 0, Qt::AlignCenter);
		layout->addWidget(leftButton, 0, Qt::AlignCenter);
		layout->addWidget(rightButton, 0, Qt::AlignCenter);
		layout->addWidget(recordButton, 0, Qt::AlignCenter);
		layout->addWidget(shotButton, 0, Qt::AlignCenter);
		layout->addWidget(gpuButton, 0, Qt::AlignCenter);
		layout->addWidget(hdrButton, 0, Qt::AlignCenter);
		layout->addWidget(sunButton, 0, Qt::AlignCenter);
		layout->addWidget(environmentButton, 0, Qt::AlignCenter);
		layout->addWidget(materialButton, 0, Qt::AlignCenter);
		layout->addWidget(cleanupButton, 0, Qt::AlignCenter);
		layout->addStretch();
		layout->setSpacing(0);
		layout->setContentsMargins(0, 0, 0, 0);

		this->connect(hideButton, SIGNAL(clicked()), this, SLOT(hideEvent()));
		this->connect(importButton, SIGNAL(clicked()), this, SLOT(importEvent()));
		this->connect(playButton, SIGNAL(clicked()), this, SLOT(playEvent()));
		this->connect(leftButton, SIGNAL(clicked()), this, SLOT(leftEvent()));
		this->connect(rightButton, SIGNAL(clicked()), this, SLOT(rightEvent()));
		this->connect(resetButton, SIGNAL(clicked()), this, SLOT(resetEvent()));
		this->connect(recordButton, SIGNAL(clicked()), this, SLOT(recordEvent()));
		this->connect(shotButton, SIGNAL(clicked()), this, SLOT(shotEvent()));
		this->connect(gpuButton, SIGNAL(clicked()), this, SLOT(gpuEvent()));
		this->connect(hdrButton, SIGNAL(clicked()), this, SLOT(hdrEvent()));
		this->connect(cleanupButton, SIGNAL(clicked()), this, SLOT(cleanupEvent()));
		this->connect(sunButton, SIGNAL(clicked()), this, SLOT(sunEvent()));
		this->connect(environmentButton, SIGNAL(clicked()), this, SLOT(environmentEvent()));
		this->connect(materialButton, SIGNAL(clicked()), this, SLOT(materialEvent()));
	}

	ToolWindow::~ToolWindow() noexcept
	{
	}

	void
	ToolWindow::showEvent(QShowEvent* e) noexcept
	{
	}

	void
	ToolWindow::hideEvent() noexcept
	{
		emit hideSignal();
	}

	void
	ToolWindow::importEvent() noexcept
	{
		emit importSignal();
	}

	void
	ToolWindow::playEvent() noexcept
	{
		if (!playEnable_)
		{
			if (playSignal(true))
			{
				playButton->setIcon(playOnIcon_);
				playButton->setToolTip(u8"暂停");
				playEnable_ = true;
			}
		}
		else
		{
			if (playSignal(false))
			{
				playButton->setIcon(playIcon_);
				playButton->setToolTip(u8"播放");
				playEnable_ = false;
			}
		}
	}

	void
	ToolWindow::resetEvent() noexcept
	{
		if (resetSignal())
		{
			playButton->setIcon(playIcon_);
			playButton->setToolTip(u8"播放");
			playEnable_ = false;
		}
	}

	void
	ToolWindow::leftEvent() noexcept
	{
		emit leftSignal();
	}

	void 
	ToolWindow::rightEvent() noexcept
	{
		emit rightSignal();
	}

	void
	ToolWindow::recordEvent() noexcept
	{
		if (!recordEnable_)
		{
			if (recordSignal(true))
			{
				recordButton->setIcon(recordOnIcon_);
				recordEnable_ = true;
			}
		}
		else
		{
			if (recordSignal(false))
			{
				recordButton->setIcon(recordIcon_);
				recordEnable_ = false;
			}
		}
	}

	void
	ToolWindow::shotEvent() noexcept
	{
		emit shotSignal();
	}

	void
	ToolWindow::gpuEvent() noexcept
	{
		if (!gpuEnable_)
		{
			if (gpuSignal(true))
			{
				gpuButton->setIcon(gpuOnIcon_);
				gpuEnable_ = true;
			}
		}
		else
		{
			if (gpuSignal(false))
			{
				gpuButton->setIcon(gpuIcon_);
				gpuEnable_ = false;
			}
		}
	}

	void
	ToolWindow::hdrEvent() noexcept
	{
		if (!hdrEnable_)
		{
			if (hdrSignal(true))
			{
				hdrButton->setIcon(hdrOnIcon_);
				hdrEnable_ = true;
			}
		}
		else
		{
			if (hdrSignal(false))
			{
				hdrButton->setIcon(hdrIcon_);
				hdrEnable_ = false;
			}
		}
	}

	void
	ToolWindow::cleanupEvent() noexcept
	{
		emit cleanupSignal();
	}

	void
	ToolWindow::sunEvent() noexcept
	{
		emit sunSignal();
	}

	void
	ToolWindow::materialEvent() noexcept
	{
		emit materialSignal();
	}

	void
	ToolWindow::environmentEvent() noexcept
	{
		emit environmentSignal();
	}

	void
	ToolWindow::mousePressEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = true;
		startPos_ = e->globalPos();
		clickPos_ = mapToParent(e->pos());
	}

	void
	ToolWindow::mouseReleaseEvent(QMouseEvent* e) noexcept
	{
		allowMove_ = false;
	}

	void
	ToolWindow::mouseMoveEvent(QMouseEvent* e) noexcept
	{
		if (allowMove_)
			parentWidget()->move(e->globalPos() - clickPos_);
	}
}