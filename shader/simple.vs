#version 330 core
layout (location = 0) in vec3 aPos; // location = 0 이 곧 VAO attribute 0번을 의미
 
// out vec4 vertexColor;

void main() {
	gl_Position = vec4(aPos, 1.0);
	// vertexColor = vec4(0.5, 0.5, 0.5, 1.0);
}