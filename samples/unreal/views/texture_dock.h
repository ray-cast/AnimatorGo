#ifndef UNREAL_TEXTURE_DOCK_H_
#define UNREAL_TEXTURE_DOCK_H_

#include <qdockwidget.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qlistwidget.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <QVBoxLayout>

#include "unreal_profile.h"
#include "unreal_behaviour.h"

namespace unreal
{
	class TextureDock final : public QDockWidget
	{
		Q_OBJECT
	public:
		TextureDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept(false);
		~TextureDock() noexcept;

		void addItem(const nlohmann::json& package) noexcept;
		void updateItems() noexcept;

	public Q_SLOTS:
		void importClickEvent();

		void itemClicked(QListWidgetItem* item);
		void itemSelected(QListWidgetItem* item);

		void resizeEvent(QResizeEvent* e) noexcept override;
		void showEvent(QShowEvent* event) noexcept override;
		void keyPressEvent(QKeyEvent* event) noexcept override;
		bool eventFilter(QObject* watched, QEvent* event);

	public:
		QLabel* title_;

		QListWidget* listWidget_;
		QWidget* mainWidget_;
		QVBoxLayout* mainLayout_;

		QVBoxLayout* topLayout_;

		QListWidgetItem* clickedItem_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<unreal::UnrealProfile> profile_;
	};
}

#endif