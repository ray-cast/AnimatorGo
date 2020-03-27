#include <octoon/model/contour_group.h>
#include <octoon/model/path_group.h>
#include <octoon/mesh/shape_mesh.h>

#include <cstring>
#include <iostream>
#include <GL/glu.h>

namespace octoon
{
	namespace model
	{
		ContourGroup::ContourGroup() noexcept
		{
		}

		ContourGroup::ContourGroup(const PathGroup& pathGroup, std::uint16_t bezierSteps) noexcept
		{
			Contours contrours;

			for (auto& it : pathGroup.getPaths())
				contrours.push_back(std::make_unique<Contour>(*it, bezierSteps));

			this->setContours(contrours);
		}

		ContourGroup::ContourGroup(Contours&& contour) noexcept
		{
			this->setContours(std::move(contour));
		}

		ContourGroup::ContourGroup(const Contours& contour) noexcept
		{
			this->setContours(contour);
		}

		ContourGroup::ContourGroup(const std::initializer_list<ContourPtr>& list) noexcept
		{
			this->setContours(list);
		}

		ContourGroup::~ContourGroup() noexcept
		{
		}

		void
		ContourGroup::setContours(Contours&& contour) noexcept
		{
			contours_ = std::move(contour);
		}

		void
		ContourGroup::setContours(const Contours& contours) noexcept
		{
			contours_.clear();

			for (auto& it : contours)
				contours_.push_back(std::make_unique<Contour>(it->points()));
		}

		void 
		ContourGroup::setContours(const std::initializer_list<ContourPtr>& list) noexcept
		{
			contours_.clear();

			for (auto& it : list)
				contours_.push_back(std::make_unique<Contour>(it->points()));
		}

		void
		ContourGroup::addContours(Contours&& contours) noexcept
		{
			for (auto& contour : contours)
				contours_.push_back(std::move(contour));
		}

		void
		ContourGroup::addContours(const Contours& contours) noexcept
		{
			for (auto& it : contours)
				contours_.push_back(std::make_unique<Contour>(it->points()));
		}

		void 
		ContourGroup::addContours(const std::initializer_list<ContourPtr>& list) noexcept
		{
			for (auto& it : list)
				contours_.push_back(std::make_unique<Contour>(it->points()));
		}

		Contours&
		ContourGroup::getContours() noexcept
		{
			return contours_;
		}

		const Contours&
		ContourGroup::getContours() const noexcept
		{
			return contours_;
		}

		Contour&
		ContourGroup::at(std::size_t index)  noexcept
		{
			assert(index < contours_.size());
			return *contours_[index];
		}

		const Contour&
		ContourGroup::at(std::size_t index) const noexcept
		{
			assert(index < contours_.size());
			return *contours_[index];
		}

		mesh::Mesh makeMesh(const Contours& contours, float thickness) noexcept
		{
			mesh::Mesh mesh;

			math::float3s tris(max_count(contours) * 6);
			math::float3* trisData = tris.data();
			math::float3s& trisMesh = mesh.getVertexArray();

			float thicknessHalf = thickness * 0.5f;

			for (auto& contour : contours)
			{
				std::size_t written = 0;

				for (std::size_t n = 0; n < contour->points().size(); ++n)
				{
					auto& p1 = contour->at(n);
					auto& p2 = (n == contour->points().size() - 1) ? contour->at(0) : contour->at(n + 1);

					trisData[written++] = math::float3(p1.x, p1.y, -thicknessHalf);
					trisData[written++] = math::float3(p2.x, p2.y, thicknessHalf);
					trisData[written++] = math::float3(p1.x, p1.y, thicknessHalf);
					trisData[written++] = math::float3(p1.x, p1.y, -thicknessHalf);
					trisData[written++] = math::float3(p2.x, p2.y, -thicknessHalf);
					trisData[written++] = math::float3(p2.x, p2.y, thicknessHalf);
				}

				trisMesh.resize(trisMesh.size() + written);
				std::memcpy(trisMesh.data() + (trisMesh.size() - written), trisData, written * sizeof(math::float3));
			}

			mesh.computeVertexNormals();
			return mesh;
		}
	
		mesh::Mesh makeMesh(const Contours& contours, float thickness, bool hollow) noexcept
		{
			if (hollow) 
				return makeMesh(contours, thickness);
			else
				return mesh::ShapeMesh(contours, thickness);
		}

		mesh::Mesh makeMesh(const ContourGroup& group, float thickness, bool hollow) noexcept
		{
			return makeMesh(group.getContours(), thickness, hollow);
		}

		mesh::Mesh makeMesh(const ContourGroups& groups, float thickness, bool hollow) noexcept
		{
			mesh::Mesh mesh;

			for (auto& group : groups)
			{
				mesh.mergeMeshes(makeMesh(group->getContours(), thickness));
				if (!hollow) mesh.mergeMeshes(mesh::ShapeMesh(group->getContours(), thickness));
			}
			return mesh;
		}

		mesh::Mesh makeMeshWireframe(const Contours& contours, float thickness) noexcept
		{
			mesh::Mesh mesh;

			math::float3s& tris = mesh.getVertexArray();
			math::uint1s& indices = mesh.getIndicesArray();

			float thicknessHalf = thickness * 0.5f;

			for (auto& contour : contours)
			{
				for (std::size_t n = 0; n < contour->points().size(); ++n)
				{
					auto& p1 = contour->at(n);
					auto& p2 = (n == contour->points().size() - 1) ? contour->at(0) : contour->at(n + 1);

					math::float3 a = math::float3(p1.x, p1.y, p1.z + thicknessHalf);
					math::float3 b = math::float3(p1.x, p1.y, p1.z - thicknessHalf);
					math::float3 c = math::float3(p2.x, p2.y, p2.z - thicknessHalf);
					math::float3 d = math::float3(p2.x, p2.y, p2.z + thicknessHalf);

					math::uint1 index = (math::uint1)tris.size();
					indices.push_back(index);
					indices.push_back(index + 1);
					indices.push_back(index + 1);
					indices.push_back(index + 2);
					indices.push_back(index + 2);
					indices.push_back(index + 3);
					indices.push_back(index + 3);
					indices.push_back(index);

					tris.push_back(a);
					tris.push_back(b);
					tris.push_back(c);
					tris.push_back(d);
				}
			}

			return mesh;
		}

		mesh::Mesh makeMeshWireframe(const ContourGroup& group, float thickness) noexcept
		{
			return makeMeshWireframe(group.getContours(), thickness);
		}

		mesh::Mesh makeMeshWireframe(const ContourGroups& groups, float thickness) noexcept
		{
			mesh::Mesh mesh;

			math::float3s& tris = mesh.getVertexArray();
			math::uint1s& indices = mesh.getIndicesArray();

			float thicknessHalf = thickness * 0.5f;

			for (auto& group : groups)
			{
				for (auto& contour : group->getContours())
				{
					for (std::size_t n = 0; n < contour->points().size(); ++n)
					{
						auto& p1 = contour->at(n);
						auto& p2 = (n == contour->points().size() - 1) ? contour->at(0) : contour->at(n + 1);

						math::float3 a = math::float3(p1.x, p1.y, p1.z + thicknessHalf);
						math::float3 b = math::float3(p1.x, p1.y, p1.z - thicknessHalf);
						math::float3 c = math::float3(p2.x, p2.y, p2.z - thicknessHalf);
						math::float3 d = math::float3(p2.x, p2.y, p2.z + thicknessHalf);

						math::uint1 index = (math::uint1)tris.size();
						indices.push_back(index);
						indices.push_back(index + 1);
						indices.push_back(index + 1);
						indices.push_back(index + 2);
						indices.push_back(index + 2);
						indices.push_back(index + 3);
						indices.push_back(index + 3);
						indices.push_back(index);

						tris.push_back(a);
						tris.push_back(b);
						tris.push_back(c);
						tris.push_back(d);
					}
				}
			}

			return mesh;
		}
	}
}