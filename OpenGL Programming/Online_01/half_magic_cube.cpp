#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include <vector>
#include <iostream>

struct point {
    GLfloat x, y, z;
};

struct point pos;   // position of the eye
struct point l;     // look/forward direction
struct point r;     // right direction
struct point u;     // up direction

float scale = 0.0f;
float s = 1.0f;
float step = 0.0f;

float gamma = 70.5287794;
float cylinder_radius = 0;
float cylinder_height = sqrt(2);

float rotation_angle = 0.0f;
float lr_move_amount = 0.0f;

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
}

// Global variables
GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;
bool isAxes = true, isCube = false, isPyramid = false;

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);   // Red
    // X axis
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glColor3f(0, 1, 0);   // Green
    // Y axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glColor3f(0, 0, 1);   // Blue
    // Z axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}


void drawTriangle() {
    // Calculate centroid of the triangle
    GLfloat centroidX = 1.0f / 3.0f;
    GLfloat centroidY = 1.0f / 3.0f;
    GLfloat centroidZ = 1.0f / 3.0f;

    glPushMatrix();
    glTranslatef(centroidX, centroidY, centroidZ); // Translate to the centroid
    glScalef(s, s, s); // Apply scaling transformation
    glTranslatef(-centroidX, -centroidY, -centroidZ); // Translate back to the origin

    glBegin(GL_TRIANGLES);
    glVertex3d(1, 0, 0);
    glVertex3d(0, 1, 0);
    glVertex3d(0, 0, 1);
    glEnd();

    glPopMatrix();
}

void drawOctahedron() {
    glPushMatrix();
    glColor3ub(228, 0, 130);
    drawTriangle();

    glColor3f(0, 0.8, 1);
    glRotated(90, 0, 1, 0);
    drawTriangle();

    glColor3ub(228, 0, 130);
    glRotated(90, 0, 1, 0);
    drawTriangle();

    glColor3f(0, 0.8, 1);
    glRotated(90, 0, 1, 0);
    drawTriangle();

    // glRotated(180, 1, 0, 0);
    // glColor3f(0, 0.8, 1);
    // drawTriangle();

    // glColor3ub(228, 0, 130);
    // glRotated(90, 0, 1, 0);
    // drawTriangle();

    // glColor3f(0, 0.8, 1);
    // glRotated(90, 0, 1, 0);
    // drawTriangle();

    // glColor3ub(228, 0, 130);
    // glRotated(90, 0, 1, 0);
    // drawTriangle();

    glPopMatrix();
}


std::vector<float> buildUnitPositiveX(int subdivision)
{
    const float DEG2RAD = acos(-1) / 180.0f;

    std::vector<float> vertices;
    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along Y-axis
    float a2;           // latitudinal angle along Z-axis

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for (unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for (unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into array
            vertices.push_back(v[0]);
            vertices.push_back(v[1]);
            vertices.push_back(v[2]);
        }
    }
    return vertices;
}


void drawFirstSphereFace() {
    std::vector<float> vertices = buildUnitPositiveX(6);
    glBegin(GL_POLYGON);
    for (int i = 0; i < vertices.size(); i += 3) {
        GLfloat x = vertices[i];
        GLfloat y = vertices[i + 1];
        GLfloat z = vertices[i + 2];
        glVertex3f(x, y, z);
    }
    glEnd();
}

void drawSphereFace() {
    std::vector<float> vertices = buildUnitPositiveX(6);
    glBegin(GL_POLYGON);
    for (int i = 0; i < vertices.size(); i += 3) {
        if (vertices[i + 1] < 0)continue;
        GLfloat x = vertices[i];
        GLfloat y = vertices[i + 1];
        GLfloat z = vertices[i + 2];
        glVertex3f(x, y, z);
    }
    glEnd();
}

void drawFirstSegment() {
    glPushMatrix();
    glTranslated((16.0 - step) / 16.0, 0.0, 0.0);
    glScaled(scale / 16.0, scale / 16.0, scale / 16.0);
    glScaled(1 / sqrt(3.0), 1 / sqrt(3.0), 1 / sqrt(3.0));
    drawFirstSphereFace();
    glPopMatrix();
}

void drawOneSegment() {
    glPushMatrix();
    glTranslated((16.0 - step) / 16.0, 0.0, 0.0);
    glScaled(scale / 16.0, scale / 16.0, scale / 16.0);
    glScaled(1 / sqrt(3.0), 1 / sqrt(3.0), 1 / sqrt(3.0));
    drawSphereFace();
    glPopMatrix();
}

void drawSphere() {
    glPushMatrix();
    glColor3ub(1, 50, 32);
    drawOneSegment();

    glColor3ub(0, 0, 170);
    glRotatef(90, 0, 1, 0);
    drawOneSegment();

    glColor3ub(1, 50, 32);
    glRotatef(90, 0, 1, 0);
    drawOneSegment();

    glColor3ub(0, 0, 170);
    glRotatef(90, 0, 1, 0);
    drawOneSegment();

    glColor3ub(170, 0, 0);
    glRotatef(90, 0, 0, 1);
    drawFirstSegment();

    // glColor3ub(170, 0, 0);
    // glRotatef(180, 0, 0, 1);
    // drawOneSegment();

    glPopMatrix();
}

void drawUpperCylinderSegment(GLfloat radius, GLfloat height)
{
    float x = 0.0f;
    float y = 0.0f;
    float stepsize = 0.1f;

    glColor3f(1, 1, 0);
    glBegin(GL_QUAD_STRIP);
    for (float angle = 0.0f; angle <= (M_PI - acos(-1 / 3)) * 180 / M_PI;angle += stepsize) {
        x = radius * cos((angle * M_PI) / 180.0);
        y = radius * sin((angle * M_PI) / 180.0);
        glVertex3f(x, height / 2.0, y);
        glVertex3f(x, -height / 2.0, y);
    }
    glEnd();
}

void drawLowerCylinderSegment(GLfloat radius, GLfloat height)
{
    float x = 0.0f;
    float y = 0.0f;
    float stepsize = 0.1f;

    glColor3f(1, 1, 0);
    glBegin(GL_QUAD_STRIP);
    for (float angle = 0.0f; angle <= (M_PI - acos(-1 / 3)) * 90 / M_PI;angle += stepsize) {
        x = radius * cos((angle * M_PI) / 180.0);
        y = radius * sin((angle * M_PI) / 180.0);
        glVertex3f(x, height / 2.0, y);
        glVertex3f(x, -height / 2.0, y);
    }
    glEnd();
}


void UpperCylinderSegmentScaling() {
    glPushMatrix();
    glTranslated((16 - step) / (2 * 16), (16 - step) / (2 * 16), 0.0);
    glRotatef(45.0, 0, 0, 1);
    glRotatef(((M_PI - acos(-1 / 3)) * 180 / M_PI) / 2, 0, 1, 0);
    drawUpperCylinderSegment(cylinder_radius, cylinder_height);
    glPopMatrix();
}

void LowerCylinderSegmentScaling() {
    glPushMatrix();
    glTranslated((16 - step) / (2 * 16), (16 - step) / (2 * 16), 0.0);
    glRotatef(45.0, 0, 0, 1);
    glRotatef(((M_PI - acos(-1 / 3)) * 180 / M_PI) / 2, 0, 1, 0);
    drawLowerCylinderSegment(cylinder_radius, cylinder_height);
    glPopMatrix();
}

void drawCylinder() {
    glPushMatrix();

    UpperCylinderSegmentScaling();

    glRotatef(90, 0, 1, 0);
    UpperCylinderSegmentScaling();

    glRotatef(90, 0, 1, 0);
    UpperCylinderSegmentScaling();

    glRotatef(90, 0, 1, 0);
    UpperCylinderSegmentScaling();

    glRotatef(90, 1, 0, 0);
    LowerCylinderSegmentScaling();

    glRotatef(90, 0, 0, 1);
    LowerCylinderSegmentScaling();

    glRotatef(90, 0, 0, 1);
    LowerCylinderSegmentScaling();

    glRotatef(90, 0, 0, 1);
    LowerCylinderSegmentScaling();

    // glRotatef(90, 1, 0, 0);
    // CylinderSegmentScaling();

    // glRotatef(90, 0, 1, 0);
    // CylinderSegmentScaling();

    // glRotatef(90, 0, 1, 0);
    // CylinderSegmentScaling();

    // glRotatef(90, 0, 1, 0);
    // CylinderSegmentScaling();
    glPopMatrix();
}


void display() {
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix


    gluLookAt(pos.x, pos.y, pos.z,
        pos.x + l.x, pos.y + l.y, pos.z + l.z,
        u.x, u.y, u.z);

    glRotatef(rotation_angle, 0, 1, 0);
    // draw
    if (isAxes) drawAxes();

    //drawOctahedron();

    drawOctahedron();
    drawSphere();

    drawCylinder();


    glutSwapBuffers();  // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y) {
    float rate = 0.075f;
    double temp;

    switch (key) {
        // Control eye (location of the eye)
        // control eyex

    case ',':                   //Octahedron to Sphere
        if (step < 16) {
            step++;
            scale++;
            s -= 1.0 / 16;

            cylinder_radius += 1 / (sqrt(3) * 16.0);
            cylinder_height -= sqrt(2) / 16.0;
        }
        break;


    case '.':                   //Sphere to Octahedron
        if (step > 0) {
            step--;
            scale--;
            s += 1.0 / 16;

            cylinder_radius -= 1 / (sqrt(3) * 16.0);
            cylinder_height += sqrt(2) / 16.0;
        }
        break;

    case 'a':                   //Rotate the object in the clockwise direction about its own axis
        rotation_angle -= 7.5;
        break;

    case 'd':                   //Rotate the object in the counter-clockwise direction about its own axis
        rotation_angle += 7.5;
        break;


    case '1':
        pos.x -= r.x;
        pos.y -= r.y;
        pos.z -= r.z;
        break;
    case '2':
        pos.x += r.x;
        pos.y += r.y;
        pos.z += r.z;
        break;

    case '3':               //look up
        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;
        break;
    case '4':               //look down
        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;
        break;

    case '5':               //tilt counter-clockwise
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;

    case '6':              //tilt clockwise
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x, int y) {
    double v = 0.25;
    double lx = centerx - eyex;
    double lz = centerz - eyez;
    float rate = 0.1f;
    double temp;
    switch (key) {
    case GLUT_KEY_RIGHT:                //camera moves right
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;
    case GLUT_KEY_LEFT:                 //camera moves left

        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);
        break;

    case GLUT_KEY_UP:                   //camera moves forward
        pos.x += l.x / 16;
        pos.y += l.y / 16;
        pos.z += l.z / 16;
        break;

    case GLUT_KEY_DOWN:                 //camera moves backward
        pos.x -= l.x / 16;
        pos.y -= l.y / 16;
        pos.z -= l.z / 16;
        break;

    case GLUT_KEY_PAGE_UP:              //camera moves up

        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);
        break;
    case GLUT_KEY_PAGE_DOWN:            //camera moves down

        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);
        break;

    default:
        return;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    pos.x = 0;pos.y = 0;pos.z = 6;

    l.x = 0;l.y = 0;l.z = -1;
    u.x = 0;u.y = 0.3;u.z = 0;
    r.x = 0.3;r.y = 0;r.z = 0;

    glutInit(&argc, argv);                      // Initialize GLUT
    glutInitWindowSize(640, 640);               // Set the window's initial width & height
    glutInitWindowPosition(50, 50);             // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");      // Create a window with the given title
    glutDisplayFunc(display);                   // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);           // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);         // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);        // Register callback handler for special-key event
    initGL();                                   // Our own OpenGL initialization
    glutMainLoop();                             // Enter the event-processing loop
    return 0;
}
