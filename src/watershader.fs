#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float worldLight;
// texture samplers
uniform sampler2D texture1;

void main()
{
	vec4 base = texture(texture1, TexCoord);
    FragColor = vec4(base.rgb * worldLight, base.a);

}