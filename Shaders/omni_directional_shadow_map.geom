// vertex shader handles each vertex individually,
// the geometry shader handles primitives, and primitives
// are things formed from more vertexes like triangles, lines
// pyramids, create more, change position values, etc.
// When we do the shadow render pass, we can create views
// in all 6 directions of our cube. 
#version 330
// we expect triangles to come in (gl draw triangles)
layout (triangles) in;
// a triangle strip is an array of triangles
// that reuses 2 of the vertices of the previous one
// so it's easier to define the strip formed by the triangles
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightMatrices[6];

out vec4 fragPos;

void main()
{
	// for each of the faces 
	for(int face = 0; face < 6; ++face)
	{
		// by selecting the GL_LAYER, we define which one of those
		// six textures we want to draw to. 
		gl_Layer = face;
		// go through each of the vertices we have been passed
		for(int i = 0; i < 3; i++)
		{
			// pass the fragment position in the pipeline
			fragPos = gl_in[i].gl_Position;
			// setup gl_position which is the one 
			// we shall emit. We want to get the fragment 
			// position with respect to the projection
			// light matrix
			gl_Position = lightMatrices[face] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}