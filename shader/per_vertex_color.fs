#version 330 core

in vec4 vertexColor;// 레스터라이제이션 과정을 거쳐 vs에서 넘어온다.
// uniform vec4 color;
out vec4 fragColor;

void main() {
	fragColor = vertexColor;
}
