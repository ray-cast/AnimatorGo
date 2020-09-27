#include "record_window.h"
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

	RecordWindow::RecordWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept
		: behaviour_(behaviour)
		, timer_(std::make_unique<QTimer>(this))
	{
		this->hide();
		this->setObjectName("recordWindow");
		this->setWindowTitle(u8"选项");
		this->setFixedWidth(340);
		this->grabKeyboard();

		title_ = std::make_unique<QLabel>();
		title_->setText(u8"渲染");

		closeButton_ = std::make_unique<QToolButton>();
		closeButton_->setObjectName("close");
		closeButton_->setToolTip(u8"关闭");

		mark_ = std::make_unique<QLabel>();
		mark_->setText(u8"创作水印");

		markButton_ = std::make_unique<QToolButton>();
		markButton_->setObjectName("mark");
		markButton_->setIcon(QIcon::fromTheme(":res/icons/rabbit.png"));
		markButton_->setIconSize(QSize(139, 143));

		quality_ = std::make_unique<QLabel>();
		quality_->setText(u8"渲染质量");

		select1_ = std::make_unique<QToolButton>();
		select1_->setObjectName("select1");
		select1_->setText(u8"高清渲染");
		select1_->setCheckable(true);
		select1_->click();

		select2_ = std::make_unique<QToolButton>();
		select2_->setObjectName("select2");
		select2_->setText(u8"极速渲染");
		select2_->setCheckable(true);

		group_ = std::make_unique<QButtonGroup>();
		group_->addButton(select1_.get(), 0);
		group_->addButton(select2_.get(), 1);

		videoRatio_ = std::make_unique<QLabel>();
		videoRatio_->setText(u8"帧速率");

		speed1_ = std::make_unique<QToolButton>();
		speed1_->setObjectName("speed1");
		speed1_->setText(u8"24");
		speed1_->setCheckable(true);
		speed1_->click();

		speed2_ = std::make_unique<QToolButton>();
		speed2_->setObjectName("speed2");
		speed2_->setText(u8"25");
		speed2_->setCheckable(true);

		speed3_ = std::make_unique<QToolButton>();
		speed3_->setObjectName("speed3");
		speed3_->setText(u8"30");
		speed3_->setCheckable(true);

		speed4_ = std::make_unique<QToolButton>();
		speed4_->setObjectName("speed4");
		speed4_->setText(u8"60");
		speed4_->setCheckable(true);

		speedGroup_ = std::make_unique<QButtonGroup>();
		speedGroup_->addButton(speed1_.get(), 0);
		speedGroup_->addButton(speed2_.get(), 1);
		speedGroup_->addButton(speed3_.get(), 2);
		speedGroup_->addButton(speed4_.get(), 3);

		frame_ = std::make_unique<QLabel>();
		frame_->setText(u8"播放");

		startLabel_ = std::make_unique<QLabel>();
		startLabel_->setText(u8"开始");

		endLabel_ = std::make_unique<QLabel>();
		endLabel_->setText(u8"- 结束");

		start_ = std::make_unique<SpinBox>();
		start_->setObjectName("start");
		start_->setAlignment(Qt::AlignRight);
		start_->setMinimum(0);
		start_->setMaximum(99999);

		end_ = std::make_unique<SpinBox>();
		end_->setObjectName("end");
		end_->setAlignment(Qt::AlignRight);
		end_->setMinimum(0);
		end_->setMaximum(99999);

		frameLayout_ = std::make_unique<QHBoxLayout>();
		frameLayout_->addSpacing(20);
		frameLayout_->addWidget(startLabel_.get(), 0, Qt::AlignLeft);
		frameLayout_->addWidget(start_.get(), 0, Qt::AlignLeft);
		frameLayout_->addWidget(endLabel_.get(), 0, Qt::AlignLeft);
		frameLayout_->addWidget(end_.get(), 0, Qt::AlignLeft);
		frameLayout_->addStretch();

		infomation_ = std::make_unique<QLabel>();
		infomation_->setText(u8"视频信息");

		animation_ = std::make_unique<QLabel>();
		animation_->setContentsMargins(20, 0, 0, 0);

		summary_ = std::make_unique<QLabel>();
		summary_->setContentsMargins(20, 0, 0, 0);

		currentFrame_ = std::make_unique<QLabel>();
		currentFrame_->setContentsMargins(20, 0, 0, 0);

		timeTotal_ = std::make_unique<QLabel>();
		timeTotal_->setContentsMargins(20, 0, 0, 0);

		backButton_ = std::make_unique<QToolButton>();
		backButton_->setObjectName("render");
		backButton_->setText(u8"开始渲染");
		backButton_->setContentsMargins(0, 0, 0, 10);

		layout_ = std::make_unique<QHBoxLayout>();
		layout_->addWidget(title_.get(), 0, Qt::AlignLeft);
		layout_->addWidget(closeButton_.get(), 0, Qt::AlignRight);

		selectLayout_ = std::make_unique<QHBoxLayout>();
		selectLayout_->addWidget(select1_.get(), 0, Qt::AlignVCenter);
		selectLayout_->addWidget(select2_.get(), 0, Qt::AlignVCenter);

		videoRatioLayout_ = std::make_unique<QHBoxLayout>();
		videoRatioLayout_->addStretch();
		videoRatioLayout_->addWidget(speed1_.get(), 0, Qt::AlignRight);
		videoRatioLayout_->addWidget(speed2_.get(), 0, Qt::AlignVCenter);
		videoRatioLayout_->addWidget(speed3_.get(), 0, Qt::AlignVCenter);
		videoRatioLayout_->addWidget(speed4_.get(), 0, Qt::AlignLeft);
		videoRatioLayout_->addStretch();
		videoRatioLayout_->setContentsMargins(20, 0, 20, 0);

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addLayout(layout_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addWidget(mark_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addWidget(markButton_.get(), 0, Qt::AlignCenter);
		mainLayout_->addSpacing(10);
		mainLayout_->addWidget(quality_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addLayout(selectLayout_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addWidget(videoRatio_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addLayout(videoRatioLayout_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addWidget(frame_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addLayout(frameLayout_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addWidget(infomation_.get());
		mainLayout_->addSpacing(10);
		mainLayout_->addWidget(animation_.get());
		mainLayout_->addWidget(summary_.get());
		mainLayout_->addWidget(currentFrame_.get());
		mainLayout_->addWidget(timeTotal_.get());
		mainLayout_->addStretch();
		mainLayout_->addWidget(backButton_.get(), 0, Qt::AlignCenter);
		mainLayout_->setContentsMargins(10, 10, 10, 10);

		connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeEvent()));
		connect(backButton_.get(), SIGNAL(clicked()), this, SLOT(clickEvent()));
		connect(select1_.get(), SIGNAL(toggled(bool)), this, SLOT(select1Event(bool)));
		connect(select2_.get(), SIGNAL(toggled(bool)), this, SLOT(select2Event(bool)));
		connect(speed1_.get(), SIGNAL(toggled(bool)), this, SLOT(speed1Event(bool)));
		connect(speed2_.get(), SIGNAL(toggled(bool)), this, SLOT(speed2Event(bool)));
		connect(speed3_.get(), SIGNAL(toggled(bool)), this, SLOT(speed3Event(bool)));
		connect(speed4_.get(), SIGNAL(toggled(bool)), this, SLOT(speed4Event(bool)));
		connect(start_.get(), SIGNAL(valueChanged(int)), this, SLOT(startEvent(int)));
		connect(end_.get(), SIGNAL(valueChanged(int)), this, SLOT(endEvent(int)));
		connect(timer_.get(), SIGNAL(timeout()), this, SLOT(timeEvent()));
	}

	RecordWindow::~RecordWindow() noexcept
	{
		title_.reset();
		closeButton_.reset();
		layout_.reset();
		select1_.reset();
		select2_.reset();
		group_.reset();
		selectLayout_.reset();
		speed1_.reset();
		speed2_.reset();
		speed3_.reset();
		speed4_.reset();
		speedGroup_.reset();
		videoRatioLayout_.reset();
		backButton_.reset();
		summary_.reset();
		currentFrame_.reset();
		frame_.reset();
		start_.reset();
		end_.reset();
		frameLayout_.reset();
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
				backButton_->setText(u8"停止渲染");
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
			backButton_->setText(u8"开始渲染");
			behaviour->stopRecord();
		}
	}

	void
	RecordWindow::showEvent(QShowEvent* event)
	{
		this->repaint();
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

			if (backButton_->text() != u8"停止渲染")
			{
				QString fileName = QFileDialog::getSaveFileName(this, u8"录制视频", "", tr("HDRi Files (*.mp4)"));
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
				auto time = std::max<int>(0, std::round(playerComponent->time() * 30.0f));
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
			auto time = std::max<int>(0, std::round(playerComponent->time() * 30.0f));
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
				backButton_->setEnabled(true);
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

			if (behaviour->getProfile()->timeModule->recordFps == 24)
				speed1_->click();
			else if (behaviour->getProfile()->timeModule->recordFps == 25)
				speed2_->click();
			else if (behaviour->getProfile()->timeModule->recordFps == 30)
				speed3_->click();
			else if (behaviour->getProfile()->timeModule->recordFps == 60)
				speed4_->click();

			this->update();
		}
	}
}