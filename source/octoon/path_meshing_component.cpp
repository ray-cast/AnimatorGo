#include <octoon/path_meshing_component.h>
#include <octoon/model/mesh.h>
#include <octoon/model/contour_group.h>
#include <octoon/runtime/except.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/runtime/json.h>

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
		, clockwise_(true)
	{
	}

	PathMeshingComponent::PathMeshingComponent(std::string&& json, std::uint16_t bezierSteps, bool clockwise) noexcept
		: bezierSteps_(6)
		, clockwise_(clockwise)
	{
		this->setBezierPath(json);
		this->setBezierSteps(bezierSteps);
	}

	PathMeshingComponent::PathMeshingComponent(const std::string& json, std::uint16_t bezierSteps, bool clockwise) noexcept
		: bezierSteps_(6)
		, clockwise_(clockwise)
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
		assert(bezierSteps > 0);

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
	PathMeshingComponent::setClockwise(bool clockwise) noexcept
	{
		clockwise_ = clockwise;

		if (this->getActive())
		{
			auto object = this->getGameObject();
			if (object && object->getActive())
				this->updateContour(json_);
		}
	}

	bool
	PathMeshingComponent::getClockwise() const noexcept
	{
		return clockwise_;
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
		instance->setName(this->getName());
		instance->setClockwise(this->getClockwise());
		instance->setBezierPath(this->getBezierPath());
		instance->setBezierSteps(this->getBezierSteps());

		return instance;
	}

	void
	PathMeshingComponent::updateContour(const std::string& data) noexcept(false)
	{
		if (data.empty())
		{
			this->setMesh(std::make_shared<model::Mesh>());
			return;
		}

		auto reader = json::parse(data);

		model::TextContours contours;

		for (auto& path : reader["paths"])
		{
			json::pointer prev = nullptr;
			json::pointer cur = nullptr;
			json::reference points = path["points"];

			auto contour = std::make_unique<model::Contour>();

			for (int index = 0; index < points.size(); index++)
			{
				cur = &points[index];

				switch (cur->at(2).get<json::number_unsigned_t>())
				{
				case POD_TT_PRIM_LINE:
				{
					contour->addPoints(math::float3(cur->at(0).get<json::number_float_t>(), cur->at(1).get<json::number_float_t>(), 0));
					prev = cur;
				}
				break;
				case POD_TT_PRIM_QSPLINE:
				{
					auto& p1 = *prev;
					auto& p2 = *cur;
					auto& p3 = points[index + 1];

					math::float3 A(p1[0].get<json::number_float_t>(), p1[1].get<json::number_float_t>(), 0.0);
					math::float3 B(p2[0].get<json::number_float_t>(), p2[1].get<json::number_float_t>(), 0.0);
					math::float3 C(p3[0].get<json::number_float_t>(), p3[1].get<json::number_float_t>(), 0.0);

					contour->addPoints(A, B, C, bezierSteps_);

					prev = &p3;

					index++;
				}
				break;
				case POD_TT_PRIM_CSPLINE:
				{
					auto& p1 = *prev;
					auto& p2 = *cur;
					auto& p3 = points[index + 1];
					auto& p4 = points[index + 2];

					math::float3 A(p1[0].get<json::number_float_t>(), p1[1].get<json::number_float_t>(), 0.0);
					math::float3 B(p2[0].get<json::number_float_t>(), p2[1].get<json::number_float_t>(), 0.0);
					math::float3 C(p3[0].get<json::number_float_t>(), p3[1].get<json::number_float_t>(), 0.0);
					math::float3 D(p4[0].get<json::number_float_t>(), p4[1].get<json::number_float_t>(), 0.0);

					contour->addPoints(A, B, C, D, bezierSteps_);

					prev = &p4;

					index += 2;
				}
				break;
				case POD_TT_PRIM_MOVE:
				{
					contour->addPoints(math::float3(cur->at(0).get<json::number_float_t>(), cur->at(1).get<json::number_float_t>(), 0));
					prev = cur;
				}
				break;
				case POD_TT_PRIM_CLOSE:
				{
					contour->addPoints(math::float3(cur->at(0).get<json::number_float_t>(), cur->at(1).get<json::number_float_t>(), 0));
					contour->addPoints(contour->at(0));

					prev = cur;
				}
				break;
				default:
					throw runtime::runtime_error::create("invalid bezier type");
				}
			}

			contour->isClockwise(clockwise_);
			contours.push_back(std::move(contour));
		}

		math::float3 center;

		auto contourGroup_ = std::make_shared<model::ContourGroup>();
		contourGroup_->setContours(std::move(contours));
		contourGroup_->normalize(center);

		auto mesh = model::makeMesh(*contourGroup_);
		mesh.computeVertexNormals();
		mesh.computeBoundingBox();

		this->setMesh(std::move(mesh));
		this->getComponent<TransformComponent>()->setLocalTranslate(center);
	}
}