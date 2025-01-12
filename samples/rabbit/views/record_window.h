#ifndef RABBIT_RECORD_WINDOW_H_
#define RABBIT_RECORD_WINDOW_H_

#include <qwidget>
#include <qdialog.h>
#include <qboxlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <optional>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtimer.h>
#include "rabbit_behaviour.h"
#include "spoiler.h"
#include <octoon/game_object.h>

namespace rabbit
{
	class FocalTargetWindow final : public QToolButton
	{
		Q_OBJECT
	public:
		FocalTargetWindow() noexcept;
		~FocalTargetWindow() noexcept;

		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;

	private:
		QPoint startPos;
	};
	
	class RecordWindow final : public QWidget
	{
		Q_OBJECT
	public:
		RecordWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept;
		~RecordWindow() noexcept;

		void update();
		void repaint();

		void startRecord(QString fileName);
		void stopRecord();

		void updateTarget();

		void showEvent(QShowEvent* event) override;
		void resizeEvent(QResizeEvent* e) noexcept override;

	private Q_SLOTS:

		void closeEvent();
		void clickEvent();
		void select1Event(bool checked);
		void select2Event(bool checked);
		void speed1Event(bool checked);
		void speed2Event(bool checked);
		void speed3Event(bool checked);
		void speed4Event(bool checked);
		void startEvent(int);
		void endEvent(int);
		void timeEvent();
		void onSppChanged(int);
		void onBouncesChanged(int);
		void onCrfChanged(double);
		void onApertureChanged(double);
		void onFocalDistanceChanged(double);

	public:
		QLabel* title_;
		QLabel* quality_;
		QLabel* videoRatio_;
		QLabel* frame_;
		QLabel* animation_;
		QLabel* summary_;
		QLabel* currentFrame_;
		QLabel* timeTotal_;
		QLabel* sppLabel;
		QLabel* bouncesLabel_;
		QLabel* crfLabel;
		QLabel* startLabel_;
		QLabel* endLabel_;
		QLabel* dofInfoLabel_;
		QLabel* apertureLabel_;
		QLabel* focalDistanceName_;
		QLabel* focalDistanceLabel_;

		QButtonGroup* group_;
		QButtonGroup* speedGroup_;

		QToolButton* select1_;
		QToolButton* select2_;
		QToolButton* speed1_;
		QToolButton* speed2_;
		QToolButton* speed3_;
		QToolButton* speed4_;
		QToolButton* recordButton_;
		QToolButton* closeButton_;
		QToolButton* markButton_;
		QToolButton* focalTargetButton_;

		QSpinBox* start_;
		QSpinBox* end_;
		QSpinBox* sppSpinbox_;
		QSpinBox* bouncesSpinbox_;
		
		QDoubleSpinBox* crfSpinbox;
		QDoubleSpinBox* apertureSpinbox_;
		QDoubleSpinBox* focalDistanceSpinbox_;

		QHBoxLayout* videoRatioLayout_;
		QHBoxLayout* frameLayout_;
		QVBoxLayout* mainLayout_;
		QTimer* timer_;

		Spoiler* markSpoiler_;
		Spoiler* cameraSpoiler_;
		Spoiler* videoSpoiler_;
		Spoiler* infoSpoiler_;
		QScrollArea* contentWidgetArea_;

		octoon::GameObjectPtr behaviour_;
	};
}

#endif