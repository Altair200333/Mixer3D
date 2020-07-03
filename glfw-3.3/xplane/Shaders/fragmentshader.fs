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

struct Polygon
{
    vec3 v1;
    vec3 v2;
    vec3 v3;
    float maxd;
    vec3 color;

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
uniform Polygon polygons[100];
uniform int polygonsCount = 0;
uniform Camera camera;


void main()
{  	
    vec3 ray = camera.front + camera.right * float(texcoord.x - camera.width / 2) * camera.scale + camera.up * float(texcoord.y - camera.height / 2) * camera.scale;


    FragColor = vec4(1,texcoord.y,1, 1.0);
} 
