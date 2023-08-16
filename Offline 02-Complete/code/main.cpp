#include<iostream>
#include<fstream>
#include<iomanip>
#include<cmath>
#include<string>
#include<cstdlib>
#include<stack>
#include<ctime>
#include<limits>
#include "bitmap_image.hpp"
using namespace std;

#define INF numeric_limits<double>::infinity()

class Point {
public:
    double x, y, z, w;
    Point() {
        w = 1.0, x = y = z = 0.0;
    }
    Point(double x, double y, double z, double w){
        this->w = w, this->x = x, this->y = y, this->z = z;
    }
    Point(double x, double y, double z) {
        w = 1.0, this->x = x, this->y = y, this->z = z;
    }

    friend ostream& operator<<(ostream &out, Point &point){
        out << '[' << point.x << ", " << point.y << ", " << point.z << ", " << point.w << ']';
        return out;
    }

    friend ifstream& operator>>(ifstream &in, Point &point) {
        in >> point.x >> point.y >> point.z;
        return in;
    }

    friend ofstream& operator<<(ofstream &out, Point &point) {
        out << fixed << setprecision(7) << point.x << ' ' << point.y << ' '  << point.z;
        return out;
    }

    Point operator^(const Point p) {
        return Point(y*p.z-z*p.y, z*p.x-x*p.z, x*p.y-y*p.x);
    }
    Point operator-(const Point p){
        return Point(x-p.x, y-p.y, z-p.z);
    }
    Point operator+(const Point p){
        return Point(x+p.x, y+p.y, z+p.z);
    }
    double operator*(const Point p){
        return x*p.x + y*p.y + z*p.z;
    }
        Point operator*(const double s){
        return Point(x*s, y*s, z*s);
    }
    void scale(){
        x /= w;
        y /= w;
        z /= w;
        w = 1.0;
    }
    void normalize(){
        double norm = sqrt(x*x + y*y + z*z);
        x /= norm;
        y /= norm;
        z /= norm;
    }
};

class Matrix {
    public:
    double M[4][4];

    Matrix() {
        for(int i=0; i<4; i++)
            for (int j = 0; j < 4; j++){
                if(i==j) M[i][j] = 1.0;
                else M[i][j] = 0.0;
            }               
    }

    Point RodriguesRotation(Point x, Point a, double theta){
        Point x1 = x * cos(theta * (2.0 * acos(0.0)) / 180.0);
        Point x2 = a * (a * x) * (1 - cos(theta * (2.0 * acos(0.0)) / 180.0));
        Point x3 = (a ^ x) * sin(theta * (2.0 * acos(0.0)) / 180.0);
        return x1 + x2 + x3;
    }

    void multiplyMatrices(Matrix matrix){
        double temp_matrix[4][4];
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                temp_matrix[i][j] = 0.0;
                for(int k=0; k<4; k++){
                    temp_matrix[i][j] += M[i][k] * matrix.M[k][j];
                }
            }
        }
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                M[i][j] = temp_matrix[i][j];
            }
        }
    
    }

    void TranslationMatrix(double tx, double ty, double tz){
        M[0][3] = tx;
        M[1][3] = ty;
        M[2][3] = tz;
    }
    void ScalingMatrix(double sx, double sy, double sz){
        M[0][0] = sx;
        M[1][1] = sy;
        M[2][2] = sz;
    }
    void RotationMatrix(double angle, double ax, double ay, double az){
        Point a(ax, ay, az);
        a.normalize();

        Point c1 = RodriguesRotation(Point(1.0, 0.0, 0.0), a, angle);
        Point c2 = RodriguesRotation(Point(0.0, 1.0, 0.0), a, angle);
        Point c3 = RodriguesRotation(Point(0.0, 0.0, 1.0), a, angle);

        //R
        M[0][0] = c1.x;
        M[1][0] = c1.y;
        M[2][0] = c1.z;

        M[0][1] = c2.x;
        M[1][1] = c2.y;
        M[2][1] = c2.z;

        M[0][2] = c3.x;
        M[1][2] = c3.y;
        M[2][2] = c3.z;
    }

    void ViewMatrix(Point eye, Point look, Point up){
        Point l = look - eye;
        l.normalize();
        Point r = l ^ up;
        r.normalize();
        Point u = r ^ l;

        //R
        M[0][0] = r.x;
        M[0][1] = r.y;
        M[0][2] = r.z;

        M[1][0] = u.x;
        M[1][1] = u.y;
        M[1][2] = u.z;

        M[2][0] = -l.x;
        M[2][1] = -l.y;
        M[2][2] = -l.z;

        //T
        Matrix T;
        T.TranslationMatrix(-eye.x, -eye.y, -eye.z);

        //V = R * T
        multiplyMatrices(T);
    }

    void ProjectionMatrix(double fovY, double aspectRatio, double near, double far){
        /* determining parameters fovX, r & t */
        double fovX = fovY * aspectRatio;
        double r = near * tan(fovX / 2.0 * (2.0 * acos(0.0)) / 180.0);
        double t = near * tan(fovY / 2.0 * (2.0 * acos(0.0)) / 180.0);

        /* generating corresponding projection matrix P */
        M[0][0] = near / r;
        M[1][1] = near / t;
        M[2][2] = -(far + near) / (far - near);
        M[2][3] = -(2.0 * far * near) / (far - near);
        M[3][2] = -1.0;
        M[3][3] = 0.0;
    }

    Matrix operator*(const Matrix matrix){
        Matrix temp;
        temp.multiplyMatrices(*this);
        temp.multiplyMatrices(matrix);
        return temp;
    }

    Point operator*(const Point point){
        double temp[4];
        for (int i = 0; i < 4; i++)
        {
            temp[i] = 0.0;

            for (int j = 0; j < 4; j++)
            {
                if (j == 0)
                    temp[i] += M[i][j] * point.x;
                else if (j == 1)
                    temp[i] += M[i][j] * point.y;
                else if (j == 2)
                    temp[i] += M[i][j] * point.z;
                else
                    temp[i] += M[i][j] * point.w;
            }
        }
        return Point(temp[0], temp[1], temp[2], temp[3]);
    }
};

struct Color {
    int r;
    int g;
    int b;
};

struct Triangle {
    Point corners[3];
    Color color;
};

static unsigned long int g_seed = 1; 
inline int Random(){ 
    g_seed = (214013 * g_seed + 2531011); 
    return (g_seed >> 16) & 0x7FFF; 
}


int main(){
    ifstream in;
    ofstream out;
    in.open("scene.txt");

    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    in >> eyeX >> eyeY >> eyeZ;
    in >> lookX >> lookY >> lookZ;
    in >> upX >> upY >> upZ;
    in >> fovY >> aspectRatio >> near >> far;


    /* stage1: modeling transformation by analyzing display code */
    string command;

    stack<Matrix> Stack;
    Stack.push(Matrix());

    out.open("stage1.txt");

    int total_tr=0;

    while(true) {
        in >> command;

        if (command.compare("triangle") == 0)
        {
            total_tr++;
            Point points[3];

            for (int i = 0; i < 3; i++)
            {
                double x, y, z;
                in >> x >> y >> z;
                points[i] = Point(x, y, z);
            }

            for(int i=0; i<3; i++){
                points[i] = Stack.top() * points[i];
                points[i].scale();
            }

            for(int i=0; i<3; i++){
                out << points[i] << endl;
            }
            out << endl;
        }
        else if (command.compare("translate") == 0)
        {
            double tx, ty, tz;
            in >> tx >> ty >> tz;

            Matrix matrix;
            matrix.TranslationMatrix(tx, ty, tz);

            Matrix temp = Stack.top() * matrix;
            Stack.pop();
            Stack.push(temp);
        }
        else if (command.compare("scale") == 0)
        {
            double sx, sy, sz;
            in >> sx >> sy >> sz;

            Matrix matrix;
            matrix.ScalingMatrix(sx, sy, sz);

            Matrix temp = Stack.top() * matrix;
            Stack.pop();
            Stack.push(temp);
        }
        else if (command.compare("rotate") == 0)
        {
            double angle, ax, ay, az;
            in >> angle >> ax >> ay >> az;

            Matrix matrix;
            matrix.RotationMatrix(angle, ax, ay, az);

            Matrix temp = Stack.top() * matrix;
            Stack.pop();
            Stack.push(temp);
        }
        else if (command.compare("push") == 0)
        {
            Matrix temp;
            temp = temp * Stack.top();
            Stack.push(temp);
        }
        else if (command.compare("pop") == 0)
        {
            Stack.pop();
        }
        else if (command.compare("end") == 0)
        {
            break;
        }
    }
    in.close();
    out.close();

    //stage2: viewing transformation
    in.open("stage1.txt");
    out.open("stage2.txt");

    Matrix matrix;
    Point eye(eyeX, eyeY, eyeZ);
    Point look(lookX, lookY, lookZ);
    Point up(upX, upY, upZ);
    matrix.ViewMatrix(eye, look, up);

    for(int i=0; i<total_tr ; i++) {
        Point points[3];

        for (int j = 0; j < 3; j++) {
            in >> points[j];
        }

        for (int j = 0; j < 3; j++) {
            points[j] = matrix * points[j];
            points[j].scale();
        }

        for (int j = 0; j < 3; j++) {
            out << points[j] << endl;
        }
        out << endl;
    }
    in.close();
    out.close();

    //stage3: projection transformation
    in.open("stage2.txt");
    out.open("stage3.txt");

    Matrix matrix2;
    matrix2.ProjectionMatrix(fovY, aspectRatio, near, far);

    for(int i=0; i<total_tr ; i++) {
        Point points[3];

        for (int j = 0; j < 3; j++) {
            in >> points[j];
        }

        for (int j = 0; j < 3; j++) {
            points[j] = matrix2 * points[j];
            points[j].scale();
        }

        for (int j = 0; j < 3; j++) {
            out << points[j] << endl;
        }
        out << endl;
    }

    in.close();
    out.close();


    //stage4:

    in.open("config.txt");
    int width, height;
    double x_left= -1.0, x_right= 1.0, y_bottom= -1.0, y_top=1.0, z_near= -1.0, z_far= 1.0;

    in >> width >> height;
    in.close();

    //code not running for 500x500, don't know why
    //that's why using 300x300
    width=300;
    height=300;

    double dx, dy, topY, bottomY, leftX, rightX;

    dx = (x_right - x_left)/width;
    dy = (y_top - y_bottom)/height;
    topY = y_top - dy/2.0;
    bottomY = y_bottom + dy/2.0;
    leftX = x_left + dx/2.0;
    rightX = x_right - dx/2.0;

    double z_buffer[height][width];
    for(int j=0; j<height; j++) {
        for(int k=0; k<width; k++) {
            z_buffer[j][k] = z_far;
        }
    }

    Color f_buffer[height][width];

    for(int j=0; j<height; j++) {
        for(int k=0; k<width; k++) {
            f_buffer[j][k].r = 0;
            f_buffer[j][k].g = 0;
            f_buffer[j][k].b = 0;
        }
    }

    in.open("stage3.txt");
    Triangle triangles[total_tr];

    for(int i=0; i<total_tr; i++) {
        for(int j=0; j<3; j++) {
            in >> triangles[i].corners[j];
        }

        triangles[i].color.r = Random();
        triangles[i].color.g = Random();
        triangles[i].color.b = Random();
    }
    in.close();

    for(int i=0; i<total_tr; i++) {
        double maxY = max(triangles[i].corners[0].y, max(triangles[i].corners[1].y, triangles[i].corners[2].y));
        double minY = min(triangles[i].corners[0].y, min(triangles[i].corners[1].y, triangles[i].corners[2].y));


        int top_scanline, bottom_scanline;
        top_scanline = (maxY >= y_top) ? 0 : (int) round((y_top - maxY) / dy);
        bottom_scanline = (minY <= y_bottom) ? height - 1 : (height - 1) - ((int) round((minY - y_bottom) / dy));


        for(int j=top_scanline; j<=bottom_scanline; j++) {
            double ys = topY - j*dy;

            Point intersection[3];
            intersection[0] = Point(INF, ys, 0, 1);
            intersection[1] = Point(INF, ys, 1, 2);
            intersection[2] = Point(INF, ys, 2, 0);

            for(int j=0; j<3; j++) {
                Point point1 = triangles[i].corners[(int) intersection[j].z];
                Point point2 = triangles[i].corners[(int) intersection[j].w];

                if(point1.y != point2.y) {
                    intersection[j].x= (point1.x + (ys - point1.y)*(point1.x - point2.x)/(point1.y - point2.y));
                }
            }

            for(int j=0; j<3; j++) {
                Point point1 = triangles[i].corners[(int) intersection[j].z];
                Point point2 = triangles[i].corners[(int) intersection[j].w];

                if(intersection[j].x != INF) {
                    if(intersection[j].x>max(point1.x, point2.x) || intersection[j].x<min(point1.x, point2.x) || intersection[j].y>max(point1.y, point2.y) || intersection[j].y<min(point1.y, point2.y)) {
                        intersection[j].x= INF;
                    }
                }
            }

            int maxIndex= -1, minIndex= -1;
            double maxX, minX;

            for(int j=0; j<3; j++) {
                if(maxIndex==-1 && minIndex==-1) {
                    if(intersection[j].x != INF) {
                        maxIndex = minIndex = j;
                        maxX = minX = intersection[j].x;
                    }
                } else {
                    if(intersection[j].x != INF) {
                        if(intersection[j].x < minX) {
                            minIndex = j;
                            minX = intersection[j].x;
                        }
                        if(intersection[j].x > maxX) {
                            maxIndex = j;
                            maxX = intersection[j].x;
                        }
                    }
                }
            }


            int left_intersection, right_intersection;
            left_intersection = (intersection[minIndex].x <= leftX) ? 0 : (int) round((intersection[minIndex].x - leftX) / dx);
            right_intersection = (intersection[maxIndex].x >= rightX) ? (width - 1) : (width - 1) - ((int) round((rightX - intersection[maxIndex].x) / dx));


            Point point1 = triangles[i].corners[(int) intersection[minIndex].z];
            Point point2 = triangles[i].corners[(int) intersection[minIndex].w];

            double za = point1.z + (intersection[minIndex].y - point1.y)*(point2.z - point1.z)/(point2.y - point1.y);

            point1 = triangles[i].corners[(int) intersection[maxIndex].z];
            point2 = triangles[i].corners[(int) intersection[maxIndex].w];

            double zb = point1.z + (intersection[maxIndex].y - point1.y)*(point2.z - point1.z)/(point2.y - point1.y);

            double z_p;
            double temp = dx*(zb - za)/(intersection[maxIndex].x - intersection[minIndex].x);

            for(int k=left_intersection; k<=right_intersection; k++) {
                if(k == left_intersection) {
                    z_p = za + ((leftX + left_intersection*dx) - intersection[minIndex].x)*(zb - za)/(intersection[maxIndex].x- intersection[minIndex].x);
                } else {
                    z_p = z_p + temp;
                }

                if(z_p>z_near && z_p<z_buffer[j][k]) {
                    z_buffer[j][k] = z_p;

                    f_buffer[j][k].r = triangles[i].color.r;
                    f_buffer[j][k].g = triangles[i].color.g;
                    f_buffer[j][k].b = triangles[i].color.b;
                }
            }
        }
    }

    out.open("./z_buffer.txt");
    for(int j=0; j<height; j++) {
        for(int k=0; k<width; k++) {
            if(z_buffer[j][k] < z_far) {
                out << z_buffer[j][k] << '\t';
            }
        }
        out << endl;
    }
    out.close();

    bitmap_image bitmapImage(width, height);
    for(int j=0; j<height; j++) {
        for(int k=0; k<width; k++) {
            bitmapImage.set_pixel(k, j, f_buffer[j][k].r, f_buffer[j][k].g, f_buffer[j][k].b);
        }
    }
    bitmapImage.save_image("./out.bmp");

    return 0;

}