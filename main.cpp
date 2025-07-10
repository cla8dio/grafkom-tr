#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h> // freeglut lebih modern dan sering direkomendasikan
#include <GL/glut.h>     // Fallback jika freeglut tidak tersedia
#endif
#include <iostream>
#include <vector>
#include <random>
#include <cmath> // Untuk M_PI dan fungsi matematika lainnya
#include <sstream> // Untuk stringstream jika diperlukan
#include <fstream> // Untuk file I/O jika diperlukan
#include <string> // Untuk string jika diperlukan
#include <cstring> // Untuk memset jika diperlukan
#include <algorithm> // Untuk std::sort dan std::unique
#include <iomanip> // Untuk std::fixed dan std::setprecision

// Definisi M_PI yang lebih presisi
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Camera variables
float cameraX = 0.0f, cameraY = 5.0f, cameraZ = 15.0f;
float cameraAngleX = 0.0f, cameraAngleY = 0.0f;
float lastMouseX = 0, lastMouseY = 0;
bool mousePressed = false;

// Day/Night Mode variable
bool isNightMode = false; // Default: Day mode

// Fungsi untuk mengatur warna (akan mempengaruhi ambient dan diffuse material karena GL_COLOR_MATERIAL)
void setColor(float r, float g, float b)
{
    glColor3f(r, g, b);
}

void setColorub(unsigned char r, unsigned char g, unsigned char b)
{
    glColor3ub(r, g, b);
}

// Fungsi bantu untuk menggambar kubus dengan normal yang benar
void drawCube(float x, float y, float z, float width, float height, float depth)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(width, height, depth);

    // Kubus memiliki 6 sisi, setiap sisi memiliki normal yang berbeda
    // Front face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal ke arah Z positif
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    // Back face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f); // Normal ke arah Z negatif
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glEnd();

    // Top face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal ke arah Y positif
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glEnd();

    // Bottom face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal ke arah Y negatif
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glEnd();

    // Right face
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f); // Normal ke arah X positif
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glEnd();

    // Left face
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f); // Normal ke arah X negatif
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glPopMatrix();
}

// Fungsi bantu untuk menggambar quad dengan normal yang benar
void drawQuad(float x1, float y1, float z1, float x2, float y2, float z2,
    float x3, float y3, float z3, float x4, float y4, float z4)
{
    // Hitung normal permukaan (asumsi quad planar)
    // Vektor dari v1 ke v2
    float vec1x = x2 - x1;
    float vec1y = y2 - y1;
    float vec1z = z2 - z1;

    // Vektor dari v1 ke v4
    float vec2x = x4 - x1;
    float vec2y = y4 - y1;
    float vec2z = z4 - z1;

    // Cross product untuk mendapatkan normal
    float nx = vec1y * vec2z - vec1z * vec2y;
    float ny = vec1z * vec2x - vec1x * vec2z;
    float nz = vec1x * vec2y - vec1y * vec2x;

    // Normalisasi
    float length = sqrt(nx * nx + ny * ny + nz * nz);
    if (length > 0) {
        nx /= length;
        ny /= length;
        nz /= length;
    }

    glBegin(GL_QUADS);
    glNormal3f(nx, ny, nz); // Terapkan normal untuk seluruh quad
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
}

// Fungsi bantu untuk menggambar segitiga dengan normal yang benar
void drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2,
    float x3, float y3, float z3)
{
    // Hitung normal permukaan
    // Vektor dari v1 ke v2
    float vec1x = x2 - x1;
    float vec1y = y2 - y1;
    float vec1z = z2 - z1;

    // Vektor dari v1 ke v3
    float vec2x = x3 - x1;
    float vec2y = y3 - y1;
    float vec2z = z3 - z1;

    // Cross product untuk mendapatkan normal
    float nx = vec1y * vec2z - vec1z * vec2y;
    float ny = vec1z * vec2x - vec1x * vec2z;
    float nz = vec1x * vec2y - vec1y * vec2x;

    // Normalisasi
    float length = sqrt(nx * nx + ny * ny + nz * nz);
    if (length > 0) {
        nx /= length;
        ny /= length;
        nz /= length;
    }

    glBegin(GL_TRIANGLES);
    glNormal3f(nx, ny, nz); // Terapkan normal untuk seluruh segitiga
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glEnd();
}

// Fungsi bantu untuk menggambar silinder dengan normal yang benar
void drawCylinder(float radius, float height, int segments, float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    // Draw the top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal ke atas
    glVertex3f(0, height / 2, 0);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        glVertex3f(radius * cosf(angle), height / 2, radius * sinf(angle));
    }
    glEnd();

    // Draw the bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal ke bawah
    glVertex3f(0, -height / 2, 0);
    for (int i = segments; i >= 0; i--)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        glVertex3f(radius * cosf(angle), -height / 2, radius * sinf(angle));
    }
    glEnd();

    // Draw the side
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        float nx = cosf(angle); // Normal mengarah keluar dari pusat silinder
        float nz = sinf(angle);
        glNormal3f(nx, 0.0f, nz);
        glVertex3f(radius * cosf(angle), height / 2, radius * sinf(angle));
        glNormal3f(nx, 0.0f, nz); // Normal yang sama untuk vertex bawah
        glVertex3f(radius * cosf(angle), -height / 2, radius * sinf(angle));
    }
    glEnd();

    glPopMatrix();
}

void drawSphere(float radius, int slices, int stacks, float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Generate sphere vertices using spherical coordinates
    for (int i = 0; i <= stacks; i++) {
        float V = (float)i / (float)stacks;
        float phi = V * M_PI; // Latitude angle from 0 to PI

        for (int j = 0; j <= slices; j++) {
            float U = (float)j / (float)slices;
            float theta = U * 2.0f * M_PI; // Longitude angle from 0 to 2*PI

            // Calculate sphere coordinates
            float x1 = radius * sin(phi) * cos(theta);
            float y1 = radius * cos(phi);
            float z1 = radius * sin(phi) * sin(theta);

            float x2 = radius * sin(phi + M_PI / stacks) * cos(theta);
            float y2 = radius * cos(phi + M_PI / stacks);
            float z2 = radius * sin(phi + M_PI / stacks) * sin(theta);

            float x3 = radius * sin(phi) * cos(theta + 2.0f * M_PI / slices);
            float y3 = radius * cos(phi);
            float z3 = radius * sin(phi) * sin(theta + 2.0f * M_PI / slices);

            float x4 = radius * sin(phi + M_PI / stacks) * cos(theta + 2.0f * M_PI / slices);
            float y4 = radius * cos(phi + M_PI / stacks);
            float z4 = radius * sin(phi + M_PI / stacks) * sin(theta + 2.0f * M_PI / slices);

            // Draw quad for this section of sphere
            if (i < stacks && j < slices) {
                glBegin(GL_QUADS);

                // Calculate normals for lighting (normalized vertex positions)
                glNormal3f(x1/radius, y1/radius, z1/radius);
                glVertex3f(x1, y1, z1);

                glNormal3f(x2/radius, y2/radius, z2/radius);
                glVertex3f(x2, y2, z2);

                glNormal3f(x4/radius, y4/radius, z4/radius);
                glVertex3f(x4, y4, z4);

                glNormal3f(x3/radius, y3/radius, z3/radius);
                glVertex3f(x3, y3, z3);

                glEnd();
            }
        }
    }

    glPopMatrix();
}

void drawCar(float x, float y, float z, float angle, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 1, 0);

    // Main car body - made shorter and more realistic proportions
    setColor(r, g, b);
    drawCube(0.0f, 0.25f, 0.0f, 1.8f, 0.4f, 3.2f); // Shorter, more realistic body

    // Hood and trunk - separate sections for realism
    setColor(r*0.95, g*0.95, b*0.95);
    drawCube(0.0f, 0.3f, 1.4f, 1.7f, 0.3f, 0.6f); // Hood
    drawCube(0.0f, 0.3f, -1.4f, 1.7f, 0.3f, 0.6f); // Trunk

    // Car roof - lower and more proportional
    setColor(r*0.85, g*0.85, b*0.85);
    drawCube(0.0f, 0.65f, 0.0f, 1.4f, 0.2f, 2.0f); // Lower roof

    // Windshield and windows with realistic angles
    setColor(0.2f, 0.2f, 0.3f); // Darker, more realistic glass

    // Windshield - angled for realism
    drawQuad(0.0f, 0.6f, 1.0f,   // Front top center
             0.7f, 0.6f, 1.0f,   // Front top right
             0.8f, 0.45f, 1.5f,  // Front bottom right
             0.0f, 0.45f, 1.5f); // Front bottom center

    drawQuad(0.0f, 0.6f, 1.0f,   // Front top center
             -0.7f, 0.6f, 1.0f,  // Front top left
             -0.8f, 0.45f, 1.5f, // Front bottom left
             0.0f, 0.45f, 1.5f); // Front bottom center

    // Rear window
    drawQuad(0.0f, 0.6f, -1.0f,   // Rear top center
             0.7f, 0.6f, -1.0f,   // Rear top right
             0.8f, 0.45f, -1.5f,  // Rear bottom right
             0.0f, 0.45f, -1.5f); // Rear bottom center

    drawQuad(0.0f, 0.6f, -1.0f,   // Rear top center
             -0.7f, 0.6f, -1.0f,  // Rear top left
             -0.8f, 0.45f, -1.5f, // Rear bottom left
             0.0f, 0.45f, -1.5f); // Rear bottom center

    // Side windows
    drawQuad(0.7f, 0.6f, 1.0f,   // Front right top
             0.7f, 0.6f, -1.0f,  // Rear right top
             0.7f, 0.45f, -1.0f, // Rear right bottom
             0.7f, 0.45f, 1.0f); // Front right bottom

    drawQuad(-0.7f, 0.6f, 1.0f,   // Front left top
             -0.7f, 0.6f, -1.0f,  // Rear left top
             -0.7f, 0.45f, -1.0f, // Rear left bottom
             -0.7f, 0.45f, 1.0f); // Front left bottom

    // Window frames - more realistic
    setColor(0.1f, 0.1f, 0.1f);
    drawCube(0.0f, 0.53f, 0.0f, 1.5f, 0.03f, 2.1f); // Main frame
    drawCube(0.0f, 0.53f, 1.0f, 1.5f, 0.03f, 0.1f); // Front frame
    drawCube(0.0f, 0.53f, -1.0f, 1.5f, 0.03f, 0.1f); // Rear frame

    // Modern LED headlights
    setColor(0.95f, 0.95f, 1.0f);
    drawCube(0.6f, 0.35f, 1.7f, 0.25f, 0.15f, 0.08f); // Right headlight
    drawCube(-0.6f, 0.35f, 1.7f, 0.25f, 0.15f, 0.08f); // Left headlight

    // Headlight housing
    setColor(0.15f, 0.15f, 0.15f);
    drawCube(0.6f, 0.35f, 1.65f, 0.3f, 0.2f, 0.1f); // Right housing
    drawCube(-0.6f, 0.35f, 1.65f, 0.3f, 0.2f, 0.1f); // Left housing

    // Modern taillights - LED style
    setColor(0.9f, 0.1f, 0.1f);
    drawCube(0.7f, 0.4f, -1.7f, 0.15f, 0.25f, 0.05f); // Right taillight
    drawCube(-0.7f, 0.4f, -1.7f, 0.15f, 0.25f, 0.05f); // Left taillight

    // Grille - more detailed
    setColor(0.15f, 0.15f, 0.15f);
    drawCube(0.0f, 0.4f, 1.72f, 0.9f, 0.2f, 0.05f); // Main grille
    drawCube(0.0f, 0.35f, 1.72f, 0.7f, 0.1f, 0.03f); // Lower grille

    // Side mirrors - more realistic
    setColor(r*0.8, g*0.8, b*0.8);
    drawCube(0.85f, 0.55f, 0.8f, 0.08f, 0.12f, 0.15f); // Right mirror
    drawCube(-0.85f, 0.55f, 0.8f, 0.08f, 0.12f, 0.15f); // Left mirror

    // Mirror glass
    setColor(0.3f, 0.3f, 0.4f);
    drawCube(0.88f, 0.55f, 0.8f, 0.06f, 0.08f, 0.12f); // Right mirror glass
    drawCube(-0.88f, 0.55f, 0.8f, 0.06f, 0.08f, 0.12f); // Left mirror glass

    // Rounded tires with better proportions
    setColor(0.1f, 0.1f, 0.1f); // Tire color

    // Front wheels - rounder and more realistic
    drawSphere(0.25f, 32, 16, -0.9f, 0.1f, 1.2f); // Front left tire
    drawSphere(0.25f, 32, 16, 0.9f, 0.1f, 1.2f);  // Front right tire

    // Rear wheels
    drawSphere(0.25f, 32, 16, -0.9f, 0.1f, -1.2f); // Rear left tire
    drawSphere(0.25f, 32, 16, 0.9f, 0.1f, -1.2f);  // Rear right tire

    // Rims - more detailed and realistic
    setColor(0.75f, 0.75f, 0.8f);
    drawCylinder(0.18f, 0.05f, 16, -0.9f, 0.1f, 1.2f); // Front left rim
    drawCylinder(0.18f, 0.05f, 16, 0.9f, 0.1f, 1.2f);  // Front right rim
    drawCylinder(0.18f, 0.05f, 16, -0.9f, 0.1f, -1.2f); // Rear left rim
    drawCylinder(0.18f, 0.05f, 16, 0.9f, 0.1f, -1.2f);  // Rear right rim

    // Rim spokes
    setColor(0.65f, 0.65f, 0.7f);
    drawCylinder(0.12f, 0.06f, 8, -0.9f, 0.1f, 1.2f); // Front left spokes
    drawCylinder(0.12f, 0.06f, 8, 0.9f, 0.1f, 1.2f);  // Front right spokes
    drawCylinder(0.12f, 0.06f, 8, -0.9f, 0.1f, -1.2f); // Rear left spokes
    drawCylinder(0.12f, 0.06f, 8, 0.9f, 0.1f, -1.2f);  // Rear right spokes

    // Door handles - more realistic
    setColor(0.25f, 0.25f, 0.25f);
    drawCube(0.85f, 0.4f, 0.3f, 0.08f, 0.03f, 0.12f); // Right handle
    drawCube(-0.85f, 0.4f, 0.3f, 0.08f, 0.03f, 0.12f); // Left handle

    // Door lines for realism
    setColor(r*0.7, g*0.7, b*0.7);
    drawCube(0.88f, 0.35f, 0.0f, 0.02f, 0.3f, 2.5f); // Right door line
    drawCube(-0.88f, 0.35f, 0.0f, 0.02f, 0.3f, 2.5f); // Left door line

    // Exhaust pipe - more realistic
    setColor(0.3f, 0.3f, 0.35f);
    drawCylinder(0.06f, 0.15f, 12, -0.5f, 0.08f, -1.8f); // Exhaust pipe

    // Exhaust tip
    setColor(0.5f, 0.5f, 0.55f);
    drawCylinder(0.05f, 0.02f, 12, -0.5f, 0.08f, -1.9f); // Exhaust tip

    // License plate - more realistic
    setColor(0.95f, 0.95f, 0.95f);
    drawCube(0.0f, 0.25f, -1.8f, 0.35f, 0.08f, 0.02f); // Plate

    // Bumpers
    setColor(r*0.9, g*0.9, b*0.9);
    drawCube(0.0f, 0.15f, 1.8f, 1.6f, 0.12f, 0.08f); // Front bumper
    drawCube(0.0f, 0.15f, -1.8f, 1.6f, 0.12f, 0.08f); // Rear bumper

    // Antenna
    setColor(0.2f, 0.2f, 0.2f);
    drawCylinder(0.01f, 0.3f, 6, 0.6f, 0.75f, -0.5f); // Antenna

    glPopMatrix();
}

// Helper function to create cars with predefined colors
void drawRealisticCar(float x, float y, float z, float angle, int colorType) {
    switch(colorType) {
        case 0: // Black car
            drawCar(x, y, z, angle, 0.1f, 0.1f, 0.1f);
            break;
        case 1: // Gray car
            drawCar(x, y, z, angle, 0.5f, 0.5f, 0.5f);
            break;
        case 2: // Red car
            drawCar(x, y, z, angle, 0.8f, 0.1f, 0.1f);
            break;
        default: // Default to black
            drawCar(x, y, z, angle, 0.1f, 0.1f, 0.1f);
            break;
    }
}

// Note: You'll need to implement these helper functions if not already available:
// - drawSphere(float radius, int slices, int stacks, float x, float y, float z)
// - setColor(float r, float g, float b)
// - drawCube(float x, float y, float z, float width, float height, float depth)
// - drawCylinder(float radius, float height, int slices, float x, float y, float z)
// - drawQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4)

// Fungsi untuk menggambar motor
void drawMotorcycle(float x, float y, float z, float angle)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 1, 0);

    // Main body
    setColor(0.3f, 0.3f, 0.3f);
    drawCube(0.0f, 0.3f, 0.0f, 0.5f, 0.3f, 1.5f);

    // Seat
    setColor(0.1f, 0.1f, 0.1f);
    drawCube(0.0f, 0.45f, 0.2f, 0.5f, 0.1f, 0.7f);

    // Wheels (as cylinders)
    setColor(0.1f, 0.1f, 0.1f);
    drawCylinder(0.2f, 0.1f, 16, 0.0f, 0.2f, -0.6f); // Roda depan
    drawCylinder(0.2f, 0.1f, 16, 0.0f, 0.2f, 0.6f);  // Roda belakang

    // Handlebar
    drawCube(0.0f, 0.6f, -0.4f, 0.7f, 0.05f, 0.05f);

    glPopMatrix();
}

void drawParkingArea()
{
    // Parking lot base
    setColor(0.2f, 0.2f, 0.2f);
    drawCube(20.5f, 0.05f, 0.0f, 9.0f, 0.1f, 20.0f);

    // Parking lines
    setColor(1.0f, 1.0f, 1.0f);
    for (float z = -6.0f; z <= 11.0f; z += 4.0f)
    {
        // Horizontal lines
        drawCube(20.0f, 0.10f, z, 10.0f, 0.01f, 0.1f);

        // Vertical lines for parking spaces
        for (float x = 15.0f; x <= 25.0f; x += 2.5f)
        {
            drawCube(x, 0.10f, z - 1.8f, 0.1f, 0.01f, 3.5f);
        }
    }

    // Add parked vehicles in the parking spots
    // First row (z = -6.0f)
   // drawRealisticCar(16.5f, 0.3f, -6.0f, 90, 0);  // Black car
    drawRealisticCar(19.0f, 0.3f, -6.0f, 90, 1);  // Gray car
    //drawRealisticCar(21.5f, 0.3f, -6.0f, 90, 2);  // Red car
   drawRealisticCar(24.0f, 0.3f, -6.0f, 90, 0);  // Black car

    // Second row (z = -2.0f)
    //drawRealisticCar(16.5f, 0.3f, -2.0f, 90, 1);  // Gray car
    drawRealisticCar(19.0f, 0.3f, -2.0f, 90, 2);  // Red car
   // drawRealisticCar(21.5f, 0.3f, -2.0f, 90, 0);  // Black car
    drawRealisticCar(24.0f, 0.3f, -2.0f, 90, 1);  // Gray car

    // Third row (z = 2.0f)
    //drawRealisticCar(16.5f, 0.3f, 2.0f, 90, 2);  // Red car
    drawRealisticCar(19.0f, 0.3f, 2.0f, 90, 0);  // Black car
   // drawRealisticCar(21.5f, 0.3f, 2.0f, 90, 1);  // Gray car
    drawRealisticCar(24.0f, 0.3f, 2.0f, 90, 2);  // Red car

    // Fourth row (z = 6.0f)
    //drawRealisticCar(16.5f, 0.3f, 6.0f, 90, 0);  // Black car
    drawRealisticCar(19.0f, 0.3f, 6.0f, 90, 1);  // Gray car
   // drawRealisticCar(21.5f, 0.3f, 6.0f, 90, 2);  // Red car
    drawRealisticCar(24.0f, 0.3f, 6.0f, 90, 0);  // Black car

}



void drawRoad()
{
    // Main road - single dark asphalt rectangle
    setColor(0.1f, 0.1f, 0.1f); // Dark asphalt
    drawCube(0.0f, 0.05f, 20.0f, 50.0f, 0.1f, 12.0f); // Full road

    // Parking access road (connected seamlessly)
    drawCube(20.5f, 0.05f, 11.5f, 9.0f, 0.1f, 3.0f);

    // Road markings (white lines only)
    setColor(1.0f, 1.0f, 1.0f);
    // Center line (dashed)
    for (float x = -25.0f; x <= 25.0f; x += 3.0f)
    {
        drawCube(x, 0.06f, 20.0f, 2.0f, 0.12f, 0.3f);
    }
    // Side lines (solid)
    drawCube(0.0f, 0.06f, 14.3f, 50.0f, 0.12f, 0.3f); // Front
    drawCube(0.0f, 0.06f, 25.7f, 50.0f, 0.12f, 0.3f); // Back
}

void drawParkingGate()
{
    // Gate mechanical box
    setColor(0.3f, 0.3f, 0.3f);
    drawCube(16.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f);

    // Gate arm
    setColor(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(16.0f, 1.0f, 0.0f);
    glRotatef(-45, 0, 1, 0); // Gate is partially open
    drawCube(0.0f, 0.0f, 0.0f, 0.1f, 0.05f, 3.0f);
    glPopMatrix();

    // Control panel
    setColor(0.1f, 0.1f, 0.1f);
    drawCube(15.5f, 0.8f, -1.5f, 0.3f, 0.5f, 0.3f);

    // Button on panel
    setColor(0.0f, 1.0f, 0.0f);
    drawCube(15.6f, 0.9f, -1.5f, 0.1f, 0.1f, 0.1f);
}

void drawFence()
{
    // Rectangular wall base
    setColorub(150, 150, 150); // Gray concrete color
    float wallWidth = 28.0f;
    float wallHeight = 1.0f;
    float wallY = 0.5f;
    float postWidth = 0.2f;
    float postDepth = 0.2f;
    float railThickness = 0.05f;
    float fenceHeight = 2.0f;
    float fenceBaseY = wallY + wallHeight / 2.0f; // Start fence from top of wall
    float topY = fenceBaseY + fenceHeight - 0.05f;
    float midY = fenceBaseY + fenceHeight / 2.0f;
    float botY = fenceBaseY + 0.05f;
    float postY = fenceBaseY + fenceHeight / 2.0f;

    // Draw the rectangular wall base (excluding front entrance area and parking access)
    // Front wall segments
    drawCube(-7.0f, wallY, 10.0f, 10.0f, wallHeight, 0.5f); // Front wall left side (-12.0 to -2.0)
    drawCube(11.0f, wallY, 10.0f, 10.0f, wallHeight, 0.5f); // Front wall right side (6.0 to 16.0)
    // Back wall
    drawCube(2.0f, wallY, -10.0f, wallWidth, wallHeight, 0.5f); // Back wall (-12.0 to 16.0)
    // Left wall
    drawCube(-12.0f, wallY, 0.0f, 0.5f, wallHeight, 20.0f); // Left wall (-10.0 to 10.0)

    // Right wall with gate opening for parking access
    // Adjusted segments to create a clear opening for the gate
    drawCube(16.0f, wallY, -6.25f, 0.5f, wallHeight, 7.5f); // Right wall bottom segment (-10.0 to -2.5)
    drawCube(16.0f, wallY, 6.25f, 0.5f, wallHeight, 7.5f);  // Right wall top segment (2.5 to 10.0)

    // Parking gate structure (pillars)
    setColorub(120, 120, 120); // Darker gray for gate pillars
    drawCube(16.0f, wallY + 1.5f, -2.0f, 0.8f, 4.0f, 0.6f); // Left gate pillar
    drawCube(16.0f, wallY + 1.5f, 2.0f, 0.8f, 4.0f, 0.6f);  // Right gate pillar

    // Gate sign
    setColorub(80, 80, 80); // Dark gray for sign background
    drawCube(16.2f, wallY + 3.0f, 0.0f, 0.3f, 1.2f, 3.5f); // Sign board

    // Sign text (Parking)
    glPushMatrix();
    glLineWidth(3.0f);
    glTranslatef(16.4f, wallY + 3.0f, 0.0f);
    setColor(1.0f, 1.0f, 1.0f);
    glScalef(0.002f, 0.002f, 0.002f);
    glRotatef(90, 0, 1, 0);
    const char* parkingText = "PARKING";
    for (const char* c = parkingText; *c != '\0'; c++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }
    glPopMatrix();

    // Entrance gate walls with name sign (pillars)
    setColorub(120, 120, 120);
    drawCube(-2.0f, wallY + 1.0f, 10.0f, 0.8f, 3.0f, 0.6f); // Left entrance pillar
    drawCube(6.0f, wallY + 1.0f, 10.0f, 0.8f, 3.0f, 0.6f);  // Right entrance pillar

    // Name sign wall
    setColorub(80, 80, 80);
    drawCube(2.0f, wallY + 1.5f, 10.2f, 7.0f, 1.5f, 0.3f); // Sign background

    // Sign text on the dark gray wall
    glPushMatrix();
    glTranslatef(2.0f, wallY + 1.8f, 10.35f);
    setColor(1.0f, 1.0f, 1.0f);
    glScalef(0.006f, 0.006f, 0.006f);

    glPushMatrix();
    glTranslatef(-400.0f, 0.0f, 0.0f);
    const char* text1 = "RUMAH DUKA";
    for (const char* c = text1; *c != '\0'; c++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-300.0f, -150.0f, 0.0f);
    const char* text2 = "Tiong Ting";
    for (const char* c = text2; *c != '\0'; c++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }
    glPopMatrix();
    glLineWidth(1.0f);

    glPopMatrix();

    // Iron fence - positioned ON TOP of concrete wall
    setColor(0.3f, 0.3f, 0.3f); // Dark gray for iron

    // Helper lambda for drawing fence segment (post and rails)
    auto drawFenceSegment = [&](float startCoord, float endCoord, float fixedCoord, bool isHorizontal) {
        float segmentLength = 1.5f; // Distance between posts
        std::vector<float> postPositions;

        // Collect all post positions
        for (float currentCoord = startCoord; currentCoord <= endCoord + 0.001f; currentCoord += segmentLength) {
            postPositions.push_back(currentCoord);
        }
        // Ensure the very end is included if not perfectly aligned
        if (std::abs(postPositions.back() - endCoord) > 0.001f) {
            postPositions.push_back(endCoord);
        }

        // Sort and remove duplicates (due to floating point precision or exact alignment)
        std::sort(postPositions.begin(), postPositions.end());
        postPositions.erase(std::unique(postPositions.begin(), postPositions.end(), [](float a, float b) { return std::abs(a - b) < 0.001f; }), postPositions.end());


        // Draw posts
        for (float pos : postPositions) {
            if (isHorizontal) { // X-axis fence
                drawCube(pos, postY, fixedCoord, postWidth, fenceHeight, postDepth);
            }
            else { // Z-axis fence
                drawCube(fixedCoord, postY, pos, postWidth, fenceHeight, postDepth);
            }
        }

        // Draw rails between adjacent posts
        for (size_t i = 0; i < postPositions.size() - 1; ++i) {
            float p1 = postPositions[i];
            float p2 = postPositions[i + 1];
            float railCenter = (p1 + p2) / 2.0f;
            float railLength = p2 - p1;

            if (isHorizontal) {
                drawCube(railCenter, topY, fixedCoord, railLength, railThickness, railThickness);
                drawCube(railCenter, midY, fixedCoord, railLength, railThickness, railThickness);
                drawCube(railCenter, botY, fixedCoord, railLength, railThickness, railThickness);
            }
            else {
                drawCube(fixedCoord, topY, railCenter, railThickness, railThickness, railLength);
                drawCube(fixedCoord, midY, railCenter, railThickness, railThickness, railLength);
                drawCube(fixedCoord, botY, railCenter, railThickness, railThickness, railLength);
            }
        }
        };

    // Front side - right section (after entrance)
    drawFenceSegment(6.0f, 16.0f, 10.0f, true);

    // Front side - left section (before entrance)
    drawFenceSegment(-12.0f, -2.0f, 10.0f, true);

    // Back side
    drawFenceSegment(-12.0f, 16.0f, -10.0f, true);

    // Left side
    drawFenceSegment(-10.0f, 10.0f, -12.0f, false);

    // Right side (split due to gate) - bottom segment
    drawFenceSegment(-10.0f, -2.5f, 16.0f, false); // Adjusted end coordinate to create gap

    // Right side (split due to gate) - top segment
    drawFenceSegment(2.5f, 10.0f, 16.0f, false); // Adjusted start coordinate to create gap
}

void drawMainBuilding()
{
    setColorub(235, 218, 174); // Warna dinding bangunan utama
    // security guard booth
    // left wall
    drawCube(-9.0f, 2.5f, 2.0f, 0.2f, 5.0f, 4.0f);
    // Back wall
    drawCube(-7.5f, 2.5f, 0.0f, 3.0f, 5.0f, 0.2f);
    // front wall (dengan bukaan untuk jendela/pintu)
    drawCube(-7.5f, 4.0f, 4.0f, 3.0f, 2.0f, 0.2f); // Bagian atas
    drawCube(-8.125f, 0.5f, 4.0f, 1.75f, 2.0f, 0.2f); // Bagian bawah kiri
    drawCube(-6.875f, 0.5f, 4.0f, 1.75f, 2.0f, 0.2f); // Bagian bawah kanan (tambahan untuk simetri)


    // Main building walls (front wall removed to show interior)
    // Left wall
    drawCube(-6.0f, 2.5f, 0.0f, 0.2f, 5.0f, 8.0f);
    // Right wall
    drawCube(10.0f, 2.5f, 0.0f, 0.2f, 5.0f, 8.0f);
    // Back wall
    drawCube(2.0f, 2.5f, -4.0f, 16.0f, 5.0f, 0.2f);
    // front wall (dengan bukaan)
    drawCube(2.0f, 4.0f, 4.0f, 16.0f, 2.0f, 0.2f); // Bagian atas
    drawCube(-1.0f, 0.5f, 4.0f, 6.0f, 2.0f, 0.2f); // Bagian bawah kiri (pintu masuk)
    drawCube(5.0f, 0.5f, 4.0f, 6.0f, 2.0f, 0.2f);  // Bagian bawah kanan (pintu masuk)
}

void drawRoof()
{
    // Main roof structure - dark metal
    setColor(0.4f, 0.4f, 0.4f);

    // Sloped roof using triangular sections
    // Front slope
    drawQuad(-7.0f, 5.0f, 4.5f, 11.0f, 5.0f, 4.5f,
        11.0f, 7.0f, 0.0f, -7.0f, 7.0f, 0.0f);

    // Back slope
    drawQuad(-7.0f, 7.0f, 0.0f, 11.0f, 7.0f, 0.0f,
        11.0f, 5.0f, -4.5f, -7.0f, 5.0f, -4.5f);

    // Side triangles (gable ends)
    setColorub(235, 218, 174); // Warna dinding
    drawTriangle(-6.0f, 5.0f, 4.5f, -6.0f, 7.0f, 0.0f, -6.0f, 5.0f, -4.5f);
    drawTriangle(10.0f, 5.0f, 4.5f, 10.0f, 5.0f, -4.5f, 10.0f, 7.0f, 0.0f);

    // Roof ridge
    setColor(0.3f, 0.3f, 0.3f);
    drawCube(2.0f, 7.0f, 0.0f, 18.2f, 0.2f, 0.3f);
}

void drawInterior()
{
    // Interior floor
    setColor(0.8f, 0.8f, 0.8f);
    drawCube(2.0f, 0.1f, 0.0f, 16.0f, 0.5f, 7.5f); // Lantai utama
    drawCube(-7.5f, 0.1f, 2.0f, 3.0f, 0.5f, 4.0f); // Lantai pos jaga

    // Tangga telah dihapus dari sini.

    // Interior walls/partitions
    setColor(0.9f, 0.9f, 0.9f);
    drawCube(-2.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);
    drawCube(2.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);
    drawCube(6.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);
    drawCube(10.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);

    // Ceiling
    setColor(0.95f, 0.95f, 0.95f);
    drawCube(2.0f, 3.8f, 0.0f, 16.0f, 0.2f, 7.5f);
}

void drawCanopy()
{
    setColor(0.5f, 0.5f, 0.5f);

    // left canopy (pos jaga)
    // Canopy roof
    drawCube(-7.5f, 5.0f, 0.5f, 6.0f, 0.2f, 9.0f);

    // Support beams
    setColor(0.4f, 0.4f, 0.4f);
    drawCube(-7.5f, 4.9f, -3.5f, 6.0f, 0.2f, 0.3f);
    drawCube(-7.5f, 4.9f, -0.6f, 6.0f, 0.2f, 0.3f);

    // front canopy (bangunan utama)
    // Canopy roof
    drawCube(2.0f, 5.0f, 6.0f, 18.0f, 0.2f, 5.0f);

    // Support columns
    setColor(0.3f, 0.3f, 0.3f);
    drawCube(-6.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(-2.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(2.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(6.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(10.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);

    // Support beams
    setColor(0.4f, 0.4f, 0.4f);
    drawCube(2.0f, 4.9f, 8.0f, 16.0f, 0.2f, 0.3f);

    drawCube(-6.0f, 4.9f, 6.0f, 0.3f, 0.2f, 5.0f);
    drawCube(-2.0f, 4.9f, 6.0f, 0.3f, 0.2f, 5.0f);
    drawCube(2.0f, 4.9f, 6.0f, 0.3f, 0.2f, 5.0f);
    drawCube(6.0f, 4.9f, 6.0f, 0.3f, 0.2f, 5.0f);
    drawCube(10.0f, 4.9f, 6.0f, 0.3f, 0.2f, 5.0f);
}

void drawPavedArea()
{
    // Paved courtyard
    setColor(0.14f, 0.14f, 0.14f);
    drawCube(2.0f, 0.05f, 2.0f, 28.0f, 0.1f, 16.0f); // Hanya satu kali

    // Paving pattern lines
    setColor(0.15f, 0.15f, 0.15f);
    // vertical
    for (float i = -12; i <= 16; i += 0.5) {
        drawCube(i, 0.06f, 2.0f, 0.1f, 0.12f, 16.0f); // Hanya satu kali
    }
    // horizontal
    for (float j = -6; j <= 10; j += 0.5) {
        drawCube(2.0f, 0.06f, j, 28.0f, 0.12f, 0.1f); // Hanya satu kali
    }
}

// Fungsi untuk menggambar satu unit lampu jalan
// Sekarang menerima GLenum light_id untuk mengontrol sumber cahaya individual
void drawStreetLight(float x, float y, float z, GLenum light_id) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Pole
    setColor(0.3f, 0.3f, 0.3f); // Dark gray
    drawCylinder(0.1f, 8.0f, 10, 0.0f, 4.0f, 0.0f); // Tinggi 8 unit, pusat di y=4.0

    // Lamp arm
    setColor(0.4f, 0.4f, 0.4f);
    drawCube(0.0f, 8.0f, 0.0f, 0.2f, 0.2f, 1.5f); // Lengan ke depan

    // Lamp head
    setColor(0.6f, 0.6f, 0.6f); // Warna casing lampu
    drawCube(0.0f, 7.8f, 0.8f, 0.5f, 0.5f, 0.5f); // Kepala lampu

    // Light source visual (bagian yang bersinar)
    if (isNightMode) {
        // Atur properti material emissive agar bagian ini "bersinar"
        GLfloat light_emissive[] = { 1.0f, 1.0f, 0.8f, 1.0f }; // Kuning terang
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, light_emissive);
        drawCube(0.0f, 7.7f, 0.8f, 0.3f, 0.3f, 0.3f); // Bagian yang bersinar
        // Reset emissive setelah menggambar agar tidak mempengaruhi objek lain
        GLfloat no_emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_emissive);
    }
    else {
        // Dalam mode siang, bagian ini tidak bersinar
        setColor(0.6f, 0.6f, 0.6f); // Warna casing lampu normal
        drawCube(0.0f, 7.7f, 0.8f, 0.3f, 0.3f, 0.3f);
    }


    // Atur posisi sumber cahaya OpenGL untuk lampu ini
    // Posisi ini relatif terhadap modelview matrix saat ini (setelah glTranslatef)
    GLfloat light_position[] = { 0.0f, 7.7f, 0.8f, 1.0f }; // Posisi lampu di dalam kepala lampu
    glLightfv(light_id, GL_POSITION, light_position);

    glPopMatrix();
}


void drawEnvironment()
{
    // Ground plane
    setColor(0.3f, 0.6f, 0.3f);
    drawCube(0.0f, -0.5f, 0.0f, 50.0f, 1.0f, 50.0f);

    // Palm trees
    // Posisi batang pohon
    float treeTrunkX[] = { -10.0f, -7.0f, -4.0f, -1.0f, 2.0f, 5.0f, 8.0f, 11.0f };
    float treeTrunkZ = -8.0f;
    setColor(0.4f, 0.2f, 0.1f); // Brown trunk
    for (float x_pos : treeTrunkX) {
        drawCylinder(0.2f, 4.0f, 10, x_pos, 2.0f, treeTrunkZ); // Gunakan drawCylinder
    }

    // Posisi daun
    setColor(0.1f, 0.7f, 0.1f); // Green leaves
    for (float x_pos : treeTrunkX) {
        // Daun bisa lebih kompleks, ini hanya kubus sebagai placeholder
        drawCube(x_pos, 4.5f, treeTrunkZ, 2.0f, 1.0f, 2.0f);
    }
}

void drawScene()
{
    // Atur warna latar belakang berdasarkan mode siang/malam
    if (isNightMode) {
        glClearColor(0.05f, 0.05f, 0.15f, 1.0f); // Langit malam gelap
    }
    else {
        glClearColor(0.6f, 0.7f, 0.9f, 1.0f); // Langit siang cerah
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Atur kamera
    gluLookAt(cameraX, cameraY, cameraZ, // Eye position
        cameraX + sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f), // Look at X
        cameraY + sin(cameraAngleX * M_PI / 180.0f),                                   // Look at Y
        cameraZ - cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f), // Look at Z
        0.0f, 1.0f, 0.0f);  // Up vector (tetap Y-axis)

    // Atur pencahayaan berdasarkan mode siang/malam
    if (isNightMode) {
        // Cahaya matahari (GL_LIGHT0) sangat redup atau mati
        GLfloat light0_ambient_night[] = { 0.05f, 0.05f, 0.1f, 1.0f };
        GLfloat light0_diffuse_night[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat light0_specular_night[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient_night);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse_night);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular_night);

        // Cahaya ambient global untuk malam
        GLfloat globalAmbientNight[] = { 0.1f, 0.1f, 0.15f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientNight);

        // Aktifkan lampu jalan yang digunakan
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
        glEnable(GL_LIGHT3);
        glEnable(GL_LIGHT4);

    }
    else {
        // Cahaya matahari (GL_LIGHT0) normal
        GLfloat light0_ambient_day[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat light0_diffuse_day[] = { 0.9f, 0.9f, 0.8f, 1.0f };
        GLfloat light0_specular_day[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient_day);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse_day);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular_day);

        // Cahaya ambient global untuk siang
        GLfloat globalAmbientDay[] = { 0.2f, 0.2f, 0.25f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientDay);

        // Nonaktifkan semua lampu jalan
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);
        glDisable(GL_LIGHT4);
    }


    drawEnvironment();
    drawRoad();
    drawParkingGate();
    drawPavedArea();
    drawFence();
    drawMainBuilding();
    drawInterior();
    drawRoof();
    drawCanopy();
    drawParkingArea();

    // Gambar lampu jalan (jumlah dikurangi) dan atur posisi sumber cahaya
    // Sisi kiri jalan
    drawStreetLight(-10.0f, 0.0f, 15.0f, GL_LIGHT1);
    drawStreetLight(10.0f, 0.0f, 15.0f, GL_LIGHT2);

    // Sisi kanan jalan
    drawStreetLight(-10.0f, 0.0f, 25.0f, GL_LIGHT3);
    drawStreetLight(10.0f, 0.0f, 25.0f, GL_LIGHT4);


    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    float moveSpeed = 0.5f;
    float radX = cameraAngleX * M_PI / 180.0f;
    float radY = cameraAngleY * M_PI / 180.0f;

    switch (key)
    {
    case 'w': // Maju
        cameraX += sin(radY) * moveSpeed;
        cameraZ -= cos(radY) * moveSpeed;
        break;
    case 's': // Mundur
        cameraX -= sin(radY) * moveSpeed;
        cameraZ += cos(radY) * moveSpeed;
        break;
    case 'a': // Geser kiri
        cameraX -= cos(radY) * moveSpeed;
        cameraZ -= sin(radY) * moveSpeed;
        break;
    case 'd': // Geser kanan
        cameraX += cos(radY) * moveSpeed;
        cameraZ += sin(radY) * moveSpeed;
        break;
    case 'q': // Naik
        cameraY += moveSpeed;
        break;
    case 'e': // Turun
        cameraY -= moveSpeed;
        break;
    case 'n': // Toggle Night/Day mode
    case 'N':
        isNightMode = !isNightMode;
        std::cout << "Mode: " << (isNightMode ? "Night" : "Day") << std::endl;
        break;
    case 27: // ESC key
         exit(0);
        break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mousePressed = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else
        {
            mousePressed = false;
        }
    }
}

void mouseMotion(int x, int y)
{
    if (mousePressed)
    {
        float sensitivity = 0.2f; // Sesuaikan sensitivitas mouse
        cameraAngleY += (x - lastMouseX) * sensitivity;
        cameraAngleX += (y - lastMouseY) * sensitivity;

        // Clamp vertical rotation (pandangan ke atas/bawah)
        if (cameraAngleX > 89.0f) // Batasi agar tidak terbalik
            cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f) // Batasi agar tidak terbalik
            cameraAngleX = -89.0f;

        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void setupLighting()
{
    glEnable(GL_LIGHTING); // Aktifkan sistem pencahayaan
    glEnable(GL_LIGHT0);   // Aktifkan sumber cahaya pertama (matahari/cahaya utama)

    // Aktifkan semua sumber cahaya yang akan digunakan untuk lampu jalan
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    // GL_LIGHT5 dan GL_LIGHT6 tidak lagi digunakan karena jumlah lampu berkurang

    // Aktifkan GL_COLOR_MATERIAL agar glColor3f/ub dapat mengatur properti material
    glEnable(GL_COLOR_MATERIAL);
    // glColor3f akan mempengaruhi properti material GL_AMBIENT dan GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Konfigurasi GL_LIGHT0 (Cahaya Matahari/Utama)
    // Posisi cahaya ini akan diatur di drawScene untuk simulasi siang/malam
    GLfloat light0_position[] = { 10.0f, 10.0f, 10.0f, 1.0f }; // Posisional
    GLfloat light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light0_diffuse[] = { 0.9f, 0.9f, 0.8f, 1.0f };
    GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Konfigurasi GL_LIGHT1 hingga GL_LIGHT4 (Lampu Jalan)
    // Properti ini akan sama untuk semua lampu jalan, hanya posisi yang berbeda
    GLfloat light_street_ambient[] = { 0.05f, 0.05f, 0.0f, 1.0f }; // Sedikit ambient kuning
    GLfloat light_street_diffuse[] = { 0.8f, 0.8f, 0.6f, 1.0f };  // Cahaya kuning terang
    GLfloat light_street_specular[] = { 0.5f, 0.5f, 0.4f, 1.0f }; // Sorotan kuning

    for (int i = 1; i <= 4; ++i) { // Loop untuk LIGHT1 hingga LIGHT4
        GLenum current_light = GL_LIGHT0 + i;
        glLightfv(current_light, GL_AMBIENT, light_street_ambient);
        glLightfv(current_light, GL_DIFFUSE, light_street_diffuse);
        glLightfv(current_light, GL_SPECULAR, light_street_specular);
        // Tambahkan redaman cahaya (attenuation) agar cahaya lampu jalan tidak terlalu jauh
        glLightf(current_light, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(current_light, GL_LINEAR_ATTENUATION, 0.05f);
        glLightf(current_light, GL_QUADRATIC_ATTENUATION, 0.01f);
    }

    // Cahaya ambient global (menerangi semua objek secara merata)
    // Ini akan diatur di drawScene berdasarkan mode siang/malam
    GLfloat globalAmbientDay[] = { 0.2f, 0.2f, 0.25f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientDay);

    // Aktifkan normalisasi normal setelah transformasi
    glEnable(GL_NORMALIZE);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1024, 768);

    int window = glutCreateWindow("Tugas Rancang AST - Tiong Ting Salatiga");

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    glutSetWindowTitle("Tugas Rancang AST - Tiong Ting Salatiga");

    glEnable(GL_DEPTH_TEST);
    setupLighting();

    glClearColor(0.6f, 0.7f, 0.9f, 1.0f);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutIdleFunc([]() { glutPostRedisplay(); });

    std::cout << "Controls:" << std::endl;
    std::cout << "WASD - Move camera horizontally" << std::endl;
    std::cout << "Q/E - Move camera up/down" << std::endl;
    std::cout << "Mouse (Left Click + Drag) - Look around" << std::endl;
    std::cout << "N - Toggle Day/Night mode" << std::endl;
    std::cout << "ESC - Exit" << std::endl;

    glutMainLoop();
    return 0;
}
