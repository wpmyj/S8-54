#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    float x;
    float y;
    float z;
} Point;

typedef struct
{
    int numPoint0;
    int numPoint1;
} Edge;

typedef struct
{
    int numEdges;
    Edge* edges;
    int numPoints;
    Point* points;
} Mesh;

typedef struct
{
    float n[4][4];
} Mat4x4;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FillMatriceRotation(Mat4x4 *mat, float angleX, float angleY, float angleZ);
void FillMatriceScale(Mat4x4 *mat, float scaleX, float scaleY, float scaleZ);
void FillMatriceMov(Mat4x4 *mat, float movX, float movY, float movZ);
void SetMatriceTransform(Mat4x4 *matTransform, const Mat4x4 *matScale, const Mat4x4 *matRotate, const Mat4x4 *matMove);
void TransformPoint(const Mat4x4 *mat, const Point *pointIn, Point *pointOut);
void PresentPointToScreen(const Point *point, float *x, float *y);
void MatrixMul(const Mat4x4 *mat0, const Mat4x4 *mat1, Mat4x4 *matOut);
