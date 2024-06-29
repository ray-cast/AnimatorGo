#include "record_dock.h"
#include <qapplication.h>
#include <qdrag.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qmimedata.h>

namespace unreal
{
	RecordDock::RecordDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept
		: behaviour_(behaviour)
		, profile_(profile)
	{
		this->setObjectName("RecordDock");
		this->setWindowTitle(tr("Video"));
		this->setFeatures(QDockWidget::NoDockWidgetFeatures);

		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		title_ = new QLabel;
		title_->setObjectName("title");
		title_->setText(tr("Video Editor"));

		headerLine_ = new QFrame;
		headerLine_->setObjectName("HLine");
		headerLine_->setFixedHeight(1);
		headerLine_->setFrameShape(QFrame::NoFrame);
		headerLine_->setFrameShadow(QFrame::Plain);
		headerLine_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

		auto titleLayout_ = new QVBoxLayout();
		titleLayout_->addWidget(title_, 0, Qt::AlignLeft);
		titleLayout_->addSpacing(8);
		titleLayout_->addWidget(headerLine_);
		titleLayout_->setSpacing(0);
		titleLayout_->setContentsMargins(4, 0, 4, 0);

		videoRatio_ = new QLabel(this);
		videoRatio_->setText(tr("Ratio"));

		ratio1_ = new QPushButton();
		ratio1_->setObjectName("Ratio");
		ratio1_->setText(tr("16:9"));
		ratio1_->setCheckable(true);
		ratio1_->installEventFilter(this);
		
		ratio2_ = new QPushButton();
		ratio2_->setObjectName("Ratio");
		ratio2_->setText(tr("9:16"));
		ratio2_->setCheckable(true);
		ratio2_->installEventFilter(this);

		ratio3_ = new QPushButton();
		ratio3_->setObjectName("Ratio");
		ratio3_->setText(tr("4:3"));
		ratio3_->setCheckable(true);
		ratio3_->installEventFilter(this);

		ratio4_ = new QPushButton();
		ratio4_->setObjectName("Ratio");
		ratio4_->setText(tr("3:4"));
		ratio4_->setCheckable(true);
		ratio4_->installEventFilter(this);

		ratioGroup_ = new QButtonGroup();
		ratioGroup_->addButton(ratio1_, 0);
		ratioGroup_->addButton(ratio2_, 1);
		ratioGroup_->addButton(ratio3_, 2);
		ratioGroup_->addButton(ratio4_, 3);

		videoRatioLayout_ = new QHBoxLayout();
		videoRatioLayout_->addStretch();
		videoRatioLayout_->addWidget(ratio1_, 0, Qt::AlignLeft);
		videoRatioLayout_->addWidget(ratio2_, 0, Qt::AlignVCenter);
		videoRatioLayout_->addWidget(ratio3_, 0, Qt::AlignVCenter);
		videoRatioLayout_->addWidget(ratio4_, 0, Qt::AlignRight);
		videoRatioLayout_->addStretch();
		videoRatioLayout_->setContentsMargins(0, 0, 0, 0);

		resolutionLabel = new QLabel(this);
		resolutionLabel->setText(tr("Resolution"));

		resolution1_ = new QPushButton();
		resolution1_->setObjectName("Resolution");
		resolution1_->setCheckable(true);
		resolution1_->installEventFilter(this);

		resolution2_ = new QPushButton();
		resolution2_->setObjectName("Resolution");
		resolution2_->setCheckable(true);
		resolution2_->installEventFilter(this);

		resolution3_ = new QPushButton();
		resolution3_->setObjectName("Resolution");
		resolution3_->setCheckable(true);
		resolution3_->installEventFilter(this);

		resolution4_ = new QPushButton();
		resolution4_->setObjectName("Resolution");
		resolution4_->setCheckable(true);
		resolution4_->installEventFilter(this);

		resolutionGroup_ = new QButtonGroup();
		resolutionGroup_->addButton(resolution1_, 0);
		resolutionGroup_->addButton(resolution2_, 1);
		resolutionGroup_->addButton(resolution3_, 2);
		resolutionGroup_->addButton(resolution4_, 3);

		quality_ = new QLabel();
		quality_->setText(tr("Render Pipeline"));

		select1_ = new QPushButton();
		select1_->setObjectName("select1");
		select1_->setText(tr("Ultra Render"));
		select1_->setCheckable(true);
		select1_->click();
		select1_->installEventFilter(this);

		select2_ = new QPushButton();
		select2_->setObjectName("select2");
		select2_->setText(tr("Realtime Render"));
		select2_->setCheckable(true);
		select2_->installEventFilter(this);

		selectGroup_ = new QButtonGroup();
		selectGroup_->addButton(select1_, 0);
		selectGroup_->addButton(select2_, 1);

		auto qualityLayout_ = new QHBoxLayout();
		qualityLayout_->addWidget(select1_, 0, Qt::AlignRight);
		qualityLayout_->addWidget(select2_, 0, Qt::AlignLeft);
		qualityLayout_->setSpacing(0);
		qualityLayout_->setContentsMargins(0, 0, 0, 0);

		videoFps_ = new QLabel();
		videoFps_->setText(tr("Frame Per Second"));

		speed1_ = new QPushButton();
		speed1_->setObjectName("Speed");
		speed1_->setText(tr("24"));
		speed1_->setCheckable(true);
		speed1_->installEventFilter(this);

		speed2_ = new QPushButton();
		speed2_->setObjectName("Speed");
		speed2_->setText(tr("25"));
		speed2_->setCheckable(true);
		speed2_->installEventFilter(this);
		
		speed3_ = new QPushButton();
		speed3_->setObjectName("Speed");
		speed3_->setText(tr("30"));
		speed3_->setCheckable(true);
		speed3_->click();
		speed3_->installEventFilter(this);

		speed4_ = new QPushButton();
		speed4_->setObjectName("Speed");
		speed4_->setText(tr("60"));
		speed4_->setCheckable(true);
		speed4_->installEventFilter(this);

		speedGroup_ = new QButtonGroup();
		speedGroup_->addButton(speed1_, 0);
		speedGroup_->addButton(speed2_, 1);
		speedGroup_->addButton(speed3_, 2);
		speedGroup_->addButton(speed4_, 3);

		encodeType_ = new QLabel();
		encodeType_->setText(tr("Encode Type"));

		mode1_ = new QPushButton();
		mode1_->setObjectName("Mode");
		mode1_->setText(tr("H264"));
		mode1_->setCheckable(true);
		mode1_->installEventFilter(this);

		mode2_ = new QPushButton();
		mode2_->setObjectName("Mode");
		mode2_->setText(tr("H265"));
		mode2_->setCheckable(true);
		mode2_->installEventFilter(this);

		mode3_ = new QPushButton();
		mode3_->setObjectName("Mode");
		mode3_->setText(tr("Images"));
		mode3_->setCheckable(true);
		mode3_->click();
		mode3_->installEventFilter(this);

		modeGroup_ = new QButtonGroup();
		modeGroup_->addButton(mode1_, 0);
		modeGroup_->addButton(mode2_, 1);
		modeGroup_->addButton(mode3_, 2);

		auto encodeLayout_ = new QHBoxLayout();
		encodeLayout_->addStretch();
		encodeLayout_->addWidget(mode1_, 0, Qt::AlignLeft);
		encodeLayout_->addWidget(mode2_, 0, Qt::AlignVCenter);
		encodeLayout_->addWidget(mode3_, 0, Qt::AlignRight);
		encodeLayout_->addStretch();
		encodeLayout_->setContentsMargins(0, 0, 0, 0);

		videoFpsLayout_ = new QHBoxLayout();
		videoFpsLayout_->addStretch();
		videoFpsLayout_->addWidget(speed1_, 0, Qt::AlignRight);
		videoFpsLayout_->addWidget(speed2_, 0, Qt::AlignVCenter);
		videoFpsLayout_->addWidget(speed3_, 0, Qt::AlignVCenter);
		videoFpsLayout_->addWidget(speed4_, 0, Qt::AlignLeft);
		videoFpsLayout_->addStretch();
		videoFpsLayout_->setContentsMargins(0, 0, 0, 0);
		
		frame_ = new QLabel();
		frame_->setText(tr("Play:"));

		startLabel_ = new QLabel();
		startLabel_->setText(tr("Start"));

		middleLabel_ = new QLabel();
		middleLabel_->setText(tr("-"));

		endLabel_ = new QLabel();
		endLabel_->setText(tr("End"));

		startFrame_ = new QSpinBox();
		startFrame_->setObjectName("Start");
		startFrame_->setAlignment(Qt::AlignRight);
		startFrame_->setMinimum(0);
		startFrame_->setMaximum(99999);
		startFrame_->installEventFilter(this);

		endFrame_ = new QSpinBox();
		endFrame_->setObjectName("End");
		endFrame_->setAlignment(Qt::AlignRight);
		endFrame_->setMinimum(0);
		endFrame_->setMaximum(99999);
		endFrame_->installEventFilter(this);

		denoiseLabel_ = new QLabel();
		denoiseLabel_->setText(tr("Denoise:"));

		denoiseButton_ = new QCheckBox();
		denoiseButton_->setCheckState(Qt::CheckState::Checked);
		denoiseButton_->installEventFilter(this);

		auto denoiseLayout_ = new QHBoxLayout();
		denoiseLayout_->addWidget(denoiseLabel_, 0, Qt::AlignLeft);
		denoiseLayout_->addWidget(denoiseButton_, 0, Qt::AlignLeft);
		denoiseLayout_->setSpacing(0);
		denoiseLayout_->setContentsMargins(0, 0, 0, 0);

		bouncesSpinbox_ = USpinLine::create(this, tr("Recursion depth per pixel:"), 1, 32, 1, 0);

		sppSpinbox_ = USpinLine::create(this, tr("Sample number per pixel:"), 1, 9999, 1, 0);

		crfSpinbox = UDoubleSpinLine::create(this, tr("Constant Rate Factor (CRF):"), 0.0f, 63.0f, 0);

		frameLayout_ = new QHBoxLayout();
		frameLayout_->addWidget(startLabel_, 0, Qt::AlignLeft);
		frameLayout_->addWidget(startFrame_, 0, Qt::AlignLeft);
		frameLayout_->addWidget(middleLabel_, 0, Qt::AlignCenter);
		frameLayout_->addWidget(endLabel_, 0, Qt::AlignRight);
		frameLayout_->addWidget(endFrame_, 0, Qt::AlignRight);
		frameLayout_->setContentsMargins(16, 0, 16, 0);

		recordButton_ = new QPushButton();
		recordButton_->setObjectName("Render");
		recordButton_->setText(tr("Start Render"));
		recordButton_->setContentsMargins(0, 0, 0, 0);
		
		auto videoLayout = new QVBoxLayout;
		videoLayout->addWidget(videoRatio_);
		videoLayout->addLayout(videoRatioLayout_);
		videoLayout->addSpacing(8);
		videoLayout->addWidget(resolutionLabel);
		videoLayout->addWidget(resolution1_);
		videoLayout->addWidget(resolution2_);
		videoLayout->addWidget(resolution3_);
		videoLayout->addWidget(resolution4_);
		videoLayout->addSpacing(8);
		videoLayout->addWidget(videoFps_);
		videoLayout->addLayout(videoFpsLayout_);
		videoLayout->addSpacing(8);
		videoLayout->addWidget(frame_);
		videoLayout->addLayout(frameLayout_);
		videoLayout->addSpacing(8);
		videoLayout->addWidget(encodeType_);
		videoLayout->addLayout(encodeLayout_);
		videoLayout->addWidget(crfSpinbox);
		videoLayout->addSpacing(8);
		videoLayout->addWidget(quality_);
		videoLayout->addLayout(qualityLayout_);
		videoLayout->addSpacing(8);
		videoLayout->addLayout(denoiseLayout_);
		videoLayout->addWidget(sppSpinbox_);
		videoLayout->addWidget(bouncesSpinbox_);
		videoLayout->addStretch();
		videoLayout->addWidget(recordButton_, 0, Qt::AlignCenter | Qt::AlignBottom);
		videoLayout->setContentsMargins(16, 0, 16, 0);

		mainLayout_ = new QVBoxLayout();
		mainLayout_->addLayout(titleLayout_);
		mainLayout_->addLayout(videoLayout);
		mainLayout_->setContentsMargins(0, 12, 0, 8);

		mainWidget_ = new QWidget;
		mainWidget_->setObjectName("RecordWidget");
		mainWidget_->setLayout(mainLayout_);
		
		this->setWidget(mainWidget_);

		profile_->cameraModule->framebufferSize += [this](const octoon::math::uint2& framebufferSize)
		{
			if (framebufferSize == octoon::math::uint2(1280, 720))
			{
				ratio1_->setChecked(true);
				resolution1_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(720, 1280))
			{
				ratio2_->setChecked(true);
				resolution1_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(1280, 960))
			{
				ratio3_->setChecked(true);
				resolution1_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(960, 1280))
			{
				ratio4_->setChecked(true);
				resolution1_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(1920, 1080))
			{
				ratio1_->setChecked(true);
				resolution2_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(1080, 1920))
			{
				ratio2_->setChecked(true);
				resolution2_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(1600, 1200))
			{
				ratio3_->setChecked(true);
				resolution2_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(1200, 1600))
			{
				ratio4_->setChecked(true);
				resolution2_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(2560, 1440))
			{
				ratio1_->setChecked(true);
				resolution3_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(1440, 2560))
			{
				ratio2_->setChecked(true);
				resolution3_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(3200, 2400))
			{
				ratio3_->setChecked(true);
				resolution3_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(2400, 3200))
			{
				ratio4_->setChecked(true);
				resolution3_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(3840, 2160))
			{
				ratio1_->setChecked(true);
				resolution4_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(2160, 3840))
			{
				ratio2_->setChecked(true);
				resolution4_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(4000, 3000))
			{
				ratio3_->setChecked(true);
				resolution4_->setChecked(true);
			}
			else if (framebufferSize == octoon::math::uint2(3000, 4000))
			{
				ratio4_->setChecked(true);
				resolution4_->setChecked(true);
			}
		};

		profile_->offlineModule->bounces += [this](std::uint32_t value)
		{
			bouncesSpinbox_->blockSignals(true);
			bouncesSpinbox_->setValue(value);
			bouncesSpinbox_->blockSignals(false);
		};

		profile_->offlineModule->spp += [this](std::uint32_t value)
		{
			sppSpinbox_->blockSignals(true);
			sppSpinbox_->setValue(value);
			sppSpinbox_->blockSignals(false);
		};

		profile_->encodeModule->crf += [this](float value)
		{
			crfSpinbox->blockSignals(true);
			crfSpinbox->doublespinbox_->setValue(value);
			crfSpinbox->blockSignals(false);
		};

		profile_->recordModule->denoise += [this](bool value)
		{
			denoiseButton_->blockSignals(true);
			denoiseButton_->setChecked(value);
			denoiseButton_->blockSignals(false);
		};

		profile_->playerModule->startFrame += [this](std::uint32_t value)
		{
			startFrame_->blockSignals(true);
			startFrame_->setValue(value);
			startFrame_->blockSignals(false);
		};

		profile_->playerModule->endFrame += [this](std::uint32_t value)
		{
			endFrame_->blockSignals(true);
			endFrame_->setValue(value);
			endFrame_->blockSignals(false);
		};

		profile_->playerModule->recordFps += [this](float value)
		{
			if (value == 24)
				speed1_->click();
			else if (value == 25)
				speed2_->click();
			else if (value == 30)
				speed3_->click();
			else if (value == 60)
				speed4_->click();
		};

		profile_->playerModule->finish += [this](bool value)
		{
			if (profile_->recordModule->active)
			{
				if (value)
					recordButton_->setText(tr("Start Render"));
				else
					recordButton_->setText(tr("Stop Render"));
			}
		};

		profile_->recordModule->active += [this](bool value)
		{
			if (value)
				recordButton_->setText(tr("Stop Render"));
			else
				recordButton_->setText(tr("Start Render"));
		};

		connect(ratio1_, SIGNAL(toggled(bool)), this, SLOT(onRatioEvent(bool)));
		connect(ratio2_, SIGNAL(toggled(bool)), this, SLOT(onRatioEvent(bool)));
		connect(ratio3_, SIGNAL(toggled(bool)), this, SLOT(onRatioEvent(bool)));
		connect(ratio4_, SIGNAL(toggled(bool)), this, SLOT(onRatioEvent(bool)));
		connect(resolution1_, SIGNAL(toggled(bool)), this, SLOT(onResolutionEvent(bool)));
		connect(resolution2_, SIGNAL(toggled(bool)), this, SLOT(onResolutionEvent(bool)));
		connect(resolution3_, SIGNAL(toggled(bool)), this, SLOT(onResolutionEvent(bool)));
		connect(resolution4_, SIGNAL(toggled(bool)), this, SLOT(onResolutionEvent(bool)));
		connect(select1_, SIGNAL(toggled(bool)), this, SLOT(select1Event(bool)));
		connect(select2_, SIGNAL(toggled(bool)), this, SLOT(select2Event(bool)));
		connect(speed1_, SIGNAL(toggled(bool)), this, SLOT(speed1Event(bool)));
		connect(speed2_, SIGNAL(toggled(bool)), this, SLOT(speed2Event(bool)));
		connect(speed3_, SIGNAL(toggled(bool)), this, SLOT(speed3Event(bool)));
		connect(speed4_, SIGNAL(toggled(bool)), this, SLOT(speed4Event(bool)));
		connect(mode1_, SIGNAL(toggled(bool)), this, SLOT(mode1Event(bool)));
		connect(mode2_, SIGNAL(toggled(bool)), this, SLOT(mode2Event(bool)));
		connect(mode3_, SIGNAL(toggled(bool)), this, SLOT(mode3Event(bool)));
		connect(denoiseButton_, SIGNAL(stateChanged(int)), this, SLOT(denoiseEvent(int)));
		connect(startFrame_, SIGNAL(valueChanged(int)), this, SLOT(startEvent(int)));
		connect(endFrame_, SIGNAL(valueChanged(int)), this, SLOT(endEvent(int)));
		connect(sppSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(onSppChanged(int)));
		connect(bouncesSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(onBouncesChanged(int)));
		connect(crfSpinbox, SIGNAL(valueChanged(double)), this, SLOT(onCrfChanged(double)));
		connect(recordButton_, SIGNAL(clicked(bool)), this, SLOT(recordEvent(bool)));
	}

	RecordDock::~RecordDock() noexcept
	{
	}

	void
	RecordDock::onRatioEvent(bool)
	{
		if (ratio1_->isChecked())
		{
			resolution1_->setText("1280*720");
			resolution2_->setText("1920*1080");
			resolution3_->setText("2560*1440");
			resolution4_->setText("3840*2160");
		}
		if (ratio2_->isChecked())
		{
			resolution1_->setText("720*1280");
			resolution2_->setText("1080*1920");
			resolution3_->setText("1440*2560");
			resolution4_->setText("2160*3840");
		}
		if (ratio3_->isChecked())
		{
			resolution1_->setText("1280*960");
			resolution2_->setText("1600*1200");
			resolution3_->setText("3200*2400");
			resolution4_->setText("4000*3000");
		}
		if (ratio4_->isChecked())
		{
			resolution1_->setText("960*1280");
			resolution2_->setText("1200*1600");
			resolution3_->setText("2400*3200");
			resolution4_->setText("3000*4000");
		}

		this->onResolutionEvent(true);
	}

	void
	RecordDock::onResolutionEvent(bool)
	{
		if (resolution1_->isChecked())
		{
			if (resolution1_->text() == "1280*720")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(1280, 720);
			else if (resolution1_->text() == "720*1280")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(720, 1280);
			else if (resolution1_->text() == "1280*960")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(1280, 960);
			else if (resolution1_->text() == "960*1280")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(960, 1280);
		}
		else if (resolution2_->isChecked())
		{
			if (resolution2_->text() == "1920*1080")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(1920, 1080);
			if (resolution2_->text() == "1080*1920")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(1080, 1920);
			if (resolution2_->text() == "1600*1200")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(1600, 1200);
			if (resolution2_->text() == "1200*1600")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(1200, 1600);
		}
		else if (resolution3_->isChecked())
		{
			if (resolution3_->text() == "2560*1440")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(2560, 1440);
			if (resolution3_->text() == "1440*2560")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(1440, 2560);
			if (resolution3_->text() == "3200*2400")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(3200, 2400);
			if (resolution3_->text() == "2400*3200")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(2400, 3200);
		}
		else if (resolution4_->isChecked())
		{
			if (resolution4_->text() == "3840*2160")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(3840, 2160);
			if (resolution4_->text() == "2160*3840")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(2160, 3840);
			if (resolution4_->text() == "4000*3000")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(4000, 3000);
			if (resolution4_->text() == "3000*4000")
				profile_->cameraModule->framebufferSize = octoon::math::uint2(3000, 4000);
		}
	}

	void
	RecordDock::onSppChanged(int value)
	{
		profile_->offlineModule->spp = value;
	}

	void
	RecordDock::onBouncesChanged(int value)
	{
		profile_->offlineModule->bounces = value;
	}

	void
	RecordDock::onCrfChanged(double value)
	{
		profile_->encodeModule->crf = value;
	}

	void
	RecordDock::select1Event(bool checked)
	{
		if (checked)
			profile_->encodeModule->quality = VideoQuality::High;
	}

	void
	RecordDock::select2Event(bool checked)
	{
		if (checked)
			profile_->encodeModule->quality = VideoQuality::Medium;
	}

	void
	RecordDock::denoiseEvent(int checked)
	{
		if (checked == Qt::CheckState::Checked)
			profile_->recordModule->denoise = true;
		else
			profile_->recordModule->denoise = false;
	}

	void
	RecordDock::speed1Event(bool checked)
	{
		if (checked)
			profile_->playerModule->recordFps = 24;
	}

	void
	RecordDock::speed2Event(bool checked)
	{
		if (checked)
			profile_->playerModule->recordFps = 25;
	}

	void
	RecordDock::speed3Event(bool checked)
	{
		if (checked)
			profile_->playerModule->recordFps = 30;
	}

	void
	RecordDock::speed4Event(bool checked)
	{
		if (checked)
			profile_->playerModule->recordFps = 60;
	}

	void
	RecordDock::mode1Event(bool checked)
	{
		if (checked)
		{
			profile_->encodeModule->encodeMode = EncodeMode::H264;
			crfSpinbox->setVisible(true);
		}
	}

	void
	RecordDock::mode2Event(bool checked)
	{
		if (checked)
		{
			profile_->encodeModule->encodeMode = EncodeMode::H265;
			crfSpinbox->setVisible(true);
		}
	}
	
	void
	RecordDock::mode3Event(bool checked)
	{
		if (checked)
		{
			profile_->encodeModule->encodeMode = EncodeMode::Frame;
			crfSpinbox->setVisible(false);
		}
	}

	void
	RecordDock::startEvent(int value)
	{
		profile_->playerModule->startFrame = value;
	}

	void
	RecordDock::endEvent(int value)
	{
		profile_->playerModule->endFrame = value;
	}

	void
	RecordDock::outputTypeEvent(int index)
	{
		if (index == 0)
			profile_->encodeModule->encodeMode = EncodeMode::H265;
		else if (index == 1)
			profile_->encodeModule->encodeMode = EncodeMode::H264;
		else if (index == 2)
			profile_->encodeModule->encodeMode = EncodeMode::Frame;
		else
			throw std::runtime_error("Unsupported EncodeMode");
	}

	void
	RecordDock::recordEvent(bool)
	{
		auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
		if (behaviour)
		{
			if (!profile_->recordModule->active)
			{
				if (profile_->playerModule->endFrame <= profile_->playerModule->startFrame)
				{
					QMessageBox::warning(this, tr("Warning"), tr("Start frame must be less than end frame."));
					return;
				}

				QString fileName;
				if (profile_->encodeModule->encodeMode == EncodeMode::H264 || profile_->encodeModule->encodeMode == EncodeMode::H265)
					fileName = QFileDialog::getSaveFileName(this, tr("Save Video"), tr("New Video"), tr("MP4 Files (*.mp4)"));
				else if (profile_->encodeModule->encodeMode == EncodeMode::Frame)
					fileName = QFileDialog::getSaveFileName(this, tr("Save Image Sequence"), "", tr("PNG Files (*.png)"));
				else
					throw std::runtime_error("Unknown encode mode");

				if (!fileName.isEmpty())
				{
					if (!behaviour->startRecord(fileName.toStdWString()))
						QMessageBox::information(this, tr("Error"), tr("Failed to create file"));
				}
			}
			else
			{
				behaviour->stopRecord();
			}
		}
	}

	void
	RecordDock::showEvent(QShowEvent* event)
	{
		startFrame_->setValue(0);
		endFrame_->setValue(profile_->playerModule->endFrame);
		denoiseButton_->setCheckState(profile_->recordModule->denoise ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		sppSpinbox_->setValue(profile_->offlineModule->spp);
		crfSpinbox->doublespinbox_->setValue(profile_->encodeModule->crf);
		bouncesSpinbox_->setValue(profile_->offlineModule->bounces);
		profile_->cameraModule->framebufferSize.submit();

		if (profile_->recordModule->active)
			recordButton_->setText(tr("Stop Render"));
		else
			recordButton_->setText(tr("Start Render"));

		auto quality = profile_->encodeModule->quality;
		if (quality == VideoQuality::High)
			select1_->click();
		else if (quality == VideoQuality::Medium)
			select2_->click();

		if (profile_->playerModule->recordFps == 24)
			speed1_->click();
		else if (profile_->playerModule->recordFps == 25)
			speed2_->click();
		else if (profile_->playerModule->recordFps == 30)
			speed3_->click();
		else if (profile_->playerModule->recordFps == 60)
			speed4_->click();

		if (profile_->encodeModule->encodeMode == EncodeMode::H264)
			mode1_->click();
		else if (profile_->encodeModule->encodeMode == EncodeMode::H265)
			mode2_->click();
		else if (profile_->encodeModule->encodeMode == EncodeMode::Frame)
			mode3_->click();
	}

	void
	RecordDock::closeEvent(QCloseEvent* event)
	{
		if (profile_->playerModule->isPlaying)
			event->ignore();
		else
			event->accept();
	}

	bool
	RecordDock::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() != QEvent::Paint &&
			event->type() != QEvent::Resize)
		{
			if (profile_->playerModule->isPlaying)
			{
				return true;
			}
		}

		return QWidget::eventFilter(watched, event);
	}
}