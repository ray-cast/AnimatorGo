#ifndef OCTOON_OGL_BASIC_H_
#define OCTOON_OGL_BASIC_H_

#include <octoon/runtime/platform.h>

#if OCTOON_BUILD_PLATFORM_WINDOWS
#	include <GL/glew.h>
#	include <GL/wglew.h>
#elif OCTOON_BUILD_PLATFORM_LINUX
#	include <X11/Xlib.h>
#	include <GL/glew.h>
#	include <GL/glxew.h>
#elif OCTOON_BUILD_PLATFORM_APPLE
#	include <GL/glew.h>
#	include <OpenGL/OpenGL.h>
#	include <OpenGL/CGLTypes.h>
#endif

#if OCTOON_BUILD_PLATFORM_LINUX
#	undef None
#	undef Always
#endif

namespace octoon
{
	namespace graphics
	{
#if defined(_BUILD_OPENGL_ES)

		typedef void (GLAPIENTRY* PFNGLACCUM) (GLenum op, GLfloat value);
		typedef void (GLAPIENTRY* PFNGLALPHAFUNC) (GLenum func, GLclampf ref);
		typedef GLboolean(GLAPIENTRY* PFNGLARETEXTURESRESIDENT) (GLsizei n, const GLuint *textures, GLboolean *residences);
		typedef void (GLAPIENTRY* PFNGLARRAYELEMENT) (GLint i);
		typedef void (GLAPIENTRY* PFNGLBEGIN) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLBINDTEXTURE) (GLenum target, GLuint texture);
		typedef void (GLAPIENTRY* PFNGLBITMAP) (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
		typedef void (GLAPIENTRY* PFNGLBLENDFUNC) (GLenum sfactor, GLenum dfactor);
		typedef void (GLAPIENTRY* PFNGLCALLLIST) (GLuint list);
		typedef void (GLAPIENTRY* PFNGLCALLLISTS) (GLsizei n, GLenum type, const void *lists);
		typedef void (GLAPIENTRY* PFNGLCLEAR) (GLbitfield mask);
		typedef void (GLAPIENTRY* PFNGLCLEARACCUM) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
		typedef void (GLAPIENTRY* PFNGLCLEARCOLOR) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
		typedef void (GLAPIENTRY* PFNGLCLEARDEPTH) (GLclampd depth);
		typedef void (GLAPIENTRY* PFNGLCLEARINDEX) (GLfloat c);
		typedef void (GLAPIENTRY* PFNGLCLEARSTENCIL) (GLint s);
		typedef void (GLAPIENTRY* PFNGLCLIPPLANE) (GLenum plane, const GLdouble *equation);
		typedef void (GLAPIENTRY* PFNGLCOLOR3B) (GLbyte red, GLbyte green, GLbyte blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3BV) (const GLbyte *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR3D) (GLdouble red, GLdouble green, GLdouble blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR3F) (GLfloat red, GLfloat green, GLfloat blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR3I) (GLint red, GLint green, GLint blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR3S) (GLshort red, GLshort green, GLshort blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR3UB) (GLubyte red, GLubyte green, GLubyte blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3UBV) (const GLubyte *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR3UI) (GLuint red, GLuint green, GLuint blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3UIV) (const GLuint *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR3US) (GLushort red, GLushort green, GLushort blue);
		typedef void (GLAPIENTRY* PFNGLCOLOR3USV) (const GLushort *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4B) (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4BV) (const GLbyte *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4D) (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4F) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4I) (GLint red, GLint green, GLint blue, GLint alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4S) (GLshort red, GLshort green, GLshort blue, GLshort alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4UB) (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4UBV) (const GLubyte *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4UI) (GLuint red, GLuint green, GLuint blue, GLuint alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4UIV) (const GLuint *v);
		typedef void (GLAPIENTRY* PFNGLCOLOR4US) (GLushort red, GLushort green, GLushort blue, GLushort alpha);
		typedef void (GLAPIENTRY* PFNGLCOLOR4USV) (const GLushort *v);
		typedef void (GLAPIENTRY* PFNGLCOLORMASK) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
		typedef void (GLAPIENTRY* PFNGLCOLORMATERIAL) (GLenum face, GLenum mode);
		typedef void (GLAPIENTRY* PFNGLCOLORPOINTER) (GLint size, GLenum type, GLsizei stride, const void *pointer);
		typedef void (GLAPIENTRY* PFNGLCOPYPIXELS) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
		typedef void (GLAPIENTRY* PFNGLCOPYTEXIMAGE1D) (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
		typedef void (GLAPIENTRY* PFNGLCOPYTEXIMAGE2D) (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
		typedef void (GLAPIENTRY* PFNGLCOPYTEXSUBIMAGE1D) (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
		typedef void (GLAPIENTRY* PFNGLCOPYTEXSUBIMAGE2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
		typedef void (GLAPIENTRY* PFNGLCULLFACE) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLDELETELISTS) (GLuint list, GLsizei range);
		typedef void (GLAPIENTRY* PFNGLDELETETEXTURES) (GLsizei n, const GLuint *textures);
		typedef void (GLAPIENTRY* PFNGLDEPTHFUNC) (GLenum func);
		typedef void (GLAPIENTRY* PFNGLDEPTHMASK) (GLboolean flag);
		typedef void (GLAPIENTRY* PFNGLDEPTHRANGE) (GLclampd zNear, GLclampd zFar);
		typedef void (GLAPIENTRY* PFNGLDISABLE) (GLenum cap);
		typedef void (GLAPIENTRY* PFNGLDISABLECLIENTSTATE) (GLenum array);
		typedef void (GLAPIENTRY* PFNGLDRAWARRAYS) (GLenum mode, GLint first, GLsizei count);
		typedef void (GLAPIENTRY* PFNGLDRAWBUFFER) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLDRAWELEMENTS) (GLenum mode, GLsizei count, GLenum type, const void *indices);
		typedef void (GLAPIENTRY* PFNGLDRAWPIXELS) (GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
		typedef void (GLAPIENTRY* PFNGLEDGEFLAG) (GLboolean flag);
		typedef void (GLAPIENTRY* PFNGLEDGEFLAGPOINTER) (GLsizei stride, const void *pointer);
		typedef void (GLAPIENTRY* PFNGLEDGEFLAGV) (const GLboolean *flag);
		typedef void (GLAPIENTRY* PFNGLENABLE) (GLenum cap);
		typedef void (GLAPIENTRY* PFNGLENABLECLIENTSTATE) (GLenum array);
		typedef void (GLAPIENTRY* PFNGLEND) (void);
		typedef void (GLAPIENTRY* PFNGLENDLIST) (void);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD1D) (GLdouble u);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD1DV) (const GLdouble *u);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD1F) (GLfloat u);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD1FV) (const GLfloat *u);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD2D) (GLdouble u, GLdouble v);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD2DV) (const GLdouble *u);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD2F) (GLfloat u, GLfloat v);
		typedef void (GLAPIENTRY* PFNGLEVALCOORD2FV) (const GLfloat *u);
		typedef void (GLAPIENTRY* PFNGLEVALMESH1) (GLenum mode, GLint i1, GLint i2);
		typedef void (GLAPIENTRY* PFNGLEVALMESH2) (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
		typedef void (GLAPIENTRY* PFNGLEVALPOINT1) (GLint i);
		typedef void (GLAPIENTRY* PFNGLEVALPOINT2) (GLint i, GLint j);
		typedef void (GLAPIENTRY* PFNGLFEEDBACKBUFFER) (GLsizei size, GLenum type, GLfloat *buffer);
		typedef void (GLAPIENTRY* PFNGLFINISH) (void);
		typedef void (GLAPIENTRY* PFNGLFLUSH) (void);
		typedef void (GLAPIENTRY* PFNGLFOGF) (GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLFOGFV) (GLenum pname, const GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLFOGI) (GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLFOGIV) (GLenum pname, const GLint *params);
		typedef void (GLAPIENTRY* PFNGLFRONTFACE) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLFRUSTUM) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
		typedef GLuint(GLAPIENTRY* PFNGLGENLISTS) (GLsizei range);
		typedef void (GLAPIENTRY* PFNGLGENTEXTURES) (GLsizei n, GLuint *textures);
		typedef void (GLAPIENTRY* PFNGLGETBOOLEANV) (GLenum pname, GLboolean *params);
		typedef void (GLAPIENTRY* PFNGLGETCLIPPLANE) (GLenum plane, GLdouble *equation);
		typedef void (GLAPIENTRY* PFNGLGETDOUBLEV) (GLenum pname, GLdouble *params);
		typedef GLenum(GLAPIENTRY* PFNGLGETERROR) (void);
		typedef void (GLAPIENTRY* PFNGLGETFLOATV) (GLenum pname, GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLGETINTEGERV) (GLenum pname, GLint *params);
		typedef void (GLAPIENTRY* PFNGLGETLIGHTFV) (GLenum light, GLenum pname, GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLGETLIGHTIV) (GLenum light, GLenum pname, GLint *params);
		typedef void (GLAPIENTRY* PFNGLGETMAPDV) (GLenum target, GLenum query, GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLGETMAPFV) (GLenum target, GLenum query, GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLGETMAPIV) (GLenum target, GLenum query, GLint *v);
		typedef void (GLAPIENTRY* PFNGLGETMATERIALFV) (GLenum face, GLenum pname, GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLGETMATERIALIV) (GLenum face, GLenum pname, GLint *params);
		typedef void (GLAPIENTRY* PFNGLGETPIXELMAPFV) (GLenum map, GLfloat *values);
		typedef void (GLAPIENTRY* PFNGLGETPIXELMAPUIV) (GLenum map, GLuint *values);
		typedef void (GLAPIENTRY* PFNGLGETPIXELMAPUSV) (GLenum map, GLushort *values);
		typedef void (GLAPIENTRY* PFNGLGETPOINTERV) (GLenum pname, void* *params);
		typedef void (GLAPIENTRY* PFNGLGETPOLYGONSTIPPLE) (GLubyte *mask);
		typedef const GLubyte * (GLAPIENTRY* PFNGLGETSTRING) (GLenum name);
		typedef void (GLAPIENTRY* PFNGLGETTEXENVFV) (GLenum target, GLenum pname, GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXENVIV) (GLenum target, GLenum pname, GLint *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXGENDV) (GLenum coord, GLenum pname, GLdouble *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXGENFV) (GLenum coord, GLenum pname, GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXGENIV) (GLenum coord, GLenum pname, GLint *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXIMAGE) (GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
		typedef void (GLAPIENTRY* PFNGLGETTEXLEVELPARAMETERFV) (GLenum target, GLint level, GLenum pname, GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXLEVELPARAMETERIV) (GLenum target, GLint level, GLenum pname, GLint *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXPARAMETERFV) (GLenum target, GLenum pname, GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLGETTEXPARAMETERIV) (GLenum target, GLenum pname, GLint *params);
		typedef void (GLAPIENTRY* PFNGLHINT) (GLenum target, GLenum mode);
		typedef void (GLAPIENTRY* PFNGLINDEXMASK) (GLuint mask);
		typedef void (GLAPIENTRY* PFNGLINDEXPOINTER) (GLenum type, GLsizei stride, const void *pointer);
		typedef void (GLAPIENTRY* PFNGLINDEXD) (GLdouble c);
		typedef void (GLAPIENTRY* PFNGLINDEXDV) (const GLdouble *c);
		typedef void (GLAPIENTRY* PFNGLINDEXF) (GLfloat c);
		typedef void (GLAPIENTRY* PFNGLINDEXFV) (const GLfloat *c);
		typedef void (GLAPIENTRY* PFNGLINDEXI) (GLint c);
		typedef void (GLAPIENTRY* PFNGLINDEXIV) (const GLint *c);
		typedef void (GLAPIENTRY* PFNGLINDEXS) (GLshort c);
		typedef void (GLAPIENTRY* PFNGLINDEXSV) (const GLshort *c);
		typedef void (GLAPIENTRY* PFNGLINDEXUB) (GLubyte c);
		typedef void (GLAPIENTRY* PFNGLINDEXUBV) (const GLubyte *c);
		typedef void (GLAPIENTRY* PFNGLINITNAMES) (void);
		typedef void (GLAPIENTRY* PFNGLINTERLEAVEDARRAYS) (GLenum format, GLsizei stride, const void *pointer);
		typedef GLboolean(GLAPIENTRY* PFNGLISENABLED) (GLenum cap);
		typedef GLboolean(GLAPIENTRY* PFNGLISLIST) (GLuint list);
		typedef GLboolean(GLAPIENTRY* PFNGLISTEXTURE) (GLuint texture);
		typedef void (GLAPIENTRY* PFNGLLIGHTMODELF) (GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLLIGHTMODELFV) (GLenum pname, const GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLLIGHTMODELI) (GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLLIGHTMODELIV) (GLenum pname, const GLint *params);
		typedef void (GLAPIENTRY* PFNGLLIGHTF) (GLenum light, GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLLIGHTFV) (GLenum light, GLenum pname, const GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLLIGHTI) (GLenum light, GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLLIGHTIV) (GLenum light, GLenum pname, const GLint *params);
		typedef void (GLAPIENTRY* PFNGLLINESTIPPLE) (GLint factor, GLushort pattern);
		typedef void (GLAPIENTRY* PFNGLLINEWIDTH) (GLfloat width);
		typedef void (GLAPIENTRY* PFNGLLISTBASE) (GLuint base);
		typedef void (GLAPIENTRY* PFNGLLOADIDENTITY) (void);
		typedef void (GLAPIENTRY* PFNGLLOADMATRIXD) (const GLdouble *m);
		typedef void (GLAPIENTRY* PFNGLLOADMATRIXF) (const GLfloat *m);
		typedef void (GLAPIENTRY* PFNGLLOADNAME) (GLuint name);
		typedef void (GLAPIENTRY* PFNGLLOGICOP) (GLenum opcode);
		typedef void (GLAPIENTRY* PFNGLMAP1D) (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
		typedef void (GLAPIENTRY* PFNGLMAP1F) (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
		typedef void (GLAPIENTRY* PFNGLMAP2D) (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
		typedef void (GLAPIENTRY* PFNGLMAP2F) (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
		typedef void (GLAPIENTRY* PFNGLMAPGRID1D) (GLint un, GLdouble u1, GLdouble u2);
		typedef void (GLAPIENTRY* PFNGLMAPGRID1F) (GLint un, GLfloat u1, GLfloat u2);
		typedef void (GLAPIENTRY* PFNGLMAPGRID2D) (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
		typedef void (GLAPIENTRY* PFNGLMAPGRID2F) (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
		typedef void (GLAPIENTRY* PFNGLMATERIALF) (GLenum face, GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLMATERIALFV) (GLenum face, GLenum pname, const GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLMATERIALI) (GLenum face, GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLMATERIALIV) (GLenum face, GLenum pname, const GLint *params);
		typedef void (GLAPIENTRY* PFNGLMATRIXMODE) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLMULTMATRIXD) (const GLdouble *m);
		typedef void (GLAPIENTRY* PFNGLMULTMATRIXF) (const GLfloat *m);
		typedef void (GLAPIENTRY* PFNGLNEWLIST) (GLuint list, GLenum mode);
		typedef void (GLAPIENTRY* PFNGLNORMAL3B) (GLbyte nx, GLbyte ny, GLbyte nz);
		typedef void (GLAPIENTRY* PFNGLNORMAL3BV) (const GLbyte *v);
		typedef void (GLAPIENTRY* PFNGLNORMAL3D) (GLdouble nx, GLdouble ny, GLdouble nz);
		typedef void (GLAPIENTRY* PFNGLNORMAL3DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLNORMAL3F) (GLfloat nx, GLfloat ny, GLfloat nz);
		typedef void (GLAPIENTRY* PFNGLNORMAL3FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLNORMAL3I) (GLint nx, GLint ny, GLint nz);
		typedef void (GLAPIENTRY* PFNGLNORMAL3IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLNORMAL3S) (GLshort nx, GLshort ny, GLshort nz);
		typedef void (GLAPIENTRY* PFNGLNORMAL3SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLNORMALPOINTER) (GLenum type, GLsizei stride, const void *pointer);
		typedef void (GLAPIENTRY* PFNGLORTHO) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
		typedef void (GLAPIENTRY* PFNGLPASSTHROUGH) (GLfloat token);
		typedef void (GLAPIENTRY* PFNGLPIXELMAPFV) (GLenum map, GLsizei mapsize, const GLfloat *values);
		typedef void (GLAPIENTRY* PFNGLPIXELMAPUIV) (GLenum map, GLsizei mapsize, const GLuint *values);
		typedef void (GLAPIENTRY* PFNGLPIXELMAPUSV) (GLenum map, GLsizei mapsize, const GLushort *values);
		typedef void (GLAPIENTRY* PFNGLPIXELSTOREF) (GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLPIXELSTOREI) (GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLPIXELTRANSFERF) (GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLPIXELTRANSFERI) (GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLPIXELZOOM) (GLfloat xfactor, GLfloat yfactor);
		typedef void (GLAPIENTRY* PFNGLPOINTSIZE) (GLfloat size);
		typedef void (GLAPIENTRY* PFNGLPOLYGONMODE) (GLenum face, GLenum mode);
		typedef void (GLAPIENTRY* PFNGLPOLYGONOFFSET) (GLfloat factor, GLfloat units);
		typedef void (GLAPIENTRY* PFNGLPOLYGONSTIPPLE) (const GLubyte *mask);
		typedef void (GLAPIENTRY* PFNGLPOPATTRIB) (void);
		typedef void (GLAPIENTRY* PFNGLPOPCLIENTATTRIB) (void);
		typedef void (GLAPIENTRY* PFNGLPOPMATRIX) (void);
		typedef void (GLAPIENTRY* PFNGLPOPNAME) (void);
		typedef void (GLAPIENTRY* PFNGLPRIORITIZETEXTURES) (GLsizei n, const GLuint *textures, const GLclampf *priorities);
		typedef void (GLAPIENTRY* PFNGLPUSHATTRIB) (GLbitfield mask);
		typedef void (GLAPIENTRY* PFNGLPUSHCLIENTATTRIB) (GLbitfield mask);
		typedef void (GLAPIENTRY* PFNGLPUSHMATRIX) (void);
		typedef void (GLAPIENTRY* PFNGLPUSHNAME) (GLuint name);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2D) (GLdouble x, GLdouble y);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2F) (GLfloat x, GLfloat y);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2I) (GLint x, GLint y);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2S) (GLshort x, GLshort y);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS2SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3D) (GLdouble x, GLdouble y, GLdouble z);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3F) (GLfloat x, GLfloat y, GLfloat z);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3I) (GLint x, GLint y, GLint z);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3S) (GLshort x, GLshort y, GLshort z);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS3SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4D) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4F) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4I) (GLint x, GLint y, GLint z, GLint w);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4S) (GLshort x, GLshort y, GLshort z, GLshort w);
		typedef void (GLAPIENTRY* PFNGLRASTERPOS4SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLREADBUFFER) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLREADPIXELS) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
		typedef void (GLAPIENTRY* PFNGLRECTD) (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
		typedef void (GLAPIENTRY* PFNGLRECTDV) (const GLdouble *v1, const GLdouble *v2);
		typedef void (GLAPIENTRY* PFNGLRECTF) (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
		typedef void (GLAPIENTRY* PFNGLRECTFV) (const GLfloat *v1, const GLfloat *v2);
		typedef void (GLAPIENTRY* PFNGLRECTI) (GLint x1, GLint y1, GLint x2, GLint y2);
		typedef void (GLAPIENTRY* PFNGLRECTIV) (const GLint *v1, const GLint *v2);
		typedef void (GLAPIENTRY* PFNGLRECTS) (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
		typedef void (GLAPIENTRY* PFNGLRECTSV) (const GLshort *v1, const GLshort *v2);
		typedef GLint(GLAPIENTRY* PFNGLRENDERMODE) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLROTATED) (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
		typedef void (GLAPIENTRY* PFNGLROTATEF) (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
		typedef void (GLAPIENTRY* PFNGLSCALED) (GLdouble x, GLdouble y, GLdouble z);
		typedef void (GLAPIENTRY* PFNGLSCALEF) (GLfloat x, GLfloat y, GLfloat z);
		typedef void (GLAPIENTRY* PFNGLSCISSOR) (GLint x, GLint y, GLsizei width, GLsizei height);
		typedef void (GLAPIENTRY* PFNGLSELECTBUFFER) (GLsizei size, GLuint *buffer);
		typedef void (GLAPIENTRY* PFNGLSHADEMODEL) (GLenum mode);
		typedef void (GLAPIENTRY* PFNGLSTENCILFUNC) (GLenum func, GLint ref, GLuint mask);
		typedef void (GLAPIENTRY* PFNGLSTENCILMASK) (GLuint mask);
		typedef void (GLAPIENTRY* PFNGLSTENCILOP) (GLenum fail, GLenum zfail, GLenum zpass);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1D) (GLdouble s);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1F) (GLfloat s);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1I) (GLint s);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1S) (GLshort s);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD1SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2D) (GLdouble s, GLdouble t);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2F) (GLfloat s, GLfloat t);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2I) (GLint s, GLint t);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2S) (GLshort s, GLshort t);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD2SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3D) (GLdouble s, GLdouble t, GLdouble r);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3F) (GLfloat s, GLfloat t, GLfloat r);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3I) (GLint s, GLint t, GLint r);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3S) (GLshort s, GLshort t, GLshort r);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD3SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4D) (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4F) (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4I) (GLint s, GLint t, GLint r, GLint q);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4S) (GLshort s, GLshort t, GLshort r, GLshort q);
		typedef void (GLAPIENTRY* PFNGLTEXCOORD4SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLTEXCOORDPOINTER) (GLint size, GLenum type, GLsizei stride, const void *pointer);
		typedef void (GLAPIENTRY* PFNGLTEXENVF) (GLenum target, GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLTEXENVFV) (GLenum target, GLenum pname, const GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLTEXENVI) (GLenum target, GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLTEXENVIV) (GLenum target, GLenum pname, const GLint *params);
		typedef void (GLAPIENTRY* PFNGLTEXGEND) (GLenum coord, GLenum pname, GLdouble param);
		typedef void (GLAPIENTRY* PFNGLTEXGENDV) (GLenum coord, GLenum pname, const GLdouble *params);
		typedef void (GLAPIENTRY* PFNGLTEXGENF) (GLenum coord, GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLTEXGENFV) (GLenum coord, GLenum pname, const GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLTEXGENI) (GLenum coord, GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLTEXGENIV) (GLenum coord, GLenum pname, const GLint *params);
		typedef void (GLAPIENTRY* PFNGLTEXIMAGE1D) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
		typedef void (GLAPIENTRY* PFNGLTEXIMAGE2D) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
		typedef void (GLAPIENTRY* PFNGLTEXPARAMETERF) (GLenum target, GLenum pname, GLfloat param);
		typedef void (GLAPIENTRY* PFNGLTEXPARAMETERFV) (GLenum target, GLenum pname, const GLfloat *params);
		typedef void (GLAPIENTRY* PFNGLTEXPARAMETERI) (GLenum target, GLenum pname, GLint param);
		typedef void (GLAPIENTRY* PFNGLTEXPARAMETERIV) (GLenum target, GLenum pname, const GLint *params);
		typedef void (GLAPIENTRY* PFNGLTEXSUBIMAGE1D) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
		typedef void (GLAPIENTRY* PFNGLTEXSUBIMAGE2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
		typedef void (GLAPIENTRY* PFNGLTRANSLATED) (GLdouble x, GLdouble y, GLdouble z);
		typedef void (GLAPIENTRY* PFNGLTRANSLATEF) (GLfloat x, GLfloat y, GLfloat z);
		typedef void (GLAPIENTRY* PFNGLVERTEX2D) (GLdouble x, GLdouble y);
		typedef void (GLAPIENTRY* PFNGLVERTEX2DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX2F) (GLfloat x, GLfloat y);
		typedef void (GLAPIENTRY* PFNGLVERTEX2FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX2I) (GLint x, GLint y);
		typedef void (GLAPIENTRY* PFNGLVERTEX2IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX2S) (GLshort x, GLshort y);
		typedef void (GLAPIENTRY* PFNGLVERTEX2SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX3D) (GLdouble x, GLdouble y, GLdouble z);
		typedef void (GLAPIENTRY* PFNGLVERTEX3DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX3F) (GLfloat x, GLfloat y, GLfloat z);
		typedef void (GLAPIENTRY* PFNGLVERTEX3FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX3I) (GLint x, GLint y, GLint z);
		typedef void (GLAPIENTRY* PFNGLVERTEX3IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX3S) (GLshort x, GLshort y, GLshort z);
		typedef void (GLAPIENTRY* PFNGLVERTEX3SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX4D) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
		typedef void (GLAPIENTRY* PFNGLVERTEX4DV) (const GLdouble *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX4F) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		typedef void (GLAPIENTRY* PFNGLVERTEX4FV) (const GLfloat *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX4I) (GLint x, GLint y, GLint z, GLint w);
		typedef void (GLAPIENTRY* PFNGLVERTEX4IV) (const GLint *v);
		typedef void (GLAPIENTRY* PFNGLVERTEX4S) (GLshort x, GLshort y, GLshort z, GLshort w);
		typedef void (GLAPIENTRY* PFNGLVERTEX4SV) (const GLshort *v);
		typedef void (GLAPIENTRY* PFNGLVERTEXPOINTER) (GLint size, GLenum type, GLsizei stride, const void *pointer);
		typedef void (GLAPIENTRY* PFNGLVIEWPORT) (GLint x, GLint y, GLsizei width, GLsizei height);

		extern PFNGLACCUM __glAccum;
		extern PFNGLALPHAFUNC __glAlphaFunc;
		extern PFNGLARETEXTURESRESIDENT __glAreTexturesResident;
		extern PFNGLARRAYELEMENT __glArrayElement;
		extern PFNGLBEGIN __glBegin;
		extern PFNGLBINDTEXTURE __glBindTexture;
		extern PFNGLBITMAP __glBitmap;
		extern PFNGLBLENDFUNC __glBlendFunc;
		extern PFNGLCALLLIST __glCallList;
		extern PFNGLCALLLISTS __glCallLists;
		extern PFNGLCLEAR __glClear;
		extern PFNGLCLEARACCUM __glClearAccum;
		extern PFNGLCLEARCOLOR __glClearColor;
		extern PFNGLCLEARDEPTH __glClearDepth;
		extern PFNGLCLEARINDEX __glClearIndex;
		extern PFNGLCLEARSTENCIL __glClearStencil;
		extern PFNGLCLIPPLANE __glClipPlane;
		extern PFNGLCOLOR3B __glColor3b;
		extern PFNGLCOLOR3BV __glColor3bv;
		extern PFNGLCOLOR3D __glColor3d;
		extern PFNGLCOLOR3DV __glColor3dv;
		extern PFNGLCOLOR3F __glColor3f;
		extern PFNGLCOLOR3FV __glColor3fv;
		extern PFNGLCOLOR3I __glColor3i;
		extern PFNGLCOLOR3IV __glColor3iv;
		extern PFNGLCOLOR3S __glColor3s;
		extern PFNGLCOLOR3SV __glColor3sv;
		extern PFNGLCOLOR3UB __glColor3ub;
		extern PFNGLCOLOR3UBV __glColor3ubv;
		extern PFNGLCOLOR3UI __glColor3ui;
		extern PFNGLCOLOR3UIV __glColor3uiv;
		extern PFNGLCOLOR3US __glColor3us;
		extern PFNGLCOLOR3USV __glColor3usv;
		extern PFNGLCOLOR4B __glColor4b;
		extern PFNGLCOLOR4BV __glColor4bv;
		extern PFNGLCOLOR4D __glColor4d;
		extern PFNGLCOLOR4DV __glColor4dv;
		extern PFNGLCOLOR4F __glColor4f;
		extern PFNGLCOLOR4FV __glColor4fv;
		extern PFNGLCOLOR4I __glColor4i;
		extern PFNGLCOLOR4IV __glColor4iv;
		extern PFNGLCOLOR4S __glColor4s;
		extern PFNGLCOLOR4SV __glColor4sv;
		extern PFNGLCOLOR4UB __glColor4ub;
		extern PFNGLCOLOR4UBV __glColor4ubv;
		extern PFNGLCOLOR4UI __glColor4ui;
		extern PFNGLCOLOR4UIV __glColor4uiv;
		extern PFNGLCOLOR4US __glColor4us;
		extern PFNGLCOLOR4USV __glColor4usv;
		extern PFNGLCOLORMASK __glColorMask;
		extern PFNGLCOLORMATERIAL __glColorMaterial;
		extern PFNGLCOLORPOINTER __glColorPointer;
		extern PFNGLCOPYPIXELS __glCopyPixels;
		extern PFNGLCOPYTEXIMAGE1D __glCopyTexImage1D;
		extern PFNGLCOPYTEXIMAGE2D __glCopyTexImage2D;
		extern PFNGLCOPYTEXSUBIMAGE1D __glCopyTexSubImage1D;
		extern PFNGLCOPYTEXSUBIMAGE2D __glCopyTexSubImage2D;
		extern PFNGLCULLFACE __glCullFace;
		extern PFNGLDELETELISTS __glDeleteLists;
		extern PFNGLDELETETEXTURES __glDeleteTextures;
		extern PFNGLDEPTHFUNC __glDepthFunc;
		extern PFNGLDEPTHMASK __glDepthMask;
		extern PFNGLDEPTHRANGE __glDepthRange;
		extern PFNGLDISABLE __glDisable;
		extern PFNGLDISABLECLIENTSTATE __glDisableClientState;
		extern PFNGLDRAWARRAYS __glDrawArrays;
		extern PFNGLDRAWBUFFER __glDrawBuffer;
		extern PFNGLDRAWELEMENTS __glDrawElements;
		extern PFNGLDRAWPIXELS __glDrawPixels;
		extern PFNGLEDGEFLAG __glEdgeFlag;
		extern PFNGLEDGEFLAGPOINTER __glEdgeFlagPointer;
		extern PFNGLEDGEFLAGV __glEdgeFlagv;
		extern PFNGLENABLE __glEnable;
		extern PFNGLENABLECLIENTSTATE __glEnableClientState;
		extern PFNGLEND __glEnd;
		extern PFNGLENDLIST __glEndList;
		extern PFNGLEVALCOORD1D __glEvalCoord1d;
		extern PFNGLEVALCOORD1DV __glEvalCoord1dv;
		extern PFNGLEVALCOORD1F __glEvalCoord1f;
		extern PFNGLEVALCOORD1FV __glEvalCoord1fv;
		extern PFNGLEVALCOORD2D __glEvalCoord2d;
		extern PFNGLEVALCOORD2DV __glEvalCoord2dv;
		extern PFNGLEVALCOORD2F __glEvalCoord2f;
		extern PFNGLEVALCOORD2FV __glEvalCoord2fv;
		extern PFNGLEVALMESH1 __glEvalMesh1;
		extern PFNGLEVALMESH2 __glEvalMesh2;
		extern PFNGLEVALPOINT1 __glEvalPoint1;
		extern PFNGLEVALPOINT2 __glEvalPoint2;
		extern PFNGLFEEDBACKBUFFER __glFeedbackBuffer;
		extern PFNGLFINISH __glFinish;
		extern PFNGLFLUSH __glFlush;
		extern PFNGLFOGF __glFogf;
		extern PFNGLFOGFV __glFogfv;
		extern PFNGLFOGI __glFogi;
		extern PFNGLFOGIV __glFogiv;
		extern PFNGLFRONTFACE __glFrontFace;
		extern PFNGLFRUSTUM __glFrustum;
		extern PFNGLGENLISTS __glGenLists;
		extern PFNGLGENTEXTURES __glGenTextures;
		extern PFNGLGETBOOLEANV __glGetBooleanv;
		extern PFNGLGETCLIPPLANE __glGetClipPlane;
		extern PFNGLGETDOUBLEV __glGetDoublev;
		extern PFNGLGETERROR __glGetError;
		extern PFNGLGETFLOATV __glGetFloatv;
		extern PFNGLGETINTEGERV __glGetIntegerv;
		extern PFNGLGETLIGHTFV __glGetLightfv;
		extern PFNGLGETLIGHTIV __glGetLightiv;
		extern PFNGLGETMAPDV __glGetMapdv;
		extern PFNGLGETMAPFV __glGetMapfv;
		extern PFNGLGETMAPIV __glGetMapiv;
		extern PFNGLGETMATERIALFV __glGetMaterialfv;
		extern PFNGLGETMATERIALIV __glGetMaterialiv;
		extern PFNGLGETPIXELMAPFV __glGetPixelMapfv;
		extern PFNGLGETPIXELMAPUIV __glGetPixelMapuiv;
		extern PFNGLGETPIXELMAPUSV __glGetPixelMapusv;
		extern PFNGLGETPOINTERV __glGetPointerv;
		extern PFNGLGETPOLYGONSTIPPLE __glGetPolygonStipple;
		extern PFNGLGETSTRING __glGetString;
		extern PFNGLGETTEXENVFV __glGetTexEnvfv;
		extern PFNGLGETTEXENVIV __glGetTexEnviv;
		extern PFNGLGETTEXGENDV __glGetTexGendv;
		extern PFNGLGETTEXGENFV __glGetTexGenfv;
		extern PFNGLGETTEXGENIV __glGetTexGeniv;
		extern PFNGLGETTEXIMAGE __glGetTexImage;
		extern PFNGLGETTEXLEVELPARAMETERFV __glGetTexLevelParameterfv;
		extern PFNGLGETTEXLEVELPARAMETERIV __glGetTexLevelParameteriv;
		extern PFNGLGETTEXPARAMETERFV __glGetTexParameterfv;
		extern PFNGLGETTEXPARAMETERIV __glGetTexParameteriv;
		extern PFNGLHINT __glHint;
		extern PFNGLINDEXMASK __glIndexMask;
		extern PFNGLINDEXPOINTER __glIndexPointer;
		extern PFNGLINDEXD __glIndexd;
		extern PFNGLINDEXDV __glIndexdv;
		extern PFNGLINDEXF __glIndexf;
		extern PFNGLINDEXFV __glIndexfv;
		extern PFNGLINDEXI __glIndexi;
		extern PFNGLINDEXIV __glIndexiv;
		extern PFNGLINDEXS __glIndexs;
		extern PFNGLINDEXSV __glIndexsv;
		extern PFNGLINDEXUB __glIndexub;
		extern PFNGLINDEXUBV __glIndexubv;
		extern PFNGLINITNAMES __glInitNames;
		extern PFNGLINTERLEAVEDARRAYS __glInterleavedArrays;
		extern PFNGLISENABLED __glIsEnabled;
		extern PFNGLISLIST __glIsList;
		extern PFNGLISTEXTURE __glIsTexture;
		extern PFNGLLIGHTMODELF __glLightModelf;
		extern PFNGLLIGHTMODELFV __glLightModelfv;
		extern PFNGLLIGHTMODELI __glLightModeli;
		extern PFNGLLIGHTMODELIV __glLightModeliv;
		extern PFNGLLIGHTF __glLightf;
		extern PFNGLLIGHTFV __glLightfv;
		extern PFNGLLIGHTI __glLighti;
		extern PFNGLLIGHTIV __glLightiv;
		extern PFNGLLINESTIPPLE __glLineStipple;
		extern PFNGLLINEWIDTH __glLineWidth;
		extern PFNGLLISTBASE __glListBase;
		extern PFNGLLOADIDENTITY __glLoadIdentity;
		extern PFNGLLOADMATRIXD __glLoadMatrixd;
		extern PFNGLLOADMATRIXF __glLoadMatrixf;
		extern PFNGLLOADNAME __glLoadName;
		extern PFNGLLOGICOP __glLogicOp;
		extern PFNGLMAP1D __glMap1d;
		extern PFNGLMAP1F __glMap1f;
		extern PFNGLMAP2D __glMap2d;
		extern PFNGLMAP2F __glMap2f;
		extern PFNGLMAPGRID1D __glMapGrid1d;
		extern PFNGLMAPGRID1F __glMapGrid1f;
		extern PFNGLMAPGRID2D __glMapGrid2d;
		extern PFNGLMAPGRID2F __glMapGrid2f;
		extern PFNGLMATERIALF __glMaterialf;
		extern PFNGLMATERIALFV __glMaterialfv;
		extern PFNGLMATERIALI __glMateriali;
		extern PFNGLMATERIALIV __glMaterialiv;
		extern PFNGLMATRIXMODE __glMatrixMode;
		extern PFNGLMULTMATRIXD __glMultMatrixd;
		extern PFNGLMULTMATRIXF __glMultMatrixf;
		extern PFNGLNEWLIST __glNewList;
		extern PFNGLNORMAL3B __glNormal3b;
		extern PFNGLNORMAL3BV __glNormal3bv;
		extern PFNGLNORMAL3D __glNormal3d;
		extern PFNGLNORMAL3DV __glNormal3dv;
		extern PFNGLNORMAL3F __glNormal3f;
		extern PFNGLNORMAL3FV __glNormal3fv;
		extern PFNGLNORMAL3I __glNormal3i;
		extern PFNGLNORMAL3IV __glNormal3iv;
		extern PFNGLNORMAL3S __glNormal3s;
		extern PFNGLNORMAL3SV __glNormal3sv;
		extern PFNGLNORMALPOINTER __glNormalPointer;
		extern PFNGLORTHO __glOrtho;
		extern PFNGLPASSTHROUGH __glPassThrough;
		extern PFNGLPIXELMAPFV __glPixelMapfv;
		extern PFNGLPIXELMAPUIV __glPixelMapuiv;
		extern PFNGLPIXELMAPUSV __glPixelMapusv;
		extern PFNGLPIXELSTOREF __glPixelStoref;
		extern PFNGLPIXELSTOREI __glPixelStorei;
		extern PFNGLPIXELTRANSFERF __glPixelTransferf;
		extern PFNGLPIXELTRANSFERI __glPixelTransferi;
		extern PFNGLPIXELZOOM __glPixelZoom;
		extern PFNGLPOINTSIZE __glPointSize;
		extern PFNGLPOLYGONMODE __glPolygonMode;
		extern PFNGLPOLYGONOFFSET __glPolygonOffset;
		extern PFNGLPOLYGONSTIPPLE __glPolygonStipple;
		extern PFNGLPOPATTRIB __glPopAttrib;
		extern PFNGLPOPCLIENTATTRIB __glPopClientAttrib;
		extern PFNGLPOPMATRIX __glPopMatrix;
		extern PFNGLPOPNAME __glPopName;
		extern PFNGLPRIORITIZETEXTURES __glPrioritizeTextures;
		extern PFNGLPUSHATTRIB __glPushAttrib;
		extern PFNGLPUSHCLIENTATTRIB __glPushClientAttrib;
		extern PFNGLPUSHMATRIX __glPushMatrix;
		extern PFNGLPUSHNAME __glPushName;
		extern PFNGLRASTERPOS2D __glRasterPos2d;
		extern PFNGLRASTERPOS2DV __glRasterPos2dv;
		extern PFNGLRASTERPOS2F __glRasterPos2f;
		extern PFNGLRASTERPOS2FV __glRasterPos2fv;
		extern PFNGLRASTERPOS2I __glRasterPos2i;
		extern PFNGLRASTERPOS2IV __glRasterPos2iv;
		extern PFNGLRASTERPOS2S __glRasterPos2s;
		extern PFNGLRASTERPOS2SV __glRasterPos2sv;
		extern PFNGLRASTERPOS3D __glRasterPos3d;
		extern PFNGLRASTERPOS3DV __glRasterPos3dv;
		extern PFNGLRASTERPOS3F __glRasterPos3f;
		extern PFNGLRASTERPOS3FV __glRasterPos3fv;
		extern PFNGLRASTERPOS3I __glRasterPos3i;
		extern PFNGLRASTERPOS3IV __glRasterPos3iv;
		extern PFNGLRASTERPOS3S __glRasterPos3s;
		extern PFNGLRASTERPOS3SV __glRasterPos3sv;
		extern PFNGLRASTERPOS4D __glRasterPos4d;
		extern PFNGLRASTERPOS4DV __glRasterPos4dv;
		extern PFNGLRASTERPOS4F __glRasterPos4f;
		extern PFNGLRASTERPOS4FV __glRasterPos4fv;
		extern PFNGLRASTERPOS4I __glRasterPos4i;
		extern PFNGLRASTERPOS4IV __glRasterPos4iv;
		extern PFNGLRASTERPOS4S __glRasterPos4s;
		extern PFNGLRASTERPOS4SV __glRasterPos4sv;
		extern PFNGLREADBUFFER __glReadBuffer;
		extern PFNGLREADPIXELS __glReadPixels;
		extern PFNGLRECTD __glRectd;
		extern PFNGLRECTDV __glRectdv;
		extern PFNGLRECTF __glRectf;
		extern PFNGLRECTFV __glRectfv;
		extern PFNGLRECTI __glRecti;
		extern PFNGLRECTIV __glRectiv;
		extern PFNGLRECTS __glRects;
		extern PFNGLRECTSV __glRectsv;
		extern PFNGLRENDERMODE __glRenderMode;
		extern PFNGLROTATED __glRotated;
		extern PFNGLROTATEF __glRotatef;
		extern PFNGLSCALED __glScaled;
		extern PFNGLSCALEF __glScalef;
		extern PFNGLSCISSOR __glScissor;
		extern PFNGLSELECTBUFFER __glSelectBuffer;
		extern PFNGLSHADEMODEL __glShadeModel;
		extern PFNGLSTENCILFUNC __glStencilFunc;
		extern PFNGLSTENCILMASK __glStencilMask;
		extern PFNGLSTENCILOP __glStencilOp;
		extern PFNGLTEXCOORD1D __glTexCoord1d;
		extern PFNGLTEXCOORD1DV __glTexCoord1dv;
		extern PFNGLTEXCOORD1F __glTexCoord1f;
		extern PFNGLTEXCOORD1FV __glTexCoord1fv;
		extern PFNGLTEXCOORD1I __glTexCoord1i;
		extern PFNGLTEXCOORD1IV __glTexCoord1iv;
		extern PFNGLTEXCOORD1S __glTexCoord1s;
		extern PFNGLTEXCOORD1SV __glTexCoord1sv;
		extern PFNGLTEXCOORD2D __glTexCoord2d;
		extern PFNGLTEXCOORD2DV __glTexCoord2dv;
		extern PFNGLTEXCOORD2F __glTexCoord2f;
		extern PFNGLTEXCOORD2FV __glTexCoord2fv;
		extern PFNGLTEXCOORD2I __glTexCoord2i;
		extern PFNGLTEXCOORD2IV __glTexCoord2iv;
		extern PFNGLTEXCOORD2S __glTexCoord2s;
		extern PFNGLTEXCOORD2SV __glTexCoord2sv;
		extern PFNGLTEXCOORD3D __glTexCoord3d;
		extern PFNGLTEXCOORD3DV __glTexCoord3dv;
		extern PFNGLTEXCOORD3F __glTexCoord3f;
		extern PFNGLTEXCOORD3FV __glTexCoord3fv;
		extern PFNGLTEXCOORD3I __glTexCoord3i;
		extern PFNGLTEXCOORD3IV __glTexCoord3iv;
		extern PFNGLTEXCOORD3S __glTexCoord3s;
		extern PFNGLTEXCOORD3SV __glTexCoord3sv;
		extern PFNGLTEXCOORD4D __glTexCoord4d;
		extern PFNGLTEXCOORD4DV __glTexCoord4dv;
		extern PFNGLTEXCOORD4F __glTexCoord4f;
		extern PFNGLTEXCOORD4FV __glTexCoord4fv;
		extern PFNGLTEXCOORD4I __glTexCoord4i;
		extern PFNGLTEXCOORD4IV __glTexCoord4iv;
		extern PFNGLTEXCOORD4S __glTexCoord4s;
		extern PFNGLTEXCOORD4SV __glTexCoord4sv;
		extern PFNGLTEXCOORDPOINTER __glTexCoordPointer;
		extern PFNGLTEXENVF __glTexEnvf;
		extern PFNGLTEXENVFV __glTexEnvfv;
		extern PFNGLTEXENVI __glTexEnvi;
		extern PFNGLTEXENVIV __glTexEnviv;
		extern PFNGLTEXGEND __glTexGend;
		extern PFNGLTEXGENDV __glTexGendv;
		extern PFNGLTEXGENF __glTexGenf;
		extern PFNGLTEXGENFV __glTexGenfv;
		extern PFNGLTEXGENI __glTexGeni;
		extern PFNGLTEXGENIV __glTexGeniv;
		extern PFNGLTEXIMAGE1D __glTexImage1D;
		extern PFNGLTEXIMAGE2D __glTexImage2D;
		extern PFNGLTEXPARAMETERF __glTexParameterf;
		extern PFNGLTEXPARAMETERFV __glTexParameterfv;
		extern PFNGLTEXPARAMETERI __glTexParameteri;
		extern PFNGLTEXPARAMETERIV __glTexParameteriv;
		extern PFNGLTEXSUBIMAGE1D __glTexSubImage1D;
		extern PFNGLTEXSUBIMAGE2D __glTexSubImage2D;
		extern PFNGLTRANSLATED __glTranslated;
		extern PFNGLTRANSLATEF __glTranslatef;
		extern PFNGLVERTEX2D __glVertex2d;
		extern PFNGLVERTEX2DV __glVertex2dv;
		extern PFNGLVERTEX2F __glVertex2f;
		extern PFNGLVERTEX2FV __glVertex2fv;
		extern PFNGLVERTEX2I __glVertex2i;
		extern PFNGLVERTEX2IV __glVertex2iv;
		extern PFNGLVERTEX2S __glVertex2s;
		extern PFNGLVERTEX2SV __glVertex2sv;
		extern PFNGLVERTEX3D __glVertex3d;
		extern PFNGLVERTEX3DV __glVertex3dv;
		extern PFNGLVERTEX3F __glVertex3f;
		extern PFNGLVERTEX3FV __glVertex3fv;
		extern PFNGLVERTEX3I __glVertex3i;
		extern PFNGLVERTEX3IV __glVertex3iv;
		extern PFNGLVERTEX3S __glVertex3s;
		extern PFNGLVERTEX3SV __glVertex3sv;
		extern PFNGLVERTEX4D __glVertex4d;
		extern PFNGLVERTEX4DV __glVertex4dv;
		extern PFNGLVERTEX4F __glVertex4f;
		extern PFNGLVERTEX4FV __glVertex4fv;
		extern PFNGLVERTEX4I __glVertex4i;
		extern PFNGLVERTEX4IV __glVertex4iv;
		extern PFNGLVERTEX4S __glVertex4s;
		extern PFNGLVERTEX4SV __glVertex4sv;
		extern PFNGLVERTEXPOINTER __glVertexPointer;
		extern PFNGLVIEWPORT __glViewport;

#define glAccum __glAccum
#define glAlphaFunc __glAlphaFunc
#define glAreTexturesResident __glAreTexturesResident
#define glArrayElement __glArrayElement
#define glBegin __glBegin
#define glBindTexture __glBindTexture
#define glBitmap __glBitmap
#define glBlendFunc __glBlendFunc
#define glCallList __glCallList
#define glCallLists __glCallLists
#define glClear __glClear
#define glClearAccum __glClearAccum
#define glClearColor __glClearColor
#define glClearDepth __glClearDepth
#define glClearIndex __glClearIndex
#define glClearStencil __glClearStencil
#define glClipPlane __glClipPlane
#define glColor3b __glColor3b
#define glColor3bv __glColor3bv
#define glColor3d __glColor3d
#define glColor3dv __glColor3dv
#define glColor3f __glColor3f
#define glColor3fv __glColor3fv
#define glColor3i __glColor3i
#define glColor3iv __glColor3iv
#define glColor3s __glColor3s
#define glColor3sv __glColor3sv
#define glColor3ub __glColor3ub
#define glColor3ubv __glColor3ubv
#define glColor3ui __glColor3ui
#define glColor3uiv __glColor3uiv
#define glColor3us __glColor3us
#define glColor3usv __glColor3usv
#define glColor4b __glColor4b
#define glColor4bv __glColor4bv
#define glColor4d __glColor4d
#define glColor4dv __glColor4dv
#define glColor4f __glColor4f
#define glColor4fv __glColor4fv
#define glColor4i __glColor4i
#define glColor4iv __glColor4iv
#define glColor4s __glColor4s
#define glColor4sv __glColor4sv
#define glColor4ub __glColor4ub
#define glColor4ubv __glColor4ubv
#define glColor4ui __glColor4ui
#define glColor4uiv __glColor4uiv
#define glColor4us __glColor4us
#define glColor4usv __glColor4usv
#define glColorMask __glColorMask
#define glColorMaterial __glColorMaterial
#define glColorPointer __glColorPointer
#define glCopyPixels __glCopyPixels
#define glCopyTexImage1D __glCopyTexImage1D
#define glCopyTexImage2D __glCopyTexImage2D
#define glCopyTexSubImage1D __glCopyTexSubImage1D
#define glCopyTexSubImage2D __glCopyTexSubImage2D
#define glCullFace __glCullFace
#define glDeleteLists __glDeleteLists
#define glDeleteTextures __glDeleteTextures
#define glDepthFunc __glDepthFunc
#define glDepthMask __glDepthMask
#define glDepthRange __glDepthRange
#define glDisable __glDisable
#define glDisableClientState __glDisableClientState
#define glDrawArrays __glDrawArrays
#define glDrawBuffer __glDrawBuffer
#define glDrawElements __glDrawElements
#define glDrawPixels __glDrawPixels
#define glEdgeFlag __glEdgeFlag
#define glEdgeFlagPointer __glEdgeFlagPointer
#define glEdgeFlagv __glEdgeFlagv
#define glEnable __glEnable
#define glEnableClientState __glEnableClientState
#define glEnd __glEnd
#define glEndList __glEndList
#define glEvalCoord1d __glEvalCoord1d
#define glEvalCoord1dv __glEvalCoord1dv
#define glEvalCoord1f __glEvalCoord1f
#define glEvalCoord1fv __glEvalCoord1fv
#define glEvalCoord2d __glEvalCoord2d
#define glEvalCoord2dv __glEvalCoord2dv
#define glEvalCoord2f __glEvalCoord2f
#define glEvalCoord2fv __glEvalCoord2fv
#define glEvalMesh1 __glEvalMesh1
#define glEvalMesh2 __glEvalMesh2
#define glEvalPoint1 __glEvalPoint1
#define glEvalPoint2 __glEvalPoint2
#define glFeedbackBuffer __glFeedbackBuffer
#define glFinish __glFinish
#define glFlush __glFlush
#define glFogf __glFogf
#define glFogfv __glFogfv
#define glFogi __glFogi
#define glFogiv __glFogiv
#define glFrontFace __glFrontFace
#define glFrustum __glFrustum
#define glGenLists __glGenLists
#define glGenTextures __glGenTextures
#define glGetBooleanv __glGetBooleanv
#define glGetClipPlane __glGetClipPlane
#define glGetDoublev __glGetDoublev
#define glGetError __glGetError
#define glGetFloatv __glGetFloatv
#define glGetIntegerv __glGetIntegerv
#define glGetLightfv __glGetLightfv
#define glGetLightiv __glGetLightiv
#define glGetMapdv __glGetMapdv
#define glGetMapfv __glGetMapfv
#define glGetMapiv __glGetMapiv
#define glGetMaterialfv __glGetMaterialfv
#define glGetMaterialiv __glGetMaterialiv
#define glGetPixelMapfv __glGetPixelMapfv
#define glGetPixelMapuiv __glGetPixelMapuiv
#define glGetPixelMapusv __glGetPixelMapusv
#define glGetPointerv __glGetPointerv
#define glGetPolygonStipple __glGetPolygonStipple
#define glGetString __glGetString
#define glGetTexEnvfv __glGetTexEnvfv
#define glGetTexEnviv __glGetTexEnviv
#define glGetTexGendv __glGetTexGendv
#define glGetTexGenfv __glGetTexGenfv
#define glGetTexGeniv __glGetTexGeniv
#define glGetTexImage __glGetTexImage
#define glGetTexLevelParameterfv __glGetTexLevelParameterfv
#define glGetTexLevelParameteriv __glGetTexLevelParameteriv
#define glGetTexParameterfv __glGetTexParameterfv
#define glGetTexParameteriv __glGetTexParameteriv
#define glHint __glHint
#define glIndexMask __glIndexMask
#define glIndexPointer __glIndexPointer
#define glIndexd __glIndexd
#define glIndexdv __glIndexdv
#define glIndexf __glIndexf
#define glIndexfv __glIndexfv
#define glIndexi __glIndexi
#define glIndexiv __glIndexiv
#define glIndexs __glIndexs
#define glIndexsv __glIndexsv
#define glIndexub __glIndexub
#define glIndexubv __glIndexubv
#define glInitNames __glInitNames
#define glInterleavedArrays __glInterleavedArrays
#define glIsEnabled __glIsEnabled
#define glIsList __glIsList
#define glIsTexture __glIsTexture
#define glLightModelf __glLightModelf
#define glLightModelfv __glLightModelfv
#define glLightModeli __glLightModeli
#define glLightModeliv __glLightModeliv
#define glLightf __glLightf
#define glLightfv __glLightfv
#define glLighti __glLighti
#define glLightiv __glLightiv
#define glLineStipple __glLineStipple
#define glLineWidth __glLineWidth
#define glListBase __glListBase
#define glLoadIdentity __glLoadIdentity
#define glLoadMatrixd __glLoadMatrixd
#define glLoadMatrixf __glLoadMatrixf
#define glLoadName __glLoadName
#define glLogicOp __glLogicOp
#define glMap1d __glMap1d
#define glMap1f __glMap1f
#define glMap2d __glMap2d
#define glMap2f __glMap2f
#define glMapGrid1d __glMapGrid1d
#define glMapGrid1f __glMapGrid1f
#define glMapGrid2d __glMapGrid2d
#define glMapGrid2f __glMapGrid2f
#define glMaterialf __glMaterialf
#define glMaterialfv __glMaterialfv
#define glMateriali __glMateriali
#define glMaterialiv __glMaterialiv
#define glMatrixMode __glMatrixMode
#define glMultMatrixd __glMultMatrixd
#define glMultMatrixf __glMultMatrixf
#define glNewList __glNewList
#define glNormal3b __glNormal3b
#define glNormal3bv __glNormal3bv
#define glNormal3d __glNormal3d
#define glNormal3dv __glNormal3dv
#define glNormal3f __glNormal3f
#define glNormal3fv __glNormal3fv
#define glNormal3i __glNormal3i
#define glNormal3iv __glNormal3iv
#define glNormal3s __glNormal3s
#define glNormal3sv __glNormal3sv
#define glNormalPointer __glNormalPointer
#define glOrtho __glOrtho
#define glPassThrough __glPassThrough
#define glPixelMapfv __glPixelMapfv
#define glPixelMapuiv __glPixelMapuiv
#define glPixelMapusv __glPixelMapusv
#define glPixelStoref __glPixelStoref
#define glPixelStorei __glPixelStorei
#define glPixelTransferf __glPixelTransferf
#define glPixelTransferi __glPixelTransferi
#define glPixelZoom __glPixelZoom
#define glPointSize __glPointSize
#define glPolygonMode __glPolygonMode
#define glPolygonOffset __glPolygonOffset
#define glPolygonStipple __glPolygonStipple
#define glPopAttrib __glPopAttrib
#define glPopClientAttrib __glPopClientAttrib
#define glPopMatrix __glPopMatrix
#define glPopName __glPopName
#define glPrioritizeTextures __glPrioritizeTextures
#define glPushAttrib __glPushAttrib
#define glPushClientAttrib __glPushClientAttrib
#define glPushMatrix __glPushMatrix
#define glPushName __glPushName
#define glRasterPos2d __glRasterPos2d
#define glRasterPos2dv __glRasterPos2dv
#define glRasterPos2f __glRasterPos2f
#define glRasterPos2fv __glRasterPos2fv
#define glRasterPos2i __glRasterPos2i
#define glRasterPos2iv __glRasterPos2iv
#define glRasterPos2s __glRasterPos2s
#define glRasterPos2sv __glRasterPos2sv
#define glRasterPos3d __glRasterPos3d
#define glRasterPos3dv __glRasterPos3dv
#define glRasterPos3f __glRasterPos3f
#define glRasterPos3fv __glRasterPos3fv
#define glRasterPos3i __glRasterPos3i
#define glRasterPos3iv __glRasterPos3iv
#define glRasterPos3s __glRasterPos3s
#define glRasterPos3sv __glRasterPos3sv
#define glRasterPos4d __glRasterPos4d
#define glRasterPos4dv __glRasterPos4dv
#define glRasterPos4f __glRasterPos4f
#define glRasterPos4fv __glRasterPos4fv
#define glRasterPos4i __glRasterPos4i
#define glRasterPos4iv __glRasterPos4iv
#define glRasterPos4s __glRasterPos4s
#define glRasterPos4sv __glRasterPos4sv
#define glReadBuffer __glReadBuffer
#define glReadPixels __glReadPixels
#define glRectd __glRectd
#define glRectdv __glRectdv
#define glRectf __glRectf
#define glRectfv __glRectfv
#define glRecti __glRecti
#define glRectiv __glRectiv
#define glRects __glRects
#define glRectsv __glRectsv
#define glRenderMode __glRenderMode
#define glRotated __glRotated
#define glRotatef __glRotatef
#define glScaled __glScaled
#define glScalef __glScalef
#define glScissor __glScissor
#define glSelectBuffer __glSelectBuffer
#define glShadeModel __glShadeModel
#define glStencilFunc __glStencilFunc
#define glStencilMask __glStencilMask
#define glStencilOp __glStencilOp
#define glTexCoord1d __glTexCoord1d
#define glTexCoord1dv __glTexCoord1dv
#define glTexCoord1f __glTexCoord1f
#define glTexCoord1fv __glTexCoord1fv
#define glTexCoord1i __glTexCoord1i
#define glTexCoord1iv __glTexCoord1iv
#define glTexCoord1s __glTexCoord1s
#define glTexCoord1sv __glTexCoord1sv
#define glTexCoord2d __glTexCoord2d
#define glTexCoord2dv __glTexCoord2dv
#define glTexCoord2f __glTexCoord2f
#define glTexCoord2fv __glTexCoord2fv
#define glTexCoord2i __glTexCoord2i
#define glTexCoord2iv __glTexCoord2iv
#define glTexCoord2s __glTexCoord2s
#define glTexCoord2sv __glTexCoord2sv
#define glTexCoord3d __glTexCoord3d
#define glTexCoord3dv __glTexCoord3dv
#define glTexCoord3f __glTexCoord3f
#define glTexCoord3fv __glTexCoord3fv
#define glTexCoord3i __glTexCoord3i
#define glTexCoord3iv __glTexCoord3iv
#define glTexCoord3s __glTexCoord3s
#define glTexCoord3sv __glTexCoord3sv
#define glTexCoord4d __glTexCoord4d
#define glTexCoord4dv __glTexCoord4dv
#define glTexCoord4f __glTexCoord4f
#define glTexCoord4fv __glTexCoord4fv
#define glTexCoord4i __glTexCoord4i
#define glTexCoord4iv __glTexCoord4iv
#define glTexCoord4s __glTexCoord4s
#define glTexCoord4sv __glTexCoord4sv
#define glTexCoordPointer __glTexCoordPointer
#define glTexEnvf __glTexEnvf
#define glTexEnvfv __glTexEnvfv
#define glTexEnvi __glTexEnvi
#define glTexEnviv __glTexEnviv
#define glTexGend __glTexGend
#define glTexGendv __glTexGendv
#define glTexGenf __glTexGenf
#define glTexGenfv __glTexGenfv
#define glTexGeni __glTexGeni
#define glTexGeniv __glTexGeniv
#define glTexImage1D __glTexImage1D
#define glTexImage2D __glTexImage2D
#define glTexParameterf __glTexParameterf
#define glTexParameterfv __glTexParameterfv
#define glTexParameteri __glTexParameteri
#define glTexParameteriv __glTexParameteriv
#define glTexSubImage1D __glTexSubImage1D
#define glTexSubImage2D __glTexSubImage2D
#define glTranslated __glTranslated
#define glTranslatef __glTranslatef
#define glVertex2d __glVertex2d
#define glVertex2dv __glVertex2dv
#define glVertex2f __glVertex2f
#define glVertex2fv __glVertex2fv
#define glVertex2i __glVertex2i
#define glVertex2iv __glVertex2iv
#define glVertex2s __glVertex2s
#define glVertex2sv __glVertex2sv
#define glVertex3d __glVertex3d
#define glVertex3dv __glVertex3dv
#define glVertex3f __glVertex3f
#define glVertex3fv __glVertex3fv
#define glVertex3i __glVertex3i
#define glVertex3iv __glVertex3iv
#define glVertex3s __glVertex3s
#define glVertex3sv __glVertex3sv
#define glVertex4d __glVertex4d
#define glVertex4dv __glVertex4dv
#define glVertex4f __glVertex4f
#define glVertex4fv __glVertex4fv
#define glVertex4i __glVertex4i
#define glVertex4iv __glVertex4iv
#define glVertex4s __glVertex4s
#define glVertex4sv __glVertex4sv
#define glVertexPointer __glVertexPointer
#define glViewport __glViewport

#endif

		bool initGLExtenstion() noexcept;
	}
}

#endif