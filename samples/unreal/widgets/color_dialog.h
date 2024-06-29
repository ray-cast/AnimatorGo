#ifndef UNREAL_COLOR_DIALOG_H_
#define UNREAL_COLOR_DIALOG_H_

#include <QtGui/qevent.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <qdockwidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qstyle>
#include <qtoolbutton.h>
#include <qwidget>

#include "uspinbox.h"

namespace unreal
{
	class ColorPlane : public QLabel
	{
		Q_OBJECT
	public:
		explicit ColorPlane(QWidget* parent = nullptr);

		void setCurrentColor(QColor color);
		QColor getCurrentColor() const;

		void paintHue(std::uint32_t w, std::uint32_t h) noexcept;
		void paintHueSelector(std::uint32_t w, std::uint32_t h) noexcept;
		void paintRectangle(std::uint32_t w, std::uint32_t h, float hue, float value) noexcept;

	protected:
		void mouseReleaseEvent(QMouseEvent* ev);
		void mousePressEvent(QMouseEvent* ev);
		void mouseDoubleClickEvent(QMouseEvent* ev);
		void mouseMoveEvent(QMouseEvent* ev);
		void paintEvent(QPaintEvent* event);

	Q_SIGNALS:
		void currentColorChanged(QColor);

	private:
		QImage hueImage_;
		QImage hueSelectorImage_;
		QImage colorImage_;
		bool mousePress_;
		bool hueSelected_;
		bool boxSelected_;
		float hue_;
		float value_;
		std::uint32_t hueWidth_;
		std::uint32_t hueHeight_;
		std::uint32_t hueCircleWidth_;
		std::uint32_t hueCircleHeight_;
		std::uint32_t rectangleWidth_;
		std::uint32_t rectangleHeight_;
		std::uint32_t boxMargin_;
		QColor currentColor_;
	};

	class ColorDialog final : public QWidget
	{
		Q_OBJECT
	public:
		ColorDialog();
		~ColorDialog();

		void setCurrentColor(QColor);
		QColor getCurrentColor() const;

	private Q_SLOTS:
		void currentColorChangedEvent(QColor);
		void sliderHEvent(int);
		void sliderSEvent(int);
		void sliderVEvent(int);
		void rejectedEvent();

	Q_SIGNALS:
		void currentColorChanged(QColor);
		void colorSelected(QColor);
		void rejected();

	private:
		bool currentColorChangedEvent_;

		octoon::math::float3 hsv_;
		octoon::math::float3 rgb_;

		std::unique_ptr<ColorPlane> label_;
		std::unique_ptr<QSpinBox> editH_;
		std::unique_ptr<QSpinBox> editS_;
		std::unique_ptr<QSpinBox> editV_;
		std::unique_ptr<QLabel> labelH_;
		std::unique_ptr<QLabel> labelS_;
		std::unique_ptr<QLabel> labelV_;
		std::unique_ptr<QSlider> sliderH_;
		std::unique_ptr<QSlider> sliderS_;
		std::unique_ptr<QSlider> sliderV_;
		std::unique_ptr<QHBoxLayout> layoutH_;
		std::unique_ptr<QHBoxLayout> layoutS_;
		std::unique_ptr<QHBoxLayout> layoutV_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
	};
}

#endif