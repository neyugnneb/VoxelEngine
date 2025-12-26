#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float Light;

uniform float worldLight;
// texture samplers
uniform sampler2D texture1;

void main()
{
	vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(texColor.rgb * Light * worldLight, texColor.a);

}