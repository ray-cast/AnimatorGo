#ifndef UNREAL_MAIN_LIGHT_WINDOW_H_
#define UNREAL_MAIN_LIGHT_WINDOW_H_

#include <qwidget.h>
#include <qlayout.h>
#include <qscrollarea.h>

#include "unreal_profile.h"
#include "../widgets/color_dialog.h"
#include "../widgets/udockwidget.h"
#include "../widgets/udoublespinbox.h"

namespace unreal
{
	class MainLightDock final : public UDockWidget
	{
		Q_OBJECT
	public:
		MainLightDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<unreal::UnrealProfile>& profile);
		~MainLightDock();

		void showEvent(QShowEvent* event) override;
		void closeEvent(QCloseEvent* event) override;
		void paintEvent(QPaintEvent* e) noexcept override;

		bool eventFilter(QObject* watched, QEvent* event);

	public Q_SLOTS:
		void currentColorChanged(QColor);
		void resetEvent();
		void intensitySliderEvent(int);
		void intensityEditEvent(double value);
		void sizeSliderEvent(int);
		void sizeEditEvent(double value);
		void sliderRotationXEvent(int);
		void editRotationXEvent(double value);
		void sliderRotationYEvent(int);
		void editRotationYEvent(double value);
		void sliderRotationZEvent(int);
		void editRotationZEvent(double value);

	private:
		QWidget* scrollWidget_;
		QScrollArea* scrollArea_;
		QVBoxLayout* scrollLayout_;
		ColorDialog* colorDialog_;
		UDoubleSpinBox* sizeSpinBox_;
		UDoubleSpinBox* intensitySpinBox_;
		UDoubleSpinBox* editRotationX_;
		UDoubleSpinBox* editRotationY_;
		UDoubleSpinBox* editRotationZ_;
		QLabel* labelSize_;
		QLabel* labelIntensity_;
		QLabel* labelRotationX_;
		QLabel* labelRotationY_;
		QLabel* labelRotationZ_;
		QSlider* sizeSlider_;
		QSlider* intensitySlider_;
		QSlider* sliderRotationX_;
		QSlider* sliderRotationY_;
		QSlider* sliderRotationZ_;
		QHBoxLayout* layout_;
		QHBoxLayout* layoutSize_;
		QHBoxLayout* layoutIntensity_;
		QHBoxLayout* layoutRotationX_;
		QHBoxLayout* layoutRotationY_;
		QHBoxLayout* layoutRotationZ_;
		QVBoxLayout* mainLayout_;
		QToolButton* resetButton_;
		QWidget* mainWidget_;
		std::shared_ptr<unreal::UnrealProfile> profile_;
	};
}

#endif