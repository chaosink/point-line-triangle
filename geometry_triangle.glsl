#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 3) out;

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

float triangle_alpha = 0.1;

void main() {
	float dist0 = distance( vec4( gl_in[0].gl_Position.x * window_width / window_height, gl_in[0].gl_Position.yzw ), vec4( xpos * window_width / window_height, ypos, 0.0, 1.0 ) );
	float dist1 = distance( vec4( gl_in[1].gl_Position.x * window_width / window_height, gl_in[1].gl_Position.yzw ), vec4( xpos * window_width / window_height, ypos, 0.0, 1.0 ) );
	if( dist0 <= max_dist_pm && dist1 <= max_dist_pm ) {
		gl_Position = gl_in[0].gl_Position;
		alpha = (1.0 - dist0 / max_dist_pm) * triangle_alpha;
		vertexOut.color = vertexIn[0].color;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		alpha = (1.0 - dist1 / max_dist_pm) * triangle_alpha;
		vertexOut.color = vertexIn[1].color;
		EmitVertex();
		gl_Position = vec4( xpos, ypos, 0.0, 1.0 );
		alpha = (1.0 - (dist0 + dist1) / 2 / max_dist_pm) * triangle_alpha;
		vertexOut.color = (vertexIn[0].color + vertexIn[1].color) / 2;
		EmitVertex();
		EndPrimitive();
	}
}

