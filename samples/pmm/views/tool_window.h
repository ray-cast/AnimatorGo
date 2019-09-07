#ifndef TOOL_WINDOW_H_
#define TOOL_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qlabel.h>
#include <QtGui/qevent.h>
#include <qdockwidget.h>

class ToolBar final : public QWidget
{
	Q_OBJECT
public:
	ToolBar(QWidget* parent) noexcept;
	~ToolBar() noexcept;

private Q_SLOTS:
	void hideEvent() noexcept;
	void importEvent() noexcept;
	void playEvent() noexcept;
	void resetEvent() noexcept;
	void leftEvent() noexcept;
	void rightEvent() noexcept;
	void recordEvent() noexcept;
	void shotEvent() noexcept;
	void gpuEvent() noexcept;
	void hdrEvent() noexcept;
	void cleanupEvent() noexcept;

Q_SIGNALS:
	void hideSignal();
	void importSignal();
	bool playSignal(bool enable);
	void resetSignal();
	void leftSignal();
	void rightSignal();
	bool recordSignal(bool enable);
	void shotSignal();
	bool gpuSignal(bool enable);
	bool hdrSignal(bool enable);
	void cleanupSignal();

private:
	bool gpuEnable_;
	bool playEnable_;
	bool recordEnable_;
	bool hdrEnable_;

	QIcon playIcon_;
	QIcon playOnIcon_;
	QIcon leftIcon_;
	QIcon rightIcon_;
	QIcon resetIcon_;
	QIcon gpuIcon_;
	QIcon gpuOnIcon_;
	QIcon recordIcon_;
	QIcon recordOnIcon_;
	QIcon hdrIcon_;
	QIcon hdrOnIcon_;

	std::unique_ptr<QToolButton> hideButton_;
	std::unique_ptr<QToolButton> importButton_;
	std::unique_ptr<QToolButton> saveButton_;
	std::unique_ptr<QToolButton> playButton_;
	std::unique_ptr<QToolButton> resetButton_;
	std::unique_ptr<QToolButton> leftButton_;
	std::unique_ptr<QToolButton> rightButton_;
	std::unique_ptr<QToolButton> recordButton_;
	std::unique_ptr<QToolButton> shotButton_;
	std::unique_ptr<QToolButton> gpuButton_;
	std::unique_ptr<QToolButton> hdrButton_;
	std::unique_ptr<QToolButton> cleanupButton_;

	std::unique_ptr<QVBoxLayout> layout_;
};

#endif