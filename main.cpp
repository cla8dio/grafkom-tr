#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>

#include <cmath>
#include <iostream>

// Camera variables
float cameraX = 0.0f, cameraY = 5.0f, cameraZ = 15.0f;
float cameraAngleX = 0.0f, cameraAngleY = 0.0f;
float lastMouseX = 0, lastMouseY = 0;
bool mousePressed = false;

// Lighting and colors
void setColor(float r, float g, float b)
{
    glColor3f(r, g, b);
}

void setColorub(unsigned char r, unsigned char g, unsigned char b)
{
    glColor3ub(r, g, b);
}

void drawCube(float x, float y, float z, float width, float height, float depth)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawSphere(float x, float y, float z, float radius)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(radius, 12, 12);
    glPopMatrix();
}

void drawQuad(float x1, float y1, float z1, float x2, float y2, float z2,
              float x3, float y3, float z3, float x4, float y4, float z4)
{
    glBegin(GL_QUADS);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
}

void drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2,
                  float x3, float y3, float z3)
{
    glBegin(GL_TRIANGLES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glEnd();
}

void drawMainBuilding()
{
    setColorub(235, 218, 174);
    drawCube(-9.0f, 2.5f, 2.0f, 0.2f, 5.0f, 4.0f);
    drawCube(-7.5f, 2.5f, 0.0f, 3.0f, 5.0f, 0.2f);
    drawCube(-7.5f, 4.0f, 4.0f, 3.0f, 2.0f, 0.2f);
    drawCube(-8.125f, 0.5f, 4.0f, 1.75f, 2.0f, 0.2f);

    drawCube(-6.0f, 2.5f, 0.0f, 0.2f, 5.0f, 8.0f);
    drawCube(10.0f, 2.5f, 0.0f, 0.2f, 5.0f, 8.0f);
    drawCube(2.0f, 2.5f, -4.0f, 16.0f, 5.0f, 0.2f);
    drawCube(2.0f, 4.0f, 4.0f, 16.0f, 2.0f, 0.2f);
}

void drawRoof()
{
    setColor(0.4f, 0.4f, 0.4f);
    drawQuad(-7.0f, 5.0f, 4.5f, 11.0f, 5.0f, 4.5f,
             11.0f, 7.0f, 0.0f, -7.0f, 7.0f, 0.0f);

    drawQuad(-7.0f, 7.0f, 0.0f, 11.0f, 7.0f, 0.0f,
             11.0f, 5.0f, -4.5f, -7.0f, 5.0f, -4.5f);

    setColorub(235, 218, 174);
    drawTriangle(-6.0f, 5.0f, 4.5f, -6.0f, 7.0f, 0.0f, -6.0f, 5.0f, -4.5f);
    drawTriangle(10.0f, 5.0f, 4.5f, 10.0f, 5.0f, -4.5f, 10.0f, 7.0f, 0.0f);

    setColor(0.3f, 0.3f, 0.3f);
    drawCube(2.0f, 7.0f, 0.0f, 18.2f, 0.2f, 0.3f);
}

void drawInterior()
{
    setColor(0.8f, 0.8f, 0.8f);
    drawCube(2.0f, 0.1f, 0.0f, 16.0f, 0.5f, 7.5f);
    drawCube(-7.5f, 0.1f, 2.0f, 3.0f, 0.5f, 4.0f);

    setColor(0.1f, 0.1f, 0.1f);
    drawCube(0.5f, 0.1f, 4.0f, 19.0f, 0.5f, 0.5f);
    drawCube(0.5f, 0.1f, 4.3f, 19.0f, 0.2f, 0.2f);

    setColor(0.9f, 0.9f, 0.9f);
    drawCube(-2.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);
    drawCube(2.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);
    drawCube(6.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);
    drawCube(10.0f, 1.5f, -1.0f, 0.2f, 3.0f, 6.0f);

    setColor(0.95f, 0.95f, 0.95f);
    drawCube(2.0f, 3.8f, 0.0f, 16.0f, 0.2f, 7.5f);
}

void drawCanopy()
{
    setColor(0.5f, 0.5f, 0.5f);
    drawCube(-7.5f, 5.0f, 0.5f, 6.0f, 0.2f, 9.0f);

    setColor(0.4f, 0.4f, 0.4f);
    drawCube(-7.5f, 4.9f, -3.5f, 6.0f, 0.2f, 0.3f);
    drawCube(-7.5f, 4.9f, -0.6f, 6.0f, 0.2f, 0.3f);

    drawCube(2.0f, 5.0f, 6.0f, 18.0f, 0.2f, 5.0f);

    setColor(0.3f, 0.3f, 0.3f);
    drawCube(-6.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(-2.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(2.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(6.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);
    drawCube(10.0f, 2.5f, 4.0f, 0.3f, 5.0f, 0.3f);

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
    setColor(0.14f, 0.14f, 0.14f);
    drawCube(0.5f, 0.05f, 3.0f, 23.0f, 0.1f, 18.0f);

    setColor(0.15f, 0.15f, 0.15f);
    for (float i = -11; i <= 12; i += 0.5f)
        drawCube(i, 0.06f, 3.0f, 0.1f, 0.12f, 18.0f);

    for (float j = -6; j <= 12; j += 0.5f)
        drawCube(0.5f, 0.06f, j, 23.0f, 0.12f, 0.1f);
}

// Fungsi untuk menggambar pagar
void drawFence()
{
    // Warna kayu cokelat untuk pagar
    setColor(0.6f, 0.4f, 0.2f);

    // Pagar depan (sebelah kiri)
    for (float x = -15.0f; x <= -10.0f; x += 1.0f)
    {
        // Tiang pagar
        drawCube(x, 1.0f, 12.0f, 0.15f, 2.0f, 0.15f);
        // Papan horizontal atas
        drawCube(x, 1.7f, 12.0f, 0.8f, 0.1f, 0.15f);
        // Papan horizontal bawah
        drawCube(x, 1.0f, 12.0f, 0.8f, 0.1f, 0.15f);
    }

    // Pagar kiri
    for (float z = 12.0f; z >= -12.0f; z -= 1.0f)
    {
        // Tiang pagar
        drawCube(-15.0f, 1.0f, z, 0.15f, 2.0f, 0.15f);
        // Papan horizontal atas
        drawCube(-15.0f, 1.7f, z, 0.15f, 0.1f, 0.8f);
        // Papan horizontal bawah
        drawCube(-15.0f, 1.0f, z, 0.15f, 0.1f, 0.8f);
    }

    // Pagar belakang
    for (float x = -15.0f; x <= 15.0f; x += 1.0f)
    {
        // Tiang pagar
        drawCube(x, 1.0f, -12.0f, 0.15f, 2.0f, 0.15f);
        // Papan horizontal atas
        drawCube(x, 1.7f, -12.0f, 0.8f, 0.1f, 0.15f);
        // Papan horizontal bawah
        drawCube(x, 1.0f, -12.0f, 0.8f, 0.1f, 0.15f);
    }

    // Pagar kanan
    for (float z = -12.0f; z <= 12.0f; z += 1.0f)
    {
        // Tiang pagar
        drawCube(15.0f, 1.0f, z, 0.15f, 2.0f, 0.15f);
        // Papan horizontal atas
        drawCube(15.0f, 1.7f, z, 0.15f, 0.1f, 0.8f);
        // Papan horizontal bawah
        drawCube(15.0f, 1.0f, z, 0.15f, 0.1f, 0.8f);
    }
}

// Fungsi untuk menggambar semak-semak
void drawBushes()
{
    // Warna hijau untuk semak-semak - posisi di tanah (y = 0)
    setColor(0.2f, 0.6f, 0.2f);

    // Semak di sekitar pagar depan (hindari area parkir mobil)
    drawSphere(-16.0f, 0.8f, 11.0f, 0.8f);
    drawSphere(-16.5f, 0.6f, 1.0f, 0.6f);
    drawSphere(-16.2f, 0.7f, -2.0f, 0.7f);

    // Semak di sekitar pagar kanan
    drawSphere(16.0f, 0.8f, 8.0f, 0.8f);
    drawSphere(16.5f, 0.6f, 5.0f, 0.6f);
    drawSphere(16.2f, 0.7f, 2.0f, 0.7f);
    drawSphere(16.0f, 0.6f, -2.0f, 0.6f);
    drawSphere(16.5f, 0.7f, -5.0f, 0.7f);

    // Semak di sekitar pagar belakang
    drawSphere(-8.0f, 0.7f, -13.0f, 0.7f);
    drawSphere(5.0f, 0.8f, -13.0f, 0.8f);
    drawSphere(12.0f, 0.6f, -13.5f, 0.6f);

    // Semak di sisi kiri pagar (hindari area parkir mobil z = 4-8)
    drawSphere(-16.0f, 0.7f, -5.0f, 0.7f);
    drawSphere(-16.5f, 0.6f, -8.0f, 0.6f);
    drawSphere(-16.2f, 0.8f, -11.0f, 0.8f);

    // Semak variasi warna hijau tua
    setColor(0.15f, 0.5f, 0.15f);
    drawSphere(-16.8f, 0.6f, 0.0f, 0.6f);
    drawSphere(16.8f, 0.6f, 0.0f, 0.6f);
    drawSphere(0.0f, 0.5f, -13.5f, 0.5f);
}

// Fungsi untuk menggambar mobil
void drawCar(float x, float y, float z, float rotationY, float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    // Bodi mobil
    setColor(r, g, b);
    drawCube(0.0f, 0.5f, 0.0f, 3.0f, 0.8f, 1.5f);

    // Atap mobil
    setColor(r * 0.8f, g * 0.8f, b * 0.8f);
    drawCube(0.0f, 1.0f, 0.0f, 2.0f, 0.6f, 1.3f);

    // Roda
    setColor(0.1f, 0.1f, 0.1f);
    drawSphere(1.0f, 0.2f, 0.8f, 0.3f);  // Roda depan kanan
    drawSphere(1.0f, 0.2f, -0.8f, 0.3f); // Roda depan kiri
    drawSphere(-1.0f, 0.2f, 0.8f, 0.3f); // Roda belakang kanan
    drawSphere(-1.0f, 0.2f, -0.8f, 0.3f); // Roda belakang kiri

    // Kaca depan
    setColor(0.7f, 0.8f, 0.9f);
    drawCube(0.8f, 1.0f, 0.0f, 0.1f, 0.5f, 1.2f);

    // Kaca belakang
    drawCube(-0.8f, 1.0f, 0.0f, 0.1f, 0.5f, 1.2f);

    // Bumper depan
    setColor(0.3f, 0.3f, 0.3f);
    drawCube(1.6f, 0.3f, 0.0f, 0.2f, 0.3f, 1.5f);

    // Bumper belakang
    drawCube(-1.6f, 0.3f, 0.0f, 0.2f, 0.3f, 1.5f);

    glPopMatrix();
}

// Fungsi untuk menggambar kedua mobil
void drawCars()
{
    // Mobil 1 - Merah, parkir berjajar di sisi kiri
    drawCar(-18.0f, 0.0f, 8.0f, 0.0f, 0.8f, 0.2f, 0.2f);

    // Mobil 2 - Biru, parkir berjajar di sisi kiri (di belakang mobil merah)
    drawCar(-18.0f, 0.0f, 4.0f, 0.0f, 0.2f, 0.2f, 0.8f);
}

void drawEnvironment()
{
    setColor(0.3f, 0.6f, 0.3f);
    drawCube(0.0f, -0.5f, 0.0f, 50.0f, 1.0f, 50.0f);

    setColor(0.4f, 0.2f, 0.1f);
    drawCube(-12.0f, 2.0f, -5.0f, 0.5f, 4.0f, 0.5f);
    drawCube(10.0f, 2.0f, -8.0f, 0.5f, 4.0f, 0.5f);

    setColor(0.1f, 0.7f, 0.1f);
    drawCube(-12.0f, 4.5f, -5.0f, 2.0f, 1.0f, 2.0f);
    drawCube(10.0f, 4.5f, -8.0f, 2.0f, 1.0f, 2.0f);
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0f, 10.0f, 40.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glTranslatef(cameraX, cameraY, cameraZ);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);

    drawEnvironment();
    drawPavedArea();
    drawMainBuilding();
    drawInterior();
    drawRoof();
    drawCanopy();
    drawFence();      // Menambahkan pagar
    drawBushes();     // Menambahkan semak-semak
    drawCars();       // Menambahkan mobil

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
    switch (key)
    {
    case 'w': cameraZ -= 0.5f; break;
    case 's': cameraZ += 0.5f; break;
    case 'a': cameraX -= 0.5f; break;
    case 'd': cameraX += 0.5f; break;
    case 'q': cameraY += 0.5f; break;
    case 'e': cameraY -= 0.5f; break;
    case 27: exit(0); break;
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
        cameraAngleY += (x - lastMouseX) * 0.5f;
        cameraAngleX += (y - lastMouseY) * 0.5f;
        cameraAngleX = std::max(-90.0f, std::min(90.0f, cameraAngleX));
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void setupLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat lightPos[] = {10.0f, 10.0f, 10.0f, 1.0f};
    GLfloat lightAmbient[] = {0.13f, 0.1f, 0.1f, 0.3f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.7f, 0.2f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Tugas Rancang AST - Tiong Ting Salatiga");

    glEnable(GL_DEPTH_TEST);
    setupLighting();

    glClearColor(0.6f, 0.7f, 0.9f, 1.0f);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    std::cout << "Controls:\nWASD - Move camera\nQ/E - Move up/down\nMouse - Look\nESC - Exit\n";

    glutMainLoop();
    return 0;
}
