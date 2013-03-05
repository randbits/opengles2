
# we just directly link to it.
noinst_LTLIBRARIES = libglesut.la

libglesut_la_SOURCES = \
	esShader.c \
	esShapes.c \
	esTransform.c \
	esUtil.c \
	esUtil.h \
	$(NULL)

libglesut_la_CPPFLAGS = \
	@X11_CFLAGS@ \
	@EGL_CFLAGS@ \
	@GLESV2_CFLAGS@ \
	$(NULL)

libglesut_la_LIBADD = \
	@X11_LIBS@ \
	@EGL_LIBS@ \
	@GLESV2_LIBS@ \
	$(NULL)
