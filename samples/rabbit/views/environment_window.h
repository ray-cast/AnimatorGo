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

		void setColor(const QColor& c, int w = 50, int h = 26);

		virtual void showEvent(QShowEvent* event) override;
		virtual void closeEvent(QCloseEvent* event) override;

	public Q_SLOTS:
		void colorMapClickEvent();
		void colorMapCheckEvent(int state);
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
		QLabel* title_;
		QLabel* imageLabel_;
		QLabel* imageName_;

		QToolButton* thumbnail;
		QCheckBox* thumbnailToggle;
		QLabel* thumbnailTitle;
		QLabel* thumbnailPath;
		QLabel* intensityLabel_;
		QToolButton* color;
		QSlider* intensitySlider;
		QDoubleSpinBox* intensitySpinBox;
		QLabel* horizontalRotationLabel_;
		QSlider* horizontalRotationSlider;
		QDoubleSpinBox* horizontalRotationSpinBox;
		QLabel* verticalRotationLabel_;
		QSlider* verticalRotationSlider;
		QDoubleSpinBox* verticalRotationSpinBox;

		Spoiler* spoiler;

		QToolButton* closeButton_;
		QToolButton* resetButton_;
		QColorDialog colorSelector_;

		octoon::GameObjectPtr behaviour_;
		octoon::hal::GraphicsTexturePtr texture;

		std::shared_ptr<QImage> image_;
		std::shared_ptr<rabbit::RabbitProfile> profile_;
	};
}

#endif