#ifndef FLOWER_TOOL_WINDOW_H_
#define FLOWER_TOOL_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qscrollarea.h>
#include <QtGui/qevent.h>
#include <qdockwidget.h>
#include "flower_profile.h"
#include "flower_behaviour.h"

namespace flower
{
	class ToolWindow final : public QWidget
	{
		Q_OBJECT
	public:
		ToolWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour, std::shared_ptr<FlowerProfile> profile) noexcept;
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
		void audioEvent() noexcept;
		void cleanupEvent() noexcept;
		void lightEvent() noexcept;
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
		bool audioSignal(bool enable);
		void cleanupSignal();
		void lightSignal();
		void sunSignal();
		void materialSignal();
		void environmentSignal();

	private:
		void paintEvent(QPaintEvent* e) noexcept override;
		void mousePressEvent(QMouseEvent* e) noexcept override;
		void mouseReleaseEvent(QMouseEvent* e) noexcept override;
		void mouseMoveEvent(QMouseEvent* e) noexcept override;
		void showEvent(QShowEvent* e) noexcept override;
		void resizeEvent(QResizeEvent* e) noexcept override;

	public:
		bool allowMove_;

		QSize size_;

		QPoint startPos_;
		QPoint clickPos_;

		bool gpuEnable_;
		bool playEnable_;
		bool recordEnable_;
		bool audioEnable_;
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
		QIcon audioIcon_;
		QIcon audioOnIcon_;
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
		QToolButton audioButton;
		QToolButton cleanupButton;
		QToolButton lightButton;
		QToolButton sunButton;
		QToolButton environmentButton;
		QToolButton materialButton;

		QWidget* contentWidget;
		QScrollArea* contentWidgetArea;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<flower::FlowerProfile> profile_;
	};
}

#endif