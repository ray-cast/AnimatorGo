#ifndef UNREAL_ASSET_BROWSE_DIALOG_H_
#define UNREAL_ASSET_BROWSE_DIALOG_H_

#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qdockwidget.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qspinbox.h>

#include "../widgets/udockwidget.h"
#include "../widgets/spoiler.h"
#include "../widgets/udoublespinbox.h"

#include "unreal_behaviour.h"
#include "unreal_profile.h"

namespace unreal
{
	class MaterialListDialog final : public QDialog
	{
		Q_OBJECT
	public:
		MaterialListDialog(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept(false);
		~MaterialListDialog() noexcept;

		void addItem(const nlohmann::json& package) noexcept(false);

	private:
		void resizeEvent(QResizeEvent* e) noexcept override;
		void showEvent(QShowEvent* event) noexcept override;
		void keyPressEvent(QKeyEvent* event) noexcept;

	public Q_SLOTS:
		void okClickEvent();
		void closeClickEvent();
		void importClickEvent();
		void itemClicked(QListWidgetItem* item);
		void itemDoubleClicked(QListWidgetItem* item);

	Q_SIGNALS:
		void itemSelected(QListWidgetItem* item);

	public:
		QListWidget* mainWidget_;
		QVBoxLayout* mainLayout_;

		QToolButton* okButton_;
		QToolButton* closeButton_;
		QToolButton* importButton_;

		QListWidgetItem* clickedItem_;

		octoon::GameObjectPtr behaviour_;
	};

	class EnvironmentListDialog final : public QDialog
	{
		Q_OBJECT
	public:
		EnvironmentListDialog(QWidget* parent, const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept(false);
		~EnvironmentListDialog() noexcept;

		void addItem(const nlohmann::json& package) noexcept(false);

	private:
		void resizeEvent(QResizeEvent* e) noexcept override;
		void showEvent(QShowEvent* event) noexcept override;
		void keyPressEvent(QKeyEvent* event) noexcept override;

	public Q_SLOTS:
		void okClickEvent();
		void closeClickEvent();
		void importClickEvent();

		void itemClicked(QListWidgetItem* item);
		void itemDoubleClicked(QListWidgetItem* item);

	Q_SIGNALS:
		void itemSelected(QListWidgetItem* item);

	public:
		QListWidget* listWidget_;
		QVBoxLayout* mainLayout_;

		QToolButton* okButton_;
		QToolButton* closeButton_;
		QToolButton* importButton_;

		QListWidgetItem* clickedItem_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<unreal::UnrealProfile> profile_;
	};
}

#endif