#include "asset_browse_dialog.h"
#include "../utils/asset_library.h"
#include <octoon/environment_light_component.h>
#include <octoon/texture/texture.h>
#include <octoon/asset_database.h>
#include <octoon/mesh_renderer_component.h>

#include <qapplication.h>
#include <qdrag.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qmimedata.h>
#include <qpainter.h>
#include <qprogressdialog.h>
#include <qgraphicseffect.h>

namespace unreal
{
	MaterialListDialog::MaterialListDialog(QWidget* parent, const octoon::GameObjectPtr& behaviour)
		: QDialog(parent)
		, behaviour_(behaviour)
		, clickedItem_(nullptr)
	{
		this->setObjectName("MaterialDialog");
		this->setWindowTitle(tr("Material Resource"));
		this->setFixedSize(720, 480);
		this->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

		okButton_ = new QToolButton;
		okButton_->setText(tr("Ok"));
		okButton_->setFixedSize(40, 24);

		closeButton_ = new QToolButton;
		closeButton_->setText(tr("Close"));
		closeButton_->setFixedSize(40, 24);
		closeButton_->setShortcut(tr("ESC"));

		importButton_ = new QToolButton;
		importButton_->setText(tr("Import"));
		importButton_->setFixedSize(48, 24);
		
		mainWidget_ = new QListWidget;
		mainWidget_->setResizeMode(QListView::Fixed);
		mainWidget_->setViewMode(QListView::IconMode);
		mainWidget_->setMovement(QListView::Static);
		mainWidget_->setIconSize(QSize(85, 85));
		mainWidget_->setDefaultDropAction(Qt::DropAction::MoveAction);
		mainWidget_->setStyleSheet("background:transparent;");
		mainWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		mainWidget_->setSpacing(6);

		auto topLayout_ = new QHBoxLayout();
		topLayout_->addWidget(importButton_, 0, Qt::AlignLeft);
		topLayout_->addStretch();
		topLayout_->setContentsMargins(10, 0, 0, 0);

		auto bottomLayout_ = new QHBoxLayout();
		bottomLayout_->addStretch();
		bottomLayout_->addWidget(okButton_, 0, Qt::AlignRight);
		bottomLayout_->addWidget(closeButton_, 0, Qt::AlignRight);
		bottomLayout_->setSpacing(2);
		bottomLayout_->setContentsMargins(0, 4, 12, 0);

		mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addLayout(topLayout_);
		mainLayout_->addWidget(mainWidget_);
		mainLayout_->addStretch();
		mainLayout_->addLayout(bottomLayout_);
		mainLayout_->setContentsMargins(4, 8, 4, 8);

		connect(okButton_, SIGNAL(clicked()), this, SLOT(okClickEvent()));
		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeClickEvent()));
		connect(importButton_, SIGNAL(clicked()), this, SLOT(importClickEvent()));
		connect(mainWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		connect(mainWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
	}

	MaterialListDialog::~MaterialListDialog() noexcept
	{
	}

	void 
	MaterialListDialog::addItem(const nlohmann::json& package) noexcept(false)
	{
		if (package.is_object())
		{
			auto item = std::make_unique<QListWidgetItem>();
			item->setData(Qt::UserRole, QString::fromStdString(package["uuid"].get<nlohmann::json::string_t>()));
			item->setSizeHint(mainWidget_->iconSize() + QSize(10, 40));

			if (package.contains("preview"))
			{
				auto filepath = QString::fromStdWString(AssetLibrary::instance()->getAssetPath(package["preview"].get<std::string>(), true).wstring());
				item->setIcon(QIcon(QPixmap(filepath)));
			}

			if (package.contains("name"))
			{
				QFontMetrics metrics(mainWidget_->font());
				auto name = QString::fromUtf8(package["name"].get<nlohmann::json::string_t>());
				item->setText(metrics.elidedText(name, Qt::ElideRight, mainWidget_->iconSize().width()));
				item->setToolTip(name);
			}

			mainWidget_->addItem(item.release());
		}
	}

	void
	MaterialListDialog::importClickEvent()
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
	MaterialListDialog::itemClicked(QListWidgetItem* item)
	{
		clickedItem_ = item;
	}

	void
	MaterialListDialog::itemDoubleClicked(QListWidgetItem* item)
	{
		if (item)
		{
			this->close();
			emit itemSelected(item);
		}
	}

	void
	MaterialListDialog::keyPressEvent(QKeyEvent * event) noexcept
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
						mainWidget_->takeItem(mainWidget_->row(clickedItem_));
						delete clickedItem_;
						clickedItem_ = mainWidget_->currentItem();
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
	MaterialListDialog::okClickEvent()
	{
		if (clickedItem_)
		{
			this->close();
			emit itemSelected(clickedItem_);
			clickedItem_ = nullptr;
		}
	}

	void
	MaterialListDialog::closeClickEvent()
	{
		this->close();
	}

	void
	MaterialListDialog::resizeEvent(QResizeEvent* e) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins();
		mainWidget_->resize(this->width(), this->height() - (margins.top() + margins.bottom()) * 2 - okButton_->height() - importButton_->height());
	}

	void
	MaterialListDialog::showEvent(QShowEvent* event) noexcept
	{
		auto behaviour = behaviour_->getComponent<unreal::UnrealBehaviour>();
		if (behaviour)
		{
			mainWidget_->clear();

			for (auto& package : AssetLibrary::instance()->getMaterialList())
				this->addItem(package);
		}
	}

	EnvironmentListDialog::EnvironmentListDialog(QWidget * parent, const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile)
		: QDialog(parent)
		, behaviour_(behaviour)
		, profile_(profile)
		, clickedItem_(nullptr)
	{
		this->setObjectName("EnvironmentDialog");
		this->setWindowTitle(tr("Environment Resource"));
		this->setFixedSize(720, 480);
		this->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

		okButton_ = new QToolButton;
		okButton_->setObjectName("Ok");
		okButton_->setText(tr("Ok"));
		
		closeButton_ = new QToolButton;
		closeButton_->setObjectName("Close");
		closeButton_->setText(tr("Close"));
		closeButton_->setShortcut(tr("ESC"));

		importButton_ = new QToolButton;
		importButton_->setObjectName("Import");
		importButton_->setText(tr("Import"));

		auto topLayout_ = new QHBoxLayout();
		topLayout_->addWidget(importButton_, 0, Qt::AlignLeft);
		topLayout_->addStretch();
		topLayout_->setContentsMargins(12, 0, 0, 0);

		auto bottomLayout_ = new QHBoxLayout();
		bottomLayout_->addStretch();
		bottomLayout_->addWidget(okButton_, 0, Qt::AlignRight);
		bottomLayout_->addWidget(closeButton_, 0, Qt::AlignRight);
		bottomLayout_->setSpacing(2);
		bottomLayout_->setContentsMargins(0, 4, 8, 0);

		listWidget_ = new QListWidget;
		listWidget_->setIconSize(QSize(160, 80));
		listWidget_->setResizeMode(QListView::Fixed);
		listWidget_->setViewMode(QListView::IconMode);
		listWidget_->setMovement(QListView::Static);
		listWidget_->setDefaultDropAction(Qt::DropAction::MoveAction);
		listWidget_->setStyleSheet("background:transparent;");
		listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		listWidget_->setSpacing(8);

		mainLayout_ = new QVBoxLayout(this);
		mainLayout_->addLayout(topLayout_);
		mainLayout_->addWidget(listWidget_);
		mainLayout_->addStretch();
		mainLayout_->addLayout(bottomLayout_);
		mainLayout_->setContentsMargins(4, 8, 4, 8);

		connect(okButton_, SIGNAL(clicked()), this, SLOT(okClickEvent()));
		connect(closeButton_, SIGNAL(clicked()), this, SLOT(closeClickEvent()));
		connect(importButton_, SIGNAL(clicked()), this, SLOT(importClickEvent()));
		connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
		connect(listWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
	}

	EnvironmentListDialog::~EnvironmentListDialog() noexcept
	{
	}

	void
	EnvironmentListDialog::addItem(const nlohmann::json& package) noexcept(false)
	{
		if (package.is_object())
		{
			auto item = std::make_unique<QListWidgetItem>();
			item->setData(Qt::UserRole, QString::fromStdString(package["uuid"].get<nlohmann::json::string_t>()));
			item->setSizeHint(listWidget_->iconSize() + QSize(8, 40));

			if (package.contains("preview"))
			{
				auto filepath = QString::fromStdWString(AssetLibrary::instance()->getAssetPath(package["preview"].get<std::string>(), true).wstring());
				item->setIcon(QIcon(QPixmap(filepath)));
			}

			if (package.contains("name"))
			{
				QFontMetrics metrics(listWidget_->font());
				auto name = QString::fromUtf8(package["name"].get<nlohmann::json::string_t>());
				item->setText(metrics.elidedText(name, Qt::ElideRight, listWidget_->iconSize().width()));
				item->setToolTip(name);
			}

			listWidget_->addItem(item.release());
		}
	}

	void
	EnvironmentListDialog::importClickEvent()
	{
		QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("Import Resource"), "", tr("HDRi Files (*.hdr)"));
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
			}
			catch (...)
			{
				AssetLibrary::instance()->saveAssets();
			}
		}
	}

	void
	EnvironmentListDialog::itemClicked(QListWidgetItem * item)
	{
		clickedItem_ = item;
	}

	void
	EnvironmentListDialog::itemDoubleClicked(QListWidgetItem * item)
	{
		this->close();

		if (item)
			emit itemSelected(item);
	}

	void
	EnvironmentListDialog::okClickEvent()
	{
		this->close();

		if (clickedItem_)
			emit itemSelected(clickedItem_);
	}

	void
	EnvironmentListDialog::closeClickEvent()
	{
		this->close();
	}

	void
	EnvironmentListDialog::resizeEvent(QResizeEvent * e) noexcept
	{
		QMargins margins = mainLayout_->contentsMargins();
		listWidget_->resize(this->width(), this->height() - (margins.top() + margins.bottom()) * 2 - okButton_->height() - importButton_->height());
	}

	void
	EnvironmentListDialog::showEvent(QShowEvent * event) noexcept
	{
		listWidget_->clear();

		for (auto& package : AssetLibrary::instance()->getHDRiList())
		{
			try
			{
				this->addItem(package);
			}
			catch (...)
			{
			}
		}
	}

	void
	EnvironmentListDialog::keyPressEvent(QKeyEvent * event) noexcept
	{
		try
		{
			if (event->key() == Qt::Key_Delete)
			{
				if (clickedItem_)
				{
					if (QMessageBox::question(this, tr("Info"), tr("Are you sure you want to delete this picture?")) == QMessageBox::Yes)
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
}