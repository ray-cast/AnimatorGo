#include <octoon/octoon.h>
#include <octoon/octoon-c.h>
#include <octoon/runtime/json.h>
#include <octoon/video/basic_material.h>

#include <octoon/transform_anim_component.h>

#include "transform_helper.h"

const char* data = R"({"v":"5.1.17","fr":29.9700012207031,"ip":0,"op":900.000036657751,"w":720,"h":480,"nm":"Comp 1","ddd":0,"assets":[{"id":"image_0","w":1920,"h":1080,"u":"images/","p":"img_0.png"}],"fonts":{"list":[{"fName":"SimHei","fFamily":"SimHei","fStyle":"Regular","ascent":66.9060571286827}]},"layers":[{"ddd":0,"ind":1,"ty":5,"nm":"B","sr":1,"ks":{"o":{"a":0,"k":100,"ix":11},"r":{"a":0,"k":55.125,"ix":10},"p":{"a":0,"k":[458,302,0],"ix":2},"a":{"a":0,"k":[0,0,0],"ix":1},"s":{"a":0,"k":[100,100,100],"ix":6}},"ao":0,"t":{"d":{"k":[{"s":{"s":256,"f":"SimHei","t":"B","j":0,"tr":0,"lh":307.2,"ls":0,"fc":[0,0.26,1],"sc":[0,0,0],"sw":0.00999999977648,"of":true},"t":0}]},"p":{},"m":{"g":1,"a":{"a":0,"k":[0,0],"ix":2}},"a":[]},"ip":0,"op":900.000036657751,"st":0,"bm":0},{"ddd":0,"ind":2,"ty":5,"nm":"A","sr":1,"ks":{"o":{"a":0,"k":100,"ix":11},"r":{"a":0,"k":0,"ix":10},"p":{"a":0,"k":[98,350,0],"ix":2},"a":{"a":0,"k":[0,0,0],"ix":1},"s":{"a":0,"k":[100,100,100],"ix":6}},"ao":0,"t":{"d":{"k":[{"s":{"s":256,"f":"SimHei","t":"A","j":0,"tr":0,"lh":307.2,"ls":0,"fc":[0,0.26,1],"sc":[0,0,0],"sw":0.00999999977648,"of":true},"t":0}]},"p":{},"m":{"g":1,"a":{"a":0,"k":[0,0],"ix":2}},"a":[]},"ip":0,"op":900.000036657751,"st":0,"bm":0},{"ddd":0,"ind":3,"ty":2,"nm":"timg.jpg","cl":"jpg","refId":"image_0","sr":1,"ks":{"o":{"a":0,"k":100,"ix":11},"r":{"a":0,"k":145.327,"ix":10},"p":{"a":0,"k":[360,240,0],"ix":2},"a":{"a":0,"k":[960,540,0],"ix":1},"s":{"a":0,"k":[100,100,100],"ix":6}},"ao":0,"ip":0,"op":900.000036657751,"st":0,"bm":0}],"markers":[],"chars":[{"ch":"A","size":256,"style":"Regular","w":50,"data":{"shapes":[{"ty":"gr","it":[{"ind":0,"ty":"sh","ix":1,"ks":{"a":0,"k":{"i":[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]],"v":[[28.516,-69.531],[20.703,-69.531],[1.172,-1.953],[10.547,-1.953],[16.016,-22.656],[33.203,-22.656],[38.672,-1.953],[48.047,-1.953]],"c":true},"ix":2},"nm":"A","mn":"ADBE Vector Shape - Group","hd":false},{"ind":1,"ty":"sh","ix":2,"ks":{"a":0,"k":{"i":[[0,0],[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0],[0,0]],"v":[[17.969,-30.078],[24.219,-53.906],[25,-53.906],[31.25,-30.078]],"c":true},"ix":2},"nm":"A","mn":"ADBE Vector Shape - Group","hd":false}],"nm":"A","np":5,"cix":2,"ix":1,"mn":"ADBE Vector Group","hd":false}]},"fFamily":"SimHei"},{"ch":"B","size":256,"style":"Regular","w":50,"data":{"shapes":[{"ty":"gr","it":[{"ind":0,"ty":"sh","ix":1,"ks":{"a":0,"k":{"i":[[2.472,2.478],[3.125,0.781],[-2.216,2.216],[0,4.688],[3.253,3.387],[6.506,0],[0,0],[0,0],[0,0],[-4.169,2.869],[0,7.556]],"o":[[-2.478,-2.472],[2.344,-0.781],[2.209,-2.209],[0,-4.688],[-3.259,-3.381],[0,0],[0,0],[0,0],[6.506,0],[4.163,-2.863],[0,-4.163]],"v":[[42.383,-31.836],[33.984,-36.719],[40.82,-41.211],[44.141,-51.563],[39.258,-63.672],[24.609,-68.75],[4.297,-68.75],[4.297,-1.953],[23.828,-1.953],[39.844,-6.25],[46.094,-21.875]],"c":true},"ix":2},"nm":"B","mn":"ADBE Vector Shape - Group","hd":false},{"ind":1,"ty":"sh","ix":2,"ks":{"a":0,"k":{"i":[[2.081,-1.691],[2.863,0],[0,0],[0,0],[0,0],[-1.953,-1.563],[0,-3.381]],"o":[[-2.087,1.697],[0,0],[0,0],[0,0],[2.863,0],[1.953,1.563],[0,3.906]],"v":[[32.031,-43.164],[24.609,-40.625],[13.281,-40.625],[13.281,-61.328],[25,-61.328],[32.227,-58.984],[35.156,-51.563]],"c":true},"ix":2},"nm":"B","mn":"ADBE Vector Shape - Group","hd":false},{"ind":2,"ty":"sh","ix":3,"ks":{"a":0,"k":{"i":[[2.472,-1.819],[4.688,0],[0,0],[0,0],[0,0],[-2.734,-1.691],[0,-4.163]],"o":[[-2.478,1.825],[0,0],[0,0],[0,0],[3.381,0],[2.734,1.697],[0,4.688]],"v":[[33.008,-12.109],[22.266,-9.375],[13.281,-9.375],[13.281,-33.203],[23.438,-33.203],[32.617,-30.664],[36.719,-21.875]],"c":true},"ix":2},"nm":"B","mn":"ADBE Vector Shape - Group","hd":false}],"nm":"B","np":6,"cix":2,"ix":1,"mn":"ADBE Vector Group","hd":false}]},"fFamily":"SimHei"}]})";
//const char* data = R"({"v":"5.1.17","fr":29.9700012207031,"ip":0,"op":900.000036657751,"w":720,"h":480,"nm":"Comp 1","ddd":0,"assets":[],"fonts":{"list":[{"fName":"SimHei","fFamily":"SimHei","fStyle":"Regular","ascent":66.9060571286827}]},"layers":[{"ddd":0,"ind":1,"ty":5,"nm":"B","sr":1,"ks":{"o":{"a":0,"k":100,"ix":11},"r":{"a":0,"k":55.125,"ix":10},"p":{"a":0,"k":[458,302,0],"ix":2},"a":{"a":0,"k":[0,0,0],"ix":1},"s":{"a":0,"k":[100,100,100],"ix":6}},"ao":0,"t":{"d":{"k":[{"s":{"s":256,"f":"SimHei","t":"B","j":0,"tr":0,"lh":307.2,"ls":0,"fc":[0,0.26,1],"sc":[0,0,0],"sw":0.00999999977648,"of":true},"t":0}]},"p":{},"m":{"g":1,"a":{"a":0,"k":[0,0],"ix":2}},"a":[]},"ip":0,"op":900.000036657751,"st":0,"bm":0},{"ddd":0,"ind":2,"ty":5,"nm":"A","sr":1,"ks":{"o":{"a":0,"k":100,"ix":11},"r":{"a":0,"k":0,"ix":10},"p":{"a":0,"k":[98,350,0],"ix":2},"a":{"a":0,"k":[0,0,0],"ix":1},"s":{"a":0,"k":[100,100,100],"ix":6}},"ao":0,"t":{"d":{"k":[{"s":{"s":256,"f":"SimHei","t":"A","j":0,"tr":0,"lh":307.2,"ls":0,"fc":[0,0.26,1],"sc":[0,0,0],"sw":0.00999999977648,"of":true},"t":0}]},"p":{},"m":{"g":1,"a":{"a":0,"k":[0,0],"ix":2}},"a":[]},"ip":0,"op":900.000036657751,"st":0,"bm":0}],"markers":[],"chars":[{"ch":"A","size":256,"style":"Regular","w":50,"data":{"shapes":[{"ty":"gr","it":[{"ind":0,"ty":"sh","ix":1,"ks":{"a":0,"k":{"i":[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]],"v":[[28.516,-69.531],[20.703,-69.531],[1.172,-1.953],[10.547,-1.953],[16.016,-22.656],[33.203,-22.656],[38.672,-1.953],[48.047,-1.953]],"c":true},"ix":2},"nm":"A","mn":"ADBE Vector Shape - Group","hd":false},{"ind":1,"ty":"sh","ix":2,"ks":{"a":0,"k":{"i":[[0,0],[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0],[0,0]],"v":[[17.969,-30.078],[24.219,-53.906],[25,-53.906],[31.25,-30.078]],"c":true},"ix":2},"nm":"A","mn":"ADBE Vector Shape - Group","hd":false}],"nm":"A","np":5,"cix":2,"ix":1,"mn":"ADBE Vector Group","hd":false}]},"fFamily":"SimHei"},{"ch":"B","size":256,"style":"Regular","w":50,"data":{"shapes":[{"ty":"gr","it":[{"ind":0,"ty":"sh","ix":1,"ks":{"a":0,"k":{"i":[[2.472,2.478],[3.125,0.781],[-2.216,2.216],[0,4.688],[3.253,3.387],[6.506,0],[0,0],[0,0],[0,0],[-4.169,2.869],[0,7.556]],"o":[[-2.478,-2.472],[2.344,-0.781],[2.209,-2.209],[0,-4.688],[-3.259,-3.381],[0,0],[0,0],[0,0],[6.506,0],[4.163,-2.863],[0,-4.163]],"v":[[42.383,-31.836],[33.984,-36.719],[40.82,-41.211],[44.141,-51.563],[39.258,-63.672],[24.609,-68.75],[4.297,-68.75],[4.297,-1.953],[23.828,-1.953],[39.844,-6.25],[46.094,-21.875]],"c":true},"ix":2},"nm":"B","mn":"ADBE Vector Shape - Group","hd":false},{"ind":1,"ty":"sh","ix":2,"ks":{"a":0,"k":{"i":[[2.081,-1.691],[2.863,0],[0,0],[0,0],[0,0],[-1.953,-1.563],[0,-3.381]],"o":[[-2.087,1.697],[0,0],[0,0],[0,0],[2.863,0],[1.953,1.563],[0,3.906]],"v":[[32.031,-43.164],[24.609,-40.625],[13.281,-40.625],[13.281,-61.328],[25,-61.328],[32.227,-58.984],[35.156,-51.563]],"c":true},"ix":2},"nm":"B","mn":"ADBE Vector Shape - Group","hd":false},{"ind":2,"ty":"sh","ix":3,"ks":{"a":0,"k":{"i":[[2.472,-1.819],[4.688,0],[0,0],[0,0],[0,0],[-2.734,-1.691],[0,-4.163]],"o":[[-2.478,1.825],[0,0],[0,0],[0,0],[3.381,0],[2.734,1.697],[0,4.688]],"v":[[33.008,-12.109],[22.266,-9.375],[13.281,-9.375],[13.281,-33.203],[23.438,-33.203],[32.617,-30.664],[36.719,-21.875]],"c":true},"ix":2},"nm":"B","mn":"ADBE Vector Shape - Group","hd":false}],"nm":"B","np":6,"cix":2,"ix":1,"mn":"ADBE Vector Group","hd":false}]},"fFamily":"SimHei"}]})";

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
		for (auto& asset : j["assets"])
		{
			auto id = asset["id"].get<json::string_t>();
			auto w = asset["w"].get<json::number_integer_t>();
			auto h = asset["h"].get<json::number_integer_t>();
			auto root = "file:C:\\Users\\Administrator\\Desktop\\" + asset["u"].get<json::string_t>();
			auto p = asset["p"].get<json::string_t>();

			_textures[id] = octoon::GamePrefabs::instance()->createTexture(root + p);
		}

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
			{
				auto refid = layer["refId"].get<json::string_t>();
				auto tex = _textures[refid];
				object = octoon::GamePrefabs::instance()->createSprite(tex, tex->getTextureDesc().getWidth(), tex->getTextureDesc().getHeight());
				object->getComponent<octoon::Transform>()->setLocalScale(octoon::math::float3(tex->getTextureDesc().getWidth(), tex->getTextureDesc().getHeight(), 1.0f));
			}
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
				object->getComponent<octoon::RenderComponent>()->getMaterial()->getParameter("color")->uniform4f(color);
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
	std::map<std::string, octoon::graphics::GraphicsTexturePtr> _textures;
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