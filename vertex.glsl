#version 330 core

layout(location = 0) in vec2 vertexPosition_modelspace;
layout(location = 1) in vec4 color;

out VertexData {
	vec4 color;
} vertexOut;
out float alpha;

void main() {
	gl_Position = vec4(vertexPosition_modelspace, 0.0, 1.0);
	vertexOut.color = color;
	alpha = color.a;
}
