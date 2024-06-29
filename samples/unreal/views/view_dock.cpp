#include "view_dock.h"
#include "unreal_behaviour.h"
#include <qevent.h>
#include <qmessagebox.h>
#include <qmimedata.h>
#include <qapplication.h>
#include <spdlog/spdlog.h>

namespace unreal
{
	ViewDock::ViewDock(const octoon::GameAppPtr& gameApp, const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept
		: QDockWidget(tr("Camera"))
		, gameApp_(gameApp)
		, behaviour_(behaviour)
		, profile_(profile)
	{
		this->setAttribute(Qt::WA_PaintOnScreen, true);
		this->setAttribute(Qt::WA_AcceptDrops, true);
		this->setObjectName("ViewDock");
		this->setMouseTracking(true);
		this->setUpdatesEnabled(false);
		this->setAcceptDrops(true);
		this->setFocusPolicy(Qt::StrongFocus);
		this->setContextMenuPolicy(Qt::PreventContextMenu);
		this->setFeatures(QDockWidget::NoDockWidgetFeatures);
		this->installEventFilter(this);
	}

	ViewDock::~ViewDock() noexcept
	{
	}

	void
	ViewDock::resizeEvent(QResizeEvent* e) noexcept
	{
		if (gameApp_->isOpen())
		{
			auto dpi = QApplication::primaryScreen()->devicePixelRatio();
			gameApp_->doWindowResize((octoon::WindHandle)this->winId(), e->size().width() * dpi, e->size().height() * dpi);
		}

		QDockWidget::resizeEvent(e);
	}

	void
	ViewDock::mouseMoveEvent(QMouseEvent* e) noexcept
	{
		if (gameApp_->isOpen())
		{
			auto dpi = QApplication::primaryScreen()->devicePixelRatio();
			gameApp_->doWindowMouseMotion((octoon::WindHandle)this->winId(), e->pos().x() * dpi, e->pos().y() * dpi);
		}
	}

	void
	ViewDock::mousePressEvent(QMouseEvent* e) noexcept
	{
		if (gameApp_->isOpen())
		{
			auto dpi = QApplication::primaryScreen()->devicePixelRatio();

			if (e->button() == Qt::LeftButton)
				gameApp_->doWindowMouseButtonDown((octoon::WindHandle)this->winId(), octoon::input::InputButton::Left, e->x() * dpi, e->y() * dpi);
			else if (e->button() == Qt::RightButton)
				gameApp_->doWindowMouseButtonDown((octoon::WindHandle)this->winId(), octoon::input::InputButton::Right, e->x() * dpi, e->y() * dpi);
			else if (e->button() == Qt::MiddleButton)
				gameApp_->doWindowMouseButtonDown((octoon::WindHandle)this->winId(), octoon::input::InputButton::Middle, e->x() * dpi, e->y() * dpi);
		}
	}

	void
	ViewDock::mouseReleaseEvent(QMouseEvent* e) noexcept
	{
		if (gameApp_->isOpen())
		{
			auto dpi = QApplication::primaryScreen()->devicePixelRatio();

			if (e->button() == Qt::LeftButton)
				gameApp_->doWindowMouseButtonUp((octoon::WindHandle)this->winId(), octoon::input::InputButton::Left, e->x() * dpi, e->y() * dpi);
			else if (e->button() == Qt::RightButton)
				gameApp_->doWindowMouseButtonUp((octoon::WindHandle)this->winId(), octoon::input::InputButton::Right, e->x() * dpi, e->y() * dpi);
			else if (e->button() == Qt::MiddleButton)
				gameApp_->doWindowMouseButtonUp((octoon::WindHandle)this->winId(), octoon::input::InputButton::Middle, e->x() * dpi, e->y() * dpi);
		}
	}

	void
	ViewDock::mouseDoubleClickEvent(QMouseEvent* e) noexcept
	{
		if (gameApp_->isOpen())
		{
			auto dpi = QApplication::primaryScreen()->devicePixelRatio();

			if (e->button() == Qt::LeftButton)
				gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)this->winId(), octoon::input::InputButton::Left, e->x() * dpi, e->y() * dpi);
			else if (e->button() == Qt::RightButton)
				gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)this->winId(), octoon::input::InputButton::Right, e->x() * dpi, e->y() * dpi);
			else if (e->button() == Qt::MiddleButton)
				gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)this->winId(), octoon::input::InputButton::Middle, e->x() * dpi, e->y() * dpi);
		}
	}

	void
	ViewDock::wheelEvent(QWheelEvent* e) noexcept
	{
		if (gameApp_->isOpen())
			gameApp_->doWindowScrool((octoon::WindHandle)this->winId(), e->angleDelta().x(), e->angleDelta().y());
	}

	void
	ViewDock::keyPressEvent(QKeyEvent* event) noexcept
	{
		if (event->key() == Qt::Key_Delete)
		{
			if (profile_->selectorModule->selectedItem_.getValue().has_value())
			{
				if (QMessageBox::question(this, tr("Info"), tr("Are you sure you want to delete this model?")) == QMessageBox::Yes)
				{
					auto item = profile_->selectorModule->selectedItem_.getValue().value().object.lock();
					auto& items = profile_->entitiesModule->objects.getValue();
					
					while (item->getParent())
						item = item->getParent();

					for (auto it = items.begin(); it != items.end(); ++it)
					{
						if (*it == item)
						{
							items.erase(it);
							break;
						}
					}

					profile_->selectorModule->selectedItem_ = std::nullopt;
					profile_->selectorModule->selectedItemHover_ = std::nullopt;
				}
			}
		}

		if (gameApp_->isOpen())
			gameApp_->doWindowKeyDown((octoon::WindHandle)this->winId(), KeyCodetoInputKey(event->key()), 0, 0);
	}

	void
	ViewDock::keyReleaseEvent(QKeyEvent* event) noexcept
	{
		if (gameApp_->isOpen() && !event->isAutoRepeat())
			gameApp_->doWindowKeyUp((octoon::WindHandle)this->winId(), KeyCodetoInputKey(event->key()), 0, 0);
	}

	void
	ViewDock::dragEnterEvent(QDragEnterEvent* event) noexcept
	{
		if (!profile_->playerModule->isPlaying)
			event->acceptProposedAction();
	}

	void
	ViewDock::dragMoveEvent(QDragMoveEvent *e) noexcept
	{
		if (gameApp_->isOpen())
		{
			auto dpi = QApplication::primaryScreen()->devicePixelRatio();
			gameApp_->doWindowMouseMotion((octoon::WindHandle)this->winId(), e->pos().x() * dpi, e->pos().y() * dpi);
		}
	}

	void
	ViewDock::dropEvent(QDropEvent* event) noexcept
	{
		try
		{
			if (behaviour_ && !profile_->playerModule->isPlaying)
			{
				auto behaviour = behaviour_->getComponent<unreal::UnrealBehaviour>();
				if (behaviour)
				{
					auto urls = event->mimeData()->urls();
					if (!urls.isEmpty())
					{
						for (auto& it : urls)
						{
							std::filesystem::path path(it.toLocalFile().toStdWString());
							if (std::filesystem::is_directory(path))
								behaviour->open(path);
							else
								behaviour->load(path);
						}

						event->accept();
					}
					else
					{
						auto lightData = event->mimeData()->data("object/light");
						if (!lightData.isEmpty())
						{
							auto lightComponent = behaviour->getComponent<LightComponent>();

							if (!lightComponent->createLight((LightType)lightData.toInt()))
							{
								QMessageBox::critical(this, tr("Error"), tr("Unsupported light type."));
							}
						}
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			spdlog::error("Function dropEvent raised exception: " + std::string(e.what()));

			QCoreApplication::processEvents();
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void 
	ViewDock::paintEvent(QPaintEvent* e) noexcept
	{
	}

	QPaintEngine*
	ViewDock::paintEngine() const noexcept
	{
		return nullptr;
	}

	octoon::input::InputKey::Code
	ViewDock::KeyCodetoInputKey(int key) noexcept
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

	bool
	ViewDock::eventFilter(QObject* watched, QEvent* event)
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