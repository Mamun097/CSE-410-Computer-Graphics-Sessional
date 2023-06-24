#include <windows.h>  // for MS Windows
#include <GL/glut.h>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);


    glColor3f(1.0f, 1.0f, 1.0f); // Set point color to white
    glPointSize(2.0f); // Set point size to 5 pixels
    glBegin(GL_POINTS);
    glVertex2f(-0.2f, 0.5f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f); // Set line color to white
    glBegin(GL_LINES);
    glVertex2f(-0.5f, 0.0f); // Starting point of the line
    glVertex2f(0.5f, 0.0f);  // Ending point of the line
    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f); // Set line color to red
    glBegin(GL_LINES);
    glVertex2f(0.0f, -0.5f); // Starting point of the line
    glVertex2f(0.0f, 0.5f);  // Ending point of the line
    glEnd();

    glFlush(); // Render the line
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutCreateWindow("OpenGL Line");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Set clear color to black
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
