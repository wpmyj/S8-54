
#include "defines.h"
#include "WelcomeScreen.h"
#include "Math3D.h"
#include "DisplayTypes.h"
#include "Display/Display.h"
#include "Display/Painter.h"

static Point points[8] = {
    {-1.0,  1.0, -1.0},
    { 1.0,  1.0, -1.0},
    { 1.0, -1.0, -1.0},
    {-1.0, -1.0, -1.0},
    {-1.0,  1.0,  1.0},
    { 1.0,  1.0,  1.0},
    { 1.0, -1.0,  1.0},
    {-1.0, -1.0,  1.0}
};

static Edge edges[12] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
    {4, 7}, {4, 5}, {5, 6}, {6, 7}
};

static Mesh mesh = {
    12,
    edges,
    8,
    points
};

static float x = 0.0; 
static float y = 0.0;
static float z = 200.0;

static float scaleX = 1.0f;
static float scaleY = 1.0f;
static float scaleZ = 1.0f;

static float angleX = 0.0;
static float angleY = 0.0;
static float angleZ = 0.0;

void WelcomeScreen_Init(void)
{
    for (int i = 0; i < 8; i++) {
        float k = 50.0f;
        points[i].x *= k;
        points[i].y *= k;
        points[i].z *= k;
    }
}

void WelcomeScreen_Update(void)
{
    Painter_BeginScene(COLOR_WHITE);
    for(int num = 0; num < 12; num++) {
        Point p0 = mesh.points[mesh.edges[num].numPoint0];
        Point p1 = mesh.points[mesh.edges[num].numPoint1];
        Point p0out = p0;
        Point p1out = p1;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Mat4x4 matScale = {0};
        FillMatriceScale(&matScale, scaleX, scaleY, scaleZ);

        TransformPoint(&matScale, &p0, &p0out);
        TransformPoint(&matScale, &p1, &p1out);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Mat4x4 matRotate = {0};
        FillMatriceRotation(&matRotate, angleX, angleY, angleZ);

        Point p0out2 = p0out;
        Point p1out2 = p1out;

        TransformPoint(&matRotate, &p0out, &p0out2);
        TransformPoint(&matRotate, &p1out, &p1out2);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Mat4x4 matMove = {0};
        FillMatriceMov(&matMove, x, y, z);

        Point p0out3 = p0out2;
        Point p1out3 = p1out2;

        TransformPoint(&matMove, &p1out2, &p0out3);
        TransformPoint(&matMove, &p1out2, &p1out3);

        float x0 = 0;
        float x1 = 0;
        float y0 = 0;
        float y1 = 0;
        PresentPointToScreen(&p0out3, &x0, &y0);
        PresentPointToScreen(&p1out3, &x1, &y1);

        Painter_DrawLineC((int)x0, (int)y0, (int)x1, (int)y1, COLOR_WHITE);
    }
    Painter_EndScene();
}
