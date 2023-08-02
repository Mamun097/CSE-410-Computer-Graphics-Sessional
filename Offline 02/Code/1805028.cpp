#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

struct Point {
    double x, y, z;
};

class Matrix{
    public:
        int row, col;
        vector<vector<double>> matrix;
        Matrix(int r, int c){
            row = r;
            col = c;
            matrix.resize(row);
            for(int i=0; i<row; i++){
                matrix[i].resize(col);
            }
        }
        void print(){
            for(int i=0; i<row; i++){
                for(int j=0; j<col; j++){
                    cout << matrix[i][j] << " ";
                }
                cout << endl;
            }
        }

        void set(int r, int c, double val){
            matrix[r][c] = val;
        }

        double get(int r, int c){
            return matrix[r][c];
        }

        Matrix operator*(Matrix &m){
            Matrix res(row, m.col);
            for(int i=0; i<row; i++){
                for(int j=0; j<m.col; j++){
                    double sum = 0;
                    for(int k=0; k<col; k++){
                        sum += matrix[i][k] * m.matrix[k][j];
                    }
                    res.matrix[i][j] = sum;
                }
            }
            return res;
        }
};

vector<double> splitStrings(string str, char dl)
{
    string word = "";
     int num = 0;
    str = str + dl;
    int l = str.size();
     vector<string> substr_list;
    for (int i = 0; i < l; i++) {
        if (str[i] != dl)
            word = word + str[i];
 
        else {
            if ((int)word.size() != 0)
                substr_list.push_back(word);
             word = "";
        }
    }

    vector<double> res;
    for(int i=0; i<substr_list.size(); i++){
        res.push_back(stod(substr_list[i]));
    }
    return res;
}


// Function to apply transformation to a point
vector<double> transformPoint(vector<double> P, Matrix M) {
    vector<double> res;
    res.push_back(P[0]*M.get(0, 0) + P[1]*M.get(0, 1) + P[2]*M.get(0, 2) + M.get(0, 3));
    res.push_back(P[0]*M.get(1, 0) + P[1]*M.get(1, 1) + P[2]*M.get(1, 2) + M.get(1, 3));
    res.push_back(P[0]*M.get(2, 0) + P[1]*M.get(2, 1) + P[2]*M.get(2, 2) + M.get(2, 3));
    return res;
}

struct Vector3D {
    double x, y, z;

    Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    // Normalize the vector (convert to a unit vector)
    void normalize() {
        double length = sqrt(x * x + y * y + z * z);
        x /= length;
        y /= length;
        z /= length;
    }

    // Overloaded dot product operator
    double operator*(const Vector3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Overloaded cross product operator
    Vector3D operator^(const Vector3D& other) const {
        return Vector3D(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    // Overloaded multiplication by scalar operator
    Vector3D operator*(double scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    // Overloaded addition operator
    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    // Overloaded subtraction operator
    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
};

// Function to perform the vector form of Rodrigues formula for rotation
Vector3D rodriguesRotation(Vector3D x, Vector3D a, double angle) {
    Vector3D x1 = x * cos(angle);
    Vector3D x2 = a * (x * a) * (1 - cos(angle));
    Vector3D x3 = a ^ x * sin(angle);
    return x1 + x2 + x3;
}

// Function to generate the rotation matrix
void generateRotationMatrix(double rotationMatrix[4][4], Vector3D axis, double angle) {
    Vector3D i(1, 0, 0);
    Vector3D j(0, 1, 0);
    Vector3D k(0, 0, 1);

    Vector3D c1 = rodriguesRotation(i, axis, angle);
    Vector3D c2 = rodriguesRotation(j, axis, angle);
    Vector3D c3 = rodriguesRotation(k, axis, angle);

    // Fill in the rotation matrix
    rotationMatrix[0][0] = c1.x; rotationMatrix[0][1] = c2.x; rotationMatrix[0][2] = c3.x; rotationMatrix[0][3] = 0;
    rotationMatrix[1][0] = c1.y; rotationMatrix[1][1] = c2.y; rotationMatrix[1][2] = c3.y; rotationMatrix[1][3] = 0;
    rotationMatrix[2][0] = c1.z; rotationMatrix[2][1] = c2.z; rotationMatrix[2][2] = c3.z; rotationMatrix[2][3] = 0;
    rotationMatrix[3][0] = 0;    rotationMatrix[3][1] = 0;    rotationMatrix[3][2] = 0;    rotationMatrix[3][3] = 1;
}

// Function to generate the view transformation matrix V
void generateViewMatrix(double viewMatrix[4][4], Vector3D eye, Vector3D look, Vector3D up) {
    // Calculate the mutually perpendicular unit vectors l, r, and u
    Vector3D l = look - eye;
    l.normalize();
    Vector3D r = up ^ l;
    r.normalize();
    Vector3D u = r ^ l;

    // Apply translation T to move the eye/camera to origin
    double translationMatrix[4][4] = {
        {1, 0, 0, -eye.x},
        {0, 1, 0, -eye.y},
        {0, 0, 1, -eye.z},
        {0, 0, 0, 1}
    };

    // Apply rotation R to align l, r, and u with the principal axes
    double rotationMatrix[4][4] = {
        {r.x, r.y, r.z, 0},
        {u.x, u.y, u.z, 0},
        {-l.x, -l.y, -l.z, 0},
        {0, 0, 0, 1}
    };

    // Calculate the view transformation matrix V=RT
    double temp[4][4];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                temp[i][j] += rotationMatrix[i][k] * translationMatrix[k][j];
            }
        }
    }

    // Copy the result to the viewMatrix
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            viewMatrix[i][j] = temp[i][j];
        }
    }
}

int main(){
    //read txt line by line
    std::ifstream inputFile("scene.txt");
    string line;
    vector<string> lines;
    while (std::getline(inputFile, line)){
        lines.push_back(line);
    }
    inputFile.close();

    //create output file named stage1.txt
    ofstream outputFile;        
    outputFile.open("stage1.txt");
    
    //initalize empty stack of matrices
    vector<Matrix> S;

    //initialize 4x4 identity matrix
    Matrix M(4, 4);
    for(int i=0; i<4; i++){
        M.set(i, i, 1);
    }

    for(int i=0;i<lines.size();i++){
        string line = lines[i];

        if(line.compare("triangle") == 0){
            //read next 3 lines
            for(int j=0; j<3; j++){
                string str=lines[i+j+1];
                //split points by space
                vector<double>point= splitStrings(str, ' ');
                vector<double>point_prime = transformPoint(point, M);
                //print point till 7 decimal places
                outputFile << fixed;
                outputFile.precision(7);
                outputFile << point_prime[0] << " " << point_prime[1] << " " << point_prime[2] << endl;
                
            }
            i+=3;
            outputFile << endl;
        }

        else if(line.compare("translate")==0){
            string str=lines[i+1];
            vector<double>point= splitStrings(str, ' ');
            Matrix T(4, 4);
            for(int i=0; i<4; i++){
                T.set(i, i, 1);
            }
            T.set(0, 3, point[0]);
            T.set(1, 3, point[1]);
            T.set(2, 3, point[2]);
            M = M * T;
            i+=1;
        }

        else if(line.compare("scale")==0){
            string str=lines[i+1];
            vector<double>point= splitStrings(str, ' ');
            Matrix S(4, 4);
            for(int i=0; i<4; i++){
                S.set(i, i, 1);
            }
            S.set(0, 0, point[0]);
            S.set(1, 1, point[1]);
            S.set(2, 2, point[2]);
            M = M * S;
            i+=1;
        }

        else if(line.compare("rotate")==0){
            //read next line
            string str=lines[i+1];
            //split points by space
            vector<double>point= splitStrings(str, ' ');
            //create rotation matrix
            Matrix R(4, 4);
            for(int i=0; i<4; i++){
                R.set(i, i, 1);
            }
            //convert degrees to radians
            double angle = point[3] * M_PI / 180;
            //generate rotation matrix
            double rotationMatrix[4][4];
            generateRotationMatrix(rotationMatrix, Vector3D(point[0], point[1], point[2]), angle);
            //fill in the rotation matrix
            for(int i=0; i<4; i++){
                for(int j=0; j<4; j++){
                    R.set(i, j, rotationMatrix[i][j]);
                }
            }
            M = M * R;
            i+=1;
        }

        else if(line.compare("push")==0){
            S.push_back(M);
        }

        else if(line.compare("pop")==0){
            M = S.back();
            S.pop_back();
        }

        else if(line.compare("end")==0){
            break;
        }
    }
    outputFile.close();

    //View Transformation
    std::ifstream inputFile2("stage1.txt");
    string line2;
    vector<string> lines2;
    while (std::getline(inputFile2, line2)){
        lines2.push_back(line2);
    }
    inputFile2.close();

    ofstream outputFile2;
    outputFile2.open("stage2.txt");

    vector<double> eye = splitStrings(lines[0], ' ');
    vector<double> look = splitStrings(lines[1], ' ');
    vector<double> up = splitStrings(lines[2], ' ');
    vector<double> line4 = splitStrings(lines[3], ' ');

    //calculate view matrix
    double viewMatrix[4][4];
    Vector3D eyeVector(eye[0], eye[1], eye[2]);
    Vector3D lookVector(look[0], look[1], look[2]);
    Vector3D upVector(up[0], up[1], up[2]);
    generateViewMatrix(viewMatrix, eyeVector, lookVector, upVector);

    Matrix P(4, 1);
    P.set(3, 0, 1);

    for(int i=0; i<lines2.size(); i++){
        //read each line
        string line = lines2[i];
        if(line.empty()){

            outputFile2<<endl;
            continue;
        }
        vector<double>point= splitStrings(line, ' ');
        P.set(0, 0, point[0]);
        P.set(1, 0, point[1]);
        P.set(2, 0, point[2]);

        //multiply with view matrix
        Matrix V(4, 4);
        for(int j=0; j<4; j++){ 
            for(int k=0; k<4; k++){
                V.set(j, k, viewMatrix[j][k]);
            }
        }
        Matrix P_prime = V*P;

        //write to file
        //print point till 7 decimal places
        outputFile2 << fixed;
        outputFile2.precision(7);
        outputFile2 << P_prime.get(0, 0) << " " << P_prime.get(1,0) << " " << P_prime.get(2,0) << endl;
    }
    outputFile2.close();

    //Projection Transformation
    std::ifstream inputFile3("stage2.txt");
    string line3;
    vector<string> lines3;
    while (std::getline(inputFile3, line3)){
        lines3.push_back(line3);
    }
    inputFile3.close();

    ofstream outputFile3;
    outputFile3.open("stage3.txt");

    //calculate projection matrix
    double fovY = line4[0];
    double aspectRatio = line4[1];
    double near = line4[2];
    double far = line4[3];

    double fovX = fovY * aspectRatio;
    double t = near * tan(fovY/2 * M_PI / 180);
    double r = near * tan(fovX/2 * M_PI / 180);

    double projectionMatrix[4][4]={
        near/r, 0, 0, 0,
        0, near/t, 0, 0,
        0, 0, -(far+near)/(far-near), -(2*far*near)/(far-near),
        0, 0, -1, 0
    };

    Matrix Projection(4, 4);
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            Projection.set(i, j, projectionMatrix[i][j]);
        }
    }

     for(int i=0; i<lines3.size(); i++){
        string line = lines3[i];
        if(line.empty()){

            outputFile3<<endl;
            continue;
        }
        vector<double>point= splitStrings(line, ' ');
        P.set(0, 0, point[0]);
        P.set(1, 0, point[1]);
        P.set(2, 0, point[2]);

        Matrix P_prime = Projection*P;

        //write to file
        //print point till 7 decimal places
        outputFile3 << fixed;
        outputFile3.precision(7);
        outputFile3 << P_prime.get(0, 0) << " " << P_prime.get(1,0) << " " << P_prime.get(2,0) << endl;
    }
    outputFile3.close();
    
    return 0;
}