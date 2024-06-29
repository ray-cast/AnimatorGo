#ifndef UNREAL_MATERIAL_EDIT_WINDOW_H_
#define UNREAL_MATERIAL_EDIT_WINDOW_H_

#include <qwidget>
#include <qdialog.h>
#include <qboxlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlistwidget.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <qcolordialog.h>
#include <optional>

#include "unreal_behaviour.h"
#include "asset_browse_dialog.h"
#include "../widgets/color_dialog.h"
#include "../widgets/spoiler.h"
#include <octoon/game_object.h>

namespace unreal
{
	class MaterialEditWindow final : public QWidget
	{
		Q_OBJECT
	public:
		MaterialEditWindow(const octoon::GameObjectPtr& behaviour);
		~MaterialEditWindow();

		void updateMaterial();
		void updatePreviewImage();

		void setMaterial(const std::shared_ptr<octoon::Material>& material);

	private:
		void setAlbedoMap(const QString& fileName);
		void setOpacityMap(const QString& fileName);
		void setNormalMap(const QString& fileName);
		void setRoughnessMap(const QString& fileName);
		void setSpecularMap(const QString& fileName);
		void setMetalnessMap(const QString& fileName);
		void setAnisotropyMap(const QString& fileName);
		void setSheenMap(const QString& fileName);
		void setClearCoatMap(const QString& fileName);
		void setClearCoatRoughnessMap(const QString& fileName);
		void setSubsurfaceMap(const QString& fileName);
		void setSubsurfaceColorMap(const QString& fileName);
		void setEmissiveMap(const QString& fileName);

	public Q_SLOTS:
		void previewButtonClickEvent();

		void itemSelected(QListWidgetItem*);

		void colorClickEvent();
		void colorChangeEvent(const QColor &color);
		void emissiveClickEvent();
		void emissiveChangeEvent(const QColor &color);

		void colorMapClickEvent();
		void normalMapClickEvent();
		void opacityMapClickEvent();
		void smoothnessMapClickEvent();
		void specularMapClickEvent();
		void metalnessMapClickEvent();
		void anisotropyMapClickEvent();
		void clearcoatMapClickEvent();
		void clearcoatRoughnessMapClickEvent();
		void sheenMapClickEvent();
		void subsurfaceMapClickEvent();
		void subsurfaceColorMapClickEvent();
		void emissiveMapClickEvent();

		void colorMapCheckEvent(int);
		void normalMapCheckEvent(int);
		void opacityMapCheckEvent(int);
		void smoothnessMapCheckEvent(int);
		void specularMapCheckEvent(int);
		void metalnessMapCheckEvent(int);
		void anisotropyMapCheckEvent(int);
		void clearcoatMapCheckEvent(int);
		void clearcoatRoughnessMapCheckEvent(int);
		void sheenMapCheckEvent(int);
		void subsurfaceMapCheckEvent(int);
		void subsurfaceColorMapCheckEvent(int);
		void emissiveMapCheckEvent(int);

		void opacityEditEvent(double);
		void opacitySliderEvent(int);

		void roughnessEditEvent(double);
		void roughnessSliderEvent(int);

		void metalEditEvent(double);
		void metalSliderEvent(int);

		void specularEditEvent(double);
		void specularSliderEvent(int);
		
		void anisotropyEditEvent(double);
		void anisotropySliderEvent(int);

		void clearcoatEditEvent(double);
		void clearcoatSliderEvent(int);

		void clearcoatRoughnessEditEvent(double);
		void clearcoatRoughnessSliderEvent(int);

		void sheenEditEvent(double);
		void sheenSliderEvent(int);

		void subsurfaceEditEvent(double);
		void subsurfaceSliderEvent(int);
		void subsurfaceColorClickEvent();
		void subsurfaceColorChangeEvent(const QColor& color);

		void refractionEditEvent(double);
		void refractionSliderEvent(int);
		void refractionIorEditEvent(double);
		void refractionIorSliderEvent(int);

		void emissiveEditEvent(double);
		void emissiveSliderEvent(int);

		void shadowCheckEvent(int);

		void closeEvent();

	public:
		enum CreateFlags
		{
			SpoilerBit = 1 << 0,
			ColorBit = 1 << 1,
			ValueBit = 1 << 2,
			TextureBit = 1 << 3,
		};

		struct MaterialUi
		{
			QToolButton* image;
			QCheckBox* check;
			QLabel* title;
			QLabel* path;
			QLabel* label_;
			QToolButton* color;
			QSlider* slider;
			QDoubleSpinBox* spinBox;

			QHBoxLayout* titleLayout;
			QVBoxLayout* rightLayout;
			QHBoxLayout* mapLayout;
			QLayout* mainLayout;

			Spoiler* spoiler;

			std::shared_ptr<octoon::Texture> texture;

			void init(const QString& name, std::uint32_t flags);
			void resetState();
			std::shared_ptr<octoon::Texture> setImage(const QString& path);
		};

		MaterialUi albedo_;
		MaterialUi opacity_;
		MaterialUi normal_;
		MaterialUi roughness_;
		MaterialUi metalness_;
		MaterialUi specular_;
		MaterialUi anisotropy_;
		MaterialUi clearcoat_;
		MaterialUi clearcoatRoughness_;
		MaterialUi sheen_;
		MaterialUi subsurface_;
		MaterialUi subsurfaceValue_;
		MaterialUi refraction_;
		MaterialUi refractionIor_;
		MaterialUi emissive_;

		Spoiler* clearCoatSpoiler_;
		Spoiler* subsurfaceSpoiler_;
		Spoiler* refractionSpoiler_;
		Spoiler* othersSpoiler_;

		QLabel* previewNameLabel_;
		QToolButton* previewButton_;
		QColorDialog albedoColor_;
		QColorDialog emissiveColor_;
		QColorDialog subsurfaceColor_;
		QCheckBox* receiveShadowCheck_;
		QToolButton* backButton_;

		MaterialListDialog* materialListDialog_;
		std::shared_ptr<octoon::MeshStandardMaterial> material_;
		octoon::GameObjectPtr behaviour_;
	};
}

#endif