#ifndef RABBIT_ENVIRONMENT_WINDOW_H_
#define RABBIT_ENVIRONMENT_WINDOW_H_

#include <qwidget.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include "spoiler.h"
#include "color_dialog.h"
#include "rabbit_profile.h"
#include "rabbit_behaviour.h"
#include <octoon/hal/graphics.h>

namespace rabbit
{
   	class EnvironmentWindow final : public QWidget
	{
		Q_OBJECT
	public:
		EnvironmentWindow(QWidget* widget, const octoon::GameObjectPtr& behaviour, const std::shared_ptr<rabbit::RabbitProfile>& profile);
		~EnvironmentWindow();

		void repaint();

		void showEvent(QShowEvent* event) override;
		void closeEvent(QCloseEvent* event) override;

	public Q_SLOTS:
		void colorMapClickEvent();
		void colorMapCheckEvent(int state);
		void backgroundMapCheckEvent(int state);
		void colorClickEvent();
		void colorChangeEvent(const QColor&);
		void closeEvent();
		void resetEvent();
		void intensitySliderEvent(int);
		void intensityEditEvent(double value);
		void horizontalRotationSliderEvent(int);
		void horizontalRotationEditEvent(double value);
		void verticalRotationSliderEvent(int);
		void verticalRotationEditEvent(double value);

	private:
		void setColor(const QColor& c, int w = 50, int h = 26);
		
	private:
		QLabel* title_;
		QLabel* imageLabel_;
		QLabel* imageName_;
		QLabel* thumbnailPath;
		QLabel* intensityLabel_;
		QLabel* horizontalRotationLabel_;
		QLabel* verticalRotationLabel_;

		QToolButton* colorButton;
		QToolButton* thumbnail;
		QToolButton* closeButton_;
		QToolButton* resetButton_;

		QCheckBox* thumbnailToggle;
		QCheckBox* backgroundToggle;

		QSlider* intensitySlider;
		QSlider* horizontalRotationSlider;
		QSlider* verticalRotationSlider;

		QDoubleSpinBox* intensitySpinBox;
		QDoubleSpinBox* horizontalRotationSpinBox;
		QDoubleSpinBox* verticalRotationSpinBox;

		QColorDialog colorSelector_;

		Spoiler* spoiler;

		octoon::GameObjectPtr behaviour_;
		octoon::hal::GraphicsTexturePtr texture;

		std::shared_ptr<QImage> image_;
		std::shared_ptr<rabbit::RabbitProfile> profile_;
	};
}

#endif