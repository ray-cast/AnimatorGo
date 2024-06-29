#include "motion_dock.h"
#include "../utils/asset_library.h"
#include "../widgets/draggable_list_widget.h"
#include <octoon/asset_database.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qevent.h>
#include <qapplication.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qprogressdialog.h>

namespace unreal
{
	MotionDock::MotionDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile)
		: behaviour_(behaviour)
		, profile_(profile)
		, clickedItem_(nullptr)
	{
		this->setWindowTitle(tr("Motion Library"));
		this->setObjectName("MotionDock");
		this->setFixedWidth(290);
		this->setFeatures(QDockWidget::NoDockWidgetFeatures);

		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		title_ = new QLabel;
		title_->setObjectName("title");
		title_->setText(tr("Motion Library"));

		auto headerLine = new QFrame;
		headerLine->setObjectName("HLine");
		headerLine->setFixedHeight(1);
		headerLine->setFrameShape(QFrame::NoFrame);
		headerLine->setFrameShadow(QFrame::Plain);
		headerLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

		topLayout_ = new QVBoxLayout();
		topLayout_->addWidget(title_, 0, Qt::AlignLeft);
		topLayout_->addSpacing(8);
		topLayout_->addWidget(headerLine);
		topLayout_->setSpacing(0);
		topLayout_->setContentsMargins(4, 0, 4, 0);

		listWidget_ = new DraggableListWindow;
		listWidget_->setIconSize(QSize(80, 80));
		listWidget_->setStyleSheet("background:transparent;");
		listWidget_->setSpacing(4);
		listWidget_->installEventFilter(this);
		
		mainLayout_ = new QVBoxLayout();
		mainLayout_->addLayout(topLayout_);
		mainLayout_->addWidget(listWidget_);
		mainLayout_->setContentsMargins(0, 12, 0, 8);

		mainWidget_ = new QWidget;
		mainWidget_->setObjectName("MotionWidget");
		mainWidget_->setLayout(mainLayout_);

		this->setWidget(mainWidget_);

		this->connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		this->connect(listWidget_, SIGNAL(itemSelected(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
	}

	MotionDock::~MotionDock() noexcept
	{
	}

	void 
	MotionDock::addItem(const nlohmann::json& package) noexcept
	{
		auto item = std::make_unique<QListWidgetItem>();
		item->setData(Qt::UserRole, QString::fromStdString(package["uuid"].get<nlohmann::json::string_t>()));
		item->setSizeHint(listWidget_->iconSize() + QSize(9, 35));

		if (package.contains("preview"))
		{
			auto filepath = QString::fromStdWString(AssetLibrary::instance()->getAssetPath(package["preview"].get<std::string>(), true).wstring());
			item->setIcon(QIcon(QPixmap(filepath)));
		}
		else
		{
			item->setIcon(QIcon(QPixmap(":res/icons/dance2.png")));
		}

		if (package.contains("name"))
		{
			QFontMetrics metrics(mainWidget_->font());
			auto name = QString::fromUtf8(package["name"].get<nlohmann::json::string_t>());
			item->setText(metrics.elidedText(name, Qt::ElideRight, listWidget_->iconSize().width()));
			item->setToolTip(name);
		}

		listWidget_->addItem(item.release());
	}

	void
	MotionDock::keyPressEvent(QKeyEvent * event) noexcept
	{
		try
		{
			if (event->key() == Qt::Key_Delete)
			{
				if (QMessageBox::question(this, tr("Info"), tr("Are you sure you want to delete this motion data?")) == QMessageBox::Yes)
				{
					if (clickedItem_)
					{
						auto uuid = clickedItem_->data(Qt::UserRole).toString();
						AssetLibrary::instance()->removeAsset(uuid.toStdString());
						listWidget_->takeItem(listWidget_->row(clickedItem_));
						delete clickedItem_;
						clickedItem_ = listWidget_->currentItem();
						AssetLibrary::instance()->saveAssets();
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());
		}
	}

	void
	MotionDock::importClickEvent()
	{
		QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("Import Resource"), "", tr("VMD Files (*.vmd)"));
		if (!filepaths.isEmpty())
		{
			try
			{
				if (filepaths.size() > 1)
				{
					QProgressDialog dialog(tr("Loading..."), tr("Cancel"), 0, filepaths.size(), this);
					dialog.setWindowTitle(tr("Loading..."));
					dialog.setWindowModality(Qt::WindowModal);
					dialog.show();

					for (qsizetype i = 0; i < filepaths.size(); i++)
					{
						dialog.setValue(i);
						dialog.setLabelText(QFileInfo(filepaths[i]).fileName());

						QCoreApplication::processEvents();
						if (dialog.wasCanceled())
							break;

						auto package = AssetLibrary::instance()->importAsset(filepaths[i].toStdWString());
						if (!package.is_null())
							this->addItem(package["uuid"].get<nlohmann::json::string_t>());
					}
				}
				else
				{
					auto package = AssetLibrary::instance()->importAsset(filepaths[0].toStdWString());
					if (!package.is_null())
						this->addItem(package["uuid"].get<nlohmann::json::string_t>());
				}

				AssetLibrary::instance()->saveAssets();
			}
			catch (...)
			{
				AssetLibrary::instance()->saveAssets();
			}
		}
	}

	void
	MotionDock::itemClicked(QListWidgetItem* item)
	{
		clickedItem_ = item;
	}

	void
	MotionDock::itemSelected(QListWidgetItem* item)
	{
		try
		{
			if (item)
			{
				auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
				if (!behaviour)
					return;

				QProgressDialog dialog(tr("Loading..."), tr("Cancel"), 0, 2, this);
				dialog.setWindowTitle(tr("Loading..."));
				dialog.setWindowModality(Qt::WindowModal);
				dialog.setValue(0);
				dialog.show();

				QCoreApplication::processEvents();

				auto uuid = item->data(Qt::UserRole).toString().toStdString();
				auto package = AssetLibrary::instance()->getPackage((std::string)uuid);
				if (package.is_object())
				{
					auto animation = AssetLibrary::instance()->loadAsset<octoon::Animation>(uuid);
					if (animation)
					{
						dialog.setValue(1);
						QCoreApplication::processEvents();

						if (animation->hasClip("Camera"))
							profile_->cameraModule->animation = animation;
						else
						{
							auto selectedItem = behaviour->getProfile()->selectorModule->selectedItemHover_.getValue();
							if (selectedItem.has_value())
							{
								if (animation && !animation->clips.empty())
								{
									auto model = selectedItem->object.lock();
									auto animator = model->getComponent<octoon::AnimatorComponent>();
									auto smr = model->getComponent<octoon::SkinnedMeshRendererComponent>();

									if (animator)
										animator->setAnimation(std::move(animation));
									else
									{
										if (smr)
											animator = model->addComponent<octoon::AnimatorComponent>(std::move(animation), smr->getBones());
										else
											animator = model->addComponent<octoon::AnimatorComponent>(std::move(animation));
									}

									animator->setName(package["name"].get<nlohmann::json::string_t>());
									animator->sample();

									if (smr)
									{
										for (auto& transform : smr->getBones())
										{
											auto solver = transform->getComponent<octoon::CCDSolverComponent>();
											if (solver)
												solver->solve();
										}
									}

									octoon::AssetDatabase::instance()->setDirty(model, true);
								}
							}
						}

						behaviour->getComponent<PlayerComponent>()->updateTimeLength();
					}

					dialog.setValue(2);
				}
			}
		}
		catch (const std::exception& e)
		{
			QMessageBox::information(this, tr("Error"), e.what());
		}
	}

	void
	MotionDock::resizeEvent(QResizeEvent* e) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins() + topLayout_->contentsMargins();
		listWidget_->resize(this->width(), mainWidget_->height() - margins.top() - margins.bottom() - title_->height());
	}

	void
	MotionDock::showEvent(QShowEvent* event) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins() + topLayout_->contentsMargins();
		listWidget_->resize(this->width(), mainWidget_->height() - margins.top() - margins.bottom() - title_->height());
		listWidget_->clear();

		for (auto& package : AssetLibrary::instance()->getMotionList())
			this->addItem(package);
	}

	bool
	MotionDock::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() != QEvent::Paint &&
			event->type() != QEvent::Resize)
		{
			if (profile_->playerModule->isPlaying)
			{
				return true;
			}
		}

		return QWidget::eventFilter(watched, event);
	}
}