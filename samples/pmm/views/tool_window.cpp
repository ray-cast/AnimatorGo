#include "tool_window.h"

ToolBar::ToolBar(QWidget* parent) noexcept
	: gpuEnable_(false)
	, playEnable_(false)
	, recordEnable_(false)
{
	playIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/play.png"));
	stopIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/stop.png"));
	gpuIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/gpu.png"));
	gpuOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/gpu-on.png"));
	recordIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/record.png"));
	recordOnIcon_ = QIcon::fromTheme("res", QIcon(":res/icons/record-on.png"));

	hideButton_ = std::make_unique<QToolButton>(this);
	hideButton_->setObjectName("hide");
	hideButton_->setToolTip(u8"隐藏工具栏");

	importButton_ = std::make_unique<QToolButton>(this);
	importButton_->setObjectName("import");
	importButton_->setToolTip(u8"加载文件");

	playButton_ = std::make_unique<QToolButton>(this);
	playButton_->setObjectName("play");
	playButton_->setIcon(playIcon_);
	playButton_->setToolTip(u8"播放");

	recordButton_ = std::make_unique<QToolButton>(this);
	recordButton_->setObjectName("record");
	recordButton_->setIcon(recordIcon_);
	recordButton_->setToolTip(u8"录制视频");

	shotButton_ = std::make_unique<QToolButton>(this);
	shotButton_->setObjectName("shot");
	shotButton_->setToolTip(u8"截图");

	gpuButton_ = std::make_unique<QToolButton>(this);
	gpuButton_->setObjectName("gpu");
	gpuButton_->setToolTip(u8"高质量渲染");

	hdrButton_ = std::make_unique<QToolButton>(this);
	hdrButton_->setObjectName("hdr");
	hdrButton_->setToolTip(u8"加载高动态图像");

	layout_ = std::make_unique<QVBoxLayout>(this);
	layout_->addWidget(hideButton_.get());
	layout_->addWidget(importButton_.get());
	layout_->addWidget(playButton_.get());
	layout_->addWidget(recordButton_.get());
	layout_->addWidget(shotButton_.get());
	layout_->addWidget(gpuButton_.get());
	layout_->addWidget(hdrButton_.get());
	layout_->addStretch();
	layout_->setSpacing(0);
	layout_->setContentsMargins(0, 0, 0, 0);

	this->connect(hideButton_.get(), SIGNAL(clicked()), this, SLOT(hideEvent()));
	this->connect(importButton_.get(), SIGNAL(clicked()), this, SLOT(importEvent()));
	this->connect(playButton_.get(), SIGNAL(clicked()), this, SLOT(playEvent()));
	this->connect(recordButton_.get(), SIGNAL(clicked()), this, SLOT(recordEvent()));
	this->connect(shotButton_.get(), SIGNAL(clicked()), this, SLOT(shotEvent()));
	this->connect(gpuButton_.get(), SIGNAL(clicked()), this, SLOT(gpuEvent()));
	this->connect(hdrButton_.get(), SIGNAL(clicked()), this, SLOT(hdrEvent()));

	this->setObjectName("toolWidget");
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

ToolBar::~ToolBar() noexcept
{
}

void
ToolBar::hideEvent() noexcept
{
	emit hideSignal();
}

void
ToolBar::importEvent() noexcept
{
	emit importSignal();
}

void
ToolBar::playEvent() noexcept
{
	if (!playEnable_)
	{
		if (playSignal(true))
		{
			playButton_->setIcon(stopIcon_);
			playButton_->setToolTip(u8"停止");
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
ToolBar::recordEvent() noexcept
{
	if (!recordEnable_)
	{
		if (recordSignal(true))
		{
			recordButton_->setIcon(recordOnIcon_);
			recordEnable_ = true;
			gpuButton_->setIcon(gpuOnIcon_);
			gpuEnable_ = true;
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
ToolBar::shotEvent() noexcept
{
	emit shotSignal();
}

void
ToolBar::gpuEvent() noexcept
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
ToolBar::hdrEvent() noexcept
{
	emit hdrSignal();
}