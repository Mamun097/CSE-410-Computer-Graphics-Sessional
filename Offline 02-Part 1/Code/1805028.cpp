#include<iostream>
#include<fstream>
#include<iomanip>
#include<cmath>
#include<string>
#include<cstdlib>
#include<stack>
#include<ctime>
#include<limits>


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
    int redValue;
    int greenValue;
    int blueValue;
};

struct Triangle {
    Point corners[3];
    Color rgb;
};


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

    int triangleCount=0;

    while(true) {
        in >> command;

        if (command.compare("triangle") == 0)
        {
            triangleCount++;
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

    for(int i=0; i<triangleCount ; i++) {
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

    for(int i=0; i<triangleCount ; i++) {
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


    return 0;
}