#version 330 core

in vec4 vertexColor;// 레스터라이제이션 과정을 거쳐 vs에서 넘어온다.
in vec2 texCoord;
// uniform vec4 color;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
	fragColor = texture(tex, texCoord) * 0.8 + texture(tex2, texCoord) * 0.2 ;
}
 