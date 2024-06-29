#include "udoublespinline.h"

namespace unreal
{
	UDoubleSpinLine::UDoubleSpinLine(QWidget* parent)
		: QWidget(parent)
	{
        setupUI();
		connect(doublespinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));		
	}
	UDoubleSpinLine::~UDoubleSpinLine()
	{
	}
	
	void
	UDoubleSpinLine::setValue(double val)
	{
		this->doublespinbox_->setValue(val);
	}

	void
	UDoubleSpinLine::setSpecialValueText(const QString& text)
	{
		this->doublespinbox_->setSpecialValueText(text);
	}

	QString
	UDoubleSpinLine::specialValueText() const
	{
		return this->doublespinbox_->specialValueText();
	}

    void
    UDoubleSpinLine::setupUI()
    {
        label_ = new QLabel(this);
        label_->setText("label");
		doublespinbox_ = new UDoubleSpinBox(this);
		doublespinbox_->setRange(0, 100);
		doublespinbox_->setValue(0);
        layout_ = new QHBoxLayout(this);
		layout_->addWidget(label_);
		layout_->addWidget(doublespinbox_);
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setSpacing(0);
        setLayout(layout_);
    }

    
    UDoubleSpinLine*
	UDoubleSpinLine::create(QWidget* parent, QString label, float min, float max, float step, float value)
    {
        UDoubleSpinLine* spinline = new UDoubleSpinLine(parent);
        spinline->label_->setText(label);
		spinline->doublespinbox_->setRange(min, max);
		spinline->doublespinbox_->setSingleStep(step);
		spinline->doublespinbox_->setValue(value);
		spinline->doublespinbox_->setAlignment(Qt::AlignRight);
        return spinline;
    }
}
