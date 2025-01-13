#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <windows.h>
#include <conio.h>

void ClearScreen()
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
        hStdOut,
        (TCHAR)' ',
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

struct Matrix {

};

struct Vec3 {
    float x, y, z;

    Vec3(float X, float Y, float Z) : x{ X }, y{ Y }, z{ Z } {}
    //Умножение матрицы на вектор, но получается так, что при записи будет вектор умножить на матрицу, а не наоборот как надо. (Все работает, просто запись не правильная)
    Vec3 operator * (float (*matrix)[3]) const
    {
        return Vec3(matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z, 
                    matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z, 
                    matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z);
    }
    Vec3() : x (0.0f), y(0.0f), z(0.0f)
    {

    }
    
};
void projectionVec3(Vec3 projCube[2][4], Vec3 const (*cube)[4], float (*projMatrix)[3]) {
    
    
    for (int i{}; i < 2; i++) {
        for (int j{}; j < 4; j++) {
            projCube[i][j] = projCube[i][j] * projMatrix;//Vec3(cube[i][j].x, cube[i][j].y, 0);
        }
    }
}
void GetRotationMatrix(float matrix[3][3], float angle) {
    float rad = angle * M_PI / 180.0f;
    matrix[0][0] = cos(rad); matrix[0][1] = 0; matrix[0][2] = sin(rad);
    matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0;
    matrix[2][0] = -sin(rad); matrix[2][1] = 0; matrix[2][2] = cos(rad);
   
}
void RotateCube(float (*matrix)[3], Vec3 cube[2][4], Vec3 (*projCube)[4], Vec3& c_vec) {
    for (int i{}; i < 2; i++) {
        for (int j{}; j < 4; j++) {
            cube[i][j].x = cube[i][j].x - c_vec.x;
            cube[i][j].y = cube[i][j].y - c_vec.y;
            cube[i][j].z = cube[i][j].z - c_vec.z;

            projCube[i][j] = cube[i][j] * matrix;

            projCube[i][j].x += c_vec.x; projCube[i][j].y += c_vec.y; projCube[i][j].z += c_vec.z;

            cube[i][j].x = cube[i][j].x + c_vec.x;
            cube[i][j].y = cube[i][j].y + c_vec.y;
            cube[i][j].z = cube[i][j].z + c_vec.z;
        }
    }
}

Vec3 GetCenterVec3(Vec3 (*cube)[4]) {
    float cvec_x{}, cvec_y{}, cvec_z{};
    int vec_count{};
    for (int i{}; i < 2; i++) {
        for (int j{}; j < 4; j++) {
            cvec_x += cube[i][j].x;
            cvec_y += cube[i][j].y;
            cvec_z += cube[i][j].z;
            vec_count++;
        }
    }
    return Vec3(cvec_x/vec_count,cvec_y/vec_count,cvec_z/vec_count);
}

void drawLine(Vec3& vec_1, Vec3& vec_2, HANDLE& handle, COORD& coordinates) {
    //Алгоритм Брезенхема
    int x1 = vec_1.x, x2 = vec_2.x;
    int y1 = vec_1.y, y2 = vec_2.y;

    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        coordinates.X = x1;
        coordinates.Y = y1;
        SetConsoleCursorPosition(handle, coordinates);
        std::cout << '*' << std::ends;

        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void printCube(Vec3 (*projCube)[4], Vec3 edges[12][2]) {
    HANDLE handle;
    COORD coordinates;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    for (int i{}; i < 12; i++) {
        drawLine(edges[i][0],edges[i][1], handle, coordinates);
    }

    for (int i{}; i < 2; i++) {
        for (int j{}; j < 4; j++) {
            coordinates.X =   projCube[i][j].x;
            coordinates.Y = projCube[i][j].y;
            SetConsoleCursorPosition(handle, coordinates);
            std::cout << '#' << std::ends;
        }
    }
    coordinates.X = 0;
    coordinates.Y = 0;
    SetConsoleCursorPosition(handle, coordinates);
    


}

int main() {
    int sizeX = 300, sizeY = 300; //Ширина и высота экрана
    float rotationMatrix[3][3]{}; //Матрица поворота
    float angle{};
    
    Vec3 Cube[2][4]{ 
        {Vec3(30.0f,30.0f - 30.0f,10.0f),Vec3(80.0f,30.0f - 30.0f,10.0f),Vec3(30.0f,50.0f - 30.0f,10.0f),Vec3(80.0f,50.0f - 30.0f,10.0f)}, //Высота и длина куба 20, по Z тоже 20; Нач. координаты куба 30,30,10; Куб состоит по сути из двух сторон, которые и создают его.
        {Vec3(30.0f,30.0f - 30.0f,60.0f),Vec3(80.0f,30.0f - 30.0f,60.0f),Vec3(30.0f,50.0f - 30.0f,60.0f),Vec3(80.0f,50.0f - 30.0f,60.0f)}
    };
    
    Vec3 projectCube[2][4] { }; //Проецированный 2D куб в пространство XY
    float projMatrix[3][3]{ //Матрица проецирования
        {0.5f,0,0},
        {0,0.5f,0},
        {0,0,0}
    };
    Vec3 c_vector{GetCenterVec3(Cube)}; //Вектор хранящий центральные коорд. для куба

    

    while (true) {
        //Очистка, поворот, проецирование, рисовка
        ClearScreen(); //Очистка экрана

        GetRotationMatrix(rotationMatrix, angle); //Получение матрицы поворота в зависимости от угла, поворот вершин куба
        RotateCube(rotationMatrix, Cube, projectCube, c_vector);

        projectionVec3(projectCube, Cube, projMatrix); //Проецирование

        //Ребра куба
        Vec3 vec_pairs[12][2]{
            {projectCube[0][0], projectCube[0][1]},{projectCube[0][2],projectCube[0][3]}, {projectCube[0][0],projectCube[0][2]}, {projectCube[0][1],projectCube[0][3]}, //Связь одной стороны куба, передней части
            {projectCube[1][0], projectCube[1][1]},{projectCube[1][2],projectCube[1][3]}, {projectCube[1][0],projectCube[1][2]}, {projectCube[1][1],projectCube[1][3]}, //Связь одной стороны куба, задней части
            {projectCube[0][0], projectCube[1][0]},{projectCube[0][1],projectCube[1][1]}, {projectCube[0][2],projectCube[1][2]}, {projectCube[0][3],projectCube[1][3]}  //Связи двух сторон куба
        };

        printCube(projectCube, vec_pairs); //Рисовка

        
        angle += 0.6f;
        if (angle >= 360.0) angle = 0.0f;
        Sleep(32);
        //Добавить вращение не только по оси Y, но и Z
    }
    return 0;
}