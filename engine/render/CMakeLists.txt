#--------------------------------------------------------------------------
# render
#--------------------------------------------------------------------------

SET(files_render_display
	window.h
	window.cc

	Vertex.h

	MeshResource.h
	MeshResource.cc

	Camera.h
	Camera.cc

	TextureResource.h
	TextureResource.cc

	ShaderResource.h
	ShaderResource.cc

	GraphicNode.h
	GraphicNode.cc

	Lighting.h
	Lighting.cc

	stb_image_write.h
	tiny_gltf.h
	json.hpp)
SOURCE_GROUP("display" FILES ${files_render_display})


SET(files_render
	${files_render_display}
	${files_render_pch})

SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(render STATIC ${files_render} ${files_pch})
TARGET_PCH(render ../)	
ADD_DEPENDENCIES(render glew glfw)
TARGET_LINK_LIBRARIES(render PUBLIC engine exts glew glfw imgui ${OPENGL_LIBS})