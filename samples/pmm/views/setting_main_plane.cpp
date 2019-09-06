#include "setting_main_plane.h"
#include <qlabel.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>

SettingMainPlane::SettingMainPlane(QWidget* parent)
	: QWidget(parent)
{
	QLabel* infoLabel = new QLabel(this);
	infoLabel->setText(u8"版本信息");
	infoLabel->setStyleSheet("color: rgb(255,255,255);");

	QToolButton* infoButton = new QToolButton(this);
	infoButton->setText(u8"检查更新");
	infoButton->setStyleSheet("background: rgb(50,50,50); border:2px solid #646464;border-radius:4px;color: rgb(200,200,200);");
	infoButton->setFixedSize(190, 35);

	QLabel* versionLabel = new QLabel(this);
	versionLabel->setText(u8"当前版本：v1.0.0");
	versionLabel->setStyleSheet("color: rgb(200,200,200);");

	QLabel* startupLabel = new QLabel(this);
	startupLabel->setText(u8"启动项");
	startupLabel->setStyleSheet("color: rgb(255,255,255);");

	QCheckBox* checkBox = new QCheckBox(this);
	checkBox->setText(u8"开机自动启动");
	checkBox->setStyleSheet("color: rgb(200,200,200);");

	QLabel* langLabel = new QLabel(this);
	langLabel->setText(u8"语言");
	langLabel->setStyleSheet("color: rgb(255,255,255);");

	QComboBox* gamCombo = new QComboBox(this);
	gamCombo->addItem(u8"中文");
	gamCombo->setStyleSheet("color: rgb(200,200,200);");
	gamCombo->setFont(QFont("Microsoft YaHei", 9, 50));

	QLabel* powerLabel = new QLabel(this);
	powerLabel->setText(u8"性能");
	powerLabel->setStyleSheet("color: rgb(255,255,255);");

	QCheckBox* lowpowerBox = new QCheckBox(this);
	lowpowerBox->setText(u8"低功耗模式");
	lowpowerBox->setStyleSheet("color: rgb(200,200,200);");

	QLabel* defaultLabel = new QLabel(this);
	defaultLabel->setText(u8"恢复默认设置");
	defaultLabel->setStyleSheet("color: rgb(255,255,255);");

	QToolButton* defaultButton = new QToolButton(this);
	defaultButton->setText(u8"立即恢复");
	defaultButton->setStyleSheet("background: rgb(50,50,50); border:2px solid #646464; border-radius:4px;color: rgb(200,200,200);");
	defaultButton->setFixedSize(190, 35);

	auto layout_ = new QVBoxLayout(this);
	layout_->addWidget(infoLabel);
	layout_->addWidget(infoButton);
	layout_->addWidget(versionLabel);
	layout_->addSpacing(10);
	layout_->addWidget(startupLabel);
	layout_->addSpacing(10);
	layout_->addWidget(checkBox);
	layout_->addSpacing(10);
	layout_->addWidget(langLabel);
	layout_->addSpacing(10);
	layout_->addWidget(gamCombo);
	layout_->addSpacing(10);
	layout_->addWidget(powerLabel);
	layout_->addSpacing(10);
	layout_->addWidget(lowpowerBox);
	layout_->addSpacing(10);
	layout_->addWidget(defaultLabel);
	layout_->addSpacing(10);
	layout_->addWidget(defaultButton);
	layout_->setContentsMargins(0, 0, 0, 10);
}

SettingMainPlane2::SettingMainPlane2(QWidget* parent)
	: QWidget(parent)
{
	QLabel* renderLabel = new QLabel(this);
	renderLabel->setText(u8"渲染设置");
	renderLabel->setStyleSheet("color: rgb(255,255,255);");

	QLabel* resolutionLabel = new QLabel(this);
	resolutionLabel->setText(u8"分辨率");
	resolutionLabel->setStyleSheet("color: rgb(200,200,200);");

	QComboBox* resolutionCombo = new QComboBox(this);
	resolutionCombo->addItem(u8"320*240");
	resolutionCombo->addItem(u8"720*480");
	resolutionCombo->addItem(u8"800*480");
	resolutionCombo->addItem(u8"1024*576");
	resolutionCombo->addItem(u8"1280x720");
	resolutionCombo->addItem(u8"1920x1080");
	resolutionCombo->setStyleSheet("color: rgb(200,200,200);");
	resolutionCombo->setFont(QFont("Microsoft YaHei", 9, 50));

	QLabel* speedLabel = new QLabel(this);
	speedLabel->setText(u8"帧速率");
	speedLabel->setStyleSheet("color: rgb(200,200,200);");

	QComboBox* speedCombo = new QComboBox(this);
	speedCombo->addItem(u8"24");
	speedCombo->addItem(u8"30");
	speedCombo->addItem(u8"60");
	speedCombo->setStyleSheet("color: rgb(200,200,200);");
	speedCombo->setFont(QFont("Microsoft YaHei", 9, 50));

	auto layout_ = new QVBoxLayout(this);
	layout_->addWidget(renderLabel);
	layout_->addSpacing(10);
	layout_->addWidget(resolutionLabel);
	layout_->addSpacing(10);
	layout_->addWidget(resolutionCombo);
	layout_->addSpacing(10);
	layout_->addWidget(speedLabel);
	layout_->addSpacing(10);	
	layout_->addWidget(speedCombo);
}

SettingMainPlane3::SettingMainPlane3(QWidget* parent)
	: QWidget(parent)
{
	auto layout_ = new QVBoxLayout(this);
	layout_->addSpacing(300);
}