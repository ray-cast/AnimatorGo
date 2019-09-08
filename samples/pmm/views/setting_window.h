#ifndef SETTING_WINDOW_H_
#define SETTING_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <QtGui/qevent.h>
#include <qlistwidget.h>
#include <qscrollarea.h>

#include "setting_main_plane.h"
#include "setting_title_window.h"
#include "mysticlit_behaviour.h"

class SettingContextPlane final : public QWidget
{
	Q_OBJECT
public:
	SettingContextPlane(QWidget* parent) noexcept;
	~SettingContextPlane() noexcept;

public Q_SLOTS:
	void valueChanged(int value);
	void itemClicked(QListWidgetItem* item);

private:
	bool m_sign;

	std::unique_ptr<QListWidget> listWidget_;
	std::unique_ptr<QListWidgetItem> listWidgetItems_[3];
	std::unique_ptr<QWidget> widgetItems_[3];
	std::unique_ptr<QVBoxLayout> gridLayout_;
	std::unique_ptr<QScrollArea> scrollArea_;
	std::unique_ptr<QWidget> scrollWidget_;
	std::unique_ptr<QHBoxLayout> layout_;
};

class SettingWindow final : public QWidget
{
	Q_OBJECT
public:
	SettingWindow(const std::shared_ptr<MysticLit::MysticLitProfile>& profile) noexcept;
	~SettingWindow() noexcept;

public Q_SLOTS:
	void closeEvent();

private:
	std::unique_ptr<QVBoxLayout> contextLayout_;
	std::shared_ptr<MysticLit::MysticLitProfile> profile_;

	std::unique_ptr<SettingTitleWindow> settingTitleWindow_;
	std::unique_ptr<SettingContextPlane> settingContextPlane_;
};

#endif