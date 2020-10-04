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
		void rotationSliderEvent(int);
		void rotationEditEvent(double value);

	private:
		enum CreateFlags
		{
			SpoilerBit = 1 << 0,
			ColorBit = 1 << 1,
			ValueBit = 1 << 2,
		};

		struct MaterialUi
		{
			QToolButton* image;
			QCheckBox* check;
			QLabel* title;
			QLabel* path;
			QLabel* label_;
			QToolButton* color;
			QSlider* slider;
			QDoubleSpinBox* spinBox;
			QLabel* rotationLabel_;
			QSlider* rotationSlider;
			QDoubleSpinBox* rotationSpinBox;

			QHBoxLayout* titleLayout;
			QVBoxLayout* rightLayout;
			QHBoxLayout* mapLayout;
			QLayout* mainLayout;

			Spoiler* spoiler;

			octoon::hal::GraphicsTexturePtr texture;

			void init(const QString& name, std::uint32_t flags);
			void resetState();
			void setColor(const QColor& c, int w = 50, int h = 26);
		};

		QLabel* path;
		MaterialUi colorMap_;
		QLabel* title_;
		QLabel* imageLabel_;
		QToolButton* closeButton_;
		QVBoxLayout* mainLayout_;
		QToolButton* resetButton_;
		QColorDialog mapColor_;
		std::shared_ptr<QImage> image_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<rabbit::RabbitProfile> profile_;
	};
}

#endif