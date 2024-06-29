#include "uspinline.h"

namespace unreal
{
	USpinLine::USpinLine(QWidget* parent)
		: QWidget(parent)
	{
        setupUI();
		connect(spinbox_, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	}
	USpinLine::~USpinLine()
	{
	}

    void
    USpinLine::setupUI()
    {
        label_ = new QLabel(this);
        label_->setText("label");
        spinbox_ = new SpinBox(this);
        spinbox_->setRange(0, 100);
        spinbox_->setValue(0);
        layout_ = new QHBoxLayout(this);
		layout_->addWidget(label_);
		layout_->addWidget(spinbox_);
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setSpacing(0);
        setLayout(layout_);
	}

	void
	USpinLine::setValue(int value)
	{
		spinbox_->setValue(value);
	}

    
    USpinLine*
    USpinLine::create(QWidget* parent, QString label, int min, int max, int step, int value)
    {
        USpinLine* spinline = new USpinLine(parent);
        spinline->label_->setText(label);
        spinline->spinbox_->setRange(min, max);
        spinline->spinbox_->setSingleStep(step);
        spinline->spinbox_->setValue(value);
		spinline->spinbox_->setAlignment(Qt::AlignRight);
        return spinline;
	}
	void
	USpinLine::onEnable()
	{
		spinbox_->setReadOnly(false);
	}
	void
	USpinLine::onDisable()
	{
		spinbox_->setReadOnly(true);
	}
}
