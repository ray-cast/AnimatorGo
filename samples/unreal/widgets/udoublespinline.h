#ifndef UNREAL_UDOUBLESPINLINE_H_
#define UNREAL_UDOUBLESPINLINE_H_

#include <QSpinBox>
#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>

#include "udoublespinbox.h"

namespace unreal
{
	class UDoubleSpinLine : public QWidget
	{
		Q_OBJECT
	public:
		explicit UDoubleSpinLine(QWidget* parent = 0);
		~UDoubleSpinLine();

		void setValue(double value);
		void setSpecialValueText(const QString& text);
		QString specialValueText() const;

        void setupUI();

        static UDoubleSpinLine* create(QWidget* parent, QString label, float min = 0, float max = 100, float step = 1, float value = 0);
	Q_SIGNALS:
		void valueChanged(double value);
    public:
        UDoubleSpinBox* doublespinbox_;
        QLabel* label_;
        QHBoxLayout* layout_;
	};
}

#endif