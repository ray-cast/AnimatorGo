#ifndef UNREAL_MATERIAL_WINDOW_H_
#define UNREAL_MATERIAL_WINDOW_H_

#include <qwidget>
#include <qdialog.h>
#include <qboxlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlistwidget.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <qcolordialog.h>
#include <optional>
#include <octoon/game_object.h>

#include "unreal_behaviour.h"
#include "material_edit_dock.h"

#include "../widgets/spoiler.h"
#include "../widgets/color_dialog.h"

namespace unreal
{
	class MaterialDock final : public QDockWidget
	{
		Q_OBJECT
	public:
		MaterialDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept(false);
		~MaterialDock() noexcept;

		void addItem(const nlohmann::json& package) noexcept(false);
		void updateItemList() noexcept;

	private:
		void showEvent(QShowEvent* e) noexcept override;
		void resizeEvent(QResizeEvent* e) noexcept override;
		void keyPressEvent(QKeyEvent* event) noexcept;

	private Q_SLOTS:
		void importClickEvent();
		void itemClicked(QListWidgetItem* item);
		void itemSelected(QListWidgetItem* item);
		bool eventFilter(QObject* watched, QEvent* event);

	private:
		QLabel* title_;
		QVBoxLayout* topLayout_;
		QVBoxLayout* materialLayout_;
		QVBoxLayout* mainLayout_;
		QListWidget* listWidget_;
		QListWidgetItem* clickedItem_;
		QScrollArea* modifyMaterialArea_;
		QWidget* mainWidget_;
		QListWidgetItem* selectedItem_;
		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<UnrealProfile> profile_;
	};
}

#endif