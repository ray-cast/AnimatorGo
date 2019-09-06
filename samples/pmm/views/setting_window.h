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

#include "setting_title_window.h"

class SettingWindow final : public QWidget
{
	Q_OBJECT
public:
	SettingWindow() noexcept;
	~SettingWindow() noexcept;

private:
	QWidget* setupContextWidget() noexcept;

public Q_SLOTS:
	void closeEvent();

private:
	std::unique_ptr<SettingTitleWindow> settingTitleWindow_;
	std::unique_ptr<QListWidget> listWidget_;
	std::unique_ptr<QListWidgetItem> listWidgetItems_[3];
	std::unique_ptr<QScrollArea> scrollArea_;
	std::unique_ptr<QWidget> scrollWidget_;
	std::unique_ptr<QHBoxLayout> layout_;
	std::unique_ptr<QGridLayout> gridLayout_;
	std::unique_ptr<QVBoxLayout> contextLayout_;
};