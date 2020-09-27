#ifndef RABBIT_TOOL_WINDOW_H_
#define RABBIT_TOOL_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qlabel.h>
#include <qdialog.h>
#include <QtGui/qevent.h>
#include <qdockwidget.h>
#include "rabbit_profile.h"
#include "rabbit_behaviour.h"

namespace rabbit
{
	class ToolWindow final : public QWidget
	{
		Q_OBJECT
	public:
		ToolWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept;
		~ToolWindow() noexcept;

		void play();
		void stop();

	private Q_SLOTS:
		void hideEvent() noexcept;
		void importEvent() noexcept;
		void playEvent() noexcept;
		void resetEvent() noexcept;
		void leftEvent() noexcept;
		void rightEvent() noexcept;
		void recordEvent() noexcept;
		void shotEvent() noexcept;
		void gpuEvent() noexcept;
		void hdrEvent() noexcept;
		void cleanupEvent() noexcept;
		void sunEvent() noexcept;
		void materialEvent() noexcept;
		void environmentEvent() noexcept;

	Q_SIGNALS:
		void hideSignal();
		void importSignal();
		bool playSignal(bool enable);
		bool resetSignal();
		void leftSignal();
		void rightSignal();
		bool recordSignal(bool enable);
		void shotSignal();
		bool gpuSignal(bool enable);
		bool hdrSignal(bool enable);
		void cleanupSignal();
		void sunSignal();
		void materialSignal();
		void environmentSignal();

	private:
		void mousePressEvent(QMouseEvent* e) noexcept override;
		void mouseReleaseEvent(QMouseEvent* e) noexcept override;
		void mouseMoveEvent(QMouseEvent* e) noexcept override;
		void showEvent(QShowEvent* e) noexcept override;

	public:
		bool allowMove_;

		QSize size_;

		QPoint startPos_;
		QPoint clickPos_;

		bool gpuEnable_;
		bool playEnable_;
		bool recordEnable_;
		bool hdrEnable_;
		bool sunEnable_;
		bool environmentEnable_;

		QIcon playIcon_;
		QIcon playOnIcon_;
		QIcon leftIcon_;
		QIcon rightIcon_;
		QIcon resetIcon_;
		QIcon gpuIcon_;
		QIcon gpuOnIcon_;
		QIcon recordIcon_;
		QIcon recordOnIcon_;
		QIcon hdrIcon_;
		QIcon hdrOnIcon_;
		QIcon sunIcon_;
		QIcon sunOnIcon_;
		QIcon environmentIcon_;
		QIcon environmentOnIcon_;

		QToolButton hideButton;
		QToolButton importButton;
		QToolButton saveButton;
		QToolButton playButton;
		QToolButton resetButton;
		QToolButton leftButton;
		QToolButton rightButton;
		QToolButton recordButton;
		QToolButton shotButton;
		QToolButton gpuButton;
		QToolButton hdrButton;
		QToolButton cleanupButton;
		QToolButton sunButton;
		QToolButton environmentButton;
		QToolButton materialButton;

		octoon::GameObjectPtr behaviour_;
	};
}

#endif