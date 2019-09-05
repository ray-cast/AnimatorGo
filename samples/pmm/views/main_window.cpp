#include "main_window.h"
#include "mysticlit_behaviour.h"
#include <qmimedata.h>
#include <qfiledialog.h>
#include <qsplashscreen.h>

MainWindow::MainWindow()
	: titleBar_(std::make_unique<TitleBar>(this))
	, viewPanel_(std::make_unique<ViewWidget>(this))
	, toolBar_(std::make_unique<ToolBar>(this))
	, hideBar_(std::make_unique<HideBar>(this))
	, init_flag(false)
{
	this->setFrameShape(Panel);
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setMouseTracking(true);
	this->setAcceptDrops(true);

	mainLayout_ = std::make_unique<QHBoxLayout>(this);
	mainLayout_->setMargin(0);
	mainLayout_->setSpacing(0);

	contextLayout_ = std::make_unique<QVBoxLayout>(this);
	contextLayout_->addWidget(titleBar_.get());
	contextLayout_->addWidget(viewPanel_.get());
	contextLayout_->setMargin(0);
	contextLayout_->setSpacing(0);

	mainLayout_->addLayout(contextLayout_.get());
	mainLayout_->addWidget(toolBar_.get());
	mainLayout_->addWidget(hideBar_.get());

	connect(hideBar_.get(), &HideBar::showSignal, this, &MainWindow::onShowToolbarSignal);
	connect(toolBar_.get(), &ToolBar::hideSignal, this, &MainWindow::onHideToolBarSignal);
	connect(toolBar_.get(), &ToolBar::importSignal, this, &MainWindow::onImportSignal);
	connect(toolBar_.get(), &ToolBar::playSignal, this, &MainWindow::onPlaySignal);
	connect(toolBar_.get(), &ToolBar::recordSignal, this, &MainWindow::onRecordSignal);
	connect(toolBar_.get(), &ToolBar::shotSignal, this, &MainWindow::onScreenShotSignal);
	connect(toolBar_.get(), &ToolBar::gpuSignal, this, &MainWindow::onOfflineModeSignal);
	connect(toolBar_.get(), &ToolBar::hdrSignal, this, &MainWindow::onImportHdriSignal);

	connect(viewPanel_.get(), &ViewWidget::paintSignal, this, &MainWindow::onPaintSignal);
	connect(viewPanel_.get(), &ViewWidget::resizeSignal, this, &MainWindow::onResizeSignal);
	connect(viewPanel_.get(), &ViewWidget::mousePressSignal, this, &MainWindow::onMousePressSignal);
	connect(viewPanel_.get(), &ViewWidget::mouseMoveSignal, this, &MainWindow::onMouseMoveSignal);
	connect(viewPanel_.get(), &ViewWidget::mouseReleaseSignal, this, &MainWindow::onMouseReleaseSignal);
	connect(viewPanel_.get(), &ViewWidget::mouseDoubleClickSignal, this, &MainWindow::onMouseDoubleClickSignal);
	connect(viewPanel_.get(), &ViewWidget::wheelSignal, this, &MainWindow::onWheelSignal);
	connect(viewPanel_.get(), &ViewWidget::dragEnterSignal, this, &MainWindow::onDragEnterSignal);
	connect(viewPanel_.get(), &ViewWidget::dropSignal, this, &MainWindow::onDropSignal);
	connect(viewPanel_.get(), &ViewWidget::showSignal, this, &MainWindow::onShowSignal);
	connect(viewPanel_.get(), &ViewWidget::updateSignal, this, &MainWindow::onUpdateSignal);
}

MainWindow::~MainWindow()
{
	if (behaviour_)
	{
		behaviour_.reset();
		behaviour_ = nullptr;
	}

	if (gameApp_)
	{
		gameApp_.reset();
		gameApp_ = nullptr;
	}
}

void
MainWindow::onHideToolBarSignal() noexcept
{
	toolBar_->hide();
	this->setFixedWidth(this->width() - toolBar_->width() + hideBar_->width());
	hideBar_->show();
}

void
MainWindow::onShowToolbarSignal() noexcept
{
	hideBar_->hide();
	this->setFixedWidth(this->width() + toolBar_->width() - hideBar_->width());
	toolBar_->show();
}

void
MainWindow::onImportSignal() noexcept
{
	if (behaviour_)
		behaviour_->getComponent<MysticLit::MysticlitBehaviour>()->open();
}

bool
MainWindow::onPlaySignal(bool enable) noexcept
{
	if (behaviour_)
	{
		auto behaviour = behaviour_->getComponent<MysticLit::MysticlitBehaviour>();
		if (behaviour->isOpen())
		{
			if (enable)
				behaviour->play();
			else
				behaviour->stop();

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool
MainWindow::onRecordSignal(bool enable) noexcept
{
	if (behaviour_)
	{
		auto behaviour = behaviour_->getComponent<MysticLit::MysticlitBehaviour>();
		if (behaviour->isOpen())
		{
			if (enable)
				behaviour->startRecord();
			else
				behaviour->stopRecord();

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void 
MainWindow::onScreenShotSignal() noexcept
{
	if (behaviour_)
	{
		auto behaviour = behaviour_->getComponent<MysticLit::MysticlitBehaviour>();
		if (behaviour->isOpen())
			behaviour->renderPicture();
	}
}

bool
MainWindow::onOfflineModeSignal(bool enable) noexcept
{
	if (behaviour_)
	{
		auto behaviour = behaviour_->getComponent<MysticLit::MysticlitBehaviour>();
		if (behaviour->isOpen())
		{
			if (enable)
				behaviour->startPathTracing();
			else
				behaviour->stopPathTracing();

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void
MainWindow::onImportHdriSignal() noexcept
{
	if (behaviour_)
	{
		auto behaviour = behaviour_->getComponent<MysticLit::MysticlitBehaviour>();
		if (behaviour->isOpen())
		{
			QString fileName = QFileDialog::getOpenFileName(this, u8"´ò¿ª", "", tr("HDRi Files (*.hdr)"));
			if (!fileName.isEmpty())
				behaviour->loadHDRi(fileName.toUtf8().data());
		}
	}
}

void
MainWindow::onPaintSignal(QPaintEvent* e) noexcept
{
}

void
MainWindow::onResizeSignal(QResizeEvent* e) noexcept
{
	if (gameApp_)
		gameApp_->doWindowResize((octoon::WindHandle)viewPanel_->winId(), viewPanel_->width(), viewPanel_->height());
}

void
MainWindow::onMouseMoveSignal(QMouseEvent* e) noexcept
{
	if (gameApp_)
		gameApp_->doWindowMouseMotion((octoon::WindHandle)viewPanel_->winId(), e->pos().x(), e->pos().y());
}

void
MainWindow::onMousePressSignal(QMouseEvent* e) noexcept
{
	if (gameApp_)
	{
		if (e->button() == Qt::LeftButton)
			gameApp_->doWindowMouseButtonDown((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Left, e->x(), e->y());
		else if (e->button() == Qt::RightButton)
			gameApp_->doWindowMouseButtonDown((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Right, e->x(), e->y());
		else if (e->button() == Qt::MiddleButton)
			gameApp_->doWindowMouseButtonDown((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Middle, e->x(), e->y());
	}
}

void
MainWindow::onMouseReleaseSignal(QMouseEvent* e) noexcept
{
	if (gameApp_)
	{
		if (e->button() == Qt::LeftButton)
			gameApp_->doWindowMouseButtonUp((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Left, e->x(), e->y());
		else if (e->button() == Qt::RightButton)
			gameApp_->doWindowMouseButtonUp((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Right, e->x(), e->y());
		else if (e->button() == Qt::MiddleButton)
			gameApp_->doWindowMouseButtonUp((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Middle, e->x(), e->y());
	}
}

void
MainWindow::onMouseDoubleClickSignal(QMouseEvent* e) noexcept
{
	if (gameApp_)
	{
		if (e->button() == Qt::LeftButton)
			gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Left, e->x(), e->y());
		else if (e->button() == Qt::RightButton)
			gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Right, e->x(), e->y());
		else if (e->button() == Qt::MiddleButton)
			gameApp_->doWindowMouseButtonDoubleClick((octoon::WindHandle)viewPanel_->winId(), octoon::input::InputButton::Middle, e->x(), e->y());
	}
}

void
MainWindow::onWheelSignal(QWheelEvent* e) noexcept
{
	if (gameApp_)
		gameApp_->doWindowScrool((octoon::WindHandle)viewPanel_->winId(), 0, e->delta());
}

void
MainWindow::onShowSignal(QShowEvent* event) noexcept
{
	onUpdateSignal();
}

void
MainWindow::onDragEnterSignal(QDragEnterEvent* event) noexcept
{
	event->acceptProposedAction();
}

void
MainWindow::onDropSignal(QDropEvent* event) noexcept
{
	if (gameApp_)
	{
		auto urls = event->mimeData()->urls();

		std::vector<QByteArray> utf8bytes;
		for (auto& it : urls)
			utf8bytes.push_back(it.path().toUtf8());

		std::vector<char*> utf8s;
		for (auto& it : utf8bytes)
			utf8s.push_back(it.data() + 1);

		gameApp_->doWindowDrop((octoon::WindHandle)viewPanel_->winId(), (std::uint32_t)utf8s.size(), (const char**)utf8s.data());
	}
}

void
MainWindow::onUpdateSignal() noexcept
{
	if (!init_flag)
	{
		init_flag = true;
		open(viewPanel_->width(), viewPanel_->height());
	}

	if (gameApp_)
		gameApp_->update();
}

bool
MainWindow::open(int w, int h) noexcept
{
	assert(!gameApp_);

	try
	{
		gameApp_ = std::make_shared<octoon::GameApp>();
		gameApp_->open((octoon::WindHandle)viewPanel_->winId(), w, h, w, h);
		gameApp_->setActive(true);

		behaviour_ = octoon::GameObject::create();
		behaviour_->addComponent<MysticLit::MysticlitBehaviour>();

		return true;
	}
	catch (...)
	{
		gameApp_.reset();
		gameApp_ = nullptr;

		return false;
	}
}