#ifndef FLOWER_LIGHT_WINDOW_H_
#define FLOWER_LIGHT_WINDOW_H_

#include <qwidget.h>
#include <qlayout.h>
#include <qscrollarea.h>
#include <qlistwidget.h>

#include "flower_profile.h"
#include "color_dialog.h"

namespace flower
{
	class LightListWindow final : public QListWidget
	{
		Q_OBJECT
	public:
		LightListWindow() noexcept(false);
		~LightListWindow() noexcept;

		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;

	private:
		QPoint startPos;
	};

	class LightWindow final : public QWidget
	{
		Q_OBJECT
	public:
		LightWindow(const std::shared_ptr<flower::FlowerProfile>& profile);
		~LightWindow();

		void repaint();

		virtual void showEvent(QShowEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;

	private Q_SLOTS:
		void closeEvent();
	
	public:
		QLabel* title_;
		QToolButton* closeButton_;
		QHBoxLayout* titleLayout_;
		LightListWindow* listWidget_;
		QVBoxLayout* mainLayout_;
	};
}

#endif