#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <ctime>

const float ropeLength = 0.5f;  // Length of the pendulum rope
const float bobRadius = 0.075f;  // Radius of the pendulum bob
const float gravity = 9.8f;  // Acceleration due to gravity

float angle = M_PI / 6.0f;  // Initial angle of the pendulum (45 degrees)
float angularVelocity = sqrt(gravity / (ropeLength * 2.1));
float t = 0.0f;
void update(int value)
{
    // Update the pendulum's motion
    angle = (M_PI / 6.0f) * cos(angularVelocity * t);
    t += 0.01;

    glutPostRedisplay();
    glutTimerFunc(10, update, 0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity(); // Reset the current matrix to identity

    // Get the current time
    time_t currentTime = time(nullptr);
    struct tm* localTime = localtime(&currentTime);
    int hours = localTime->tm_hour;
    int minutes = localTime->tm_min;
    int seconds = localTime->tm_sec;

    // Calculate the angles for hour, minute, and second hands
    float angleHour = 90.0f - ((hours % 12) + minutes / 60.0f + seconds / 3600.0f) * 30.0f;
    float angleMinute = 90.0f - (minutes + seconds / 60.0f) * 6.0f;
    float angleSecond = 90.0f - seconds * 6.0f;

    glColor3f(0.3058f, 0.1294f, 0.1215f);
    glBegin(GL_QUADS);  // Begin drawing filled rectangle
    glVertex2f(-0.35f, -0.6f);  // Bottom-left vertex
    glVertex2f(0.35f, -0.6f);  // Bottom-right vertex
    glVertex2f(0.35f, 0.1f);  // Top-right vertex
    glVertex2f(-0.35f, 0.1f);  // Top-left vertex
    glEnd();  // End drawing filled rectangle

    glBegin(GL_TRIANGLES);  // Begin drawing filled triangle
    glVertex2f(-0.35f, -0.6f);  // Bottom-left vertex
    glVertex2f(0.35f, -0.6f);  // Bottom-right vertex
    glVertex2f(0.0f, -0.95f);  // Bottom-right vertex
    glEnd();  // End drawing filled triangle

    glLineWidth(4.0f);
    glColor3f(0.57f, 0.31f, 0.23f);
    glBegin(GL_LINES);
    glVertex2f(-0.35f, 0.1f);
    glVertex2f(-0.35f, -0.6f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.0f, -0.95f);
    glVertex2f(-0.35f, -0.6f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.35f, -0.6f);
    glVertex2f(0.0f, -0.95f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.35f, -0.6f);
    glVertex2f(0.35f, 0.1f);
    glEnd();

    // Define the center and radius of the circle
    GLfloat centerX = 0.0f;
    GLfloat centerY = 0.4f;
    GLfloat radius = 0.5f;
    int numSegments = 180; // Number of line segments used to approximate the circle

    glLineWidth(6.0f);
    for (int k = 0;k < 6;k++) {
        if (k == 0) {
            glColor3f(0.8f, 0.8f, 0.8f);
            radius = 0.5;
        }
        else if (k == 1) {
            glColor3f(0.3058f, 0.1294f, 0.1215f);
            radius = 0.5 - (0.1 / 6);
        }
        else if (k == 2) {
            glColor3f(0.43529f, 0.21176f, 0.17647f);
            radius = 0.5 - 2 * (0.1 / 6);
        }
        else if (k == 3) {
            glColor3f(0.53f, 0.27f, 0.23137f);
            radius = 0.5 - 3 * (0.1 / 6);
        }
        else if (k == 4) {
            glColor3f(0.57f, 0.31f, 0.23f);
            radius = 0.5 - 4 * (0.1 / 6);
        }
        else if (k == 5) {
            glColor3f(0.59f, 0.32f, 0.24f);
            radius = 0.5 - 5 * (0.1 / 6);
        }
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < numSegments; ++i)
        {
            GLfloat theta = 2.0f * M_PI * static_cast<GLfloat>(i) / static_cast<GLfloat>(numSegments);
            GLfloat x = radius * cos(theta) + centerX;
            GLfloat y = radius * sin(theta) + centerY;
            glVertex2f(x, y);

        }
        glEnd();
    }

    glColor3f(0.9f, 0.9f, 0.9f);  // Set color to red
    glLoadIdentity();  // Reset the current matrix to identity
    glBegin(GL_TRIANGLE_FAN);  // Begin drawing filled circle
    glVertex2f(0.0f, 0.0f);  // Center vertex

    for (int i = 0; i <= numSegments; i++)
    {
        float theta = 2.0f * M_PI * float(i) / float(numSegments);  // Calculate the angle for each segment
        float x = radius * cos(theta);  // Calculate the x-coordinate
        float y = radius * sin(theta) + 0.4;  // Calculate the y-coordinate
        glVertex2f(x, y);  // Add a vertex to the triangle fan
    }
    glEnd();  // End drawing filled circle

    glColor3f(0.0f, 0.0f, 0.0f); // Set color to white
    glPointSize(4.0f); // Set point size
    glBegin(GL_POINTS);
    glVertex2f(0.0f, 0.4f); // Draw a point at (0.5, 0.5)
    glEnd();

    // Draw the markings
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 360; i += 6)
    {
        float t = 0.09;
        if (i % 90 == 0) {
            t = 0.04;
        }

        else if (i % 30 == 0)t = 0.06;

        float angle = i * M_PI / 180.0f;
        float x1 = (0.3f + t) * sin(angle);
        float y1 = (0.3f + t) * cos(angle);
        float x2 = 0.4f * sin(angle);
        float y2 = 0.4f * cos(angle);
        glVertex2f(x1, y1 + 0.4f);
        glVertex2f(x2, y2 + 0.4f);
    }
    glEnd();

    glColor3f(0.1f, 0.1f, 0.1f);
    // Draw the hour hand
    glTranslatef(0.0f, 0.4f, 0.0f); // Move to the center 
    glRotatef(angleHour, 0.0f, 0.0f, 1.0f);
    glLineWidth(3.5f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.2f, 0.0f);
    glEnd();

    // Draw the minute hand
    glLoadIdentity();
    glTranslatef(0.0f, 0.4f, -1.0f);
    glRotatef(angleMinute, 0.0f, 0.0f, 1.0f);
    glLineWidth(2.4f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.26f, 0.0f);
    glEnd();

    // Draw the second hand
    glLoadIdentity();
    glTranslatef(0.0f, 0.4f, -1.0f);
    glRotatef(angleSecond, 0.0f, 0.0f, 1.0f);
    glLineWidth(1.3f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.32f, 0.0f);
    glEnd();


    glLoadIdentity();  // Reset the current matrix to identity
    glTranslatef(0.0f, -0.12f, -1.0f);
    glRotatef(angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);  // Rotate the pendulum bob

    // Draw the pendulum rope
    glLineWidth(3.0f);
    glColor3f(0.55686, 0.30196, 0.117647);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);  // Top of the pendulum rope
    glVertex2f(0.0f, -ropeLength);  // Bottom of the pendulum rope
    glEnd();

    // Draw the pendulum bob
    glColor3f(0.7529, 0.7529, 0.7529);
    glTranslatef(0.0f, -ropeLength, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i++)
    {
        float theta = 2.0f * M_PI * float(i) / float(360);
        float x = bobRadius * cos(theta);
        float y = bobRadius * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();

    // Draw the circle
    glLoadIdentity();
    glTranslatef(0.0f, -0.01f, -1.0f);
    glColor3f(0.59f, 0.32f, 0.24f);
    centerX = 0.0f;
    centerY = -0.1f;
    radius = 0.025f;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY); // Center of the circle

    for (int i = 0; i <= 100; i++)
    {
        float angle = 2.0f * M_PI * float(i) / float(100);
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();


    glFlush(); // Render the line
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Wall Clock");
    glClearColor(0.1f, 0.05f, 0.05f, 0.0f); // Set clear color to black
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0); // Start the update timer immediately
    glutMainLoop();

    return 0;
}
