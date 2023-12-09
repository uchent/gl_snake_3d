#version 400

uniform vec4 Light_specular;
uniform vec4 Light_diffuse;
uniform vec4 Light_ambient;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
uniform sampler2D mainTexture;
uniform sampler2D normalTexture;

in vec2 Texcoord;
in vec3 lightDir, eyeDir;
in float attenuation;

out vec4 outColor;

void main (void)
{
	vec3 normal = texture(normalTexture, Texcoord.st).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec4 ambientTerm, diffuseTerm, specularTerm;

	ambientTerm = Light_ambient * vec4(Ka, 1.0);

	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	vec3 E = normalize(eyeDir);
	vec3 R = reflect(-L, N);

	float D = dot(N,L);
	
	if(D > 0.0)
	{
		diffuseTerm = Light_diffuse * vec4(Kd, 1.0) * D;

		float specular = pow( max(dot(R, E), 0.0), Ns);
		specularTerm = Light_specular * vec4(Ks, 1.0) * specular;
	}

	//outColor = vec4(0.0,1.0,0.0,1.0) * (attenuation * diffuseTerm + ambientTerm) + attenuation * specularTerm;
	outColor = texture2D(mainTexture, Texcoord.st) * (attenuation * diffuseTerm + ambientTerm) + attenuation * specularTerm;
}