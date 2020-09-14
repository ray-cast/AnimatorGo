#ifndef RABBIT_MATERIAL_WINDOW_H_
#define RABBIT_MATERIAL_WINDOW_H_

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

#include <optional>

#include "rabbit_behaviour.h"
#include "color_dialog.h"
#include "spoiler.h"
#include <octoon/game_object.h>

namespace rabbit
{
	class MaterialEditWindow final : public QWidget
	{
		Q_OBJECT
	public:
		MaterialEditWindow(QWidget* widget, const octoon::GameObjectPtr& behaviour);
		~MaterialEditWindow();

		void repaint();
		void setMaterial(const std::shared_ptr<octoon::material::Material>& material);

	private:
		QWidget* createSummary();
		QWidget* createAlbedo();
		QWidget* createNormal();
		QWidget* createSmoothness();
		QWidget* createMetalness();
		QWidget* createSheen();
		QWidget* createClearCoat();
		QWidget* createSubsurface();
		QWidget* createEmissive();

	public Q_SLOTS:
		void albedoColorChanged(QColor);
		void emissiveColorChanged(QColor);

		void smoothEditEvent(double);
		void smoothSliderEvent(int);

		void metalEditEvent(double);
		void metalSliderEvent(int);
		
		void anisotropyEditEvent(double);
		void anisotropySliderEvent(int);

		void clearcoatEditEvent(double);
		void clearcoatSliderEvent(int);

		void clearcoatRoughnessEditEvent(double);
		void clearcoatRoughnessSliderEvent(int);

		void sheenEditEvent(double);
		void sheenSliderEvent(int);

		void subsurfaceEditEvent(double);
		void subsurfaceSliderEvent(int);

		void closeEvent();

	public:
		QLabel* imageLabel_;
		QLabel* textLabel_;
		QLabel* normalLabel_;
		QLabel* albedoLabel_;
		QLabel* smoothnessLabel_;
		QLabel* metalnessLabel_;
		QLabel* anisotropyLabel_;
		QLabel* clearcoatLabel_;
		QLabel* clearcoatRoughnessLabel_;
		QLabel* sheenLabel_;
		QLabel* subsurfaceLabel_;
		QSlider* smoothnessSlider_;
		QSlider* metalnessSlider_;
		QSlider* anisotropySlider_;
		QSlider* clearcoatSlider_;
		QSlider* clearcoatRoughnessSlider_;
		QSlider* sheenSlider_;
		QSlider* subsurfaceSlider_;
		QDoubleSpinBox* smoothnessSpinBox_;		
		QDoubleSpinBox* metalnessSpinBox_;
		QDoubleSpinBox* anisotropySpinBox_;
		QDoubleSpinBox* clearcoatSpinBox_;
		QDoubleSpinBox* clearcoatRoughnessSpinBox_;
		QDoubleSpinBox* sheenSpinBox_;
		QDoubleSpinBox* subsurfaceSpinBox_;
		QLabel* emissiveLabel_;
		ColorDialog* albedoColor_;
		ColorDialog* emissiveColor_;
		QToolButton* okButton_;
		std::shared_ptr<octoon::material::Material> material_;
		octoon::GameObjectPtr behaviour_;
	};

	class MaterialListWindow final : public QListWidget
	{
		Q_OBJECT
	public:
		MaterialListWindow() noexcept(false);
		~MaterialListWindow() noexcept;

		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);

	private:
		QPoint startPos;
	};

	class MaterialWindow final : public QWidget
	{
		Q_OBJECT
	public:
		MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept(false);
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
		QLabel* title_;
		QToolButton* closeButton_;
		QHBoxLayout* titleLayout_;
		QVBoxLayout* materialLayout_;
		QVBoxLayout* mainLayout_;
		QListWidget* listWidget_;
		MaterialEditWindow* modifyWidget_;
		QScrollArea* modifyMaterialArea_;
		octoon::GameObjectPtr behaviour_;
	};
}

#endif