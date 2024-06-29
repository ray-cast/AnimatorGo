#include "toplevel_bar.h"
#include "spdlog/spdlog.h"
#include <qmessagebox.h>
#include <qgraphicseffect.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qmimedata.h>
#include <qprogressdialog.h>
#include <qscrollarea.h>
#include <octoon/asset_database.h>

namespace unreal
{
	ToplevelBar::ToplevelBar(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept
		: behaviour_(behaviour)
		, profile_(profile)
		, playEnable_(false)
		, gpuEnable_(false)
		, audioEnable_(false)
		, hdrEnable_(false)
		, playIcon_(QIcon::fromTheme("res", QIcon(":res/icons/play.png")))
		, playOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/play-on.png")))
		, volumeOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/volumeMiddle.png")))
		, volumeOffIcon_(QIcon::fromTheme("res", QIcon(":res/icons/volumeCross.png")))
		, gpuIcon_(QIcon::fromTheme("res", QIcon(":res/icons/gpu.png")))
		, gpuOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/gpu-on.png")))
		, audioIcon_(QIcon::fromTheme("res", QIcon(":res/icons/music.svg")))
		, audioOnIcon_(QIcon::fromTheme("res", QIcon(":res/icons/music-on.png")))
	{
		this->setObjectName("ToplevelBar");
		this->setMovable(false);
		this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		openButton_ = new QToolButton;
		openButton_->setObjectName("folderPlus");
		openButton_->setText(tr("Open"));
		openButton_->setToolTip(tr("Open Project"));
		openButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		openButton_->installEventFilter(this);
		openButton_->setShortcut(tr("Ctrl+O"));

		saveButton_ = new QToolButton;
		saveButton_->setObjectName("save");
		saveButton_->setText(tr("Save"));
		saveButton_->setToolTip(tr("Save Project"));
		saveButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		saveButton_->installEventFilter(this);
		saveButton_->setShortcut(tr("Ctrl+S"));
		
		loadButton_ = new QToolButton;
		loadButton_->setObjectName("import");
		loadButton_->setText(tr("Load"));
		loadButton_->setToolTip(tr("Import Resource File(.pmm, .pmx, .wav)"));
		loadButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		loadButton_->installEventFilter(this);

		audioButton_ = new QToolButton;
		audioButton_->setObjectName("audio");
		audioButton_->setText(tr("Music"));
		audioButton_->setToolTip(tr("Set Background Audio File"));
		audioButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		audioButton_->installEventFilter(this);

		shotButton_ = new QToolButton;
		shotButton_->setObjectName("shot");
		shotButton_->setText(tr("Screenshot"));
		shotButton_->setToolTip(tr("Denoising Screenshot"));
		shotButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		shotButton_->installEventFilter(this);

		gpuButton_ = new QToolButton;
		gpuButton_->setObjectName("gpu");
		gpuButton_->setText(tr("Render"));
		gpuButton_->setToolTip(tr("Enable High Quality Rendering"));
		gpuButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		gpuButton_->installEventFilter(this);

		cleanupButton_ = new QToolButton;
		cleanupButton_->setObjectName("cleanup");
		cleanupButton_->setText(tr("Cleanup"));
		cleanupButton_->setToolTip(tr("Cleanup Scene"));
		cleanupButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		cleanupButton_->installEventFilter(this);

		playButton_ = new QToolButton;
		playButton_->setObjectName("play");
		playButton_->setText(tr("Play"));
		playButton_->setToolTip(tr("Play Animation"));
		playButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		playButton_->setCheckable(true);
		
		leftButton_ = new QToolButton;
		leftButton_->setObjectName("left");
		leftButton_->setText(tr("Left"));
		leftButton_->setToolTip(tr("Backward 1 second"));
		leftButton_->installEventFilter(this);
		leftButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		rightButton_ = new QToolButton;
		rightButton_->setObjectName("right");
		rightButton_->setText(tr("Right"));
		rightButton_->setToolTip(tr("Forward 1 second"));
		rightButton_->installEventFilter(this);
		rightButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		resetButton_ = new QToolButton;
		resetButton_->setObjectName("reset");
		resetButton_->setText(tr("Reset"));
		resetButton_->setToolTip(tr("Reset States"));
		resetButton_->installEventFilter(this);
		resetButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		volumeButton_ = new QToolButton;
		volumeButton_->setText(tr("Volume"));
		volumeButton_->setObjectName("volumeMiddle");
		volumeButton_->setToolTip(tr("Volume"));
		volumeButton_->installEventFilter(this);
		volumeButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		volumeSlider_ = new QSlider;
		volumeSlider_->setObjectName("Slider");
		volumeSlider_->setToolTip(tr("Volume"));
		volumeSlider_->setOrientation(Qt::Horizontal);
		volumeSlider_->setRange(0, 100);
		volumeSlider_->setValue(100);

		auto splitLine = new QFrame;
		splitLine->setObjectName("VLine");
		splitLine->setLineWidth(1);
		splitLine->setFrameShape(QFrame::VLine);
		splitLine->setFrameShadow(QFrame::Plain);
		splitLine->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
		splitLine->setContentsMargins(0, 8, 0, 8);

		auto splitLine2 = new QFrame;
		splitLine2->setObjectName("VLine");
		splitLine2->setLineWidth(1);
		splitLine2->setFrameShape(QFrame::VLine);
		splitLine2->setFrameShadow(QFrame::Plain);
		splitLine2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
		splitLine2->setContentsMargins(0, 8, 0, 8);
		
		auto splitLine3 = new QFrame;
		splitLine3->setObjectName("VLine");
		splitLine3->setLineWidth(1);
		splitLine3->setFrameShape(QFrame::VLine);
		splitLine3->setFrameShadow(QFrame::Plain);
		splitLine3->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
		splitLine3->setContentsMargins(0, 8, 0, 8);

		layout_ = new QHBoxLayout;
		layout_->setObjectName("ToplevelLayout");
		layout_->addStretch();
		layout_->addWidget(openButton_, 0, Qt::AlignCenter);
		layout_->addWidget(saveButton_, 0, Qt::AlignCenter);
		layout_->addWidget(loadButton_, 0, Qt::AlignCenter);
		layout_->addWidget(cleanupButton_, 0, Qt::AlignCenter);
		layout_->addWidget(splitLine);
		layout_->addWidget(gpuButton_, 0, Qt::AlignCenter);
		layout_->addWidget(shotButton_, 0, Qt::AlignCenter);
		layout_->addWidget(splitLine2);
		layout_->addWidget(resetButton_, 0, Qt::AlignCenter);
		layout_->addWidget(leftButton_, 0, Qt::AlignCenter);
		layout_->addWidget(playButton_, 0, Qt::AlignCenter);
		layout_->addWidget(rightButton_, 0, Qt::AlignCenter);
		layout_->addWidget(splitLine3);
		layout_->addWidget(audioButton_, 0, Qt::AlignCenter);
		layout_->addWidget(volumeButton_, 0, Qt::AlignCenter);
		layout_->addSpacing(4);
		layout_->addWidget(volumeSlider_);
		layout_->addStretch();
		layout_->setSpacing(0);
		layout_->setContentsMargins(0, 0, 0, 0);
		
		auto mainWidget = new QWidget;
		mainWidget->setObjectName("ToplevelWidget");
		mainWidget->setLayout(layout_);

		this->addWidget(mainWidget);

		profile->playerModule->isPlaying += [this](bool value) {
			this->update();
		};

		profile->playerModule->finish += [this](bool value) {
			this->update();
		};

		profile->offlineModule->enable += [this](bool value) {
			this->update();
		};

		profile->soundModule->sound += [this](const octoon::GameObjectPtr& value) {
			this->update();
		};

		profile->soundModule->volume += [this](float value) {
			volumeSlider_->setValue(value * 100.0f);
		};

		this->connect(openButton_, SIGNAL(clicked()), this, SLOT(openEvent()));
		this->connect(loadButton_, SIGNAL(clicked()), this, SLOT(importEvent()));
		this->connect(saveButton_, SIGNAL(clicked()), this, SLOT(saveEvent()));
		this->connect(audioButton_, SIGNAL(clicked()), this, SLOT(audioEvent()));
		this->connect(shotButton_, SIGNAL(clicked()), this, SLOT(shotEvent()));
		this->connect(gpuButton_, SIGNAL(clicked()), this, SLOT(gpuEvent()));
		this->connect(cleanupButton_, SIGNAL(clicked()), this, SLOT(cleanupEvent()));
		this->connect(resetButton_, SIGNAL(clicked()), this, SLOT(resetEvent()));
		this->connect(playButton_, SIGNAL(clicked()), this, SLOT(playEvent()));
		this->connect(leftButton_, SIGNAL(clicked()), this, SLOT(leftEvent()));
		this->connect(rightButton_, SIGNAL(clicked()), this, SLOT(rightEvent()));
		this->connect(volumeButton_, SIGNAL(clicked()), this, SLOT(volumeEvent()));
		this->connect(volumeSlider_, SIGNAL(valueChanged(int)), this, SLOT(volumeSliderEvent(int)));
	}

	ToplevelBar::~ToplevelBar() noexcept
	{
	}

	void
	ToplevelBar::openEvent() noexcept
	{
		spdlog::debug("Entered openEvent");

		try
		{
			if (!profile_->path.empty() || octoon::AssetDatabase::instance()->isDirty())
			{
				if (QMessageBox::question(this, tr("Info"), tr("Do you want to discard your local changes?")) == QMessageBox::No)
					return;

				QCoreApplication::processEvents();
			}
			
			if (behaviour_ && !profile_->playerModule->isPlaying)
			{
				auto behaviour = behaviour_->getComponent<unreal::UnrealBehaviour>();
				if (behaviour)
				{
					QString fileName = QFileDialog::getExistingDirectory(this, tr("Open Project"));
					if (!fileName.isEmpty())
					{
#if 1
						auto path = fileName.toStdWString();
						if (std::filesystem::exists(std::filesystem::path(path).append("manifest.json")))
						{
							QProgressDialog dialog(tr("Loading..."), tr("Cancel"), 0, 1, this);
							dialog.setWindowTitle(tr("Open Project..."));
							dialog.setWindowModality(Qt::WindowModal);
							dialog.setValue(0);
							dialog.show();

							QCoreApplication::processEvents();

							behaviour->open(path);

							dialog.setValue(1);
						}
						else
						{
							QCoreApplication::processEvents();
							QMessageBox::critical(this, tr("Error"), tr("Can't find manifest.json in %1").arg(fileName));
						}
#else
						// load task
						auto fn = [&]() {
							behaviour->open(fileName.toStdWString());
						};
						QFuture<void> fu = QtConcurrent::run(fn);

						// progress dialog
						QProgressDialog dialog(tr("Opening"), tr("Cancel"), 0, 2000, this);
						dialog.setWindowTitle(tr("Open Progress"));
						dialog.setWindowModality(Qt::WindowModal);
						dialog.show();
						for (int i = 0; i < 1900; i++)
						{
							dialog.setValue(i);
							QCoreApplication::processEvents();
							if (dialog.wasCanceled())
								break;
						}

						// wait finish
						fu.waitForFinished();

						// left progress
						for (int i = 1900; i < 2000; i++)
						{
							dialog.setValue(i);
							QCoreApplication::processEvents();
							if (dialog.wasCanceled())
								break;
						}
						dialog.setValue(2000);
#endif
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			QCoreApplication::processEvents();

			spdlog::error("Function importEvent raised exception: " + std::string(e.what()));
			QMessageBox::critical(this, tr("Error"), tr("Failed to open project: ") + QString::fromStdString(e.what()));
		}
		spdlog::debug("Exited openEvent");
	}
	
	void
	ToplevelBar::importEvent() noexcept
	{
		spdlog::debug("Entered importEvent");
		try
		{
			if (behaviour_ && !profile_->playerModule->isPlaying)
			{
				auto behaviour = behaviour_->getComponent<unreal::UnrealBehaviour>();
				if (behaviour)
				{
					QString fileName = QFileDialog::getOpenFileName(this, tr("Import Resource"), "", tr("All Files(*.pmm *.pmx *.abc *.fbx *.obj *.vmd *.scene);; PMM Files (*.pmm);; PMX Files (*.pmx);; VMD Files (*.vmd);; Abc Files (*.abc);; FBX Files (*.fbx);; OBJ Files (*.obj)"));
					if (!fileName.isEmpty())
					{
						behaviour->load(fileName.toStdWString());
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			QCoreApplication::processEvents();

			spdlog::error("Function importEvent raised exception: " + std::string(e.what()));
			QMessageBox::critical(this, tr("Error"), tr("Failed to import resource: ") + QString::fromStdString(e.what()));
		}

		spdlog::debug("Exited importEvent");
	}

	void
	ToplevelBar::saveEvent() noexcept
	{
		try
		{
			spdlog::debug("Entered saveEvent");

			if (behaviour_)
			{
				auto filepath = this->profile_->path;
				if (filepath.empty())
				{
					QString fileName = QFileDialog::getExistingDirectory(this, tr("New Project"));
					if (fileName.isEmpty())
						return;
					filepath = fileName.toStdWString();
				}
				
				QProgressDialog dialog(tr("Save..."), tr("Cancel"), 0, 1, this);
				dialog.setWindowTitle(tr("Save Project"));
				dialog.setWindowModality(Qt::WindowModal);
				dialog.setValue(0);
				dialog.show();

				QCoreApplication::processEvents();

				auto behaviour = behaviour_->getComponent<unreal::UnrealBehaviour>();
				behaviour->save(filepath);

				dialog.setValue(1);

				QApplication::beep();
			}

			spdlog::debug("Exited saveEvent");
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	ToplevelBar::audioEvent() noexcept
	{
		spdlog::debug("Entered audioEvent");

		try
		{
			if (behaviour_ && !profile_->playerModule->isPlaying && !profile_->recordModule->active)
			{
				if (!audioEnable_)
				{
					QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("All Files(*.wav *.mp3 *.flac *.ogg);; Wav Files (*.wav);; MP3 Files (*.mp3);; FLAC Files (*.flac);; OGG Files (*.ogg)"));
					if (!fileName.isEmpty())
					{
						profile_->soundModule->filepath = fileName.toUtf8().toStdString();
						audioButton_->setIcon(audioOnIcon_);
						audioEnable_ = true;
					}
				}
				else
				{
					profile_->soundModule->filepath = std::string();
					audioButton_->setIcon(audioIcon_);
					audioEnable_ = false;
				}
			}
		}
		catch (const std::exception& e)
		{
			spdlog::error("Function audioEvent raised exception: " + std::string(e.what()));

			QCoreApplication::processEvents();
			QMessageBox::critical(this, tr("Error"), e.what());
		}

		spdlog::debug("Exited audioEvent");
	}

	void
	ToplevelBar::shotEvent() noexcept
	{
		try
		{
			spdlog::debug("Entered shotEvent");

			if (behaviour_)
			{
				QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Files (*.png)"));
				if (!fileName.isEmpty())
				{
					auto behaviour = behaviour_->getComponent<unreal::UnrealBehaviour>();
					behaviour->renderPicture(fileName.toStdWString());
				}
			}

			spdlog::debug("Exited shotEvent");
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	ToplevelBar::gpuEvent() noexcept
	{
		try
		{
			spdlog::debug("Entered gpuEvent");

			if (!gpuEnable_)
			{
				profile_->offlineModule->setEnable(true);
				gpuButton_->setIcon(gpuOnIcon_);
				gpuEnable_ = true;
			}
			else
			{
				profile_->offlineModule->setEnable(false);
				gpuButton_->setIcon(gpuIcon_);
				gpuEnable_ = false;
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	ToplevelBar::cleanupEvent() noexcept
	{
		try
		{
			spdlog::debug("Entered cleanupEvent");

			auto behaviour = behaviour_->getComponent<unreal::UnrealBehaviour>();
			if (behaviour)
			{
				if (!profile_->path.empty() || octoon::AssetDatabase::instance()->isDirty())
				{
					if (QMessageBox::question(this, tr("Info"), tr("Do you want to discard your local changes?")) == QMessageBox::Yes)
						behaviour->reset();
				}
				else
				{
					behaviour->reset();
				}
			}

			spdlog::debug("Exited cleanupEvent");
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	ToplevelBar::playEvent() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->recordModule->active)
			{
				auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
				if (behaviour)
				{
					if (playEnable_)
					{
						behaviour->pause();
						playButton_->setIcon(playIcon_);
						playButton_->setToolTip(tr("Play"));
						playEnable_ = false;
					}
					else
					{
						behaviour->play();
						playButton_->setIcon(playOnIcon_);
						playButton_->setToolTip(tr("Pause"));
						playEnable_ = true;
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	ToplevelBar::resetEvent() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->playerModule->isPlaying)
			{
				auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
				if (behaviour->isOpen())
				{
					auto player = behaviour->getComponent<PlayerComponent>();
					if (player)
					{
						player->reset();
						playButton_->setIcon(playIcon_);
						playButton_->setToolTip(tr("Play"));
						playEnable_ = false;
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	ToplevelBar::leftEvent() noexcept
	{
		try
		{
			if (!profile_->playerModule->isPlaying)
			{
				auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
				if (behaviour->isOpen())
				{
					auto player = dynamic_cast<PlayerComponent*>(behaviour->getComponent<PlayerComponent>());
					if (player)
						player->sample(-1.0f);
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void 
	ToplevelBar::rightEvent() noexcept
	{
		try
		{
			if (!profile_->playerModule->isPlaying)
			{
				auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
				if (behaviour->isOpen())
				{
					auto player = dynamic_cast<PlayerComponent*>(behaviour->getComponent<PlayerComponent>());
					if (player)
						player->sample(1.0f);
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	ToplevelBar::volumeEvent() noexcept
	{
		if (!volumeEnable_)
		{
			this->profile_->soundModule->volume = 1.0f;
			volumeButton_->setIcon(volumeOnIcon_);
			volumeButton_->setToolTip(tr("Volume"));
			volumeEnable_ = true;
		}
		else
		{
			this->profile_->soundModule->volume = 0.0f;
			volumeButton_->setIcon(volumeOffIcon_);
			volumeButton_->setToolTip(tr("VolumeOff"));
			volumeEnable_ = false;
		}
	}
	
	void
	ToplevelBar::volumeSliderEvent(int value)
	{
		auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
		if (behaviour)
		{
			this->profile_->soundModule->volume = value / 100.0f;

			if (value == 0 && volumeEnable_)
			{
				volumeButton_->setIcon(volumeOffIcon_);
				volumeButton_->setToolTip(tr("VolumeOff"));
				volumeEnable_ = false;
			}
			else if (value != 0 && !volumeEnable_)
			{
				volumeButton_->setIcon(volumeOnIcon_);
				volumeButton_->setToolTip(tr("Volume"));
				volumeEnable_ = true;
			}
		}
	}

	void
	ToplevelBar::update() noexcept
	{
		if (profile_->playerModule->isPlaying)
		{
			if (!playEnable_)
			{
				playButton_->setIcon(playOnIcon_);
				playEnable_ = true;
			}
		}
		else
		{
			if (playEnable_)
			{
				playButton_->setIcon(playIcon_);
				playEnable_ = false;
			}
		}

		if (this->profile_->offlineModule->getEnable())
		{
			if (!gpuEnable_)
			{
				gpuButton_->setIcon(gpuOnIcon_);
				gpuEnable_ = true;
			}
		}
		else
		{
			if (gpuEnable_)
			{
				gpuButton_->setIcon(gpuIcon_);
				gpuEnable_ = false;
			}
		}

		if (this->profile_->soundModule->sound.getValue())
		{
			if (!audioEnable_)
			{
				audioButton_->setIcon(audioOnIcon_);
				audioEnable_ = true;
			}
		}
		else
		{
			if (audioEnable_)
			{
				audioButton_->setIcon(audioIcon_);
				audioEnable_ = false;
			}
		}
	}

	bool
	ToplevelBar::eventFilter(QObject* watched, QEvent* event)
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