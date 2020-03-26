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

		void setMaterial(const std::shared_ptr<octoon::material::Material>& material);

	private:
		QWidget* createSummary();
		QWidget* createAlbedo();
		QWidget* createNormal();
		QWidget* createSmoothness();
		QWidget* createMetalness();
		QWidget* createEmissive();

	public Q_SLOTS:
		void albedoColorChanged(QColor);
		void emissiveColorChanged(QColor);

		void smoothEditEvent(double);
		void smoothSliderEvent(int);

		void metalEditEvent(double);
		void metalSliderEvent(int);

		void closeEvent();

	public:
		QLabel* imageLabel_;
		QLabel* textLabel_;
		QLabel* normalLabel_;
		QLabel* albedoLabel_;
		ColorDialog* albedoColor_;
		QSlider* smoothnessSlider_;
		QLabel* smoothnessLabel_;
		QDoubleSpinBox* smoothnessSpinBox_;
		QSlider* metalnessSlider_;
		QLabel* metalnessLabel_;
		QDoubleSpinBox* metalnessSpinBox_;
		QLabel* emissiveLabel_;
		ColorDialog* emissiveColor_;
		QToolButton* okButton_;
		std::shared_ptr<octoon::material::Material> material_;
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
		void okEvent();
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