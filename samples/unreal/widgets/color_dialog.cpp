#include "color_dialog.h"
#include <qevent.h>
#include <qpainter.h>
#include <qapplication.h>

namespace unreal
{
	ColorPlane::ColorPlane(QWidget* parent)
		: QLabel(parent)
		, mousePress_(false)
		, hueSelected_(false)
		, boxSelected_(false)
		, hueWidth_(0)
		, hueHeight_(0)
		, hueCircleWidth_(0)
		, hueCircleHeight_(0)
		, rectangleWidth_(0)
		, rectangleHeight_(0)
		, boxMargin_(42)
		, currentColor_(255, 255, 255)
	{
		this->setObjectName("ColorPlane");
	}

	void
	ColorPlane::mouseReleaseEvent(QMouseEvent* ev)
	{
		hueSelected_ = false;
		boxSelected_ = false;
		mousePress_ = false;
	}

	void
	ColorPlane::mousePressEvent(QMouseEvent* ev)
	{
		mousePress_ = true;

		auto mouseX_ = ev->pos().x();
		auto mouseY_ = ev->pos().y();

		if (mouseX_ > boxMargin_ && mouseY_ > boxMargin_ && mouseX_ < this->width() - boxMargin_ && mouseY_ < this->height() - boxMargin_)
		{
			boxSelected_ = true;
		}
		else
		{
			float u = float(mouseX_) / this->width() * 2.0f - 1.0f;
			float v = float(mouseY_) / this->height() * 2.0f - 1.0f;

			float length = octoon::math::length(octoon::math::float2(u, v));
			if (length <= 1.0f && length >= 0.85f)
				hueSelected_ = true;
		}

		this->mouseMoveEvent(ev);
	}

	void
	ColorPlane::mouseDoubleClickEvent(QMouseEvent* ev)
	{
	}

	void
	ColorPlane::mouseMoveEvent(QMouseEvent* ev)
	{
		if (mousePress_)
		{
			auto mouseX_ = ev->pos().x();
			auto mouseY_ = ev->pos().y();

			if (boxSelected_)
			{
				float s = mouseX_ - (int)boxMargin_;
				float v = mouseY_ - (int)boxMargin_;

				s /= (this->width() - boxMargin_ * 2);
				v /= (this->height() - boxMargin_ * 2);
				s = std::clamp(s, 0.0f, 1.0f);
				v = std::clamp(v, 0.0f, 1.0f);

				auto hsv = currentColor_.toHsv();
				this->setCurrentColor(QColor::fromHsvF(hsv.hsvHueF(), s, 1.0f - v));
			}

			if (hueSelected_)
			{
				float u = float(mouseX_) / this->width() * 2.0f - 1.0f;
				float v = float(mouseY_) / this->height() * 2.0f - 1.0f;
				float angle = atan2(u, v) / octoon::math::PI;

				auto h = std::clamp(angle * 0.5f + 0.5f, 0.0f, 1.0f);
				auto hsv = currentColor_.toHsv();
				this->setCurrentColor(QColor::fromHsvF(h, hsv.hsvSaturationF(), hsv.valueF()));
			}
		}
	}

	void
	ColorPlane::setCurrentColor(QColor color)
	{
		currentColor_ = color;
		emit currentColorChanged(currentColor_);
		this->repaint();
	}

	QColor
	ColorPlane::getCurrentColor() const
	{
		return currentColor_;
	}

	void
	ColorPlane::paintHue(std::uint32_t width, std::uint32_t height) noexcept
	{
		if (hueWidth_ != width || hueHeight_ != height)
		{
			if (hueImage_.width() != width || hueImage_.height() != height)
				hueImage_ = QImage(width, height, QImage::Format_RGBA8888);

			for (std::uint32_t y = 0; y < height; y++)
			{
				for (std::uint32_t x = 0; x < width; x++)
				{
					float u = float(x) / (width - 1) * 2.0f - 1.0f;
					float v = float(y) / (height - 1) * 2.0f - 1.0f;

					float angle = std::atan2(u, v) / octoon::math::PI;
					float length = octoon::math::length(octoon::math::float2(u, v));
					float h = std::clamp(angle * 0.5f + 0.5f, 0.0f, 1.0f);

					if (length <= 0.99f && length >= 0.85f)
					{
						hueImage_.setPixelColor(x, y, QColor::fromHsvF(h, 1.0f, 1.0f));
					}
					else if (length >= 0.99f)
					{
						auto smooth = octoon::math::smoothlerp(1.0f, 0.0f, (length - 0.99f) * 50.0f);

						auto rgb = QColor::fromHsvF(h, 1.0f, 1.0f);
						rgb.setRed(octoon::math::lerp<float>(250, rgb.red(), smooth));
						rgb.setGreen(octoon::math::lerp<float>(250, rgb.green(), smooth));
						rgb.setBlue(octoon::math::lerp<float>(250, rgb.blue(), smooth));

						hueImage_.setPixelColor(x, y, rgb);
					}
					else if (length <= 0.85f)
					{
						auto smooth = octoon::math::smoothlerp(1.0f, 0.0f, (0.85f - length) * 50.0f);

						auto rgb = QColor::fromHsvF(h, 1.0f, 1.0f);
						rgb.setRed(octoon::math::lerp<float>(250, rgb.red(), smooth));
						rgb.setGreen(octoon::math::lerp<float>(250, rgb.green(), smooth));
						rgb.setBlue(octoon::math::lerp<float>(250, rgb.blue(), smooth));

						hueImage_.setPixelColor(x, y, rgb);
					}
				}
			}

			hueWidth_ = width;
			hueHeight_ = height;
		}
	}

	void
	ColorPlane::paintHueSelector(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (hueCircleWidth_ != w || hueCircleHeight_ != h)
		{
			if (hueSelectorImage_.width() != w || hueSelectorImage_.height() != h)
				hueSelectorImage_ = QImage(w, h, QImage::Format_RGBA8888);

			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					float u = float(x) / (w - 1) * 2.0f - 1.0f;
					float v = float(y) / (h - 1) * 2.0f - 1.0f;

					float length = octoon::math::length(octoon::math::float2(u, v));
					if (length >= 0.97f)
					{
						auto smooth = octoon::math::smoothlerp(1.0f, 0.0f, (length - 0.97f) * 5.0f);
						hueSelectorImage_.setPixelColor(x, y, QColor::fromRgbF(1.0f, 1.0f, 1.0f, smooth));
					}
					else if (length <= 0.84f)
					{
						auto smooth = octoon::math::smoothlerp(1.0f, 0.0f, (0.84f - length) * 5.0f);
						hueSelectorImage_.setPixelColor(x, y, QColor::fromRgbF(1.0f, 1.0f, 1.0f, smooth));
					}
					else
					{
						hueSelectorImage_.setPixelColor(x, y, QColor::fromRgbF(1.0f, 1.0f, 1.0f));
					}
				}
			}

			hueCircleWidth_ = w;
			hueCircleHeight_ = h;
		}
	}

	void
	ColorPlane::paintRectangle(std::uint32_t width, std::uint32_t height, float hue, float value) noexcept
	{
		if (rectangleWidth_ != width || rectangleHeight_ != height || hue_ != hue || value_ != value)
		{
			if (colorImage_.width() != width || colorImage_.height() != height)
				colorImage_ = QImage(width, height, QImage::Format_RGB888);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					float u = float(x) / (width - 1);
					float v = float(y) / (height - 1);
					colorImage_.setPixelColor(x, y, QColor::fromHsvF(hue, u, std::clamp(1.0f - v, 0.0f, 1.0f)));
				}
			}

			hue_ = hue;
			value_ = value;
			rectangleWidth_ = width;
			rectangleHeight_ = height;
		}
	}

	void
	ColorPlane::paintEvent(QPaintEvent* event)
	{
		auto painter = QPainter(this);
		auto dpi = QApplication::primaryScreen()->devicePixelRatio();

		auto width = painter.window().width();
		auto height = painter.window().height();
		auto hueWidth = 18;
		auto hueHeight = 18;

		this->paintHue(width * dpi, height * dpi);
		this->paintHueSelector(hueWidth * dpi, hueHeight * dpi);
		this->paintRectangle(width - boxMargin_ * 2, height - boxMargin_ * 2, currentColor_.toHsv().hsvHueF(), currentColor_.toHsv().valueF());
		
		auto hsv = currentColor_.toHsv();
		auto s = hsv.hsvSaturationF();
		auto value = 1.0f - hsv.valueF();
		auto angle = 360.f - hsv.hsvHue();
		auto angle_x = -90 + angle;

		auto mouseHueX = (0.915f * std::cos(octoon::math::radians(angle_x)) * 0.5 + 0.5) * width - hueWidth / 2;
		auto mouseHueY = (0.915f * -std::cos(octoon::math::radians(angle)) * 0.5 + 0.5) * height - hueHeight / 2;

		painter.drawImage(QRect(0, 0, width, height), hueImage_);
		painter.drawImage(QRect(mouseHueX, mouseHueY, hueWidth, hueHeight), hueSelectorImage_);
		painter.drawImage(boxMargin_, boxMargin_, colorImage_);

		auto mouseX = boxMargin_ + s * (width - 84);
		auto mouseY = boxMargin_ + value * (height - 84);

		for (int i = -5; i <= 5; i++)
			painter.drawPoint(std::clamp<int>(mouseX + i, 0, width), mouseY);

		for (int i = -5; i <= 5; i++)
			painter.drawPoint(mouseX, std::clamp<int>(mouseY + i, 0, width));

		QLabel::paintEvent(event);
	}

	ColorDialog::ColorDialog()
	{
		this->setObjectName("ColorDialog");

		label_ = std::make_unique<ColorPlane>();
		label_->setMinimumSize(200, 200);

		labelH_ = std::make_unique<QLabel>();
		labelH_->setText(tr("Hue"));
		labelS_ = std::make_unique<QLabel>();
		labelS_->setText(tr("Saturation"));
		labelV_ = std::make_unique<QLabel>();
		labelV_->setText(tr("Value"));

		editH_ = std::make_unique<QSpinBox>();
		editH_->setFixedWidth(48);
		editH_->setMaximum(360);
		editH_->setAlignment(Qt::AlignRight);
		editS_ = std::make_unique<QSpinBox>();
		editS_->setFixedWidth(48);
		editS_->setMaximum(255);
		editS_->setAlignment(Qt::AlignRight);
		editV_ = std::make_unique<QSpinBox>();
		editV_->setFixedWidth(48);
		editV_->setMaximum(255);
		editV_->setAlignment(Qt::AlignRight);

		sliderH_ = std::make_unique<QSlider>();
		sliderH_->setObjectName("H");
		sliderH_->setOrientation(Qt::Horizontal);
		sliderH_->setMinimum(0);
		sliderH_->setMaximum(360);
		sliderH_->setFixedWidth(200);

		sliderS_ = std::make_unique<QSlider>();
		sliderS_->setObjectName("S");
		sliderS_->setOrientation(Qt::Horizontal);
		sliderS_->setMinimum(0);
		sliderS_->setMaximum(255);
		sliderS_->setFixedWidth(200);

		sliderV_ = std::make_unique<QSlider>();
		sliderV_->setObjectName("Value");
		sliderV_->setOrientation(Qt::Horizontal);
		sliderV_->setMinimum(0);
		sliderV_->setMaximum(255);
		sliderV_->setFixedWidth(200);

		layoutH_ = std::make_unique<QHBoxLayout>();
		layoutH_->addWidget(labelH_.get(), 0, Qt::AlignLeft);
		layoutH_->addWidget(editH_.get(), 0, Qt::AlignRight);
		layoutH_->setContentsMargins(0, 4, 0, 0);

		layoutS_ = std::make_unique<QHBoxLayout>();
		layoutS_->addWidget(labelS_.get(), 0, Qt::AlignLeft);
		layoutS_->addWidget(editS_.get(), 0, Qt::AlignRight);
		layoutS_->setContentsMargins(0, 4, 0, 0);

		layoutV_ = std::make_unique<QHBoxLayout>();
		layoutV_->addWidget(labelV_.get(), 0, Qt::AlignLeft);
		layoutV_->addWidget(editV_.get(), 0, Qt::AlignRight);
		layoutV_->setContentsMargins(0, 4, 0, 0);

		mainLayout_ = std::make_unique<QVBoxLayout>(this);
		mainLayout_->addWidget(label_.get(), 0, Qt::AlignHCenter | Qt::AlignTop);
		mainLayout_->addLayout(layoutH_.get(), 0);
		mainLayout_->addWidget(sliderH_.get(), 0, Qt::AlignHCenter);
		mainLayout_->addLayout(layoutS_.get(), 0);
		mainLayout_->addWidget(sliderS_.get(), 0, Qt::AlignHCenter);
		mainLayout_->addLayout(layoutV_.get(), 0);
		mainLayout_->addWidget(sliderV_.get(), 0, Qt::AlignHCenter);
		mainLayout_->setContentsMargins(0, 0, 0, 0);

		this->connect(sliderH_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderHEvent(int)));
		this->connect(sliderS_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderSEvent(int)));
		this->connect(sliderV_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderVEvent(int)));
		this->connect(editH_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderHEvent(int)));
		this->connect(editS_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderSEvent(int)));
		this->connect(editV_.get(), SIGNAL(valueChanged(int)), this, SLOT(sliderVEvent(int)));
		this->connect(label_.get(), SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChangedEvent(QColor)));
	}

	ColorDialog::~ColorDialog()
	{
		labelH_.reset();
		labelS_.reset();
		labelV_.reset();
		editH_.reset();
		editS_.reset();
		editV_.reset();
		layoutH_.reset();
		layoutS_.reset();
		layoutV_.reset();
	}

	void
	ColorDialog::setCurrentColor(QColor color)
	{
		label_->setCurrentColor(color);
	}

	QColor
	ColorDialog::getCurrentColor() const
	{
		return label_->getCurrentColor();
	}

	void
	ColorDialog::rejectedEvent()
	{
		emit rejected();
	}

	void
	ColorDialog::currentColorChangedEvent(QColor color)
	{
		currentColorChangedEvent_ = true;
		auto hsv = color.toHsv();
		sliderH_->setValue(hsv.hsvHue());
		sliderS_->setValue(hsv.hsvSaturation());
		sliderV_->setValue(hsv.value());
		editH_->setValue(hsv.hsvHue());
		editS_->setValue(hsv.hsvSaturation());
		editV_->setValue(hsv.value());
		currentColorChangedEvent_ = false;
		emit currentColorChanged(color);
	}

	void
	ColorDialog::sliderHEvent(int value)
	{
		if (!currentColorChangedEvent_)
		{
			auto hsv = label_->getCurrentColor().toHsv();
			auto rgb = QColor::fromHsvF(value / float(sliderH_->maximum()), hsv.hsvSaturationF(), hsv.valueF());
			label_->setCurrentColor(rgb);
		}
	}

	void
	ColorDialog::sliderSEvent(int value)
	{
		if (!currentColorChangedEvent_)
		{
			auto hsv = label_->getCurrentColor().toHsv();
			auto rgb = QColor::fromHsvF(hsv.hsvHueF(), value / float(sliderS_->maximum()), hsv.valueF());
			label_->setCurrentColor(rgb);
		}
	}

	void
	ColorDialog::sliderVEvent(int value)
	{
		if (!currentColorChangedEvent_)
		{
			auto hsv = label_->getCurrentColor().toHsv();
			auto rgb = QColor::fromHsvF(hsv.hsvHueF(), hsv.hsvSaturationF(), value / float(sliderV_->maximum()));
			label_->setCurrentColor(rgb);
		}
	}
}