#ifndef RABBIT_MATERIAL_WINDOW_H_
#define RABBIT_MATERIAL_WINDOW_H_

#include <qwidget>
#include <qdialog.h>
#include <qboxlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <optional>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlistwidget.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <QParallelAnimationGroup>
#include <QScrollArea>

#include "rabbit_behaviour.h"
#include "../libs/tinyobj/tiny_obj_loader.h"
#include "./color_dialog.h"
#include <octoon/game_object.h>

namespace rabbit
{
	class Spoiler : public QWidget {
		Q_OBJECT
	private:
		QGridLayout mainLayout;
		QToolButton toggleButton;
		QFrame headerLine;
		QParallelAnimationGroup toggleAnimation;
		QScrollArea contentArea;
		int animationDuration{ 300 };
	public:
		explicit Spoiler(const QString& title = "", const int animationDuration = 200, QWidget* parent = 0);
		void setContentLayout(QLayout& contentLayout);
	};

	class MaterialModifyWindow final : public QWidget
	{
		Q_OBJECT
	public:
		MaterialModifyWindow(QWidget* widget);
		~MaterialModifyWindow();

		void repaint();

		virtual void showEvent(QShowEvent* event) override;

	public Q_SLOTS:
		void colorSelected(QColor);

	private:
		QLabel* imageLabel_;
		QLabel* textLabel_;
		ColorDialog* albedoColor_;
		ColorDialog* normalColor_;
		ColorDialog* smoothnessColor_;
		ColorDialog* metalnessColor_;
		ColorDialog* emissiveColor_;
	};

	class MaterialWindow final : public QWidget
	{
		Q_OBJECT
	public:
		MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept;
		~MaterialWindow() noexcept;

		void showEvent(QShowEvent* event) noexcept override;

		void dragEnterEvent(QDragEnterEvent* event) noexcept override;
		void dropEvent(QDropEvent* event) noexcept override;

		std::string currentItem() const noexcept;

		void updateList();

	private Q_SLOTS:
		void closeEvent();
		void itemClicked(QListWidgetItem* item);
		void itemDoubleClicked(QListWidgetItem* item);

	private:
		std::unique_ptr<QLabel> title_;
		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<QHBoxLayout> titleLayout_;
		std::unique_ptr<QVBoxLayout> materialLayout_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
		std::unique_ptr<QListWidget> listWidget_;
		std::unique_ptr<MaterialModifyWindow> modifyWidget_;
		QScrollArea* modifyMaterialArea_;
		octoon::GameObjectPtr behaviour_;
	};
}

#endif