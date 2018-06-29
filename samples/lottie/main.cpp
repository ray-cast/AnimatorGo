#include <octoon/octoon.h>
#include <octoon/octoon-c.h>
#include <octoon/runtime/json.h>

#include <fstream>

#include "transform_helper.h"
#include "shape_helper.h"
#include "solid_helper.h"
#include "still_helper.h"
#include "camera_helper.h"
#include "layer_helper.h"

#include "transform_anim_component.h"

class LottieApp
{
	using json = octoon::runtime::json;

public:
	LottieApp()
	{
	}

	bool prepareCanvas(const json& j)
	{
		width_ = j["w"];
		height_ = j["h"];
		fps_ = j["fr"];
		focalLength_ = 50.0f;
		filmSize_ = 36.0f;
		zoom_ = focalLength_ / filmSize_ * width_;

		auto success = ::OctoonOpenWindow("Octoon Studio", width_, height_);
		if (success)
		{
			camera_ = octoon::GamePrefabs::instance()->createFilmCamera(width_, zoom_);
			camera_->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(width_ / 2.0f, height_ / 2.0f, -zoom_));
		}

		return success;
	}

	bool prepareAssets(const json& j)
	{
		for (auto& asset : j["assets"])
		{
			auto id = asset["id"].get<json::string_t>();

			if (id.compare(0, 5, "image") == 0)
			{
				auto u = "file:C:\\Users\\Administrator\\Desktop\\" + asset["u"].get<json::string_t>();
				auto p = asset["p"].get<json::string_t>();

				_textures[id] = octoon::GamePrefabs::instance()->createTexture(u + p);
			}
			else if (id.compare(0, 4, "comp") == 0)
			{

			}
		}

		return true;
	}

	bool prepareLayers(const json& j)
	{
		auto& layers = j["layers"];
		for (auto it = layers.rbegin(); it != layers.rend(); ++it)
		{
			LayerHelper layer(*it);

			octoon::GameObjectPtr object;

			switch (layer.type)
			{
			case LayerHelper::LayerTypes::solid:
			{
				object = octoon::GamePrefabs::instance()->createSpriteSquare(layer.solid.sw, layer.solid.sh);
				object->getComponent<octoon::RenderComponent>()->getMaterial()->getParameter("color")->uniform4f(octoon::math::float4(layer.solid.color, 1.0f));
			}
			break;
			case LayerHelper::LayerTypes::still:
			{
				object = octoon::GamePrefabs::instance()->createSprite(_textures[layer.still.refid]);
			}
			break;
			case LayerHelper::LayerTypes::text:
			{			
				object = octoon::GamePrefabs::instance()->createText(layer.text.text.c_str(), layer.text.size);
				object->getComponent<octoon::RenderComponent>()->getMaterial()->getParameter("color")->uniform4f(octoon::math::float4(layer.text.color, 1.0f));
			}
			break;
			case LayerHelper::LayerTypes::shape:
			{
				ShapeHelper shape;
			}
			break;
			case LayerHelper::LayerTypes::camera:
			{
				camera_ = nullptr;
				object = octoon::GamePrefabs::instance()->createFilmCamera(width_, layer.camera.zoom[0].value);
			}
			break;
			default:
				continue;
				break;
			}

			// object->setLayer(layer.threeLayer);

			auto transform = object->addComponent<TransformAnimComponent>();
			transform->setAnchorPoint(std::move(layer.transform.anchor));
			transform->setTranslate(std::move(layer.transform.pos));
			transform->setScale(std::move(layer.transform.scale));
			transform->setOrientation(std::move(layer.transform.orientation));
			transform->setRotationX(std::move(layer.transform.rx));
			transform->setRotationY(std::move(layer.transform.ry));
			transform->setRotationZ(std::move(layer.transform.rz));

			layers_.push_back(std::move(object));
		}

		return true;
	}

private:
	float fps_;
	float zoom_;
	float focalLength_;
	float filmSize_;
	std::uint32_t width_, height_;

	octoon::GameObjects layers_;
	octoon::GameObjectPtr camera_;
	std::map<std::string, octoon::graphics::GraphicsTexturePtr> _textures;
};

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	auto json = octoon::runtime::json::parse(std::ifstream("C:\\Users\\Administrator\\Desktop\\data.json"));

	LottieApp app;

	if (app.prepareCanvas(json))
	{
		app.prepareAssets(json);
		app.prepareLayers(json);

		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}