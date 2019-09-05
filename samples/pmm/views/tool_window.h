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
	ToolBar(QWidget* parent);
	~ToolBar();

private Q_SLOTS:
	void hideEvent() noexcept;
	void importEvent() noexcept;
	void playEvent() noexcept;
	void recordEvent() noexcept;
	void shotEvent() noexcept;
	void gpuEvent() noexcept;
	void hdrEvent() noexcept;

Q_SIGNALS:
	void hideSignal();
	void importSignal();
	void playSignal(bool enable);
	void recordSignal(bool enable);
	void shotSignal();
	void gpuSignal(bool enable);
	void hdrSignal();

private:
	void resizeEvent(QResizeEvent* e) noexcept override;

private:
	bool gpuEnable_;
	bool playEnable_;
	bool recordEnable_;

	QIcon playIcon_;
	QIcon stopIcon_;
	QIcon gpuIcon_;
	QIcon gpuOnIcon_;
	QIcon recordIcon_;
	QIcon recordOnIcon_;

	std::unique_ptr<QToolButton> hideButton_;
	std::unique_ptr<QToolButton> importButton_;
	std::unique_ptr<QToolButton> saveButton_;
	std::unique_ptr<QToolButton> playButton_;
	std::unique_ptr<QToolButton> recordButton_;
	std::unique_ptr<QToolButton> shotButton_;
	std::unique_ptr<QToolButton> gpuButton_;
	std::unique_ptr<QToolButton> hdrButton_;

	std::unique_ptr<QVBoxLayout> layout_;
};

#endif