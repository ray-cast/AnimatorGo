#ifndef UNREAL_UDOUBLESPINBOX_H_
#define UNREAL_UDOUBLESPINBOX_H_

#include <QDoubleSpinBox>

namespace unreal
{
	class UDoubleSpinBox : public QDoubleSpinBox
	{
		Q_OBJECT
	public:
		explicit UDoubleSpinBox(QWidget* parent = 0);
		~UDoubleSpinBox();

		void
		focusInEvent(QFocusEvent* event) override
		{
			this->grabKeyboard();
			QDoubleSpinBox::focusInEvent(event);
		}

		void
		focusOutEvent(QFocusEvent* event) override
		{
			this->releaseKeyboard();
			QDoubleSpinBox::focusOutEvent(event);
		}
	};
}

#endif