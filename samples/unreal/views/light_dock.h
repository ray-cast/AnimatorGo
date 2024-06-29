#ifndef UNREAL_LIGHT_DOCK_H_
#define UNREAL_LIGHT_DOCK_H_

#include <qwidget.h>
#include <qlayout.h>
#include <qscrollarea.h>
#include <qlistwidget.h>

#include "unreal_profile.h"
#include "../widgets/udockwidget.h"
#include "../widgets/color_dialog.h"

namespace unreal
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

	class LightDock final : public UDockWidget
	{
		Q_OBJECT
	public:
		LightDock(const std::shared_ptr<unreal::UnrealProfile>& profile);
		~LightDock();

		void showEvent(QShowEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;
		void closeEvent(QCloseEvent* event) override;

	public:
		LightListWindow* listWidget_;
		std::shared_ptr<unreal::UnrealProfile> profile_;

		QWidget* mainWidget_;
		QVBoxLayout* mainLayout_;
	};
}

#endif