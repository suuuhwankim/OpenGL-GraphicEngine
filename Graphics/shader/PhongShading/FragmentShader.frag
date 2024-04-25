/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VertexShader.frag
Purpose: Fragment shader for Phong Shading
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 11/05/2021
End Header --------------------------------------------------------*/

#version 430 core

in vec4 normal;
in vec3 fragPos;
in vec3 entity;
in vec2 uvs;
flat in int doCalc;

uniform vec3 objColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 g_ambient;
uniform vec3 fogColor;
uniform float zNear;
uniform float zFar;

// diffuse
uniform sampler2D tex1;
// specular
uniform sampler2D tex2;

uniform sampler2D cube[6];

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

uniform int isReflect;
uniform int isRefract;
uniform int isShading;
uniform float fresnel;

uniform float inputRatio;
uniform float mixRatio;

uniform DirLight dirLights[16];
uniform PointLight pointLights[16];
uniform SpotLight spotLights[16];

out vec4 fragColor;

int planeNum;

vec3 CalReflect(vec3 dir, vec3 norm)
{
	vec3 result = 2 * dot(dir, norm) * norm - dir;
	return result;
}

vec3 CalRefract(vec3 I, vec3 N, float eta)
{
	vec3 R;
	float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));

	if(k < 0.0)
		R = vec3(0.0);
	else
		R = eta * I - (eta * dot(N, I) + sqrt(k)) * N;

	return R;
}

vec2 CalEnvironmentUV(vec3 vEntity)
{
	vec3 absVec = abs(vEntity);
	vec2 uv = vec2(0.0);
	float max;

	if(absVec.x >= absVec.y && absVec.x >= absVec.z)
	{	
		(vEntity.x < 0.0) ? (uv.x = vEntity.z) : (uv.x = -vEntity.z);
		uv.y = vEntity.y;
		(vEntity.x < 0.0) ? (planeNum = 3) : (planeNum = 1);

		uv.x *= -1;
		max = absVec.x;
	}
	else if(absVec.y >= absVec.x && absVec.y >= absVec.z)
	{
		(vEntity.y < 0.0) ? (uv.y = vEntity.z) : (uv.y = -vEntity.z);
		uv.x = vEntity.x;

		(vEntity.y < 0.0) ? (planeNum = 5) : (planeNum = 4);
	
		uv.y *= -1;
		max = absVec.y;
	}
	else if(absVec.z >= absVec.y && absVec.z >= absVec.x)
	{
		(vEntity.z < 0.0) ? (uv.x = -vEntity.x) : (uv.x = vEntity.x);
		uv.y = vEntity.y;

		(vEntity.z < 0.0) ? (planeNum = 0) : (planeNum = 2);
		
		uv.x *= -1;
		max = absVec.z;
	}
	
	return ((uv / max)  + vec2(1.0)) * 0.5;
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

	vec3 reflectDir = CalReflect(lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 30.f);

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

	vec3 reflectDir = CalReflect(lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 30.f);
	vec3 specular = light.specular * spec * material.ks * light.color;

	float distance = length(light.position - fragPos);
	float att = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= att;
	diffuse *= att;
	specular *= att;

	vec3 result = (ambient + diffuse + specular);
	return result;

}

vec3 CalSpotLigths(SpotLight light, vec3 norm, vec3 viewDir, Material material)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 ambient = light.ambient * light.color;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * material.kd * light.color;

	vec3 reflectDir = CalReflect(lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 30.0f);
	vec3 specular = light.specular * spec * material.ks * light.color;

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	intensity = pow(intensity, light.fallOff);
	diffuse *= intensity;
	specular *= intensity ;

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
	vec3 color = vec3(0.0);
	vec3 reflectColor = vec3(0.0);
	vec3 refractColor = vec3(0.0);

	if(isReflect == 1  && isRefract == 0)
	{
		vec3 R = CalReflect(viewDir, norm);
		vec2 envUV = CalEnvironmentUV(R);
		color = texture(cube[planeNum], envUV).rgb;
	}
	else if(isReflect == 0 && isRefract == 1)
	{
		float ratio = 1.0 / inputRatio;
		vec3 R = CalRefract(-viewDir, norm, ratio);
		vec2 envUV = CalEnvironmentUV(R);
		color = texture(cube[planeNum], envUV).rgb;
	}
	else if(isReflect == 1 && isRefract == 1)
	{
		vec3 reflectR = CalReflect(viewDir, norm);
		vec2 reflectUV = CalEnvironmentUV(reflectR);
		reflectColor = texture(cube[planeNum], reflectUV).rgb;

		float ratio = 1.0 / inputRatio;
		vec3 refractR[3];

		refractR[0] = CalRefract(-viewDir, norm, ratio * fresnel * 1.1);
		refractR[1] = CalRefract(-viewDir, norm, ratio * fresnel * 1.2);
		refractR[2] = CalRefract(-viewDir, norm, ratio * fresnel * 1.3);

		vec2 refractUV[3];

		refractUV[0] = CalEnvironmentUV(refractR[0]);
		refractUV[1] = CalEnvironmentUV(refractR[1]);
		refractUV[2] = CalEnvironmentUV(refractR[2]);

		refractColor.r = texture(cube[planeNum], refractUV[0]).r;
		refractColor.g = texture(cube[planeNum], refractUV[1]).g;
		refractColor.b = texture(cube[planeNum], refractUV[2]).b;

		color = mix(reflectColor, refractColor, mixRatio);
	}

	vec3 result = vec3(0.0);

	if(isShading == 1 || (isReflect == 0 && isRefract == 0))
	{
		if (doCalc == 1)
		{
		// planar = 1, cylin = 2, sph =3
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

		for (int i = 0; i < numDir; i++)
		{
			local += CalDirectionalLights(dirLights[i], norm, viewDir, material);
		}

		for (int i = 0; i < numPoint; i++)
		{
			local += CalPointLights(pointLights[i], norm, viewDir, material);
		}

		for (int i = 0; i < numSpot; i++)
		{
			local += CalSpotLigths(spotLights[i], norm, viewDir, material);
		}

		local += g_ambient;
		result = S * local + (1 - S) * fogColor;

		if(isReflect == 0 && isRefract == 0)
			fragColor = vec4(result, 1.0);
		else
			fragColor = vec4(mix(color, result, 0.5), 1.0);

	}
	else
	{
		fragColor = vec4(color, 1.0);
	}

}