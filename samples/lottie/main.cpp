#include <octoon/octoon.h>
#include <octoon/octoon-c.h>
#include <octoon/runtime/json.h>
#include <octoon/video/basic_material.h>

#include <octoon/transform_anim_component.h>

#include "transform_helper.h"

const char* data = R"({"v":"5.1.17","fr":29.9700012207031,"ip":0,"op":900.000036657751,"w":720,"h":480,"nm":"Comp 1","ddd":0,"assets":[{"id":"image_0","w":1920,"h":1080,"u":"images/","p":"img_0.png"}],"layers":[{"ddd":0,"ind":1,"ty":2,"nm":"1.png","cl":"png","refId":"image_0","sr":1,"ks":{"o":{"a":0,"k":100,"ix":11},"r":{"a":0,"k":0,"ix":10},"p":{"a":0,"k":[360,240,0],"ix":2},"a":{"a":0,"k":[960,540,0],"ix":1},"s":{"a":0,"k":[100,100,100],"ix":6}},"ao":0,"ip":0,"op":900.000036657751,"st":0,"bm":0}],"markers":[]})";

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
		w = j["w"];
		h = j["h"];
		fps_ = j["fr"];

		return ::OctoonOpenWindow("Octoon Studio", w, h);
	}

	bool prepareAssets(const json& j)
	{
		return true;
	}

	bool prepareLayers(const json& j)
	{
		camera_ = octoon::GamePrefabs::instance()->createCamera2D();
		camera_->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.0f, 0.0f, 0.0f, 1.0));

		for (auto& layer : j["layers"])
		{
			octoon::GameObjectPtr object;

			auto type = (LayerTypes)layer["ty"].get<json::number_unsigned_t>();
			switch (type)
			{
			case LayerTypes::precomp:
			break;
			case LayerTypes::solid:
			break;
			case LayerTypes::still:
			break;
			case LayerTypes::nullLayer:
			break;
			case LayerTypes::shape:
			break;
			case LayerTypes::text:
			{
				auto& t = layer["t"]["d"]["k"][0]["s"];
				auto& fc = t["fc"];

				auto text = t["t"].get<json::string_t>();
				auto size = t["s"].get<json::number_unsigned_t>();
				auto color = octoon::math::float4(fc[0].get<json::number_float_t>(), fc[1].get<json::number_float_t>(), fc[2].get<json::number_float_t>(), 1.0f);
				
				object = octoon::GamePrefabs::instance()->createText(text.c_str(), size);
				std::dynamic_pointer_cast<octoon::video::BasicMaterial>(object->getComponent<octoon::RenderComponent>()->getMaterial())->setBaseColor(color);
				object->addComponent<octoon::GuizmoComponent>(camera_);
			}
			break;
			case LayerTypes::audio:
			break;
			case LayerTypes::pholderVideo:
			break;
			case LayerTypes::imageSeq:
			break;
			case LayerTypes::video:
			break;
			case LayerTypes::pholderStill:
			break;
			case LayerTypes::guide:
			break;
			case LayerTypes::adjustment:
			break;
			case LayerTypes::camera:
			break;
			case LayerTypes::light:
			break;
			default:
				break;
			}

			auto& ks = layer["ks"];
			if (!ks.is_null())
			{
				TransformHelper t(ks, fps_);

				auto transform = object->addComponent<octoon::TransformAnimComponent>();
				transform->setTranslate(t.pos);
				transform->setScale(t.scale);
				transform->setRotation(t.rotation);
			}

			layers_.push_back(std::move(object));
		}

		return true;
	}

private:
	float fps_;
	std::uint32_t w, h;

	octoon::GameObjects layers_;
	octoon::GameObjectPtr camera_;
};

#include <octoon/model/animation_curve.h>

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	auto json = octoon::runtime::json::parse(data);

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