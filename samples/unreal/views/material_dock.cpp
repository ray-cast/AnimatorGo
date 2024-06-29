#include "material_dock.h"
#include "../utils/asset_library.h"
#include "../widgets/draggable_list_widget.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qevent.h>
#include <qscrollbar.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qapplication.h>
#include <codecvt>
#include <qpainter.h>
#include <qcolordialog.h>
#include <qtreewidget.h>
#include <qprogressdialog.h>
#include <octoon/asset_database.h>

namespace unreal
{
	MaterialDock::MaterialDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept(false)
		: behaviour_(behaviour)
		, profile_(profile)
		, clickedItem_(nullptr)
	{
		this->setObjectName("MaterialDock");
		this->setWindowTitle(tr("Material"));
		this->setFixedWidth(290);
		this->setFeatures(QDockWidget::NoDockWidgetFeatures);

		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		title_ = new QLabel;
		title_->setObjectName("title");
		title_->setText(tr("Material Library"));

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
		
		mainLayout_ = new QVBoxLayout;
		mainLayout_->addLayout(topLayout_);
		mainLayout_->addWidget(listWidget_);
		mainLayout_->setContentsMargins(0, 12, 0, 8);

		mainWidget_ = new QWidget;
		mainWidget_->setObjectName("MaterialWidget");
		mainWidget_->setLayout(mainLayout_);

		this->setWidget(mainWidget_);

		this->connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		this->connect(listWidget_, SIGNAL(itemSelected(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
	}

	MaterialDock::~MaterialDock() noexcept
	{
	}

	void
	MaterialDock::importClickEvent()
	{
		QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("Import Resource"), "", tr("NVIDIA MDL Files (*.mdl)"));
		if (!filepaths.isEmpty())
		{
			try
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

					auto list = AssetLibrary::instance()->importAsset(filepaths[i].toStdWString());
					for (auto& it : list)
						this->addItem(it.get<nlohmann::json::string_t>());
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
	MaterialDock::addItem(const nlohmann::json& package) noexcept(false)
	{
		if (package.is_object())
		{
			auto item = std::make_unique<QListWidgetItem>();
			item->setData(Qt::UserRole, QString::fromStdString(package["uuid"].get<nlohmann::json::string_t>()));
			item->setSizeHint(listWidget_->iconSize() + QSize(9, 40));
			
			if (package.contains("preview"))
			{
				auto filepath = QString::fromStdWString(AssetLibrary::instance()->getAssetPath(package["preview"].get<std::string>(), true).wstring());
				item->setIcon(QIcon(QPixmap(filepath)));
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
	}

	void
	MaterialDock::updateItemList() noexcept
	{
		listWidget_->clear();

		for (auto& it : AssetLibrary::instance()->getMaterialList())
		{
			try
			{
				this->addItem(it);
			}
			catch (...)
			{
			}
		}
	}

	void
	MaterialDock::itemClicked(QListWidgetItem* item)
	{
		clickedItem_ = item;
	}

	void
	MaterialDock::itemSelected(QListWidgetItem* item)
	{
		if (item)
		{
			auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
			if (!behaviour)
				return;

			auto selectedItem = behaviour->getProfile()->selectorModule->selectedItemHover_.getValue();
			if (selectedItem)
			{
				auto uuid = item->data(Qt::UserRole).toString().toStdString();
				auto material = AssetLibrary::instance()->loadAsset<octoon::Material>(uuid);
				if (material)
				{
					auto hit = selectedItem.value();
					auto meshRenderer = hit.object.lock()->getComponent<octoon::MeshRendererComponent>();
					if (meshRenderer)
						meshRenderer->setMaterial(material, hit.mesh);
				}
			}
		}
	}

	void
	MaterialDock::keyPressEvent(QKeyEvent * event) noexcept
	{
		try
		{
			if (event->key() == Qt::Key_Delete)
			{
				if (QMessageBox::question(this, tr("Info"), tr("Are you sure you want to delete this material?")) == QMessageBox::Yes)
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
	MaterialDock::resizeEvent(QResizeEvent* e) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins() + topLayout_->contentsMargins();
		listWidget_->resize(mainWidget_->width(), mainWidget_->height() - margins.top() - margins.bottom() - title_->height());
	}

	void
	MaterialDock::showEvent(QShowEvent* event) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins() + topLayout_->contentsMargins();
		listWidget_->resize(mainWidget_->width(), mainWidget_->height() - margins.top() - margins.bottom() - title_->height());
		listWidget_->show();

		this->updateItemList();
	}

	bool
	MaterialDock::eventFilter(QObject* watched, QEvent* event)
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