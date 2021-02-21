#include <octoon/mesh/shape_mesh.h>
#include <GL/glu.h>

namespace octoon
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
		const GLdouble* d = (GLdouble*)vertex;
		g_tris.emplace_back((float)d[0], (float)d[1], (float)d[2]);
	}

	void APIENTRY combineCallback(GLdouble coords[3], GLdouble* points[4], GLfloat weight[4], GLdouble* dataOut[3])
	{
	}

	OctoonImplementSubClass(ShapeMesh, Mesh, "ShapeMesh");

	ShapeMesh::ShapeMesh() noexcept
	{
	}

	ShapeMesh::ShapeMesh(const model::Contours& contours, float thickness) noexcept
	{
		std::vector<math::double3> vertices(model::sum(contours) * 2);

		GLUtesselator* tobj = gluNewTess();

		gluTessCallback(tobj, GLU_TESS_BEGIN, (void(APIENTRY*) ()) & beginCallback);
		gluTessCallback(tobj, GLU_TESS_END, (void(APIENTRY*) ()) & endCallback);
		gluTessCallback(tobj, GLU_TESS_VERTEX, (void(APIENTRY*) ()) & vertexCallback);
		gluTessCallback(tobj, GLU_TESS_ERROR, (void(APIENTRY*) ()) & errorCallback);
		gluTessCallback(tobj, GLU_TESS_COMBINE, (void(APIENTRY*) ()) & combineCallback);
		gluTessCallback(tobj, GLU_TESS_EDGE_FLAG, (void(APIENTRY*) ()) & flagCallback);

		gluTessProperty(tobj, GLU_TESS_TOLERANCE, 0);
		gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

		std::size_t index = 0;

		for (std::uint8_t face = 0; face < 2; face++)
		{
			gluTessBeginPolygon(tobj, nullptr);
			gluTessNormal(tobj, 0.0f, 0.0f, face ? 1.0f : -1.0f);

			for (auto& contour_ : contours)
			{
				gluTessBeginContour(tobj);

				for (auto& it : contour_->points())
				{
					auto& d = vertices[index++];
					d[0] = it.x;
					d[1] = it.y;
					d[2] = it.z + face ? -thickness * 0.5f : thickness * 0.5f;

					gluTessVertex(tobj, d.ptr(), d.ptr());
				}

				gluTessEndContour(tobj);
			}

			gluTessEndPolygon(tobj);

			math::float3s& trisMesh = this->getVertexArray();
			trisMesh.resize(trisMesh.size() + g_tris.size());
			std::memcpy(trisMesh.data() + (trisMesh.size() - g_tris.size()), g_tris.data(), g_tris.size() * sizeof(math::float3));
		}

		gluDeleteTess(tobj);

		this->computeVertexNormals();
	}
}