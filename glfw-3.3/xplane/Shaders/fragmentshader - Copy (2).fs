#version 330 core
#define M_PI 3.1415926535897932384626433832795
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

uniform int polygonsCount = 0;
uniform int reflects = 0;
uniform Light lights[40];
uniform int lightsCount = 0;
uniform Camera camera;

uniform samplerBuffer VertexSampler0;
uniform samplerBuffer background;

Polygon getPolygon(int id)
{
    int off = id*5;
    vec4 v1 = texelFetch(VertexSampler0, off + 0);
    vec4 v2 = texelFetch(VertexSampler0, off + 1);
    vec4 v3 = texelFetch(VertexSampler0, off + 2);
    vec4 v4 = texelFetch(VertexSampler0, off + 3);
    vec4 v5 = texelFetch(VertexSampler0, off + 4);
    //                    v1               v2                   v3                    norm                    maxd                 color            rougn  transp ior
    Polygon p = Polygon(vec3(v1.xyz), vec3(v1.w, v2.x, v2.y), vec3(v2.z, v2.w, v3.x), vec3(v3.y, v3.z, v3.w), v4.x, Material(vec3(v4.y, v4.z, v4.w), v5.x, v5.y, v5.z));
    return p;//polygons[id];
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

        float attenuation = 1;
        bool openToLight = false;
        Hit lightHits = getHit(dirToLight, offset);
        if (dot(lightHits.pos - lightPosition, dirToLight) > 0)
            attenuation = 1;
        else
        {
            attenuation = lightHits.hit ? lightHits.mat.transparency : 1;

            while (lightHits.hit && lightHits.mat.transparency != 0)
            {
                offset = lightHits.pos + dirToLight * 0.0001f;
                lightHits = getHit(dirToLight, offset);
                if(dot(lightHits.pos - lightPosition, dirToLight) > 0)
                    break;
                if (lightHits.hit)
                    attenuation *= lightHits.mat.transparency;
            }
        }
        float slope = abs(dot(hit.normal, normalize(hit.pos - lightPosition)));

        diffuse = attenuation * clampColor(multyplyByLight(hit.mat.color, lightColor)  * slope * (lights[i].intensity / length2(dirToLight)));
        
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
vec2 getMapAngles(vec3 ray)
{
    float theta = atan(ray.z, ray.x) * 180 / M_PI + 180;
    float alpha = atan(ray.y, sqrt(ray.x * ray.x + ray.z * ray.z)) * 180 / M_PI + 90;
    return vec2(theta, alpha);
}
uniform int e_w;
uniform int e_h;
vec3 getBackgroundColor(vec3 ray)
{
    return vec3(0.1,0.1,0.1);
    //vec2 c = getMapAngles(ray);

    //float x = e_w * c.x / 360;
    //float y = e_h * c.y / 180;
    //int id = int(x+y*e_w);
    //return texelFetch(background, 0).xyz;
    
}
vec3 castRay(vec3 ray, vec3 src, int reflects)
{
    vec3 color = vec3( 10, 10, 10 );
    Hit surfaceHit = getHit(ray, src);
    
    if (surfaceHit.hit)
    {
        color = getDiffuse(surfaceHit);
        Hit lastHit = surfaceHit;
        vec3 lastRay = ray;
        
        while(reflects>0)
        {
            vec3 reflection = reflect(normalize(lastRay), lastHit.normal);
            vec3 offset = getOffset(lastHit, reflection);

            Hit hit = getHit(reflection, offset);
            if (hit.hit)
            {
                color += getDiffuse(hit) * (1-lastHit.mat.roughness);
            }
            else
            {
                color += (1 - lastHit.mat.roughness)*getBackgroundColor(reflection);
                break;
            }
            lastHit = hit;
            lastRay = reflection;
            reflects--;
        }
    }
    else
    {
        color = getBackgroundColor(ray);
    }
    return clampColor(color);
}
uniform float startX;
uniform float endX;
uniform float startY;
uniform float endY;
void main()
{  	
    if(texcoord.x>=startX && texcoord.x<=endX && texcoord.y>=startY && texcoord.y<=endY)
    {
        vec3 ray = camera.front + camera.right * float(texcoord.x*camera.width - camera.width / 2) * camera.scale + camera.up * float(texcoord.y*camera.height - camera.height / 2) * camera.scale;

        vec3 color = castRay(ray, camera.position, reflects);
        FragColor = vec4(color, 1.0);
    }
    else
        discard; 
} 
