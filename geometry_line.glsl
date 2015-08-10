#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in VertexData {
	vec4 color;
} vertexIn[];
out float alpha;
out VertexData {
	vec4 color;
} vertexOut;

uniform float xpos;
uniform float ypos;
uniform int window_width;
uniform int window_height;
uniform float max_dist_pm;

void main() {
	float dist = distance(vec4(gl_in[0].gl_Position.x * window_width / window_height, gl_in[0].gl_Position.yzw), vec4(xpos * window_width / window_height, ypos, 0.0, 1.0));
	if(dist <= max_dist_pm) {
		gl_Position = gl_in[0].gl_Position;
		alpha = 1.0 - dist / max_dist_pm;
		vertexOut.color = vertexIn[0].color;
		EmitVertex();
		gl_Position = vec4(xpos, ypos, 0.0, 1.0);
		alpha = 1.0 - dist / max_dist_pm;
		vertexOut.color = vertexIn[0].color;
		EmitVertex();
		EndPrimitive();
	}
}
