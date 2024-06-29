#ifndef UNREAL_SETTING_WINDOW_H_
#define UNREAL_SETTING_WINDOW_H_

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
#include <QDialog>
#include <qpropertyanimation.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <QTranslator>

#include "../widgets/upanel.h"

#include "title_bar.h"
#include "unreal_behaviour.h"

namespace unreal
{
	class SettingMainPlaneGeneral final : public UPanel
	{
		Q_OBJECT
	public:
		SettingMainPlaneGeneral(QWidget* parent, const std::shared_ptr<unreal::UnrealBehaviour>& behaviour);

		void retranslate() override;
		void setupUI();

		QLabel* infoLabel;
		QPushButton* infoButton;
		QLabel* versionLabel;

	private:
		QVBoxLayout* layout_;
	};

	class SettingMainPlaneInterface final : public UPanel
	{
		Q_OBJECT
	public:
		SettingMainPlaneInterface(QWidget* parent);

		void retranslate();
		void setupUI();

		QLabel* langLabel_;
		QComboBox* langCombo_;

		std::vector<QString> languages_;
	private:
		QVBoxLayout* layout_;
	};

	class SettingContextPlane final : public UPanel
	{
		Q_OBJECT
	public:
		SettingContextPlane(QWidget* parent, const std::shared_ptr<unreal::UnrealBehaviour>& behaviour) noexcept(false);
		~SettingContextPlane() noexcept;

		void retranslate();

	public Q_SLOTS:
		void valueChanged(int value);
		void itemClicked(QListWidgetItem* item);

		void onCheckVersion();
		void onLangCombo(int index);
	
	Q_SIGNALS:
		void languageChangeSignal(QString filename);
	private:
		bool m_sign;
		std::shared_ptr<unreal::UnrealBehaviour> behaviour_;

		QListWidget* listWidget_;
		QListWidgetItem* listWidgetItems_[3];
		SettingMainPlaneGeneral* mainPlaneGeneral_;
		SettingMainPlaneInterface* mainPlaneInterface_;
		QVBoxLayout* gridLayout_;
		QScrollArea* scrollArea_;
		QWidget* scrollWidget_;
		QHBoxLayout* layout_;
	};

	class SettingWindow final : public QDialog
	{
		Q_OBJECT
	public:
		SettingWindow(const std::shared_ptr<unreal::UnrealBehaviour>& behaviour) noexcept;
		~SettingWindow() noexcept;

	public Q_SLOTS:
		void closeEvent();

	Q_SIGNALS:
		void languageChangeSignal(QString filename);		

	private:
		std::unique_ptr<QVBoxLayout> contextLayout_;
		std::unique_ptr<QPropertyAnimation> closeAnimation_;

		std::unique_ptr<TitleBar> settingTitleWindow_;
		std::unique_ptr<SettingContextPlane> settingContextPlane_;
	};
}

#endif