#version 330 core
out vec4 FragColor;

in vec2 texcoord;

struct Camera
{
    float scale;
    vec3 position;
    vec3 front;
    vec3 right;
    vec3 up;
    int width;
    int height;
};

struct Material
{
    vec3 color;
    float roughness;
    float transparency;
    float ior;
};
struct Polygon
{
    vec3 v1;
    vec3 v2;
    vec3 v3;
    vec3 normal;
    float maxd;
    Material mat;
};
struct Hit
{
    vec3 pos;
    vec3 normal;  
    bool hit;
    Material mat;
};
struct Light
{
    vec3 pos;
    vec3 color;
    float intensity;
};
vec3 intersectPoint(vec3 rayVector, vec3 rayPoint, vec3 planeNormal, vec3 planePoint)
{
    vec3 diff = rayPoint - planePoint;
    float prod1 = dot(diff, planeNormal);
    float prod2 = dot(rayVector, planeNormal);
    float ratio = prod1 / prod2;

    return rayPoint - rayVector * ratio;
}
bool pointInPolygon(vec3 v, Polygon p)
{
    float area = length(cross((p.v1 - p.v2), (p.v3 - p.v2))) * 0.5f;

    float a = length(cross((v - p.v2), (v - p.v3))) / (2 * area);
    float b = length(cross((v - p.v3), (v - p.v1))) / (2 * area);
    float c = length(cross((v - p.v2), (v - p.v1))) / (2 * area);

    float tresh = 1.0f + 0.0001f;
    return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
}

uniform Polygon polygons[400];
uniform int polygonsCount = 0;

uniform Light lights[40];
uniform int lightsCount = 0;
uniform Camera camera;



Polygon getPolygon(int id)
{
    return polygons[id];
}

float dist2(vec3 v1, vec3 v2)
{
    return dot(v1-v2, v1-v2);
}

Hit getHit(vec3 ray, vec3 src)
{
    Hit closestHit = Hit( vec3(100000, 100000, 100000), vec3(0, 0, 1), false, Material(vec3(1, 1, 1),1,1,1));
    
    for (int i=0;i<polygonsCount;i++)
    {
        vec3 hit = intersectPoint(ray, src, getPolygon(i).normal, getPolygon(i).v1);
        if (dist2(hit, getPolygon(i).v1) > getPolygon(i).maxd)
            continue;
        if (pointInPolygon(hit, getPolygon(i))
            && dist2(closestHit.pos, src) > dist2(hit, src) && dot(hit - src, ray) > 0)
        {
            closestHit = Hit(hit, getPolygon(i).normal, true, getPolygon(i).mat);
        }
        
    }

    return closestHit;
}
vec3 clampColor(vec3 color)
{
    return vec3(clamp(color.x, 0, 1),clamp(color.y, 0, 1),clamp(color.z, 0, 1));
}
vec3 multyplyByLight(vec3 color, vec3 light)
{
    return vec3(color.x * light.x, color.y * light.y ,color.z * light.z);
}
float length2(vec3 v)
{
    return dot(v,v);
}
vec3 getDiffuse(Hit hit)
{
    vec3 color = vec3(0, 0, 0);
    for (int i=0;i<lightsCount;i++)
    {
        vec3 diffuse;
        vec3 lightPosition = lights[i].pos;
        vec3 lightColor = lights[i].color;

        vec3 dirToLight = lightPosition - hit.pos;
        vec3 offset = hit.pos + dirToLight * 0.0001f;

        Hit lightHits = getHit(dirToLight, offset);
        float attenuation = lightHits.hit ? lightHits.mat.transparency : 1;
        while (lightHits.hit && lightHits.mat.transparency != 0)
        {
            offset = lightHits.pos + dirToLight * 0.0001f;
            lightHits = getHit(dirToLight, offset);
            if (lightHits.hit)
                attenuation *= lightHits.mat.transparency;
        }

        if (!lightHits.hit)
        {
            float slope = abs(dot(hit.normal, normalize(hit.pos - lightPosition)));

            diffuse = attenuation * clampColor(multyplyByLight(hit.mat.color, lightColor) * slope * (lights[i].intensity / length2(dirToLight)));
        }
        else
        {
            diffuse = diffuse * 0.1f;
        }
        color += diffuse;
    }

    return clampColor(color);
}
vec3 reflect(vec3 I, vec3 N)
{
    return I - N * (dot(I, N) * 2.0f);
}
vec3 refract(vec3 I, vec3 N, float eta_t, float eta_i)
{
    float cosi = -max(-1.f, min(1.0f, dot(I, N)));
    if (cosi < 0) return refract(I, -N, eta_i, eta_t);
    float eta = eta_i / eta_t;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? reflect(I, N) : I * eta + N * (eta * cosi - sqrt(k));
}
vec3 getOffset(Hit surfaceHit, vec3 direction)
{
    return dot(direction, surfaceHit.normal) < 0
        ? surfaceHit.pos - surfaceHit.normal * 0.0001f
        : surfaceHit.pos + surfaceHit.normal * 0.0001f;
}
vec3 castRay(vec3 ray, vec3 src, int reflects)
{
    vec3 color = vec3(10, 10, 10);

    Hit surfaceHit = getHit(ray, src);

    if (surfaceHit.hit)
    {
        color = getDiffuse(surfaceHit);

        vec3 reflection = reflect(normalize(ray), surfaceHit.normal);
        vec3 offset = getOffset(surfaceHit, reflection);
        vec3 reflectedColor = vec3(0,0,0);//castRay(reflection, offset, reflects - 1);


        vec3 refraction = refract(normalize(ray), surfaceHit.normal, surfaceHit.mat.ior);
        offset = getOffset(surfaceHit, refraction);

        vec3 refractedColor = vec3(0,0,0);//(surfaceHit.mat.transparency < 1) ? castRay(refraction, offset, reflects - 1) : vec3(0, 0, 0);

        float nonTransparency = 1 - surfaceHit.mat.transparency;
        float transparency = surfaceHit.mat.transparency;
        float rough = surfaceHit.mat.roughness;
        return clampColor((color * rough + reflectedColor * (1 - rough)) * nonTransparency + transparency * refractedColor);
        
    }
    else
    {
        color = vec3(0.1,0.1,0.1);
    }
    return color;
}
void main()
{  	
    vec3 ray = camera.front + camera.right * float(texcoord.x*camera.width - camera.width / 2) * camera.scale - camera.up * float(texcoord.y*camera.height - camera.height / 2) * camera.scale;

    vec3 color = castRay(ray, camera.position, 1);
    FragColor = vec4(color, 1.0);
} 
