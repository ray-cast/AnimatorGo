#ifndef RABBIT_SETTING_WINDOW_H_
#define RABBIT_SETTING_WINDOW_H_

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
#include <qcombobox.h>
#include <qdialog.h>
#include <qpropertyanimation.h>
#include <qspinbox.h>

#include "title_bar.h"
#include "rabbit_behaviour.h"

namespace rabbit
{
	class SettingMainPlane final : public QWidget
	{
		Q_OBJECT
	public:
		SettingMainPlane(QWidget* parent, const std::shared_ptr<rabbit::RabbitBehaviour>& behaviour);

		std::unique_ptr<QLabel> infoLabel;
		std::unique_ptr<QToolButton> infoButton;
		std::unique_ptr<QLabel> versionLabel;
		std::unique_ptr<QLabel> resetLabel;
		std::unique_ptr<QToolButton> resetButton;

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

	private:
		std::unique_ptr<QVBoxLayout> layout_;
	};

	class SettingMainPlane3 final : public QWidget
	{
		Q_OBJECT
	public:
		SettingMainPlane3(QWidget* parent);
	};

	class SettingContextPlane final : public QWidget
	{
		Q_OBJECT
	public:
		SettingContextPlane(QWidget* parent, const std::shared_ptr<rabbit::RabbitBehaviour>& behaviour) noexcept;
		~SettingContextPlane() noexcept;

	public Q_SLOTS:
		void valueChanged(int value);
		void itemClicked(QListWidgetItem* item);

		void onResetButton();
		void onResolutionCombo(int index);
		void onCheckVersion();

	private:
		bool m_sign;
		std::shared_ptr<rabbit::RabbitBehaviour> behaviour_;

		std::unique_ptr<QListWidget> listWidget_;
		std::unique_ptr<QListWidgetItem> listWidgetItems_[3];
		std::unique_ptr<SettingMainPlane> mainPlane_;
		std::unique_ptr<SettingMainPlane2> mainPlane2_;
		std::unique_ptr<SettingMainPlane3> mainPlane3_;
		std::unique_ptr<QVBoxLayout> gridLayout_;
		std::unique_ptr<QScrollArea> scrollArea_;
		std::unique_ptr<QWidget> scrollWidget_;
		std::unique_ptr<QHBoxLayout> layout_;
	};

	class SettingWindow final : public QDialog
	{
		Q_OBJECT
	public:
		SettingWindow(const std::shared_ptr<rabbit::RabbitBehaviour>& behaviour) noexcept;
		~SettingWindow() noexcept;

	public Q_SLOTS:
		void closeEvent();

	private:
		std::unique_ptr<QVBoxLayout> contextLayout_;
		std::unique_ptr<QPropertyAnimation> closeAnimation_;

		std::unique_ptr<TitleBar> settingTitleWindow_;
		std::unique_ptr<SettingContextPlane> settingContextPlane_;
	};
}

#endif