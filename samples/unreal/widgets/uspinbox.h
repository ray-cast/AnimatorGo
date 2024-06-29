#ifndef UNREAL_USPINBOX_H_
#define UNREAL_USPINBOX_H_

#include <QSpinBox>

namespace unreal
{
	class SpinBox : public QSpinBox
	{
		Q_OBJECT
	public:
		explicit SpinBox(QWidget* parent = 0);
		~SpinBox();

		void
		focusInEvent(QFocusEvent* event) override
		{
			this->grabKeyboard();
			QSpinBox::focusInEvent(event);
		}

		void
		focusOutEvent(QFocusEvent* event) override
		{
			this->releaseKeyboard();
			QSpinBox::focusOutEvent(event);
		}
	};
}

#endif