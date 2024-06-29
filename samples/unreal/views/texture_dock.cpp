#include "texture_dock.h"
#include "../utils/asset_library.h"
#include "../widgets/draggable_list_widget.h"

#include <qpainter.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qevent.h>
#include <qapplication.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qprogressdialog.h>
#include <qgraphicseffect.h>
#include <QToolButton>

#include <octoon/asset_database.h>

namespace unreal
{
	TextureDock::TextureDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile)
		: behaviour_(behaviour)
		, profile_(profile)
		, clickedItem_(nullptr)
	{
		this->setWindowTitle(tr("Texture Library"));
		this->setObjectName("TextureDock");
		this->setFixedWidth(290);
		this->setFeatures(QDockWidget::NoDockWidgetFeatures);
		
		auto oldTitleBar = this->titleBarWidget();
		this->setTitleBarWidget(new QWidget());
		delete oldTitleBar;

		title_ = new QLabel;
		title_->setObjectName("title");
		title_->setText(tr("Texture Library"));

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
		mainWidget_->setObjectName("TextureWidget");
		mainWidget_->setLayout(mainLayout_);
		
		this->setWidget(mainWidget_);

		this->connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		this->connect(listWidget_, SIGNAL(itemSelected(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
	}

	TextureDock::~TextureDock() noexcept
	{
	}

	void 
	TextureDock::addItem(const nlohmann::json& package) noexcept
	{
		if (package.is_object())
		{
			auto item = std::make_unique<QListWidgetItem>();
			item->setData(Qt::UserRole, QString::fromStdString(package["uuid"].get<nlohmann::json::string_t>()));
			item->setSizeHint(listWidget_->iconSize() + QSize(9, 35));

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
	TextureDock::updateItems() noexcept
	{
		for (auto& package : AssetLibrary::instance()->getTextureList())
			this->addItem(package);
	}

	void
	TextureDock::keyPressEvent(QKeyEvent * event) noexcept
	{
		try
		{
			if (event->key() == Qt::Key_Delete)
			{
				if (QMessageBox::question(this, tr("Info"), tr("Are you sure you want to delete this texture?")) == QMessageBox::Yes)
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
	TextureDock::importClickEvent()
	{
		QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("Import Resource"), "", tr("PMX Files (*.pmx)"));
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

					auto package = AssetLibrary::instance()->importAsset(filepaths[i].toStdWString());
					if (!package.is_null())
						this->addItem(package);
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
	TextureDock::itemClicked(QListWidgetItem* item)
	{
		clickedItem_ = item;
	}

	void
	TextureDock::itemSelected(QListWidgetItem* item)
	{
		if (!item)
			return;
		
		auto uuid = item->data(Qt::UserRole).toString().toStdString();
		if (uuid.empty())
			return;

		auto behaviour = behaviour_->getComponent<UnrealBehaviour>();
		if (behaviour)
		{
			auto selectedItem = behaviour->getProfile()->selectorModule->selectedItemHover_.getValue();
			if (selectedItem)
			{
				auto texture = AssetLibrary::instance()->loadAsset<octoon::Texture>(uuid);
				if (texture)
				{
					auto hit = selectedItem.value();
					auto meshRenderer = hit.object.lock()->getComponent<octoon::MeshRendererComponent>();
					if (meshRenderer)
					{
						auto material = meshRenderer->getMaterial(hit.mesh);
						if (material)
							material->downcast_pointer<octoon::MeshStandardMaterial>()->setColorMap(std::move(texture));
					}
				}
			}
		}
	}

	void
	TextureDock::resizeEvent(QResizeEvent* e) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins() + topLayout_->contentsMargins();
		listWidget_->resize(mainWidget_->width(), mainWidget_->height() - margins.top() - margins.bottom() - title_->height());
	}

	void
	TextureDock::showEvent(QShowEvent* event) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins() + topLayout_->contentsMargins();
		listWidget_->resize(mainWidget_->width(), mainWidget_->height() - margins.top() - margins.bottom() - title_->height());
		listWidget_->clear();

		this->updateItems();
	}

	bool
	TextureDock::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() != QEvent::Paint &&
			event->type() != QEvent::Resize)
		{
			if (profile_->playerModule->isPlaying)
				return true;
		}

		return QWidget::eventFilter(watched, event);
	}
}