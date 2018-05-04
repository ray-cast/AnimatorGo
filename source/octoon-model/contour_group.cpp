#include <octoon/model/contour_group.h>
#include <octoon/model/mesh.h>

#include <cstring>
#include <iostream>
#include <GL/glu.h>

namespace octoon
{
	namespace model
	{
		static std::vector<math::float3> g_tris;

		void APIENTRY beginCallback(GLenum which)
		{
			g_tris.clear();
		}

		void APIENTRY endCallback(void)
		{
		}

		void APIENTRY flagCallback(GLboolean)
		{
		}

		void APIENTRY errorCallback(GLenum errorCode)
		{
			std::cerr << "Tessellation Error:" << ::gluErrorString(errorCode) << std::endl;
		}

		void APIENTRY vertexCallback(GLvoid* vertex)
		{
			const GLdouble *d = (GLdouble *)vertex;
			g_tris.emplace_back((float)d[0], (float)d[1], (float)d[2]);
		}

		void APIENTRY combineCallback(GLdouble coords[3], GLdouble* points[4], GLfloat weight[4], GLdouble* dataOut[3])
		{
		}

		ContourGroup::ContourGroup() noexcept
		{
		}

		ContourGroup::ContourGroup(Contours&& contour) noexcept
		{
			this->setContours(std::move(contour));
		}

		ContourGroup::ContourGroup(const Contours& contour) noexcept
		{
			this->setContours(contour);
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
		ContourGroup::setContours(const Contours& textContour) noexcept
		{
			Contours contours;

			for (auto& it : textContour)
			{
				auto contour = std::make_unique<Contour>();
				contour->points() = it->points();
				contour->isClockwise(it->isClockwise());

				contours.push_back(std::move(contour));
			}

			contours_ = std::move(contours);
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

		std::size_t
		ContourGroup::count() const noexcept
		{
			return contours_.size();
		}

		Contour&
		ContourGroup::at(std::size_t index)  noexcept
		{
			assert(index < count());
			return *contours_[index];
		}

		const Contour&
		ContourGroup::at(std::size_t index) const noexcept
		{
			assert(index < count());
			return *contours_[index];
		}

		std::size_t
		ContourGroup::countOfPoints() const noexcept
		{
			std::size_t sum = 0;

			for (auto& it : contours_)
				sum += it->count();

			return sum;
		}

		void
		ContourGroup::normalize(math::float3& center) noexcept
		{
			math::float3 min(std::numeric_limits<float>::max());
			math::float3 max(-std::numeric_limits<float>::max());

			for (auto& contour : contours_)
			{
				for (auto& it : contour->points())
				{
					min = math::min(it, min);
					max = math::max(it, max);
				}
			}

			center = min + (max - min) * 0.5f;

			for (auto& contour : contours_)
			{
				for (auto& it : contour->points())
					it -= center;
			}
		}

		ContourGroupPtr
		ContourGroup::clone() const noexcept
		{
			auto instance = std::make_shared<ContourGroup>();
			instance->setContours(this->getContours());
			return instance;
		}

		Mesh makeMesh(const ContourGroup& group, float thickness) noexcept
		{
			Mesh mesh;

			std::size_t maxElement = 0;
			for (auto& contour : group.getContours())
					maxElement = std::max(maxElement, contour->points().size());

			math::float3s tris(maxElement * 6);
			math::float3* trisData = tris.data();
			math::float3s& trisMesh = mesh.getVertexArray();

			std::vector<math::double3> vertices(group.countOfPoints() * 2);

			for (auto& contour : group.getContours())
			{
				std::size_t written = 0;

				for (std::size_t n = 0; n < contour->count() - 1; ++n)
				{
					auto& p1 = contour->at(n);
					auto& p2 = contour->at(n + 1);

					trisData[written++] = math::float3(p1.x, p1.y, -thickness);
					trisData[written++] = math::float3(p2.x, p2.y, thickness);
					trisData[written++] = math::float3(p1.x, p1.y, thickness);
					trisData[written++] = math::float3(p1.x, p1.y, -thickness);
					trisData[written++] = math::float3(p2.x, p2.y, -thickness);
					trisData[written++] = math::float3(p2.x, p2.y, thickness);
				}

				trisMesh.resize(trisMesh.size() + written);
				std::memcpy(trisMesh.data() + (trisMesh.size() - written), trisData, written * sizeof(math::float3));

				if (contour->isClockwise())
				{
					GLUtesselator* tobj = gluNewTess();

					gluTessCallback(tobj, GLU_TESS_BEGIN, (void(APIENTRY *) ()) &beginCallback);
					gluTessCallback(tobj, GLU_TESS_END, (void(APIENTRY *) ()) &endCallback);
					gluTessCallback(tobj, GLU_TESS_VERTEX, (void(APIENTRY *) ()) &vertexCallback);
					gluTessCallback(tobj, GLU_TESS_ERROR, (void(APIENTRY *) ()) &errorCallback);
					gluTessCallback(tobj, GLU_TESS_COMBINE, (void(APIENTRY *) ()) &combineCallback);
					gluTessCallback(tobj, GLU_TESS_EDGE_FLAG, (void(APIENTRY *) ()) &flagCallback);

					gluTessProperty(tobj, GLU_TESS_TOLERANCE, 0);
					gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

					gluTessNormal(tobj, 0.0f, 0.0f, 0.0f);

					std::size_t index = 0;

					for (std::uint8_t face = 0; face < 2; face++)
					{
						gluTessBeginPolygon(tobj, nullptr);

						for (auto& it : group.getContours())
						{
							gluTessBeginContour(tobj);

							for (std::size_t p = 0; p < it->count() - 1; ++p)
							{
								auto& p1 = it->at(p);
								auto& d = vertices[index++];
								d[0] = p1.x;
								d[1] = p1.y;
								d[2] = p1.z + face ? -1.0 : 1.0;

								gluTessVertex(tobj, d.ptr(), d.ptr());
							}

							gluTessEndContour(tobj);
						}

						gluTessEndPolygon(tobj);

						trisMesh.resize(trisMesh.size() + g_tris.size());
						std::memcpy(trisMesh.data() + (trisMesh.size() - g_tris.size()), g_tris.data(), g_tris.size() * sizeof(math::float3));
					}

					gluDeleteTess(tobj);
				}
			}

			return mesh;
		}

		Mesh makeMesh(const ContourGroups& groups, float thickness) noexcept
		{
			Mesh mesh;

			for (auto& group : groups)
				mesh.combineMeshes(makeMesh(*group, thickness), true);

			return mesh;
		}

		Mesh makeMeshWireframe(const ContourGroup& group, float thickness) noexcept
		{
			Mesh mesh;

			math::float3s& tris = mesh.getVertexArray();
			math::uint1s& indices = mesh.getIndicesArray();

			for (auto& contour : group.getContours())
			{
				for (std::size_t n = 0; n < contour->count() - 1; ++n)
				{
					auto p1 = contour->at(n);
					auto p2 = contour->at(n + 1);

					math::float3 a = math::float3(p1.x, p1.y, thickness);
					math::float3 b = math::float3(p1.x, p1.y, -thickness);
					math::float3 c = math::float3(p2.x, p2.y, -thickness);
					math::float3 d = math::float3(p2.x, p2.y, thickness);

					std::uint32_t index = tris.size();
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

		Mesh makeMeshWireframe(const ContourGroups& groups, float thickness) noexcept
		{
			Mesh mesh;

			math::float3s& tris = mesh.getVertexArray();
			math::uint1s& indices = mesh.getIndicesArray();

			for (auto& group : groups)
			{
				for (auto& contour : group->getContours())
				{
					for (std::size_t n = 0; n < contour->count() - 1; ++n)
					{
						auto p1 = contour->at(n);
						auto p2 = contour->at(n + 1);

						math::float3 a = math::float3(p1.x, p1.y, thickness);
						math::float3 b = math::float3(p1.x, p1.y, -thickness);
						math::float3 c = math::float3(p2.x, p2.y, -thickness);
						math::float3 d = math::float3(p2.x, p2.y, thickness);

						auto index = tris.size();
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