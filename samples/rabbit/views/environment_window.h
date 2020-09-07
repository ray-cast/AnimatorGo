#ifndef RABBIT_ENVIRONMENT_WINDOW_H_
#define RABBIT_ENVIRONMENT_WINDOW_H_

#include <qwidget.h>
#include "color_dialog.h"
#include "rabbit_profile.h"

namespace rabbit
{
   	class EnvironmentWindow final : public QWidget
	{
		Q_OBJECT
	public:
		EnvironmentWindow(QWidget* widget, const std::shared_ptr<rabbit::RabbitProfile>& profile);
		~EnvironmentWindow();

		void repaint();

		virtual void showEvent(QShowEvent* event) override;
		virtual void closeEvent(QCloseEvent* event) override;

	public Q_SLOTS:
		void currentColorChanged(QColor);
		void closeEvent();
		void resetEvent();
		void sliderIntensityEvent(int);
		void editIntensityEvent(double value);

	private:
		std::unique_ptr<QLabel> title_;
		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<ColorDialog> colorDialog_;
		std::unique_ptr<QDoubleSpinBox> editIntensity_;
		std::unique_ptr<QLabel> labelIntensity_;
		std::unique_ptr<QSlider> sliderIntensity_;
		std::unique_ptr<QHBoxLayout> layout_;
		std::unique_ptr<QHBoxLayout> layoutIntensity_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
		std::unique_ptr<QToolButton> resetButton_;
		std::shared_ptr<rabbit::RabbitProfile> profile_;
	};
}

#endif