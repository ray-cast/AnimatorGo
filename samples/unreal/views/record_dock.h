#ifndef UNREAL_RECORD_DOCK_H_
#define UNREAL_RECORD_DOCK_H_

#include <QComboBox>
#include <octoon/game_object.h>
#include <optional>
#include <qboxlayout.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qdockwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <qtoolbutton.h>

#include "unreal_behaviour.h"

#include "../widgets/spoiler.h"
#include "../widgets/uspinbox.h"
#include "../widgets/uspinline.h"
#include "../widgets/udoublespinbox.h"
#include "../widgets/udoublespinline.h"

namespace unreal
{
	class RecordDock final : public QDockWidget
	{
		Q_OBJECT
	  public:
		RecordDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept;
		~RecordDock() noexcept;

		void showEvent(QShowEvent* event) override;
		void closeEvent(QCloseEvent* event) override;

		bool eventFilter(QObject* watched, QEvent* event);

	  private Q_SLOTS:
		void onRatioEvent(bool);
		void onResolutionEvent(bool);
		void recordEvent(bool);
		void select1Event(bool checked);
		void select2Event(bool checked);
		void denoiseEvent(int checked);
		void speed1Event(bool checked);
		void speed2Event(bool checked);
		void speed3Event(bool checked);
		void speed4Event(bool checked);
		void mode1Event(bool checked);
		void mode2Event(bool checked);
		void mode3Event(bool checked);
		void startEvent(int);
		void endEvent(int);
		void outputTypeEvent(int);
		void onSppChanged(int);
		void onBouncesChanged(int);
		void onCrfChanged(double);

	  public:
		QLabel* videoFps_;
		QLabel* videoRatio_;
		QLabel* quality_;
		QLabel* frame_;
		QLabel* encodeType_;
		QLabel* sppLabel;
		QLabel* bouncesLabel_;
		QLabel* crfLabel;
		QLabel* startLabel_;
		QLabel* middleLabel_;
		QLabel* endLabel_;
		QLabel* denoiseLabel_;
		QLabel* resolutionLabel;
		QLabel* title_;

		QFrame* headerLine_;

		QButtonGroup* selectGroup_;
		QButtonGroup* speedGroup_;
		QButtonGroup* modeGroup_;
		QButtonGroup* ratioGroup_;
		QButtonGroup* resolutionGroup_;

		QPushButton* ratio1_;
		QPushButton* ratio2_;
		QPushButton* ratio3_;
		QPushButton* ratio4_;

		QPushButton* resolution1_;
		QPushButton* resolution2_;
		QPushButton* resolution3_;
		QPushButton* resolution4_;

		QPushButton* select1_;
		QPushButton* select2_;

		QPushButton* speed1_;
		QPushButton* speed2_;
		QPushButton* speed3_;
		QPushButton* speed4_;

		QPushButton* mode1_;
		QPushButton* mode2_;
		QPushButton* mode3_;

		QPushButton* recordButton_;
		QCheckBox* denoiseButton_;

		QSpinBox* startFrame_;
		QSpinBox* endFrame_;
		USpinLine* sppSpinbox_;
		USpinLine* bouncesSpinbox_;

		UDoubleSpinLine* crfSpinbox;

		QHBoxLayout* videoRatioLayout_;
		QHBoxLayout* videoFpsLayout_;
		QHBoxLayout* frameLayout_;
		QVBoxLayout* resolutionLayout_;
		QVBoxLayout* mainLayout_;

		QWidget* mainWidget_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<UnrealProfile> profile_;
	};
}

#endif