#ifndef SETTING_MAIN_PLANE_H_
#define SETTING_MAIN_PLANE_H_

#include <qwidget>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qboxlayout.h>

class SettingMainPlane final : public QWidget
{
	Q_OBJECT
public:
	SettingMainPlane(QWidget* parent);

	std::unique_ptr<QLabel> infoLabel;
	std::unique_ptr<QToolButton> infoButton;
	std::unique_ptr<QLabel> versionLabel;

private:
	std::unique_ptr<QVBoxLayout> layout_;
};

class SettingMainPlane2 final : public QWidget
{
	Q_OBJECT
public:
	SettingMainPlane2(QWidget* parent);

	std::unique_ptr<QLabel> renderLabel;
	std::unique_ptr<QLabel> resolutionLabel;
	std::unique_ptr<QComboBox> resolutionCombo;
	std::unique_ptr<QLabel> speedLabel;
	std::unique_ptr<QComboBox> speedCombo;

private:
	std::unique_ptr<QVBoxLayout> layout_;
};

class SettingMainPlane3 final : public QWidget
{
	Q_OBJECT
public:
	SettingMainPlane3(QWidget* parent);
};

#endif