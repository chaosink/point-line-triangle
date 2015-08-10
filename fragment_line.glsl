#version 330 core

in float alpha;
in VertexData {
	vec4 color;
} vertexIn;
out vec4 color;

void main() {
	color = vec4(vertexIn.color.rgb, alpha);
}
