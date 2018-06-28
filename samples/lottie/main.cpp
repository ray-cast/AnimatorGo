#include <octoon/octoon.h>
#include <octoon/octoon-c.h>
#include <octoon/runtime/json.h>

#include <fstream>

#include "transform_helper.h"
#include "transform_anim_component.h"

class LottieApp
{
	using json = octoon::runtime::json;

	enum class LayerTypes 
	{
		precomp = 0,
		solid = 1,
		still = 2,
		nullLayer = 3,
		shape = 4,
		text = 5,
		audio = 6,
		pholderVideo = 7,
		imageSeq = 8,
		video = 9,
		pholderStill = 10,
		guide = 11,
		adjustment = 12,
		camera = 13,
		light = 14
	};

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
		for (auto& layer : j["layers"])
		{
			octoon::GameObjectPtr object;

			auto type = (LayerTypes)layer["ty"].get<json::number_unsigned_t>();
			switch (type)
			{
			case LayerTypes::still:
			{
				auto refid = layer["refId"].get<json::string_t>();
				auto tex = _textures[refid];
				object = octoon::GamePrefabs::instance()->createSprite(tex, tex->getTextureDesc().getWidth(), tex->getTextureDesc().getHeight());
			}
			break;
			case LayerTypes::text:
			{
				auto& t = layer["t"]["d"]["k"][0]["s"];
				auto& fc = t["fc"];
				auto text = t["t"].get<json::string_t>();
				auto size = t["s"].get<json::number_unsigned_t>();
				auto color = octoon::math::float4(fc[0].get<json::number_float_t>(), fc[1].get<json::number_float_t>(), fc[2].get<json::number_float_t>(), 1.0f);
				
				object = octoon::GamePrefabs::instance()->createText(text.c_str(), size);
				object->getComponent<octoon::RenderComponent>()->getMaterial()->getParameter("color")->uniform4f(color);
			}
			break;
			case LayerTypes::camera:
			{
				auto zoom = layer["co"][0]["v"]["k"].get<json::number_float_t>();

				camera_ = nullptr;
				object = octoon::GamePrefabs::instance()->createFilmCamera(width_, zoom);
				//object->setLayer(1);
			}
			break;
			default:
				continue;
				break;
			}

			/*auto& ddd = layer["ddd"];
			if (ddd.get<json::number_unsigned_t>())
				object->setLayer(1);*/

			auto& ks = layer["ks"];
			if (!ks.is_null())
			{
				TransformHelper t(ks);

				auto transform = object->addComponent<TransformAnimComponent>();
				transform->setTranslate(t.pos);
				transform->setScale(t.scale);
				transform->setOrientation(t.orientation);
				transform->setRotationX(t.rx);
				transform->setRotationY(t.ry);
				transform->setRotationZ(t.rz);
			}

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