#include "main_window.h"
#include "rabbit_behaviour.h"
#include <qmimedata.h>
#include <qfiledialog.h>
#include <qcolordialog.h>
#include <qsplashscreen.h>
#include <qmessagebox.h>
#include <fstream>
#include <filesystem>
#include "controllers/offline_component.h"

namespace rabbit
{
	octoon::input::InputKey::Code KeyCodetoInputKey(int key) noexcept
	{
		switch (key)
		{
		case Qt::Key_unknown: return octoon::input::InputKey::Code::None;
		case Qt::Key_Space: return octoon::input::InputKey::Code::Space;
		case Qt::Key_Apostrophe: return octoon::input::InputKey::Code::Apostrophe;
		case Qt::Key_Comma: return octoon::input::InputKey::Code::Comma;
		case Qt::Key_Minus: return octoon::input::InputKey::Code::Minus;
		case Qt::Key_Period: return octoon::input::InputKey::Code::Period;
		case Qt::Key_Slash: return octoon::input::InputKey::Code::Slash;
		case Qt::Key_0: return octoon::input::InputKey::Code::Key0;
		case Qt::Key_1: return octoon::input::InputKey::Code::Key1;
		case Qt::Key_2: return octoon::input::InputKey::Code::Key2;
		case Qt::Key_3: return octoon::input::InputKey::Code::Key3;
		case Qt::Key_4: return octoon::input::InputKey::Code::Key4;
		case Qt::Key_5: return octoon::input::InputKey::Code::Key5;
		case Qt::Key_6: return octoon::input::InputKey::Code::Key6;
		case Qt::Key_7: return octoon::input::InputKey::Code::Key7;
		case Qt::Key_8: return octoon::input::InputKey::Code::Key8;
		case Qt::Key_9: return octoon::input::InputKey::Code::Key9;
		case Qt::Key_Semicolon: return octoon::input::InputKey::Code::Semicolon;
		case Qt::Key_Equal: return octoon::input::InputKey::Code::Equal;
		case Qt::Key_A: return octoon::input::InputKey::Code::A;
		case Qt::Key_B: return octoon::input::InputKey::Code::B;
		case Qt::Key_C: return octoon::input::InputKey::Code::C;
		case Qt::Key_D: return octoon::input::InputKey::Code::D;
		case Qt::Key_E: return octoon::input::InputKey::Code::E;
		case Qt::Key_F: return octoon::input::InputKey::Code::F;
		case Qt::Key_G: return octoon::input::InputKey::Code::G;
		case Qt::Key_H: return octoon::input::InputKey::Code::H;
		case Qt::Key_I: return octoon::input::InputKey::Code::I;
		case Qt::Key_J: return octoon::input::InputKey::Code::J;
		case Qt::Key_K: return octoon::input::InputKey::Code::K;
		case Qt::Key_L: return octoon::input::InputKey::Code::L;
		case Qt::Key_M: return octoon::input::InputKey::Code::M;
		case Qt::Key_N: return octoon::input::InputKey::Code::N;
		case Qt::Key_O: return octoon::input::InputKey::Code::O;
		case Qt::Key_P: return octoon::input::InputKey::Code::P;
		case Qt::Key_Q: return octoon::input::InputKey::Code::Q;
		case Qt::Key_R: return octoon::input::InputKey::Code::R;
		case Qt::Key_S: return octoon::input::InputKey::Code::S;
		case Qt::Key_T: return octoon::input::InputKey::Code::T;
		case Qt::Key_U: return octoon::input::InputKey::Code::U;
		case Qt::Key_V: return octoon::input::InputKey::Code::V;
		case Qt::Key_W: return octoon::input::InputKey::Code::W;
		case Qt::Key_X: return octoon::input::InputKey::Code::X;
		case Qt::Key_Y: return octoon::input::InputKey::Code::Y;
		case Qt::Key_Z: return octoon::input::InputKey::Code::Z;
		case Qt::Key_BracketLeft: return octoon::input::InputKey::Code::LeftBracket;
		case Qt::Key_Backslash: return octoon::input::InputKey::Code::Backslash;
		case Qt::Key_BracketRight: return octoon::input::InputKey::Code::RightBracket;
		case Qt::Key_Word: return octoon::input::InputKey::Code::World1;
		case Qt::Key_Escape: return octoon::input::InputKey::Code::Escape;
		case Qt::Key_Enter: return octoon::input::InputKey::Code::Enter;
		case Qt::Key_Tab: return octoon::input::InputKey::Code::Tab;
		case Qt::Key_Backspace: return octoon::input::InputKey::Code::Backspace;
		case Qt::Key_Insert: return octoon::input::InputKey::Code::Insert;
		case Qt::Key_Delete: return octoon::input::InputKey::Code::Delete;
		case Qt::Key_Right: return octoon::input::InputKey::Code::ArrowRight;
		case Qt::Key_Left: return octoon::input::InputKey::Code::ArrowLeft;
		case Qt::Key_Down: return octoon::input::InputKey::Code::ArrowDown;
		case Qt::Key_Up: return octoon::input::InputKey::Code::ArrowUp;
		case Qt::Key_PageUp: return octoon::input::InputKey::Code::PageUp;
		case Qt::Key_PageDown: return octoon::input::InputKey::Code::PageDown;
		case Qt::Key_Home: return octoon::input::InputKey::Code::Home;
		case Qt::Key_End: return octoon::input::InputKey::Code::End;
		case Qt::Key_CapsLock: return octoon::input::InputKey::Code::CapsLock;
		case Qt::Key_ScrollLock: return octoon::input::InputKey::Code::ScrollLock;
		case Qt::Key_NumLock: return octoon::input::InputKey::Code::NumLock;
		case Qt::Key_Print: return octoon::input::InputKey::Code::PrintScreen;
		case Qt::Key_Pause: return octoon::input::InputKey::Code::Pause;
		case Qt::Key_F1: return octoon::input::InputKey::Code::F1;
		case Qt::Key_F2: return octoon::input::InputKey::Code::F2;
		case Qt::Key_F3: return octoon::input::InputKey::Code::F3;
		case Qt::Key_F4: return octoon::input::InputKey::Code::F4;
		case Qt::Key_F5: return octoon::input::InputKey::Code::F5;
		case Qt::Key_F6: return octoon::input::InputKey::Code::F6;
		case Qt::Key_F7: return octoon::input::InputKey::Code::F7;
		case Qt::Key_F8: return octoon::input::InputKey::Code::F8;
		case Qt::Key_F9: return octoon::input::InputKey::Code::F9;
		case Qt::Key_F10: return octoon::input::InputKey::Code::F10;
		case Qt::Key_F11: return octoon::input::InputKey::Code::F11;
		case Qt::Key_F12: return octoon::input::InputKey::Code::F12;
		case Qt::Key_F13: return octoon::input::InputKey::Code::F13;
		case Qt::Key_F14: return octoon::input::InputKey::Code::F14;
		case Qt::Key_F15: return octoon::input::InputKey::Code::F15;
		case Qt::Key_F16: return octoon::input::InputKey::Code::F16;
		case Qt::Key_F17: return octoon::input::InputKey::Code::F17;
		case Qt::Key_F18: return octoon::input::InputKey::Code::F18;
		case Qt::Key_F19: return octoon::input::InputKey::Code::F19;
		case Qt::Key_F20: return octoon::input::InputKey::Code::F20;
		case Qt::Key_F21: return octoon::input::InputKey::Code::F21;
		case Qt::Key_F22: return octoon::input::InputKey::Code::F22;
		case Qt::Key_F23: return octoon::input::InputKey::Code::F23;
		case Qt::Key_F24: return octoon::input::InputKey::Code::F24;
		case Qt::Key_F25: return octoon::input::InputKey::Code::F25;
		case Qt::Key_Launch0: return octoon::input::InputKey::Code::KP_0;
		case Qt::Key_Launch1: return octoon::input::InputKey::Code::KP_1;
		case Qt::Key_Launch2: return octoon::input::InputKey::Code::KP_2;
		case Qt::Key_Launch3: return octoon::input::InputKey::Code::KP_3;
		case Qt::Key_Launch4: return octoon::input::InputKey::Code::KP_4;
		case Qt::Key_Launch5: return octoon::input::InputKey::Code::KP_5;
		case Qt::Key_Launch6: return octoon::input::InputKey::Code::KP_6;
		case Qt::Key_Launch7: return octoon::input::InputKey::Code::KP_7;
		case Qt::Key_Launch8: return octoon::input::InputKey::Code::KP_8;
		case Qt::Key_Launch9: return octoon::input::InputKey::Code::KP_9;
		case Qt::Key_Shift: return octoon::input::InputKey::Code::LeftShift;
		case Qt::Key_Control: return octoon::input::InputKey::Code::LeftControl;
		case Qt::Key_Alt: return octoon::input::InputKey::Code::LeftAlt;
		case Qt::Key_Super_L: return octoon::input::InputKey::Code::LeftSuper;
		case Qt::Key_Super_R: return octoon::input::InputKey::Code::RightShift;
		case Qt::Key_Menu: return octoon::input::InputKey::Code::Menu;
		default:
			return octoon::input::InputKey::Code::None;
		}
	}

	MainListener::MainListener(SplashScreen* splash, const std::string& path)
		: splash_(splash)
		, stream_(path)
	{
	}

	void 
	MainListener::onMessage(std::string_view message) noexcept
	{
		if (splash_)
		{
			splash_->message->setText(QString::fromStdString(std::string(message)));
			splash_->repaint();
		}

		if (stream_.good())
		{
			stream_ << message;
			stream_ << "\n";
			stream_.flush();
		}
	}

	MainWindow::MainWindow(SplashScreen* splash) noexcept(false)
		: init_flag(false)
		, profile_(rabbit::RabbitProfile::load("./config/config.conf"))
		, behaviour_(octoon::GameObject::create())
		, splash_(splash)
		, listener_(std::make_shared<MainListener>(splash, "./log.txt"))
	{
		this->setFrameShape(Panel);
		this->setObjectName("mainWindow");
		this->setWindowFlags(Qt::FramelessWindowHint);
		this->installEventFilter(this);

		QImage image(":res/icons/rabbit.png");
		auto w = image.width();
		auto h = image.height();
		auto bits = image.bits();
		auto channel = image.bitPlaneCount() / 8;

		profile_->markModule->width = w;
		profile_->markModule->height = h;
		profile_->markModule->channel = channel;
		profile_->markModule->pixels.resize(w * h * 4);
		std::memcpy(profile_->markModule->pixels.data(), bits, w * h * 4);

		titleBar_ = std::make_unique<TitleWindow>(this, profile_);
		hideBar_ = std::make_unique<HideBar>(this);
		toolBar_ = std::make_unique<ToolWindow>(this, behaviour_);
		viewPanel_ = std::make_unique<ViewWidget>(this, profile_);
		sunWindow_ = std::make_unique<SunWindow>(profile_);
		environmentWindow_ = std::make_unique<EnvironmentWindow>(this, profile_);
		recordWindow_ = std::make_unique<RecordWindow>(this, behaviour_);
		materialWindow_ = std::make_unique<MaterialWindow>(this, behaviour_);

		mainLayout_ = std::make_unique<QHBoxLayout>(this);
		mainLayout_->setMargin(0);
		mainLayout_->setSpacing(0);

		contextLayout_ = std::make_unique<QVBoxLayout>();
		contextLayout_->addWidget(titleBar_.get());
		contextLayout_->addWidget(viewPanel_.get());
		contextLayout_->setMargin(0);
		contextLayout_->setSpacing(0);

		mainLayout_->addLayout(contextLayout_.get());
		mainLayout_->addWidget(toolBar_.get());
		mainLayout_->addWidget(hideBar_.get());
		mainLayout_->addWidget(sunWindow_.get());
		mainLayout_->addWidget(environmentWindow_.get());
		mainLayout_->addWidget(recordWindow_.get());
		mainLayout_->addWidget(materialWindow_.get());

		connect(titleBar_.get(), &TitleWindow::settingSignal, this, &MainWindow::onSettingSignal);
		connect(titleBar_.get(), &TitleWindow::vipSignal, this, &MainWindow::onVipSignal);

		connect(hideBar_.get(), &HideBar::showSignal, this, &MainWindow::onShowToolbarSignal);

		connect(toolBar_.get(), &ToolWindow::hideSignal, this, &MainWindow::onHideToolBarSignal);
		connect(toolBar_.get(), &ToolWindow::importSignal, this, &MainWindow::onImportSignal);
		connect(toolBar_.get(), &ToolWindow::playSignal, this, &MainWindow::onPlaySignal);
		connect(toolBar_.get(), &ToolWindow::resetSignal, this, &MainWindow::onResetSignal);
		connect(toolBar_.get(), &ToolWindow::leftSignal, this, &MainWindow::onLeftSignal);
		connect(toolBar_.get(), &ToolWindow::rightSignal, this, &MainWindow::onRightSignal);
		connect(toolBar_.get(), &ToolWindow::recordSignal, this, &MainWindow::onRecordSignal);
		connect(toolBar_.get(), &ToolWindow::shotSignal, this, &MainWindow::onScreenShotSignal);
		connect(toolBar_.get(), &ToolWindow::gpuSignal, this, &MainWindow::onOfflineModeSignal);
		connect(toolBar_.get(), &ToolWindow::hdrSignal, this, &MainWindow::onImportHdriSignal);
		connect(toolBar_.get(), &ToolWindow::cleanupSignal, this, &MainWindow::onCleanupSignal);
		connect(toolBar_.get(), &ToolWindow::sunSignal, this, &MainWindow::onSunSignal);
		connect(toolBar_.get(), &ToolWindow::environmentSignal, this, &MainWindow::onEnvironmentSignal);
		connect(toolBar_.get(), &ToolWindow::materialSignal, this, &MainWindow::onMaterialSignal);		

		connect(viewPanel_.get(), &ViewWidget::resizeSignal, this, &MainWindow::onResizeSignal);
		connect(viewPanel_.get(), &ViewWidget::mousePressSignal, this, &MainWindow::onMousePressSignal);
		connect(viewPanel_.get(), &ViewWidget::mouseMoveSignal, this, &MainWindow::onMouseMoveSignal);
		connect(viewPanel_.get(), &ViewWidget::mouseReleaseSignal, this, &MainWindow::onMouseReleaseSignal);
		connect(viewPanel_.get(), &ViewWidget::mouseDoubleClickSignal, this, &MainWindow::onMouseDoubleClickSignal);
		connect(viewPanel_.get(), &ViewWidget::wheelSignal, this, &MainWindow::onWheelSignal);
		connect(viewPanel_.get(), &ViewWidget::dragEnterSignal, this, &MainWindow::onDragEnterSignal);
		connect(viewPanel_.get(), &ViewWidget::dragMoveSignal, this, &MainWindow::onDragMoveSignal);
		connect(viewPanel_.get(), &ViewWidget::dropSignal, this, &MainWindow::onDropSignal);
		connect(viewPanel_.get(), &ViewWidget::showSignal, this, &MainWindow::onShowSignal);
		connect(viewPanel_.get(), &ViewWidget::updateSignal, this, &MainWindow::onUpdateSignal);
	}

	MainWindow::~MainWindow()
	{
		toolBar_.reset();
		viewPanel_.reset();
		loginWindow_.reset();
		settingWindow_.reset();
		sunWindow_.reset();
		environmentWindow_.reset();
		titleBar_.reset();
		recordWindow_.reset();
		materialWindow_.reset();
		std::filesystem::create_directories("config");
		rabbit::RabbitProfile::save("./config/config.conf", *profile_);

		profile_.reset();
		behaviour_.reset();
		gameApp_.reset();
	}

	void
	MainWindow::onHideToolBarSignal() noexcept
	{
		toolBar_->hide();
		this->setFixedWidth(this->width() - toolBar_->width() + hideBar_->width());
		hideBar_->show();
	}

	void
	MainWindow::onShowToolbarSignal() noexcept
	{
		hideBar_->hide();
		this->setFixedWidth(this->width() + toolBar_->width() - hideBar_->width());
		toolBar_->show();
	}

	void
	MainWindow::onImportSignal() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->timeModule->playing_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour)
				{
					QString fileName = QFileDialog::getOpenFileName(this, u8"打开项目", "", tr("All Files(*.pmm *.pmx *.abc *.hdr *.mdl);; PMM Files (*.pmm);; PMX Files (*.pmx);; Abc Files (*.abc);; HDRi Files (*.hdr);; MDL Files (*.mdl)"));
					if (!fileName.isEmpty())
					{
						try
						{
							behaviour->open(fileName.toUtf8().data());
						}
						catch (const std::exception& e)
						{
							QMessageBox msg(this);
							msg.setWindowTitle(u8"错误");
							msg.setText(e.what());
							msg.setIcon(QMessageBox::Information);
							msg.setStandardButtons(QMessageBox::Ok);

							msg.exec();
						}
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	bool
	MainWindow::onPlaySignal(bool enable) noexcept
	{
		try
		{
			if (behaviour_ && !profile_->h264Module->enable)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					if (enable)
						behaviour->play();
					else
						behaviour->stop();

					return true;
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
					return false;
				}
			}
			
			return false;
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
			return false;
		}
	}

	bool
	MainWindow::onResetSignal() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->timeModule->playing_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					auto player = dynamic_cast<PlayerComponent*>(behaviour->getComponent<PlayerComponent>());
					if (player)
					{
						player->reset();
						return true;
					}
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
				}
			}

			return false;
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
			return false;
		}
	}

	void
	MainWindow::onLeftSignal() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->timeModule->playing_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					auto player = dynamic_cast<PlayerComponent*>(behaviour->getComponent<PlayerComponent>());
					if (player)
						player->sample(-1.0f);
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void 
	MainWindow::onRightSignal() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->timeModule->playing_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					auto player = dynamic_cast<PlayerComponent*>(behaviour->getComponent<PlayerComponent>());
					if (player)
						player->sample(1.0f);
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	bool
	MainWindow::onRecordSignal(bool enable) noexcept
	{
		try
		{
			if (behaviour_ && (!profile_->timeModule->playing_ || profile_->h264Module->enable))
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					if (recordWindow_->isHidden())
					{
						this->hideSliderWindow();
						this->setFixedWidth(this->width() + recordWindow_->minimumWidth());
						recordWindow_->show();
					}
					else
					{
						recordWindow_->close();
						this->setFixedWidth(this->width() - recordWindow_->width());
					}
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
					return false;
				}
			}
	
			return false;
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();

			return false;
		}
	}

	void 
	MainWindow::onScreenShotSignal() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->timeModule->playing_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					QString fileName = QFileDialog::getSaveFileName(this, u8"保存图像", "", tr("PNG Files (*.png)"));
					if (!fileName.isEmpty())
						behaviour->renderPicture(fileName.toUtf8().data());
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	bool
	MainWindow::onOfflineModeSignal(bool enable) noexcept
	{
		try
		{
			if (behaviour_ && !profile_->h264Module->enable)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					auto offline = behaviour->getComponent<OfflineComponent>();
					if (offline)
					{
						if (enable)
							offline->setActive(true);
						else
							offline->setActive(false);

					}

					return true;
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();

			return false;
		}
	}

	bool
	MainWindow::onImportHdriSignal(bool enable) noexcept
	{
		try
		{
			if (behaviour_ && !profile_->timeModule->playing_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
				{
					if (enable)
					{
						QString fileName = QFileDialog::getOpenFileName(this, u8"打开图像", "", tr("HDRi Files (*.hdr)"));
						if (!fileName.isEmpty())
						{
							behaviour->loadHDRi(fileName.toUtf8().data());
							return true;
						}
					}
					else
					{
						behaviour->clearHDRi();
						return true;
					}
				}
				else
				{
					QMessageBox msg(this);
					msg.setWindowTitle(u8"提示");
					msg.setText(u8"请加载一个.pmm工程");
					msg.setIcon(QMessageBox::Information);
					msg.setStandardButtons(QMessageBox::Ok);

					msg.exec();
				}
			}

			return false;
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();

			return false;
		}
	}

	void
	MainWindow::onSunSignal() noexcept
	{
		try
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour->isOpen())
			{
				if (profile_->entitiesModule->sunLight && !profile_->timeModule->playing_)
				{
					if (sunWindow_->isHidden())
					{
						this->hideSliderWindow();
						this->setFixedWidth(this->width() + sunWindow_->minimumWidth());
						sunWindow_->show();
					}
					else
					{
						sunWindow_->close();
						this->setFixedWidth(this->width() - sunWindow_->width());
					}
				}
			}
			else
			{
				QMessageBox msg(this);
				msg.setWindowTitle(u8"提示");
				msg.setText(u8"请加载一个.pmm工程");
				msg.setIcon(QMessageBox::Information);
				msg.setStandardButtons(QMessageBox::Ok);

				msg.exec();
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void
	MainWindow::onEnvironmentSignal() noexcept
	{
		try
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour->isOpen())
			{
				if (profile_->entitiesModule->enviromentLight && !profile_->timeModule->playing_)
				{
					if (environmentWindow_->isHidden())
					{
						this->hideSliderWindow();
						this->setFixedWidth(this->width() + environmentWindow_->minimumWidth());
						environmentWindow_->show();
					}
					else
					{
						environmentWindow_->close();
						this->setFixedWidth(this->width() - environmentWindow_->width());
					}
				}
			}
			else
			{
				QMessageBox msg(this);
				msg.setWindowTitle(u8"提示");
				msg.setText(u8"请加载一个.pmm工程");
				msg.setIcon(QMessageBox::Information);
				msg.setStandardButtons(QMessageBox::Ok);

				msg.exec();
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void
	MainWindow::onMaterialSignal() noexcept
	{
		try
		{
			auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
			if (behaviour->isOpen())
			{
				if (!profile_->timeModule->playing_)
				{
					if (materialWindow_->isHidden())
					{
						this->hideSliderWindow();
						this->setFixedWidth(this->width() + materialWindow_->minimumWidth());
						materialWindow_->show();
					}
					else
					{
						materialWindow_->close();
						this->setFixedWidth(this->width() - materialWindow_->width());
					}
				}
			}
			else
			{
				QMessageBox msg(this);
				msg.setWindowTitle(u8"提示");
				msg.setText(u8"请加载一个.pmm工程");
				msg.setIcon(QMessageBox::Information);
				msg.setStandardButtons(QMessageBox::Ok);

				msg.exec();
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void 
	MainWindow::onCleanupSignal() noexcept
	{
		try
		{
			if (behaviour_ && !profile_->timeModule->playing_)
			{
				auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
				if (behaviour->isOpen())
					behaviour->close();
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void
	MainWindow::onProfileSignal(const rabbit::RabbitProfile& profile) noexcept
	{
	}

	void
	MainWindow::onSettingSignal() noexcept
	{
		try
		{
			settingWindow_ = std::make_unique<SettingWindow>(behaviour_->getComponent<rabbit::RabbitBehaviour>());
			settingWindow_->move(this->pos().x() + (this->width() - settingWindow_->width()) / 2, this->pos().y() + (this->height() - settingWindow_->height()) / 2);
			settingWindow_->exec();
			rabbit::RabbitProfile::save("./config/config.conf", *profile_);
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void 
	MainWindow::onVipSignal() noexcept
	{
		try
		{
			if (profile_->clientModule->isLogin())
			{
				infoWindow_ = std::make_unique<InfoWindow>(this, behaviour_->getComponent<RabbitBehaviour>());
				infoWindow_->connect(infoWindow_.get(), &InfoWindow::loginOut, this->titleBar_.get(), &TitleWindow::loginOut);
				infoWindow_->show();
				//infoWindow_->move(titleBar_->mapToGlobal(titleBar_->vipButton_->pos() + QPoint(-infoWindow_->size().width() + titleBar_->vipButton_->size().height(), titleBar_->vipButton_->size().height())));
			}
			else
			{
				loginWindow_ = std::make_unique<LoginWindow>(behaviour_->getComponent<RabbitBehaviour>());
				loginWindow_->connect(loginWindow_.get(), &LoginWindow::loginSuccess, this->titleBar_.get(), &TitleWindow::loginSuccess);
				loginWindow_->show();
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(e.what());
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();
		}
	}

	void
	MainWindow::onResizeSignal(QResizeEvent* e) noexcept
	{
		if (gameApp_)
			gameApp_->doWindowResize((octoon::WindHandle)viewPanel_->winId(), viewPanel_->width(), viewPanel_->height());
	}

	void
	MainWindow::onMouseMoveSignal(QMouseEvent* e) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
			gameApp_->doWindowMouseMotion((octoon::WindHandle)viewPanel_->winId(), e->pos().x(), e->pos().y());
	}

	void
	MainWindow::onMousePressSignal(QMouseEvent* e) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
		{
			if (e->button() == Qt::LeftButton)
				gameApp_->doWindowMouseButtonDown((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Left, e->x(), e->y());
			else if (e->button() == Qt::RightButton)
				gameApp_->doWindowMouseButtonDown((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Right, e->x(), e->y());
			else if (e->button() == Qt::MiddleButton)
				gameApp_->doWindowMouseButtonDown((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Middle, e->x(), e->y());
		}
	}

	void
	MainWindow::onMouseReleaseSignal(QMouseEvent* e) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
		{
			if (e->button() == Qt::LeftButton)
				gameApp_->doWindowMouseButtonUp((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Left, e->x(), e->y());
			else if (e->button() == Qt::RightButton)
				gameApp_->doWindowMouseButtonUp((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Right, e->x(), e->y());
			else if (e->button() == Qt::MiddleButton)
				gameApp_->doWindowMouseButtonUp((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Middle, e->x(), e->y());
		}
	}

	void
	MainWindow::onMouseDoubleClickSignal(QMouseEvent* e) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
		{
			if (e->button() == Qt::LeftButton)
				gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Left, e->x(), e->y());
			else if (e->button() == Qt::RightButton)
				gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Right, e->x(), e->y());
			else if (e->button() == Qt::MiddleButton)
				gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Middle, e->x(), e->y());
		}
	}

	void
	MainWindow::onWheelSignal(QWheelEvent* e) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
			gameApp_->doWindowScrool((octoon::WindHandle)viewPanel_->winId(), e->angleDelta().x(), e->angleDelta().y());
	}

	void
	MainWindow::onShowSignal(QShowEvent* event) noexcept
	{
		onUpdateSignal();
	}

	void
	MainWindow::onDragEnterSignal(QDragEnterEvent* event) noexcept
	{
		if (!profile_->timeModule->playing_)
			event->acceptProposedAction();
	}

	void
	MainWindow::onDropSignal(QDropEvent* event) noexcept
	{
		if (gameApp_)
		{
			auto urls = event->mimeData()->urls();
			if (!urls.isEmpty())
			{
				std::vector<QByteArray> utf8bytes;
				for (auto& it : urls)
					utf8bytes.push_back(it.path().toUtf8());

				if (!utf8bytes.empty())
				{
					std::vector<char*> utf8s;
					for (auto& it : utf8bytes)
						utf8s.push_back(it.data() + 1);

					gameApp_->doWindowDrop((octoon::WindHandle)viewPanel_->winId(), (std::uint32_t)utf8s.size(), (const char**)utf8s.data());
				}

				event->accept();
			}
			else
			{
				auto mineData = event->mimeData()->data("object/material");
				if (!mineData.isEmpty())
				{
					auto behaviour = behaviour_->getComponent<rabbit::RabbitBehaviour>();
					auto selectedItem = behaviour->getComponent<DragComponent>()->getHoverItem();
					if (selectedItem)
					{
						auto hit = selectedItem.value();
						auto materialComponent = behaviour->getComponent<MaterialComponent>();
						auto material = materialComponent->getMaterial(mineData.toStdString());

						auto meshRenderer = hit.object->getComponent<octoon::MeshRendererComponent>();
						if (meshRenderer)
							meshRenderer->setMaterial(material, hit.mesh);
					}
				}
			}
		}
	}

	void
	MainWindow::onDragMoveSignal(QDragMoveEvent *e) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
			gameApp_->doWindowMouseMotion((octoon::WindHandle)viewPanel_->winId(), e->pos().x(), e->pos().y());
	}

	void
	MainWindow::onUpdateSignal() noexcept
	{
#if defined(_WINDOWS) && !defined(_DEBUG)
		if (IsDebuggerPresent())
			std::exit(0);
#endif
		if (!init_flag)
		{
			init_flag = true;
			open(viewPanel_->width(), viewPanel_->height());
		}

		if (gameApp_)
			gameApp_->update();
	}

	void
	MainWindow::keyPressEvent(QKeyEvent* event) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
			gameApp_->doWindowKeyDown((octoon::WindHandle)viewPanel_->winId(), KeyCodetoInputKey(event->key()), 0, 0);
	}
	
	void
	MainWindow::keyReleaseEvent(QKeyEvent* event) noexcept
	{
		if (gameApp_ && !profile_->timeModule->playing_)
			gameApp_->doWindowKeyUp((octoon::WindHandle)viewPanel_->winId(), KeyCodetoInputKey(event->key()), 0, 0);
	}

	bool
	MainWindow::eventFilter(QObject* watched, QEvent* event)
	{
		if (watched == this)
		{
			if (QEvent::WindowActivate == event->type())
			{
				this->activateWindow();
				return true;
			}
		}

		return QWidget::eventFilter(watched, event);
	}

	bool
	MainWindow::open(int w, int h) noexcept
	{
		assert(!gameApp_);

		try
		{
			gameApp_ = std::make_shared<octoon::GameApp>();
			gameApp_->setGameListener(listener_);
			gameApp_->open((octoon::WindHandle)viewPanel_->winId(), w, h, w, h);
			gameApp_->setActive(true);
			listener_->splash_ = nullptr;

			auto behaviour = behaviour_->addComponent<rabbit::RabbitBehaviour>(profile_);
			behaviour->addMessageListener("rabbit:project:open", [this](const std::any&)
			{
				recordWindow_->repaint();
				sunWindow_->repaint();
				environmentWindow_->repaint();
			});

			behaviour->addMessageListener("rabbit:player:finish", [this](const std::any&)
			{
				if (toolBar_->playEnable_)
					toolBar_->stop();
				else
					recordWindow_->stopRecord();
			});

			behaviour->addMessageListener("rabbit:offline", [this](const std::any& enable)
			{
				if (std::any_cast<bool>(enable))
				{
					toolBar_->gpuButton.setIcon(toolBar_->gpuOnIcon_);
					toolBar_->gpuEnable_ = true;
				}
				else
				{
					toolBar_->gpuButton.setIcon(toolBar_->gpuIcon_);
					toolBar_->gpuEnable_ = false;
				}
			});

			return true;
		}
		catch (const std::exception& e)
		{
			QMessageBox msg(this);
			msg.setWindowTitle(u8"错误");
			msg.setText(u8"当前您的显卡可能并不支持OpenCL，或正在使用核显");
			msg.setIcon(QMessageBox::Information);
			msg.setStandardButtons(QMessageBox::Ok);

			msg.exec();

			listener_->onMessage(e.what());

			gameApp_.reset();
			gameApp_ = nullptr;

			exit(0);
		}
	}

	void
	MainWindow::hideSliderWindow() noexcept
	{
		if (!recordWindow_->isHidden())
		{
			recordWindow_->close();
			this->setFixedWidth(this->width() - recordWindow_->width());
		}
		if (!sunWindow_->isHidden())
		{
			sunWindow_->close();
			this->setFixedWidth(this->width() - sunWindow_->width());
		}
		if (!environmentWindow_->isHidden())
		{
			environmentWindow_->close();
			this->setFixedWidth(this->width() - environmentWindow_->width());
		}
		if (!materialWindow_->isHidden())
		{
			materialWindow_->close();
			this->setFixedWidth(this->width() - materialWindow_->width());
		}
	}
}