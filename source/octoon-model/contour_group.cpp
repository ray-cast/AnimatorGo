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
			static int count = 0;
			static math::Triangle g_tri;

			const GLdouble *d;
			d = (GLdouble *)vertex;

			if (count == 0)
			{
				g_tri.a.x = d[0];
				g_tri.a.y = d[1];
				g_tri.a.z = d[2];
				count++;
			}
			else if (count == 1)
			{
				g_tri.c.x = d[0];
				g_tri.c.y = d[1];
				g_tri.c.z = d[2];
				count++;
			}
			else if (count == 2)
			{
				g_tri.b = math::float3(d[0], d[1], d[2]);

				g_tris.push_back(g_tri.a);
				g_tris.push_back(g_tri.b);
				g_tris.push_back(g_tri.c);
				count = 0;
			}
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

			math::float3s tris(group.countOfPoints() * sizeof(math::Triangle) / sizeof(math::float3) * 2);
			math::float3* trisData = tris.data();
			math::float3s& trisMesh = mesh.getVertexArray();

			for (std::size_t written = 0, i = 0; i < group.count(); ++i)
			{
				const Contour& contour = group.at(i);

				for (std::size_t n = 0; n < contour.count() - 1; ++n)
				{
					auto p1 = contour.at(n);
					auto p2 = contour.at(n + 1);

					math::Triangle t1;
					t1.a = math::float3(p1.x, p1.y, -thickness);
					t1.b = math::float3(p2.x, p2.y, thickness);
					t1.c = math::float3(p1.x, p1.y, thickness);

					math::Triangle t2;
					t2.a = math::float3(p1.x, p1.y, -thickness);
					t2.b = math::float3(p2.x, p2.y, -thickness);
					t2.c = math::float3(p2.x, p2.y, thickness);

					std::memcpy(trisData + written, t1.ptr(), sizeof(math::Triangle));
					std::memcpy(trisData + written + sizeof(math::Triangle) / sizeof(math::float3), t2.ptr(), sizeof(math::Triangle));

					written += sizeof(math::Triangle) / sizeof(math::float3) * 2;
				}

				trisMesh.insert(trisMesh.end(), tris.begin(), tris.end());

				if (contour.isClockwise())
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
					std::vector<math::double3> vertices(group.countOfPoints() * 2);

					for (std::uint8_t face = 0; face < 2; face++)
					{
						gluTessBeginPolygon(tobj, nullptr);

						for (std::size_t j = 0; j < group.count(); ++j)
						{
							const Contour& it = group.at(j);

							gluTessBeginContour(tobj);

							for (std::size_t p = 0; p < it.count() - 1; ++p)
							{
								auto& p1 = it.at(p);
								auto& d = vertices[index++];
								d[0] = p1.x;
								d[1] = p1.y;
								d[2] = p1.z + face ? -1.0 : 1.0;

								gluTessVertex(tobj, d.ptr(), d.ptr());
							}

							gluTessEndContour(tobj);
						}

						gluTessEndPolygon(tobj);

						trisMesh.insert(trisMesh.end(), g_tris.begin(), g_tris.end());
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
				mesh.combineMeshes(makeMesh(*group), true);

			return mesh;
		}

		Mesh makeMeshWireframe(const ContourGroup& group, float thickness) noexcept
		{
			Mesh mesh;

			math::float3s& tris = mesh.getVertexArray();

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

					tris.push_back(a);
					tris.push_back(b);
					tris.push_back(b);
					tris.push_back(c);
					tris.push_back(c);
					tris.push_back(d);
					tris.push_back(d);
					tris.push_back(a);
				}
			}

			return mesh;
		}

		Mesh makeMeshWireframe(const ContourGroups& groups, float thickness) noexcept
		{
			Mesh mesh;

			math::float3s& tris = mesh.getVertexArray();

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

						tris.push_back(a);
						tris.push_back(b);
						tris.push_back(b);
						tris.push_back(c);
						tris.push_back(c);
						tris.push_back(d);
						tris.push_back(d);
						tris.push_back(a);
					}
				}
			}

			return mesh;
		}
	}
}