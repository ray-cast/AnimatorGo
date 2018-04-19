#include <octoon/path_meshing_component.h>
#include <octoon/video/mesh.h>
#include <octoon/video/text_contour_group.h>
#include <octoon/runtime/except.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>

// #include <json/json.h>

#define POD_TT_PRIM_NONE 0
#define POD_TT_PRIM_LINE 1   	// line to, 一个点［x,y]
#define POD_TT_PRIM_QSPLINE 2	// qudratic bezier to, 三个点［[controlX,controlY]，[endX,endY]］
#define POD_TT_PRIM_CSPLINE 3	// cubic bezier to, 三个点［[control1X,control1Y]，[control2X,control2Y]，[endX,endY]］
#define POD_TT_PRIM_ARCLINE 4	// arc to, 三个点［[startX,startY]，[endX,endY]，[startDegree,sweepAngle]］
#define POD_TT_PRIM_MOVE 8   	// move to, 一个点［x,y]
#define POD_TT_PRIM_CLOSE 9  	// close path, equal to fisrt point of path

namespace octoon
{
	OctoonImplementSubClass(PathMeshingComponent, MeshFilterComponent, "PathMeshingComponent")

	PathMeshingComponent::PathMeshingComponent() noexcept
		: bezierSteps_(6)
	{
	}

	PathMeshingComponent::PathMeshingComponent(std::string&& json, std::uint16_t bezierSteps) noexcept
		: bezierSteps_(6)
	{
		this->setBezierPath(json);
		this->setBezierSteps(bezierSteps);
	}

	PathMeshingComponent::PathMeshingComponent(const std::string& json, std::uint16_t bezierSteps) noexcept
		: bezierSteps_(6)
	{
		this->setBezierPath(json);
		this->setBezierSteps(bezierSteps);
	}

	PathMeshingComponent::~PathMeshingComponent() noexcept
	{
	}

	void
	PathMeshingComponent::setBezierPath(std::string&& json) noexcept
	{
		json_ = std::move(json);

		if (this->getActive())
		{
			auto object = this->getGameObject();
			if (object && object->getActive())
				this->updateContour(json_);
		}
	}

	void
	PathMeshingComponent::setBezierPath(const std::string& json) noexcept
	{
		json_ = json;

		if (this->getActive())
		{
			auto object = this->getGameObject();
			if (object && object->getActive())
				this->updateContour(json_);
		}
	}

	const std::string&
	PathMeshingComponent::getBezierPath() const noexcept
	{
		return json_;
	}

	void
	PathMeshingComponent::setBezierSteps(std::uint16_t bezierSteps) noexcept
	{
		if (bezierSteps_ != bezierSteps)
		{
			bezierSteps_ = bezierSteps;

			if (this->getActive())
			{
				auto object = this->getGameObject();
				if (object && object->getActive())
					this->updateContour(json_);
			}
		}
	}

	std::uint16_t
	PathMeshingComponent::getBezierSteps() const noexcept
	{
		return bezierSteps_;
	}

	void
	PathMeshingComponent::onActivate() noexcept(false)
	{
		if (!json_.empty())
			this->updateContour(json_);
	}

	void
	PathMeshingComponent::onDeactivate() noexcept
	{
	}

	GameComponentPtr
	PathMeshingComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PathMeshingComponent>();
		instance->setBezierSteps(this->getBezierSteps());
		instance->setBezierPath(this->getBezierPath());

		return instance;
	}

	void
	PathMeshingComponent::updateContour(const std::string& json) noexcept(false)
	{
		/*Json::Value root;
		Json::Reader reader;

		if (json.empty())
		{
			this->getComponent<MeshFilter>()->setMesh(std::make_shared<video::Mesh>());
			return;
		}

		if (!reader.parse(json, root))
			throw runtime::runtime_error::create("failed to parse json");

		Json::Value& paths = root.optJSONArray("paths");
		int pathsLength = paths.length();

		video::TextContours contours;

		for (int i = 0; i < pathsLength; i++)
		{
			Json::Value* prev = nullptr;
			Json::Value* cur = nullptr;
			Json::Value& points = paths.opt(i).optJSONArray("points");

			int length = points.length();

			auto contour = std::make_unique<video::TextContour>();

			for (int index = 0; index < length; index++)
			{
				cur = &points.opt(index);

				switch (cur->opt(2).asUInt())
				{
				case POD_TT_PRIM_LINE:
				{
					contour->addPoints(math::float3(cur->opt(0).asFloat(), cur->opt(1).asFloat(), 0));
					prev = cur;
				}
				break;
				case POD_TT_PRIM_QSPLINE:
				{
					auto& p1 = *prev;
					auto& p2 = *cur;
					auto& p3 = points.opt(index + 1);

					math::float3 A(p1.opt(0).asFloat(), p1.opt(1).asFloat(), 0.0);
					math::float3 B(p2.opt(0).asFloat(), p2.opt(1).asFloat(), 0.0);
					math::float3 C(p3.opt(0).asFloat(), p3.opt(1).asFloat(), 0.0);

					contour->addPoints(A, B, C, bezierSteps_);

					prev = &p3;

					index++;
				}
				break;
				case POD_TT_PRIM_CSPLINE:
				{
					auto& p1 = *prev;
					auto& p2 = *cur;
					auto& p3 = points.opt(index + 1);
					auto& p4 = points.opt(index + 2);

					math::float3 A(p1.opt(0).asFloat(), p1.opt(1).asFloat(), 0.0);
					math::float3 B(p2.opt(0).asFloat(), p2.opt(1).asFloat(), 0.0);
					math::float3 C(p3.opt(0).asFloat(), p3.opt(1).asFloat(), 0.0);
					math::float3 D(p4.opt(0).asFloat(), p4.opt(1).asFloat(), 0.0);

					contour->addPoints(A, B, C, D, bezierSteps_);

					prev = &p4;

					index += 2;
				}
				break;
				case POD_TT_PRIM_MOVE:
				{
					contour->addPoints(math::float3(cur->opt(0).asFloat(), cur->opt(1).asFloat(), 0));
					prev = cur;
				}
				break;
				case POD_TT_PRIM_CLOSE:
				{
					contour->addPoints(math::float3(cur->opt(0).asFloat(), cur->opt(1).asFloat(), 0));
					contour->addPoints(contour->at(0));

					prev = cur;
				}
				break;
				default:
					throw runtime::runtime_error::create("invalid type of bezier");
					break;
				}
			}

			contour->isClockwise(true);
			contours.push_back(std::move(contour));
		}

		auto mesh = std::make_shared<video::Mesh>();

		math::float3 center;

		auto contourGroup_ = std::make_shared<video::TextContourGroup>();
		contourGroup_->setContours(std::move(contours));
		contourGroup_->normalize(center);
		contourGroup_->buildMeshes(*mesh);

		this->setMesh(std::move(mesh));
		this->getComponent<Transform>()->setLocalTranslate(center);*/
	}
}