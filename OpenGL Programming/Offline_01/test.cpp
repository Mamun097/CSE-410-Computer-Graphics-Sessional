/ draw a cylinder from + angle to - angle
void drawCylinder(float angle = M_PI / 2 - acos(1 / sqrt(3)), float radius = 1 / sqrt(3),
    float height = sqrt(2)) // these values come from spec & geometry
{
    float angle_stepsize = 0.1;
    int segments = (int)(angle * 2 / angle_stepsize) + 1;

    height *= (1 - scale);
    radius *= scale;

    // Draw the tube
    glPushMatrix();
    glTranslatef(1 / sqrt(2) * (1 - scale), 0, 0);
    glTranslatef(0, 0, -height / 2);
    glRotatef(-angle * 180 / M_PI, 0, 0, 1);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++)
    {
        float x = radius * cos(i * angle_stepsize);
        float y = radius * sin(i * angle_stepsize);
        float z = 0;
        glVertex3f(x, y, z);
        glVertex3f(x, y, z + height);
    }
    glEnd();
    glPopMatrix();
}