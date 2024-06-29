#ifndef UNREAL_ENVIRONMENT_WINDOW_H_
#define UNREAL_ENVIRONMENT_WINDOW_H_

#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qdockwidget.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qspinbox.h>

#include "../widgets/udockwidget.h"
#include "../widgets/spoiler.h"
#include "../widgets/udoublespinbox.h"

#include "unreal_behaviour.h"
#include "unreal_profile.h"
#include "asset_browse_dialog.h"

namespace unreal
{
	class EnvironmentDock final : public QDockWidget
	{
		Q_OBJECT
	public:
		EnvironmentDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile);
		~EnvironmentDock();

		void showEvent(QShowEvent* event) override;
		void closeEvent(QCloseEvent* event) override;

		bool eventFilter(QObject* watched, QEvent* event);

	public Q_SLOTS:
		void previewClickEvent(bool checked);
		void thumbnailClickEvent();
		void thumbnailToggleEvent(int state);
		void backgroundMapCheckEvent(int state);
		void colorClickEvent();
		void colorChangeEvent(const QColor&);
		void resetEvent();
		void intensitySliderEvent(int);
		void intensityEditEvent(double value);
		void horizontalRotationSliderEvent(int);
		void horizontalRotationEditEvent(double value);
		void verticalRotationSliderEvent(int);
		void verticalRotationEditEvent(double value);
		void itemSelected(QListWidgetItem* item);

	private:
		void setColor(const QColor& c, int w = 50, int h = 26);
		void setPreviewImage(QString name, std::shared_ptr<QImage> image);
		void setThumbnailImage(QString name, const QImage& image);
		void updatePreviewImage();

	private:
		QLabel* previewName_;
		QLabel* thumbnailPath;
		QLabel* intensityLabel_;
		QLabel* horizontalRotationLabel_;
		QLabel* verticalRotationLabel_;
		QLabel* title_;

		QFrame* headerLine_;

		QToolButton* previewButton_;
		QToolButton* colorButton;
		QToolButton* thumbnail;
		QToolButton* resetButton_;

		QCheckBox* thumbnailToggle;
		QCheckBox* backgroundToggle;

		QSlider* intensitySlider;
		QSlider* horizontalRotationSlider;
		QSlider* verticalRotationSlider;

		UDoubleSpinBox* intensitySpinBox;
		UDoubleSpinBox* horizontalRotationSpinBox;
		UDoubleSpinBox* verticalRotationSpinBox;

		QColorDialog* colorSelector_;

		Spoiler* spoiler;

		octoon::GameObjectPtr behaviour_;

		std::shared_ptr<QImage> previewImage_;
		std::shared_ptr<unreal::UnrealProfile> profile_;
		std::unique_ptr<EnvironmentListDialog> environmentListDialog_;
	};
}

#endif