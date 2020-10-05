#include "record_window.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qevent.h>

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

	class DoubleSpinBox final : public QDoubleSpinBox
	{
	public:
		void focusInEvent(QFocusEvent* event) override
		{
			this->grabKeyboard();
			QDoubleSpinBox::focusInEvent(event);
		}

		void focusOutEvent(QFocusEvent* event) override
		{
			this->releaseKeyboard();
			QDoubleSpinBox::focusOutEvent(event);
		}
	};

	RecordWindow::RecordWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept
		: behaviour_(behaviour)
		, timer_(new QTimer(this))
	{
		this->hide();
		this->setObjectName("RecordWindow");
		this->setWindowTitle(u8"选项");
		this->setFixedWidth(340);
		this->grabKeyboard();

		title_ = new QLabel();
		title_->setText(u8"渲染");

		closeButton_ = new QToolButton();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		markButton_ = new QToolButton();
		markButton_->setObjectName("mark");
		markButton_->setIcon(QIcon::fromTheme(":res/icons/append2.png"));
		markButton_->setIconSize(QSize(139, 143));

		quality_ = new QLabel();
		quality_->setText(u8"渲染质量");

		select1_ = new QToolButton();
		select1_->setObjectName("select1");
		select1_->setText(u8"高清渲染");
		select1_->setCheckable(true);
		select1_->click();

		select2_ = new QToolButton();
		select2_->setObjectName("select2");
		select2_->setText(u8"极速渲染");
		select2_->setCheckable(true);

		group_ = new QButtonGroup();
		group_->addButton(select1_, 0);
		group_->addButton(select2_, 1);

		videoRatio_ = new QLabel();
		videoRatio_->setText(u8"帧速率");

		speed1_ = new QToolButton();
		speed1_->setObjectName("speed1");
		speed1_->setText(u8"24");
		speed1_->setCheckable(true);
		speed1_->click();

		speed2_ = new QToolButton();
		speed2_->setObjectName("speed2");
		speed2_->setText(u8"25");
		speed2_->setCheckable(true);

		speed3_ = new QToolButton();
		speed3_->setObjectName("speed3");
		speed3_->setText(u8"30");
		speed3_->setCheckable(true);

		speed4_ = new QToolButton();
		speed4_->setObjectName("speed4");
		speed4_->setText(u8"60");
		speed4_->setCheckable(true);

		speedGroup_ = new QButtonGroup();
		speedGroup_->addButton(speed1_, 0);
		speedGroup_->addButton(speed2_, 1);
		speedGroup_->addButton(speed3_, 2);
		speedGroup_->addButton(speed4_, 3);

		frame_ = new QLabel();
		frame_->setText(u8"播放:");

		startLabel_ = new QLabel();
		startLabel_->setText(u8"开始");

		endLabel_ = new QLabel();
		endLabel_->setText(u8"- 结束");

		start_ = new SpinBox();
		start_->setObjectName("start");
		start_->setAlignment(Qt::AlignRight);
		start_->setMinimum(0);
		start_->setMaximum(99999);

		end_ = new SpinBox();
		end_->setObjectName("end");
		end_->setAlignment(Qt::AlignRight);
		end_->setMinimum(0);
		end_->setMaximum(99999);

		bouncesLabel_ = new QLabel();
		bouncesLabel_->setText(u8"每像素光线递归深度:");
		bouncesLabel_->setStyleSheet("color: rgb(200,200,200);");

		bouncesSpinbox_ = new SpinBox();
		bouncesSpinbox_->setMinimum(1);
		bouncesSpinbox_->setMaximum(16);
		bouncesSpinbox_->setValue(0);
		bouncesSpinbox_->setAlignment(Qt::AlignRight);
		bouncesSpinbox_->setFixedWidth(100);

		sppLabel = new QLabel();
		sppLabel->setText(u8"每像素采样数:");
		sppLabel->setStyleSheet("color: rgb(200,200,200);");

		sppSpinbox_ = new SpinBox();
		sppSpinbox_->setMinimum(0);
		sppSpinbox_->setMaximum(9999);
		sppSpinbox_->setValue(0);
		sppSpinbox_->setAlignment(Qt::AlignRight);
		sppSpinbox_->setFixedWidth(100);

		crfSpinbox = new DoubleSpinBox();
		crfSpinbox->setMinimum(0);
		crfSpinbox->setMaximum(63.0);
		crfSpinbox->setValue(0);
		crfSpinbox->setAlignment(Qt::AlignRight);
		crfSpinbox->setFixedWidth(100);

		crfLabel = new QLabel();
		crfLabel->setText(u8"压制质量(CRF):");
		crfLabel->setStyleSheet("color: rgb(200,200,200);");

		frameLayout_ = new QHBoxLayout();
		frameLayout_->addSpacing(20);
		frameLayout_->addWidget(startLabel_, 0, Qt::AlignLeft);
		frameLayout_->addWidget(start_, 0, Qt::AlignLeft);
		frameLayout_->addWidget(endLabel_, 0, Qt::AlignLeft);
		frameLayout_->addWidget(end_, 0, Qt::AlignLeft);
		frameLayout_->addStretch();

		animation_ = new QLabel();
		animation_->setContentsMargins(20, 0, 0, 0);

		summary_ = new QLabel();
		summary_->setContentsMargins(20, 0, 0, 0);

		currentFrame_ = new QLabel();
		currentFrame_->setContentsMargins(20, 0, 0, 0);

		timeTotal_ = new QLabel();
		timeTotal_->setContentsMargins(20, 0, 0, 0);

		recordButton_ = new QToolButton();
		recordButton_->setObjectName("render");
		recordButton_->setText(u8"开始渲染");
		recordButton_->setContentsMargins(0, 0, 0, 0);

		auto titleLayout = new QHBoxLayout();
		titleLayout->addSpacing(closeButton_->iconSize().width());
		titleLayout->addStretch();
		titleLayout->addWidget(title_, 0, Qt::AlignCenter);
		titleLayout->addStretch();
		titleLayout->addWidget(closeButton_, 0, Qt::AlignRight);
		titleLayout->setContentsMargins(0, 0, 0, 0);

		videoRatioLayout_ = new QHBoxLayout();
		videoRatioLayout_->addStretch();
		videoRatioLayout_->addWidget(speed1_, 0, Qt::AlignRight);
		videoRatioLayout_->addWidget(speed2_, 0, Qt::AlignVCenter);
		videoRatioLayout_->addWidget(speed3_, 0, Qt::AlignVCenter);
		videoRatioLayout_->addWidget(speed4_, 0, Qt::AlignLeft);
		videoRatioLayout_->addStretch();
		videoRatioLayout_->setContentsMargins(0, 0, 0, 0);

		auto selectLayout_ = new QHBoxLayout();
		selectLayout_->addWidget(select1_, 0, Qt::AlignLeft);
		selectLayout_->addWidget(select2_, 0, Qt::AlignLeft);
		selectLayout_->setContentsMargins(0, 0, 0, 0);

		auto videoLayout = new QVBoxLayout;
		videoLayout->addWidget(quality_);
		videoLayout->addLayout(selectLayout_);
		videoLayout->addSpacing(10);
		videoLayout->addWidget(videoRatio_);
		videoLayout->addLayout(videoRatioLayout_);
		videoLayout->addSpacing(10);
		videoLayout->addWidget(frame_);
		videoLayout->addLayout(frameLayout_);
		videoLayout->addSpacing(10);
		videoLayout->addWidget(sppLabel);
		videoLayout->addWidget(sppSpinbox_);
		videoLayout->addSpacing(10);
		videoLayout->addWidget(bouncesLabel_);
		videoLayout->addWidget(bouncesSpinbox_);
		videoLayout->addSpacing(10);
		videoLayout->addWidget(crfLabel);
		videoLayout->addWidget(crfSpinbox);
		videoLayout->setContentsMargins(20, 10, 0, 0);

		auto markLayout = new QVBoxLayout;
		markLayout->addWidget(markButton_, 0, Qt::AlignCenter);

		auto infoLayout = new QVBoxLayout;
		infoLayout->addWidget(animation_);
		infoLayout->addWidget(summary_);
		infoLayout->addWidget(currentFrame_);
		infoLayout->addWidget(timeTotal_);

		markSpoiler_ = new Spoiler(u8"水印");
		markSpoiler_->setContentLayout(*markLayout);

		videoSpoiler_ = new Spoiler(u8"渲染设置");
		videoSpoiler_->setContentLayout(*videoLayout);

		infoSpoiler_ = new Spoiler(u8"视频信息");
		infoSpoiler_->setContentLayout(*infoLayout);

		auto contentLayout = new QVBoxLayout(this);
		contentLayout->addWidget(markSpoiler_);
		contentLayout->addWidget(videoSpoiler_);
		contentLayout->addWidget(infoSpoiler_);
		contentLayout->addStretch();

		auto contentWidget = new QWidget;
		contentWidget->setLayout(contentLayout);

		contentWidgetArea_ = new QScrollArea();
		contentWidgetArea_->setWidget(contentWidget);
		contentWidgetArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		contentWidgetArea_->setWidgetResizable(true);

		mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addLayout(titleLayout);
		mainLayout_->addWidget(contentWidgetArea_);
		mainLayout_->addStretch();
		mainLayout_->addWidget(recordButton_, 0, Qt::AlignCenter);
		mainLayout_->setContentsMargins(0, 10, 2, 10);

		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(recordButton_, SIGNAL(clicked()), this, SLOT(clickEvent()));
		connect(select1_, SIGNAL(toggled(bool)), this, SLOT(select1Event(bool)));
		connect(select2_, SIGNAL(toggled(bool)), this, SLOT(select2Event(bool)));
		connect(speed1_, SIGNAL(toggled(bool)), this, SLOT(speed1Event(bool)));
		connect(speed2_, SIGNAL(toggled(bool)), this, SLOT(speed2Event(bool)));
		connect(speed3_, SIGNAL(toggled(bool)), this, SLOT(speed3Event(bool)));
		connect(speed4_, SIGNAL(toggled(bool)), this, SLOT(speed4Event(bool)));
		connect(start_, SIGNAL(valueChanged(int)), this, SLOT(startEvent(int)));
		connect(end_, SIGNAL(valueChanged(int)), this, SLOT(endEvent(int)));
		connect(sppSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(onSppChanged(int)));
		connect(bouncesSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(onBouncesChanged(int)));
		connect(crfSpinbox, SIGNAL(valueChanged(double)), this, SLOT(onCrfChanged(double)));
		connect(timer_, SIGNAL(timeout()), this, SLOT(timeEvent()));
	}

	RecordWindow::~RecordWindow() noexcept
	{
	}

	void
	RecordWindow::startRecord(QString fileName)
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			if (behaviour->startRecord(fileName.toUtf8().data()))
			{
				start_->setEnabled(false);
				end_->setEnabled(false);
				timer_->start();
				recordButton_->setText(u8"停止渲染");
			}
			else
			{
				QMessageBox msg(this);
				msg.setWindowTitle(u8"错误");
				msg.setText(u8"创建文件失败");
				msg.setIcon(QMessageBox::Information);
				msg.setStandardButtons(QMessageBox::Ok);

				msg.exec();
			}
		}
	}

	void
	RecordWindow::stopRecord()
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			timer_->stop();
			start_->setEnabled(true);
			end_->setEnabled(true);
			recordButton_->setText(u8"开始渲染");
			behaviour->stopRecord();
		}
	}

	void
	RecordWindow::onSppChanged(int value)
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
			behaviour->getProfile()->timeModule->spp = value;
	}

	void
	RecordWindow::onBouncesChanged(int value)
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
			behaviour->getComponent<rabbit::OfflineComponent>()->setMaxBounces(value);
	}

	void
	RecordWindow::onCrfChanged(double value)
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
			behaviour->getProfile()->h265Module->crf = value;
	}

	void
	RecordWindow::showEvent(QShowEvent* event)
	{
		this->repaint();
	}

	void
	RecordWindow::resizeEvent(QResizeEvent* e) noexcept
	{
		contentWidgetArea_->setFixedHeight(
			e->size().height() - 
			this->recordButton_->height() - 
			this->closeButton_->height() - 
			mainLayout_->contentsMargins().bottom() * 2 -
			mainLayout_->contentsMargins().top() * 2);
	}

	void
	RecordWindow::closeEvent()
	{
		this->close();
		parentWidget()->setFixedWidth(parentWidget()->width() - this->width());
	}

	void
	RecordWindow::clickEvent()
	{
		VideoQuality quality = VideoQuality::Medium;
		if (select1_->isChecked())
			quality = VideoQuality::High;
		if (select2_->isChecked())
			quality = VideoQuality::Medium;

		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			behaviour->getProfile()->h265Module->setVideoQuality(quality);

			if (recordButton_->text() != u8"停止渲染")
			{
				QString fileName = QFileDialog::getSaveFileName(this, u8"录制视频", "", tr("MP4 Files (*.mp4)"));
				if (!fileName.isEmpty())
					this->startRecord(fileName);
			}
			else
			{
				this->stopRecord();
			}
		}
	}

	void
	RecordWindow::select1Event(bool checked)
	{
		this->update();
	}
	
	void
	RecordWindow::select2Event(bool checked)
	{
		this->update();
	}

	void
	RecordWindow::speed1Event(bool checked)
	{
		if (checked)
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour)
				behaviour->getProfile()->timeModule->recordFps = 24;

			this->update();
		}
	}

	void
	RecordWindow::speed2Event(bool checked)
	{
		if (checked)
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour)
				behaviour->getProfile()->timeModule->recordFps = 25;

			this->update();
		}
	}

	void
	RecordWindow::speed3Event(bool checked)
	{
		if (checked)
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour)
				behaviour->getProfile()->timeModule->recordFps = 30;

			this->update();
		}
	}

	void
	RecordWindow::speed4Event(bool checked)
	{
		if (checked)
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour)
				behaviour->getProfile()->timeModule->recordFps = 60;

			this->update();
		}
	}

	void
	RecordWindow::startEvent(int value)
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			behaviour->getProfile()->timeModule->startFrame = value;
			this->update();
		}
	}

	void
	RecordWindow::endEvent(int value)
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			behaviour->getProfile()->timeModule->endFrame = value;
			this->update();
		}
	}

	void
	RecordWindow::timeEvent()
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			auto playerComponent = behaviour->getComponent<PlayerComponent>();
			if (playerComponent)
			{
				auto time = std::max<int>(0, std::round(behaviour->getProfile()->timeModule->curTime * 30.0f));
				currentFrame_->setText(QString(u8"当前视频渲染帧数：%1").arg(time));
			}
		}
	}

	void
	RecordWindow::update()
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			auto playerComponent = dynamic_cast<PlayerComponent*>(behaviour->getComponent<PlayerComponent>());
			auto animLength = std::max<int>(1, std::round(playerComponent->timeLength() * 30.0f));

			auto startFrame = start_->value();
			auto endFrame = end_->value();
			auto time = std::max<int>(0, std::round(behaviour->getProfile()->timeModule->curTime * 30.0f));
			auto timeLength = std::max<int>(1, (endFrame - startFrame) / 30.0f * behaviour->getProfile()->timeModule->recordFps);

			animation_->setText(QString(u8"视频动作帧数：%1").arg(animLength));
			summary_->setText(QString(u8"视频渲染帧数：%1").arg(timeLength));	
			currentFrame_->setText(QString(u8"当前视频渲染帧数：%1").arg(time));

			if (select1_->isChecked())
			{
				timeTotal_->setText(QString(u8"视频渲染预估时间：%1分钟").arg((timeLength * 10 / 60)));
			}
			else
			{
				recordButton_->setEnabled(true);
				timeTotal_->setText(QString(u8"视频渲染预估时间：%1分钟").arg((timeLength / 15 / 60)));
			}
		}
	}

	void 
	RecordWindow::repaint()
	{
		auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
		if (behaviour)
		{
			auto playerComponent = behaviour->getComponent<PlayerComponent>();
			int timeLength = (int)std::round(playerComponent->timeLength() * 30);

			start_->setValue(0);
			end_->setValue(timeLength);

			auto profile = behaviour->getProfile();
			if (profile->timeModule->recordFps == 24)
				speed1_->click();
			else if (profile->timeModule->recordFps == 25)
				speed2_->click();
			else if (profile->timeModule->recordFps == 30)
				speed3_->click();
			else if (profile->timeModule->recordFps == 60)
				speed4_->click();

			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour)
				bouncesSpinbox_->setValue(behaviour->getComponent<rabbit::OfflineComponent>()->getMaxBounces());

			sppSpinbox_->setValue(profile->timeModule->spp);
			crfSpinbox->setValue(profile->h265Module->crf);

			this->update();
		}
	}
}