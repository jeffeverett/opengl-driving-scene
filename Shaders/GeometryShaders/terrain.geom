#version 400

// Triangles in and out
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// Input
in vec3 tePosition[3];
in vec2 teTexCoords[3];
in vec3 tePatchDistance[3];

// Output
out vec3 gPosition;
out vec2 gTexCoords;
out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;

// Uniforms
uniform mat3 model;

void main()
{
   // Compute normal as cross product
   vec3 A = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
   vec3 B = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);

   // Transform normal
   mat3 normalMatrix = transpose(inverse(mat3(model)));
   gFacetNormal = normalMatrix * normalize(cross(A, B));

   // First vertex
   gPosition = tePosition[0];
   gTexCoords = teTexCoords[0];
   gPatchDistance = tePatchDistance[0];
   gTriDistance = vec3(1, 0, 0);
   gl_Position = gl_in[0].gl_Position;
   EmitVertex();

   // Second vertex
   gPosition = tePosition[1];
   gTexCoords = teTexCoords[1];
   gPatchDistance = tePatchDistance[1];
   gTriDistance = vec3(0, 1, 0);
   gl_Position = gl_in[1].gl_Position;
   EmitVertex();

   // Third vertex
   gPosition = tePosition[2];
   gTexCoords = teTexCoords[2];
   gPatchDistance = tePatchDistance[2];
   gTriDistance = vec3(0, 0, 1);
   gl_Position = gl_in[2].gl_Position;
   EmitVertex();

   // Done with triangle
   EndPrimitive();
}
