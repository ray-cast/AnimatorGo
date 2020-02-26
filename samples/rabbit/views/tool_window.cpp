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
	{
		playIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/play.png"));
		playOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/play-on.png"));
		gpuIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/gpu.png"));
		gpuOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/gpu-on.png"));
		recordIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/record.png"));
		recordOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/record-on.png"));
		hdrIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/hdr.png"));
		hdrOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/hdr-on.png"));
		sunIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/sun.png"));
		sunOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/sun-on.png"));
		environmentIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/environment.png"));
		environmentOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/environment-on.png"));

		hideButton_ = std::make_unique<QToolButton>(this);
		hideButton_->setObjectName("hide");
		hideButton_->setToolTip(u8"隐藏工具栏");

		importButton_ = std::make_unique<QToolButton>(this);
		importButton_->setObjectName("import");
		importButton_->setToolTip(u8"加载文件");

		playButton_ = std::make_unique<QToolButton>(this);
		playButton_->setObjectName("play");
		playButton_->setToolTip(u8"播放");

		resetButton_ = std::make_unique<QToolButton>(this);
		resetButton_->setObjectName("reset");
		resetButton_->setToolTip(u8"重置");

		leftButton_ = std::make_unique<QToolButton>(this);
		leftButton_->setObjectName("left");
		leftButton_->setToolTip(u8"快退");

		rightButton_ = std::make_unique<QToolButton>(this);
		rightButton_->setObjectName("right");
		rightButton_->setToolTip(u8"快进");

		recordButton_ = std::make_unique<QToolButton>(this);
		recordButton_->setObjectName("record");
		recordButton_->setToolTip(u8"录制视频");

		shotButton_ = std::make_unique<QToolButton>(this);
		shotButton_->setObjectName("shot");
		shotButton_->setToolTip(u8"降噪截图");

		gpuButton_ = std::make_unique<QToolButton>(this);
		gpuButton_->setObjectName("gpu");
		gpuButton_->setToolTip(u8"开启高质量渲染模式");

		hdrButton_ = std::make_unique<QToolButton>(this);
		hdrButton_->setObjectName("hdr");
		hdrButton_->setToolTip(u8"加载高动态图像");

		cleanupButton_ = std::make_unique<QToolButton>(this);
		cleanupButton_->setObjectName("cleanup");
		cleanupButton_->setToolTip(u8"清空项目");

		sunButton_ = std::make_unique<QToolButton>(this);
		sunButton_->setObjectName("sun");
		sunButton_->setToolTip(u8"主光源");

		environmentButton_ = std::make_unique<QToolButton>(this);
		environmentButton_->setObjectName("environment");
		environmentButton_->setToolTip(u8"环境光");

		materialButton_ = std::make_unique<QToolButton>(this);
		materialButton_->setObjectName("material");
		materialButton_->setToolTip(u8"材质");

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addWidget(hideButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(importButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(playButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(resetButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(leftButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(rightButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(recordButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(shotButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(gpuButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(hdrButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(sunButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(environmentButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(materialButton_.get(), 0, Qt::AlignCenter);
		layout_->addWidget(cleanupButton_.get(), 0, Qt::AlignCenter);
		layout_->addStretch();
		layout_->setSpacing(0);
		layout_->setContentsMargins(0, 0, 0, 0);

		this->connect(hideButton_.get(), SIGNAL(clicked()), this, SLOT(hideEvent()));
		this->connect(importButton_.get(), SIGNAL(clicked()), this, SLOT(importEvent()));
		this->connect(playButton_.get(), SIGNAL(clicked()), this, SLOT(playEvent()));
		this->connect(leftButton_.get(), SIGNAL(clicked()), this, SLOT(leftEvent()));
		this->connect(rightButton_.get(), SIGNAL(clicked()), this, SLOT(rightEvent()));
		this->connect(resetButton_.get(), SIGNAL(clicked()), this, SLOT(resetEvent()));
		this->connect(recordButton_.get(), SIGNAL(clicked()), this, SLOT(recordEvent()));
		this->connect(shotButton_.get(), SIGNAL(clicked()), this, SLOT(shotEvent()));
		this->connect(gpuButton_.get(), SIGNAL(clicked()), this, SLOT(gpuEvent()));
		this->connect(hdrButton_.get(), SIGNAL(clicked()), this, SLOT(hdrEvent()));
		this->connect(cleanupButton_.get(), SIGNAL(clicked()), this, SLOT(cleanupEvent()));
		this->connect(sunButton_.get(), SIGNAL(clicked()), this, SLOT(sunEvent()));
		this->connect(environmentButton_.get(), SIGNAL(clicked()), this, SLOT(environmentEvent()));
		this->connect(materialButton_.get(), SIGNAL(clicked()), this, SLOT(materialEvent()));

		this->setObjectName("toolWidget");
		this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
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
				playButton_->setIcon(playOnIcon_);
				playButton_->setToolTip(u8"暂停");
				playEnable_ = true;
			}
		}
		else
		{
			if (playSignal(false))
			{
				playButton_->setIcon(playIcon_);
				playButton_->setToolTip(u8"播放");
				playEnable_ = false;
			}
		}
	}

	void
	ToolWindow::resetEvent() noexcept
	{
		if (resetSignal())
		{
			playButton_->setIcon(playIcon_);
			playButton_->setToolTip(u8"播放");
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
				recordButton_->setIcon(recordOnIcon_);
				recordEnable_ = true;
			}
		}
		else
		{
			if (recordSignal(false))
			{
				recordButton_->setIcon(recordIcon_);
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
				gpuButton_->setIcon(gpuOnIcon_);
				gpuEnable_ = true;
			}
		}
		else
		{
			if (gpuSignal(false))
			{
				gpuButton_->setIcon(gpuIcon_);
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
				hdrButton_->setIcon(hdrOnIcon_);
				hdrEnable_ = true;
			}
		}
		else
		{
			if (hdrSignal(false))
			{
				hdrButton_->setIcon(hdrIcon_);
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