#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData {
	vec4 color;
} vertexIn[];
out VertexData {
	vec4 color;
} vertexOut;

uniform float xpos;
uniform float ypos;
uniform int window_width;
uniform int window_height;
uniform float max_dist_pm;
uniform float max_dist_pp;

float triangle_alpha = 0.2;

void main() {
	float dist0 = distance(vec4(gl_in[0].gl_Position.x * window_width / window_height, gl_in[0].gl_Position.yzw), vec4(xpos * window_width / window_height, ypos, 0.0, 1.0));
	float dist1 = distance(vec4(gl_in[1].gl_Position.x * window_width / window_height, gl_in[1].gl_Position.yzw), vec4(xpos * window_width / window_height, ypos, 0.0, 1.0));
//	float dist2 = distance(vec4(gl_in[0].gl_Position.x * window_width / window_height, gl_in[0].gl_Position.yzw), vec4(gl_in[1].gl_Position.x * window_width / window_height, gl_in[1].gl_Position.yzw));
	if(dist0 <= max_dist_pm && dist1 <= max_dist_pm) {
		gl_Position = gl_in[0].gl_Position;
		vertexOut.color = vec4(vertexIn[0].color.rgb, vertexIn[0].color.a * (1.0 - dist0 / max_dist_pm) * triangle_alpha); // 1
//		vertexOut.color = vec4(vertexIn[0].color.rgb, vertexIn[0].color.a * (1.0 - dist0 / max_dist_pm) * triangle_alpha * (1 - dist2 / max_dist_pp)); // 2
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		vertexOut.color = vec4(vertexIn[1].color.rgb, vertexIn[1].color.a * (1.0 - dist1 / max_dist_pm) * triangle_alpha); // 1
//		vertexOut.color = vec4(vertexIn[1].color.rgb, vertexIn[1].color.a * (1.0 - dist1 / max_dist_pm) * triangle_alpha * (1 - dist2 / max_dist_pp)); // 2
		EmitVertex();
		gl_Position = vec4(xpos, ypos, 0.0, 1.0);
//		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, triangle_alpha); // 1.1
		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, (1.0 - (dist0 + dist1) / 2 / max_dist_pm) * triangle_alpha); // 1.2
//		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, (1.0 - (dist0 + dist1) / 2 / max_dist_pm) * triangle_alpha * (1 - dist2 / max_dist_pp)); // 2
		EmitVertex();
		EndPrimitive();

/*		gl_Position = gl_in[0].gl_Position;
		vertexOut.color = vec4(vertexIn[0].color.rgb, vertexIn[0].color.a * (1.0 - dist0 / max_dist_pm));
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		vertexOut.color = vec4(vertexIn[1].color.rgb, vertexIn[1].color.a * (1.0 - dist1 / max_dist_pm));
		EmitVertex();
		gl_Position = gl_in[0].gl_Position + vec4(0.001, 0.001, 0, 0);
		vertexOut.color = vec4(vertexIn[0].color.rgb, vertexIn[0].color.a * (1.0 - dist0 / max_dist_pm));
		EmitVertex();
		gl_Position = gl_in[1].gl_Position + vec4(0.001, 0.001, 0, 0);
		vertexOut.color = vec4(vertexIn[1].color.rgb, vertexIn[1].color.a * (1.0 - dist1 / max_dist_pm));
		EmitVertex();
		EndPrimitive();*/

/*		gl_Position = gl_in[0].gl_Position;
//		vertexOut.color = vec4(vertexIn[0].color.rgb, vertexIn[0].color.a * (1.0 - dist0 / max_dist_pm) * triangle_alpha);
		vertexOut.color = vec4(vertexIn[0].color.rgb, vertexIn[0].color.a * (1.0 - dist0 / max_dist_pm) * triangle_alpha * (1 - dist2 / max_dist_pp));
		EmitVertex();
		gl_Position = reflect(normalize(vec4(xpos, ypos, 0.0, 1.0) - gl_in[1].gl_Position), normalize(gl_in[0].gl_Position - vec4(xpos, ypos, 0.0, 1.0)))
		* (2 * length(vec4(xpos, ypos, 0.0, 1.0) - gl_in[0].gl_Position) - length(vec4(xpos, ypos, 0.0, 1.0) - gl_in[1].gl_Position)) + vec4(xpos, ypos, 0.0, 1.0);
		vertexOut.color = vec4(vertexIn[0].color.rgb, 0);
		EmitVertex();
		gl_Position = vec4(xpos, ypos, 0.0, 1.0);
//		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, triangle_alpha);
//		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, (1.0 - (dist0 + dist1) / 2 / max_dist_pm) * triangle_alpha);
		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, (1.0 - (dist0 + dist1) / 2 / max_dist_pm) * triangle_alpha * (1 - dist2 / max_dist_pp));
		EmitVertex();
		EndPrimitive();

		gl_Position = reflect(normalize(vec4(xpos, ypos, 0.0, 1.0) - gl_in[0].gl_Position), normalize(gl_in[1].gl_Position - vec4(xpos, ypos, 0.0, 1.0)))
		* (2 * length(vec4(xpos, ypos, 0.0, 1.0) - gl_in[1].gl_Position) - length(vec4(xpos, ypos, 0.0, 1.0) - gl_in[0].gl_Position)) + vec4(xpos, ypos, 0.0, 1.0);
		vertexOut.color = vec4(vertexIn[1].color.rgb, 0);
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
//		vertexOut.color = vec4(vertexIn[1].color.rgb, vertexIn[1].color.a * (1.0 - dist1 / max_dist_pm) * triangle_alpha);
		vertexOut.color = vec4(vertexIn[1].color.rgb, vertexIn[0].color.a * (1.0 - dist1 / max_dist_pm) * triangle_alpha * (1 - dist2 / max_dist_pp));
		EmitVertex();
		gl_Position = vec4(xpos, ypos, 0.0, 1.0);
//		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, triangle_alpha);
//		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, (1.0 - (dist0 + dist1) / 2 / max_dist_pm) * triangle_alpha);
		vertexOut.color = vec4(((vertexIn[0].color + vertexIn[1].color) / 2).rgb, (1.0 - (dist0 + dist1) / 2 / max_dist_pm) * triangle_alpha * (1 - dist2 / max_dist_pp));
		EmitVertex();
		EndPrimitive();*/
	}
}
