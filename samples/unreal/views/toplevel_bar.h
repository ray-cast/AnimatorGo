#ifndef UNREAL_TOPLEVEL_WINDOW_H_
#define UNREAL_TOPLEVEL_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qdockwidget.h>
#include <qtoolButton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <QtGui/qevent.h>
#include <qtoolbar.h>
#include <QSlider>

#include "unreal_profile.h"
#include "unreal_behaviour.h"

namespace unreal
{
	class ToplevelBar final : public QToolBar
	{
		Q_OBJECT
	public:
		ToplevelBar(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<unreal::UnrealProfile>& profile) noexcept;
		~ToplevelBar() noexcept;

		void update() noexcept;
		bool eventFilter(QObject* watched, QEvent* event);

	public Q_SLOTS:
		void openEvent() noexcept;
		void importEvent() noexcept;
		void saveEvent() noexcept;
		void shotEvent() noexcept;
		void gpuEvent() noexcept;
		void audioEvent() noexcept;
		void cleanupEvent() noexcept;
		void playEvent() noexcept;
		void resetEvent() noexcept;
		void leftEvent() noexcept;
		void rightEvent() noexcept;
		void volumeEvent() noexcept;
		void volumeSliderEvent(int value);

	public:
		bool playEnable_;
		bool volumeEnable_;

		QIcon playIcon_;
		QIcon playOnIcon_;
		QIcon leftIcon_;
		QIcon rightIcon_;
		QIcon resetIcon_;
		QIcon volumeOnIcon_;
		QIcon volumeOffIcon_;

		bool gpuEnable_;
		bool audioEnable_;
		bool hdrEnable_;

		QIcon gpuIcon_;
		QIcon gpuOnIcon_;
		QIcon audioIcon_;
		QIcon audioOnIcon_;

		QToolButton* openButton_;
		QToolButton* saveButton_;
		QToolButton* loadButton_;
		QToolButton* shotButton_;
		QToolButton* gpuButton_;
		QToolButton* audioButton_;
		QToolButton* cleanupButton_;
		QToolButton* playButton_;
		QToolButton* resetButton_;
		QToolButton* leftButton_;
		QToolButton* rightButton_;
		QToolButton* volumeButton_;

		QSlider* volumeSlider_;
		QHBoxLayout* layout_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<unreal::UnrealProfile> profile_;
	};
}

#endif