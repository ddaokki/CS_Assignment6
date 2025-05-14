#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLFW_STATIC
#include <GLFW/glfw3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;
using namespace glm;

const int width = 512, height = 512;

struct Triangle {
    int v0, v1, v2;
};

vector<vec3> vertices;
vector<vec3> normals;
vector<Triangle> triangles;
float depthBuffer[height][width];
unsigned char image[height][width * 3];

vec3 ka(0, 1, 0), kd(0, 0.5, 0), ks(0.5, 0.5, 0.5);
float p = 32.0f;
float Ia = 0.2f;
vec3 lightPos(-4, 4, -3);
vec3 lightColor(1.0f);

vec3 transformModel(const vec3& p) {
    return vec3(2 * p.x, 2 * p.y, 2 * p.z - 7.0f);
}

vec3 transformProjection(const vec3& p) {
    float l = -0.1f, r = 0.1f, b = -0.1f, t = 0.1f, n = -0.1f, f = -1000.0f;
    float x = (2 * n / (r - l)) * p.x / -p.z;
    float y = (2 * n / (t - b)) * p.y / -p.z;
    float z = (f + n) / (n - f) + (2 * f * n / (n - f)) / -p.z;
    return vec3(x, y, z);
}

void transformViewport(vec3& p) {
    p.x = (p.x + 1) * 0.5f * width;
    p.y = (1 - p.y) * 0.5f * height;
}

vec3 computeLighting(const vec3& pos, const vec3& normal) {
    vec3 ambient = Ia * ka;
    vec3 L = normalize(lightPos - pos);
    vec3 V = normalize(-pos);
    vec3 R = reflect(-L, normal);
    float diff = std::max(dot(normal, L), 0.0f);
    vec3 diffuse = diff * kd * lightColor;
    float spec = pow(std::max(dot(R, V), 0.0f), p);
    vec3 specular = spec * ks * lightColor;
    vec3 color = ambient + diffuse + specular;
    color = pow(color, vec3(1.0f / 2.2f));
    return clamp(color, 0.0f, 1.0f);
}

void setPixel(int x, int y, const vec3& color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        image[y][x * 3 + 0] = (unsigned char)(color.r * 255);
        image[y][x * 3 + 1] = (unsigned char)(color.g * 255);
        image[y][x * 3 + 2] = (unsigned char)(color.b * 255);
    }
}

void rasterize(const vector<vec3>& screenVerts, const vector<vec3>& worldVerts, const vector<vec3>& worldNormals) {
    for (const auto& tri : triangles) {
        vec3 v0 = screenVerts[tri.v0];
        vec3 v1 = screenVerts[tri.v1];
        vec3 v2 = screenVerts[tri.v2];

        vec3 w0 = worldVerts[tri.v0];
        vec3 w1 = worldVerts[tri.v1];
        vec3 w2 = worldVerts[tri.v2];

        vec3 n0 = worldNormals[tri.v0];
        vec3 n1 = worldNormals[tri.v1];
        vec3 n2 = worldNormals[tri.v2];

        int minX = std::max(0, (int)floor(min({ v0.x, v1.x, v2.x })));
        int maxX = std::min(width - 1, (int)ceil(max({ v0.x, v1.x, v2.x })));
        int minY = std::max(0, (int)floor(min({ v0.y, v1.y, v2.y })));
        int maxY = std::min(height - 1, (int)ceil(max({ v0.y, v1.y, v2.y })));

        float denom = ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
        if (abs(denom) < 1e-5f) continue;

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                float a = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / denom;
                float b = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / denom;
                float c = 1 - a - b;
                if (a >= 0 && b >= 0 && c >= 0) {
                    float z = a * v0.z + b * v1.z + c * v2.z;
                    if (z < depthBuffer[y][x]) {
                        depthBuffer[y][x] = z;
                        vec3 pos = a * w0 + b * w1 + c * w2;
                        vec3 normal = normalize(a * n0 + b * n1 + c * n2);
                        if (dot(normal, normalize(-pos)) < 0.0f) normal = -normal;
                        vec3 color = computeLighting(pos, normal);
                        setPixel(x, y, color);
                    }
                }
            }
        }
    }
}

void createSphere() {
    int w = 20, h = 10;
    for (int j = 1; j < h - 1; ++j) {
        float theta = (float)j / (h - 1) * M_PI;
        for (int i = 0; i < w; ++i) {
            float phi = (float)i / w * 2 * M_PI;
            float x = sin(theta) * cos(phi);
            float y = cos(theta);
            float z = -sin(theta) * sin(phi);
            vertices.push_back(vec3(x, y, z));
            normals.push_back(normalize(vec3(x, y, z)));
        }
    }
    int north = vertices.size();
    vertices.push_back(vec3(0, 1, 0)); normals.push_back(vec3(0, 1, 0));
    int south = vertices.size();
    vertices.push_back(vec3(0, -1, 0)); normals.push_back(vec3(0, -1, 0));

    for (int j = 0; j < h - 3; ++j) {
        for (int i = 0; i < w; ++i) {
            int curr = j * w + i;
            int next = j * w + (i + 1) % w;
            int below = (j + 1) * w + i;
            int belowNext = (j + 1) * w + (i + 1) % w;
            triangles.push_back({ curr, belowNext, next });
            triangles.push_back({ curr, below, belowNext });
        }
    }
    for (int i = 0; i < w; ++i) {
        int next = (i + 1) % w;
        triangles.push_back({ north, next, i });
        int base = (h - 3) * w;
        triangles.push_back({ south, base + i, base + next });
    }
}

void render() {
    createSphere();
    vector<vec3> screenVerts, worldVerts, worldNormals;
    for (size_t i = 0; i < vertices.size(); ++i) {
        vec3 model = transformModel(vertices[i]);
        vec3 normal = normalize(normals[i]);
        vec3 viewDir = normalize(-model);
        if (dot(normal, viewDir) < 0.0f) normal = -normal;
        worldVerts.push_back(model);
        worldNormals.push_back(normal);
        vec3 proj = transformProjection(model);
        transformViewport(proj);
        screenVerts.push_back(proj);
    }
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            depthBuffer[y][x] = numeric_limits<float>::infinity();
    fill(&image[0][0], &image[0][0] + sizeof(image), 0);
    rasterize(screenVerts, worldVerts, worldNormals);
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(width, height, "Q3 - Phong Shading", NULL, NULL);
    if (!window) {
        glfwTerminate(); return -1;
    }
    glfwMakeContextCurrent(window);
    render();
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
