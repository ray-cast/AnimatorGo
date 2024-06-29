#ifndef UNREAL_CAMERA_DOCK_H_
#define UNREAL_CAMERA_DOCK_H_

#include <qdockwidget.h>
#include <qdialog.h>
#include <qboxlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <optional>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <qcheckbox.h>
#include <qslider.h>
#include "unreal_behaviour.h"

#include <octoon/game_object.h>

#include "../widgets/udockwidget.h"
#include "../widgets/spoiler.h"
#include "../widgets/draggable_button.h"
#include "../widgets/uspinbox.h"
#include "../widgets/udoublespinbox.h"
#include "../widgets/udoublespinline.h"


namespace unreal
{
	class CameraDock final : public UDockWidget
	{
		Q_OBJECT
	public:
		CameraDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept;
		~CameraDock() noexcept;

		void showEvent(QShowEvent* event) override;
		void closeEvent(QCloseEvent* event) override;

		bool eventFilter(QObject* watched, QEvent* event);

	private Q_SLOTS:
		void onFovChanged(double);
		void onFovSliderEvent(int);

		void onFocalLengthChanged(double);
		void onApertureChanged(double);
		void onFocusDistanceChanged(double);
		void dofEvent(int state);
		void onUpdateTarget();
		void onLoadAnimation();
		void onUnloadAnimation();

	public:
		QLabel* dofInfoLabel_;
		QLabel* focalLengthLabel_;
		QLabel* focusDistanceName_;
		QLabel* focusDistanceLabel_;
		QLabel* dofLabel_;
		QLabel* title_;

		QFrame* headerLine_;

		QCheckBox* dofButton_;
		QToolButton* focusTargetButton_;
		QSlider* fovSlider;
		UDoubleSpinLine* fovSpinbox_;
		UDoubleSpinLine* apertureSpinbox_;
		UDoubleSpinLine* focalLengthSpinbox_;
		UDoubleSpinLine* focusDistanceSpinbox_;

		QToolButton* loadButton_;
		QToolButton* unloadButton_;

		QWidget* mainWidget_;
		QVBoxLayout* mainLayout_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<UnrealProfile> profile_;
	};
}

#endif