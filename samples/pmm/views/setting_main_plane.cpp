#include "setting_main_plane.h"
#include <qlabel.h>
#include <qboxlayout.h>

SettingMainPlane::SettingMainPlane(QWidget* parent)
	: QWidget(parent)
{
	QLabel* imageLabel = new QLabel(this);
	QPixmap pixmap(":res/icons/logo.png");
	pixmap = pixmap.scaled(300, 300, Qt::KeepAspectRatio);
	imageLabel->setPixmap(pixmap);
	imageLabel->setStyleSheet("background: white;");
	imageLabel->setAlignment(Qt::AlignCenter);

	auto layout_ = new QHBoxLayout(this);
	layout_->addWidget(imageLabel);
	layout_->setSpacing(0);
	layout_->setContentsMargins(0, 0, 0, 0);
}