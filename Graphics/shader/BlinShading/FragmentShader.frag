/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FragmnetShader.frag
Purpose: Fragment shader for Blin Shading
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 11/06/2021
End Header --------------------------------------------------------*/

#version 430 core

in vec4 normal;
in vec3 fragPos;
in vec3 entity;
flat in int doCalc;
in vec2 uvs;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform vec3 objColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 g_ambient;
uniform vec3 fogColor;
uniform float zNear;
uniform float zFar;

struct Material
{
	vec3 kd;
	vec3 ks;
};

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
};

struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	vec3 color;
	float cutOff;
	float outerCutOff;
	float fallOff;
};

uniform int numDir;
uniform int numPoint;
uniform int numSpot;

uniform int mapType;

uniform DirLight dirLights[16];
uniform PointLight pointLights[16];
uniform SpotLight spotLights[16];

out vec4 fragColor;

vec3 Halfway(vec3 light, vec3 view)
{
	vec3 result = normalize(light + view);
	return result;
}

vec2 CubeMapping(vec3 vEntity)
{
	vec3 absVec = abs(vEntity);
	vec2 uv = vec2(0.0);


	if(absVec.x >= absVec.y && absVec.x >= absVec.z)
	{
		(vEntity.x < 0.0) ? (uv.x = vEntity.z) : (uv.x = -vEntity.z);
		uv.y = vEntity.y;
	}
	else if(absVec.y >= absVec.x && absVec.y >= absVec.z)
	{
		(vEntity.y < 0.0) ? (uv.y = vEntity.z) : (uv.y = -vEntity.z);
		uv.x = vEntity.x;
	}
	else if(absVec.z >= absVec.y && absVec.z >= absVec.x)
	{
		(vEntity.z < 0.0) ? (uv.x = -vEntity.x) : (uv.x = vEntity.x);
		uv.y = vEntity.y;
	}

	return (uv + vec2(1.0)) * 0.5;
}

vec2 CylindricalMapping(vec3 vEntity)
{
	float theta = atan(vEntity.y / vEntity.x);
	float z = (vEntity.z - (-1.0)) / 2.0;
	vec2 uv = vec2(theta / radians(360.f), z);
	return (uv + vec2(1.0)) * 0.5;
}

vec2 SphericalMapping(vec3 vEntity)
{
	float theta = atan(vEntity.y / vEntity.x);
	float r = sqrt(vEntity.x * vEntity.x + vEntity.y * vEntity.y + vEntity.z * vEntity.z);
	float phi = acos(vEntity.z / r);

	vec2 uv = vec2(theta / radians(360.f), (radians(180.f) - phi)/ radians(180.f) );
	return (uv + vec2(1.0)) * 0.5;
}


vec3 CalDirectionalLights(DirLight light, vec3 norm, vec3 viewDir, Material material)
{
	vec3 lightDir = normalize(-light.direction);

	float diff = max(dot(norm, lightDir), 0.0);

	vec3 halfWay = Halfway(lightDir, viewDir);
	float spec = pow(max(dot(norm, halfWay), 0.0), 30.0f);

	vec3 ambient = light.ambient * light.color;
	vec3 diffuse = light.diffuse * diff * material.kd * light.color;
	vec3 specular = light.specular * spec * material.ks * light.color;

	vec3 result = (ambient + diffuse + specular);
	return result;
}

vec3 CalPointLights(PointLight light, vec3 norm, vec3 viewDir, Material material)
{
	vec3 ambient = light.ambient * light.color;

	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * material.kd * light.color;

	vec3 halfWay = Halfway(lightDir, viewDir);
	float spec = pow(max(dot(norm, halfWay), 0.0), 30.0f);
	vec3 specular = light.specular * spec * material.ks * light.color;

	float distance = length(light.position - fragPos);
	float att = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= att;
	diffuse *= att;
	specular *= att;

	vec3 result = (ambient + diffuse + specular);
	return result;

}
vec3 CalSpotLights(SpotLight light, vec3 norm, vec3 viewDir, Material material)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 ambient = light.ambient * light.color;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * material.kd * light.color;

	vec3 halfWay = Halfway(lightDir, norm);
	float spec = pow(max(dot(norm, halfWay), 0.0), 30.0f);
	vec3 specular = light.specular * spec * material.ks * light.color;

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	intensity = pow(intensity, light.fallOff);
	diffuse *= intensity;
	specular *= intensity;

	float distance = length(light.position - fragPos);
	float att = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= att;
	diffuse *= att;
	specular *= att;

	vec3 result = ambient + diffuse + specular;
	return result;
}

void main()
{
	vec3 local = vec3(0.0);
	vec3 norm = normalize(normal.xyz);
	vec3 viewDir = normalize(viewPos - fragPos);

	float S = (zFar - distance(viewPos, fragPos)) / zFar - zNear;

	vec2 uv;
	if (doCalc == 1)
	{
		// planar = 1, cylin = 2, sph = 3
		if (mapType == 1)
		{
			uv = CubeMapping(entity);
		}
		else if (mapType == 2)
		{
			uv = CylindricalMapping(entity);
		}
		else if (mapType == 3)
		{
			uv = SphericalMapping(entity);
		}
	}
	else
	{
		uv = uvs;
	}

	Material material;
	material.kd = texture(tex1, uv).rgb;
	material.ks = texture(tex2, uv).rgb;

	for (int i = 0; i < numDir; ++i)
	{
		local += CalDirectionalLights(dirLights[i], norm, viewDir, material);
	}

	for (int i = 0; i < numPoint; ++i)
	{
		local += CalPointLights(pointLights[i], norm, viewDir, material);
	}

	for (int i = 0; i < numSpot; ++i)
	{
		local += CalSpotLights(spotLights[i], norm, viewDir, material);
	}

	local += g_ambient;
	vec3 result = S * local + (1 - S) * fogColor;
	fragColor = vec4(result, 1.0);
}