#ifndef UNREAL_LOGIN_WINDOW_H_
#define UNREAL_LOGIN_WINDOW_H_

#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qaction.h>
#include <qevent.h>
#include <qcheckbox.h>
#include <qvalidator.h>
#include <qstackedwidget.h>
#include <qpropertyanimation.h>

#include "unreal_behaviour.h"

namespace unreal
{
	class LineEdit final : public QLineEdit
	{
	public:
		void focusInEvent(QFocusEvent* event) override
		{
			this->grabKeyboard();
			QLineEdit::focusInEvent(event);
		}

		void focusOutEvent(QFocusEvent* event) override
		{
			this->releaseKeyboard();
			QLineEdit::focusOutEvent(event);
		}
	};

	class LoginWidget final : public QWidget
	{
		Q_OBJECT
	public:
		LoginWidget(QWidget* parent) noexcept;
		~LoginWidget() noexcept;

	public Q_SLOTS:
		void eyeToggled(bool);

	public:
		std::unique_ptr<QValidator> validator_;

		std::unique_ptr<QAction> userAccount_;
		std::unique_ptr<QAction> passAccount_;
		std::unique_ptr<QAction> eyeAccount_;

		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<QPushButton> logoButton_;
		std::unique_ptr<QLabel> selectButton_;
		std::unique_ptr<QLabel> validButton_;
		std::unique_ptr<QLabel> userLabel_;
		std::unique_ptr<QLabel> passLabel_;
		std::unique_ptr<LineEdit> usernameEdit_;
		std::unique_ptr<LineEdit> passwordEdit_;
		std::unique_ptr<QToolButton> loginButton_;
		std::unique_ptr<QCheckBox> autoLoginBox_;
		std::unique_ptr<QPushButton> forgetButton_;
		std::unique_ptr<QPushButton> registerButton_;

		std::unique_ptr<QHBoxLayout> hLayout_;
		std::unique_ptr<QVBoxLayout> layout_;

		std::unique_ptr<QWidget> loginWidget_;
	};

	class ForgetWidget final : public QWidget
	{
		Q_OBJECT
	public:
		ForgetWidget(QWidget* parent) noexcept;
		~ForgetWidget() noexcept;

	public:
		std::unique_ptr<QValidator> validator_;

		std::unique_ptr<QAction> userAccount_;
		std::unique_ptr<QAction> passAccount_;

		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<QPushButton> logoButton_;
		std::unique_ptr<QLabel> selectButton_;
		std::unique_ptr<QLabel> validButton_;
		std::unique_ptr<QLabel> userLabel_;
		std::unique_ptr<QLabel> passLabel_;
		std::unique_ptr<LineEdit> usernameEdit_;
		std::unique_ptr<LineEdit> passwordEdit_;
		std::unique_ptr<QToolButton> forgetButton_;
		std::unique_ptr<QPushButton> returnButton_;

		std::unique_ptr<QVBoxLayout> layout_;
	};

	class RegisterWidget final : public QWidget
	{
		Q_OBJECT
	public:
		RegisterWidget(QWidget* parent) noexcept;
		~RegisterWidget() noexcept;

	public Q_SLOTS:
		void eyeToggled(bool);

	public:
		std::unique_ptr<QValidator> validator_;

		std::unique_ptr<QAction> userAccount_;
		std::unique_ptr<QAction> passAccount_;
		std::unique_ptr<QAction> eyeAccount_;

		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<QPushButton> logoButton_;
		std::unique_ptr<QLabel> selectButton_;
		std::unique_ptr<QLabel> validButton_;
		std::unique_ptr<QLabel> userLabel_;
		std::unique_ptr<QLabel> passLabel_;
		std::unique_ptr<LineEdit> usernameEdit_;
		std::unique_ptr<LineEdit> passwordEdit_;
		std::unique_ptr<QToolButton> registerButton_;
		std::unique_ptr<QPushButton> returnButton_;

		std::unique_ptr<QVBoxLayout> layout_;
	};

	class LoginWindow final : public QStackedWidget
	{
		Q_OBJECT
	public:
		LoginWindow(const std::shared_ptr<unreal::UnrealBehaviour>& behaviour) noexcept;
		~LoginWindow() noexcept;

	public Q_SLOTS:
		void closeEvent();
		void loginEvent();
		void registerEvent();
		void forgetEvent();
		void gotoRegisterEvent();
		void gotoForgetEvent();
		void gotoLoginEvent();
		void rotateFinished();
		void autoLoginToggled(bool);
		void paintEvent(QPaintEvent* event);
		void shakeEvent();

	Q_SIGNALS:
		void loginSuccess();

	private:
		void mousePressEvent(QMouseEvent* e) noexcept override;
		void mouseReleaseEvent(QMouseEvent* e) noexcept override;
		void mouseMoveEvent(QMouseEvent* e) noexcept override;
		void keyPressEvent(QKeyEvent* event) noexcept override;
		void keyReleaseEvent(QKeyEvent* event) noexcept override;

	private:
		bool allowMove_;
		bool isRoratingWindow_;
		std::uint32_t nextPageIndex_;

		QPoint startPos_;
		QPoint clickPos_;

		std::unique_ptr<QPropertyAnimation> rotateAnimation_;
		std::unique_ptr<QPropertyAnimation> positionAnimation_;
		std::unique_ptr<QPropertyAnimation> closeAnimation_;

		std::unique_ptr<LoginWidget> loginWidget_;
		std::unique_ptr<ForgetWidget> forgetWidget_;
		std::unique_ptr<RegisterWidget> registerWidget_;

		std::shared_ptr<unreal::UnrealBehaviour> behaviour_;
	};
}

#endif