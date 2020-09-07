#ifndef RABBIT_SUN_WINDOW_H_
#define RABBIT_SUN_WINDOW_H_

#include <qwidget.h>
#include <qlayout.h>
#include <qscrollarea.h>

#include "rabbit_profile.h"
#include "color_dialog.h"

namespace rabbit
{
	class SunWindow final : public QWidget
	{
		Q_OBJECT
	public:
		SunWindow(const std::shared_ptr<rabbit::RabbitProfile>& profile);
		~SunWindow();

		void repaint();

		virtual void showEvent(QShowEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;
		virtual void closeEvent(QCloseEvent* event) override;

	public Q_SLOTS:
		void currentColorChanged(QColor);
		void resetEvent();
		void closeEvent();
		void sliderIntensityEvent(int);
		void editIntensityEvent(double value);
		void sliderRotationXEvent(int);
		void editRotationXEvent(double value);
		void sliderRotationYEvent(int);
		void editRotationYEvent(double value);
		void sliderRotationZEvent(int);
		void editRotationZEvent(double value);

	private:
		std::unique_ptr<QWidget> scrollWidget_;
		std::unique_ptr<QScrollArea> scrollArea_;
		std::unique_ptr<QVBoxLayout> scrollLayout_;
		std::unique_ptr<QLabel> title_;
		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<ColorDialog> colorDialog_;
		std::unique_ptr<QDoubleSpinBox> editIntensity_;
		std::unique_ptr<QDoubleSpinBox> editRotationX_;
		std::unique_ptr<QDoubleSpinBox> editRotationY_;
		std::unique_ptr<QDoubleSpinBox> editRotationZ_;
		std::unique_ptr<QLabel> labelIntensity_;
		std::unique_ptr<QLabel> labelRotationX_;
		std::unique_ptr<QLabel> labelRotationY_;
		std::unique_ptr<QLabel> labelRotationZ_;
		std::unique_ptr<QSlider> sliderIntensity_;
		std::unique_ptr<QSlider> sliderRotationX_;
		std::unique_ptr<QSlider> sliderRotationY_;
		std::unique_ptr<QSlider> sliderRotationZ_;
		std::unique_ptr<QHBoxLayout> layout_;
		std::unique_ptr<QHBoxLayout> layoutIntensity_;
		std::unique_ptr<QHBoxLayout> layoutRotationX_;
		std::unique_ptr<QHBoxLayout> layoutRotationY_;
		std::unique_ptr<QHBoxLayout> layoutRotationZ_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
		std::unique_ptr<QToolButton> resetButton_;
		std::shared_ptr<rabbit::RabbitProfile> profile_;
	};
}

#endif