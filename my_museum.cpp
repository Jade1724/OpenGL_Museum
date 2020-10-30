//  ===================================================================
//  COSC363 Assignment1 - OpenGL Museum
//  ===================================================================

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <climits>
#include <math.h>
#include <GL/freeglut.h>
#include "loadTGA.h"

#define PI 3.141592
#define GA 9.8

//--Globals--------------------------------
int cam_height = 10;
float angle=0, look_x=0.0, look_y=10.0, look_z=-1.0, eye_x=0.0, eye_y=10.0, eye_z=0.0;
double DegreeToRadian = 3.14159/180.0;
int nvert, nface;                   //total number of vertices and faces
float *x, *y, *z;                   //vertex coordinates
int *nv, *t1, *t2, *t3, *t4;        //number of vertices and vertex indices of each face
GLuint txId[8];   //Texture ids

float crankAngle, rodAngle, rodPosition, pistonHeight;       // globals for piston-crankshaft engine model
const float crankRadius=2.0, rodLength = 7.5;
const float omega=(rodLength/crankRadius)*asin(crankRadius/rodLength);

float fanAngle;         // Ceiling fan

float buoy_x, buoy_y;   // lifebuoy displacement
int totalTime;
//float buoy_xArray[40], buoy_yArray[40];

float white[] = {1., 1., 1., 1.};      // colour vectors
float black[] = {0., 0., 0.};
float red[] = {1., 0., 0.};
float grey[] = {0.57, 0.64, 0.69};
float brown[] = {0.6, 0.46, 0.33};
float lightGreen[] = {0.48, 0.71, 0.38};
float silver[] = {0.52, 0.52, 0.51};

float gx=0.0, gy=20.0, gz=-75;           // light position;
float shadowMat[16] = { gy,0,0,0, -gx,0,-gz,-1,
0,0,gy,0, 0,0,0,gy };

//--Modeling fucntion prototype declaration--------------------
void detectCollision(void);
void drawDeck(void);
void drawBuilding(void);
void skybox(void);
void drawEngineRoom(void);
void allocateStairs(void);
void allocateFence(void);
void drawEngine(void);
void allocateTablesChairs(void);
void drawCeilingFan(void);
void drawHelm(void);
void drawControls(void);
void throwBuoy(void);
void putTextures(void);
void normal(float x1, float y1, float z1,
            float x2, float y2, float z2,
              float x3, float y3, float z3 );

//--Load textures-------------------------
void loadTexture()
{
    glGenTextures(8, txId);

    glBindTexture(GL_TEXTURE_2D, txId[0]);
    loadTGA("image/ft.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture
    loadTGA("image/up.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[2]);  //Use this texture
    loadTGA("image/dn.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[3]);  //Use this texture
    loadTGA("image/rt.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[4]);  //Use this texture
    loadTGA("image/bk.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[5]);  //Use this texture
    loadTGA("image/lt.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[6]);  //Use this texture
    loadTGA("image/controlPanel.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[7]);  //Use this texture
    loadTGA("image/chairTexture.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}


void myTimer(int value)
{
    float delta = omega / 20.0;     // 20.0 cycle per second timer
    float crankAngleRad;

    float theta = PI/6.0;
    float v0 = 20.0;
    float t;

    crankAngle += (delta * 180/PI);
    crankAngleRad = crankAngle * PI/180;
    rodAngle = (crankRadius*omega/rodLength) * sin(crankAngleRad) * 180.0/PI;
    rodPosition = crankRadius*cos(crankAngleRad) + sqrt(pow(rodLength, 2.0) - pow(crankRadius, 2.0)*pow(sin(crankAngleRad), 2.0));
    pistonHeight = rodPosition + 6.0;

    fanAngle++;

    totalTime++;
    t = (totalTime % 100) / 20.0;
    buoy_x = v0 * t * cos(theta);
    buoy_y = -0.5*GA*pow(t, 2.0) + v0*t*sin(theta);

    glutPostRedisplay();
    glutTimerFunc(50, myTimer, 0);
}


void special(int key, int x, int y)
{
    detectCollision();

    if(key == GLUT_KEY_LEFT) angle -= 0.1;  //Change direction
    else if(key == GLUT_KEY_RIGHT) angle += 0.1;
    else if(key == GLUT_KEY_DOWN)
    {  //Move backward
        eye_x -= sin(angle);
        eye_z += cos(angle);
    }
    else if(key == GLUT_KEY_UP)
    { //Move forward
        eye_x += sin(angle);
        eye_z -= cos(angle);
    }

    look_x = eye_x + 100*sin(angle);
    look_z = eye_z - 100*cos(angle);

    glutPostRedisplay();
}


//--Draw models in the scene--------------
void drawModel(void)
{
    skybox();
    drawBuilding();
    allocateTablesChairs();
    drawControls();

    glPushMatrix();
    drawDeck();
    drawEngineRoom();
    allocateStairs();
    allocateFence();
    drawEngine();
    //allocateTablesChairs();
    drawCeilingFan();
    //drawControls();
    throwBuoy();
    glPopMatrix();

}



//--Display function------------------
void display(void)
{
    float light_pos[4] = {gx, gy, gz, 1.0};

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye_x, eye_y, eye_z,  look_x, look_y, look_z,   0, 1, 0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    drawModel();

    glutSwapBuffers();

}

//--------------------------------------
void initialize(void)
{
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    loadTexture();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 10.0, 1000.0);

}


//----------------------------------------
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(300, 50);
    glutCreateWindow ("OpenGL Museum");
    initialize();

    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutTimerFunc(50, myTimer, 0);
    glutMainLoop();
    return 0;
}

void detectCollision(void)
{
    if (eye_y == 10.0) {            // Initial floor
        if ((-30.0 <= eye_z  && eye_z <= 25.0) && (50.0 >= eye_x && eye_x >= 7.5)) {
                if (eye_z > 15.0) eye_z = 15.0;
                if (eye_x > 40.0) eye_x = 40.0;
                if (eye_z < -20.0) eye_z = -20.0;
            }
        else if  ((-30.0 <= eye_z && eye_z <= 25.0) && (7.5 > eye_x && eye_x >= -7.5)) {
                if (eye_z > 15.0) eye_z = 15.0;
        }
        else if ((-30.0 <= eye_z && eye_z <= 25.0) && (-7.5 >= eye_x && eye_x >= -50.0)) {
                if (eye_z > 15.0) eye_z = 15.0;
                if (eye_x < -40.0) eye_x = -40.0;
                if (eye_z < -20.0) eye_z = -20.0;
                if (eye_x <= -19.0 && eye_x >= -31.0) {
                    if (eye_z > 10.0) {
                        eye_y = 40.0;
                        eye_x = -30.0;
                        eye_z = 42.0;
                        look_y = 40.0;
                    }
                }
            }
        else if (-30.0 >= eye_z && eye_z > -102.0) {
            if (eye_x > 7.0) eye_x = 7.0;
            if (eye_x < -7.0) eye_x = -7.0;
        }
        else if ((-102.0 >= eye_z && eye_z >= -134.0) && (-7.5 <= eye_x && eye_x <= 7.5)) {
                if (eye_x > 7.0) eye_x = 7.0;
                if (eye_z < -124.0) eye_z = -124.0;
            }
        else if ((-107.0 >= eye_z && eye_z > -134.0) && (-35.0 < eye_x && eye_x < -7.5)) {
                if (-30.0 > eye_x) eye_x = -30.0;
                if (eye_z > -102.0) eye_z = -102.0;
            }
        else if (-134.0 >= eye_z && eye_z > -150.0) {
            if (eye_x < -30.0) eye_x = -30.0;
            if (eye_z < -145.0) eye_z = -145;
            if (eye_x >= -7.5) {
                eye_y = -20.0;
                eye_x = 10.0;
                look_y = -20.0;
            }
        }
    }

    else if (eye_y == -20.0) {
        if (eye_z <= -31.0 && eye_z >= -120.0) {
            if (eye_x > 10.0) eye_x = 10.0;
            if (eye_x < -10.0) eye_x = -10.0;
        }
        else if (eye_z <= -110.0 && eye_z >= -134.0) {
            if (eye_x >= 10.0 && eye_x <= 35.0) {
                if (eye_x > 25.0) eye_x = 25.0;
                if (eye_z > -125.0) eye_z = -125.0;
            }
            else if (eye_x <= 10.0 && eye_x >= -10.0) {
                if (eye_z < -130) eye_z = -130.0;
            }
            else if (eye_x <= -10.0 && eye_x >= -35.0) {
                if (eye_z < -130.0) eye_z = -130.0;
                if (eye_z > -125.0) eye_z = -125.0;
                if (eye_x < -25.0) eye_x = -25.0;
            }
        }
        else if ((eye_z <= -134 && eye_z >= -150) && (eye_x >= 0.0)) {
            if (eye_x > 25.0) eye_x = 25.0;
            if (eye_z < -140) eye_z = -140;
            if (eye_x < 10.0) {
                eye_y = 10.0;
                eye_x = -20.0;
                look_y = 10.0;
            }
        }
    }

    else if (eye_y == 40.0) {
        if ((eye_z <= 50.0 && eye_z >= 35.0) && (eye_x < -25.0 && eye_x > -50.0)) {
            if (eye_z > 40.0) eye_z = 40.0;
            if (eye_x < -40.0) eye_x = -40.0;
            if (eye_z < 38.0) {
                if (eye_x > -41.0 && eye_x < -29.0) {
                    eye_y = 10.0;
                    eye_x = -20.0;
                    eye_z = 5.0;
                    look_y = 10.0;
                } else {
                    eye_z = 38.0;
                }
            }
        }
        //if (eye_z < 30.0) eye_z = 30.0;
        else if ((eye_z <= 50.0 && eye_z >= 38.0) && (eye_x < -7.5 && eye_x > -25.0)) {
            if (eye_z > 40.0) eye_z = 40.0;
            if (eye_x > -10.0) eye_x = -10.0;
        }

        else if ((eye_z <= 38.0 && eye_z >= 30.0) && (eye_x < -7.5 && eye_x > -25.0)) {
            if (eye_x < -25.0) eye_x = -25.0;
            if (eye_z < 35.0) eye_z = 35.0;
        }
        else if ((eye_x <= 15.0 && eye_x >= -7.5) && (eye_z >= 30.0 && eye_z <= 38.0)) {
            if (eye_z > 36.0) eye_z = 36.0;
            if (eye_z < 35.0) eye_z = 35.0;
            if (eye_x > 4.0) eye_x = 4.0;
        }

    }
}
//--Modeling functions------------------------------------------------------------------------------------


void drawDeck(void)
{
    float darkGreen[] = {0.12, 0.3, 0.17};
    /* Defining the coordinates of floor of deck which consists of five parts */
    const int n = 11;
    float floor1x[] = {0.0, -20.0, -35.0, -45.0, -48.5, -50.0, 50.0, 48.5, 45.0, 35.0, 20.0};
    float floor1y[] = {-20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0, -20.0};
    float floor1z[] = {300.0, 275.0, 250.0, 225.0, 200.0, 175.0, 175.0, 200.0, 225.0, 250.0, 275.0};

    const int m = 4;
    float floor2x[] = {-50.0, -50.0, 50.0, 50.0};
    float floor2y[] = {-30.0, -30.0, -30.0, -30.0};
    float floor2z[] = {175.0, 50.0, 50.0, 175.0};

    float floor3ax[] = {-50.0, -50.0, 50.0, 50.0};
    float floor3ay[] = {0.0, 0.0, 0.0, 0.0};
    float floor3az[] = {55.0, -134.0, -134.0, 55.0};

    float floor3bx[] = {-50.0, -50.0, -19.0, -19.0};
    float floor3by[] = {0.0, 0.0, 0.0, 0.0};
    float floor3bz[] = {-134.0, -150.0, -150.0, -134.0};

    float floor3cx[] = {35.0, 35.0, 50.0, 50.0};
    float floor3cy[] = {0.0, 0.0, 0.0, 0.0};
    float floor3cz[] = {-134.0, -150.0, -150.0, -134.0};

    float floor4x[] = {-50.0, -50.0, 50.0, 50.0};
    float floor4y[] = {-20.0, -20.0, -20.0, -20.0};
    float floor4z[] = {-150.0, -400.0, -400.0, -150.0};

    int theta = 0;
    float floor5x[n];
    float floor5y[n];
    float floor5z[n];

    for (int i=0; i<n; i++) {
        floor5x[i] = -50.0*cos(theta*DegreeToRadian);
        floor5y[i] = -10.0;
        floor5z[i] = -50.0*sin(theta*DegreeToRadian) - 400.0;
        theta += 18;
    }

    glColor3fv(grey);
    glBegin(GL_POLYGON);    // Floor1 (the most front) of the ship
        for (int i=0; i<n; i++)
            glVertex3f(floor1x[i], floor1y[i], floor1z[i]);
    glEnd();

    glColor3fv(white);
    glBegin(GL_POLYGON);        // Wall joining floor1 and floor2
        glVertex3f(floor1x[5], floor1y[5], floor1z[5]);
        glVertex3f(floor2x[0], floor2y[0], floor2z[0]);
        glVertex3f(floor2x[3], floor2y[3], floor2z[3]);
        glVertex3f(floor1x[6], floor1y[6], floor1z[6]);
    glEnd();

    glColor3fv(grey);
    glBegin(GL_POLYGON);        // Floor2 (the second front) of the ship
        for (int i=0; i<m; i++)
            glVertex3f(floor2x[i], floor2y[i], floor2z[i]);
    glEnd();

    glColor3fv(brown);
    glBegin(GL_POLYGON);        // Floor3a (the middle deck) of the ship
        for (int i=0; i<m; i++)
            glVertex3f(floor3ax[i], floor3ay[i], floor3az[i]);
    glEnd();

    glBegin(GL_POLYGON);        // Floor3b (the middle deck) of the ship
        for (int i=0; i<m; i++)
            glVertex3f(floor3bx[i], floor3by[i], floor3bz[i]);
    glEnd();

    glBegin(GL_POLYGON);        // Floor3c (the middle deck) of the ship
        for (int i=0; i<m; i++)
            glVertex3f(floor3cx[i], floor3cy[i], floor3cz[i]);
    glEnd();

    glColor3fv(darkGreen);
    glBegin(GL_POLYGON);        // Floor4 (the second rear) of the ship
        for (int i=0; i<m; i++)
            glVertex3f(floor4x[i], floor4y[i], floor4z[i]);
    glEnd();

    glBegin(GL_POLYGON);        // Wall joining floor4 and floor5
        glVertex3f(floor4x[1], floor4y[1], floor4z[1]);
        glVertex3f(floor5x[0], floor5y[0], floor5z[0]);
        glVertex3f(floor5x[10], floor5y[10], floor5z[10]);
        glVertex3f(floor4x[2], floor4y[2], floor4z[2]);

    glBegin(GL_POLYGON);        // Floor5 (the most rear) of the ship
        for (int i=0; i<n; i++)
            glVertex3f(floor5x[i], floor5y[i], floor5z[i]);
    glEnd();

}

void drawBuilding(void)
{
    int n = 8;
    float roofvx[] = {-50.0, -32.5, -15.0, -15.0, 15.0, 15.0, 32.5, 50.0};
    float roofvy[] = {45.0, 45.0, 45.0, 45.0, 45.0, 45.0, 45.0, 45.0};
    float roofvz[] = {41.5, 41.5, 41.5, 32.5, 32.5, 41.5, 41.5, 41.5};

    float roofwx[] = {-50.0, -32.5, -15.0, -5.0, 5.0, 15.0, 32.5, 50.0};
    float roofwy[] = {45.0, 45.0, 45.0, 45.0, 45.0, 45.0, 45.0, 45.0};
    float roofwz[] = {50.0, 53.0, 55.0, 56.0, 56.0, 55.0, 53.0, 50.0};


    float thirdFloorvx[] = {-50.0, -40.0, -30.0, -30.0, 30.0, 30.0, 40.0, 50.0};
    float thirdFloorvy[] = {30.01, 30.01, 30.01, 30.01, 30.01, 30.01, 30.01, 30.01};
    float thirdFloorvz[] = {41.5, 41.5, 41.5, 30.0, 30.0, 41.5, 41.5, 41.5};

    float thirdFloorwx[] = {-50.0, -32.5, -15.0, -5.0, 5.0, 15.0, 32.5, 50.0};
    float thirdFloorwy[] = {30.01, 30.01, 30.01, 30.01, 30.01, 30.01, 30.01, 30.01};
    float thirdFloorwz[] = {50.0, 53.0, 55.0, 56.0, 56.0, 55.0, 53.0, 50.0};


    float secondFloorvx[] = {-50.0, -30.0, -20.0, -10.0, 10.0, 20.0, 30.0, 50.0};
    float secondFloorvy[] = {15.01, 15.01, 15.01, 15.01, 15.01, 15.01, 15.01, 15.01};
    float secondFloorvz[] = {25.0, 25.0, 25.0, 25.0, 25.0, 25.0, 25.0, 25.0};

    float secondFloorwx[] = {-50.0, -32.5, -15.0, -5.0, 5.0, 15.0, 32.5, 50.0};
    float secondFloorwy[] = {15.01, 15.01, 15.01, 15.01, 15.01, 15.01, 15.01, 15.01};
    float secondFloorwz[] = {50.0, 53.0, 55.0, 56.0, 56.0, 55.0, 53.0, 50.0};


    float shieldavx[] = {-50.0, -32.5, -15.0, -5.0, 5.0, 15.0, 32.5, 50.0};
    float shieldavy[] = {30.0, 30.0, 30.0, 30.0, 30.0, 30.0, 30.0, 30.0};
    float shieldavz[] = {50.0, 53.0, 55.0, 56.0, 56.0, 55.0, 53.0, 50.0};

    float shieldawx[] = {-50.0, -32.5, -15.0, -5.0, 5.0, 15.0, 32.5, 50.0};
    float shieldawy[] = {37.0, 37.0, 37.0, 37.0, 37.0, 37.0, 37.0, 37.0};
    float shieldawz[] = {50.0, 53.0, 55.0, 56.0, 56.0, 55.0, 53.0, 50.0};


    float shieldbvx[] = {-50.0, -32.5, -15.0, -5.0, 5.0, 15.0, 32.5, 50.0};
    float shieldbvy[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float shieldbvz[] = {50.0, 53.0, 55.0, 56.0, 56.0, 55.0, 53.0, 50.0};

    float shieldbwx[] = {-50.0, -32.5, -15.0, -5.0, 5.0, 15.0, 32.5, 50.0};
    float shieldbwy[] = {22.0, 22.0, 22.0, 22.0, 22.0, 22.0, 22.0, 22.0};
    float shieldbwz[] = {50.0, 53.0, 55.0, 56.0, 56.0, 55.0, 53.0, 50.0};

    int m = 5;
    float rightShieldvx[] = {50.0, 50.0, 50.0, 50.0, 50.0};
    float rightShieldvy[] = {45.0, 15.0, 10.0, 5.0, 0.0};
    float rightShieldvz[] = {50.0, 50.0, 50.0, 50.0, 50.0};

    float rightShieldwx[] = {50.0, 50.0, 50.0, 50.0, 50.0};
    float rightShieldwy[] = {45.0, 15.0, 10.0, 5.0, 0.0};
    float rightShieldwz[] = {41.5, 41.5, 40.5, 38.0, 34.0};


    float leftShieldvx[] = {-50.0, -50.0, -50.0, -50.0, -50.0};
    float leftShieldvy[] = {45.0, 15.0, 10.0, 5.0, 0.0};
    float leftShieldvz[] = {41.5, 41.5, 40.5, 38.0, 34.0};

    float leftShieldwx[] = {-50.0, -50.0, -50.0, -50.0, -50.0};
    float leftShieldwy[] = {45.0, 15.0, 10.0, 5.0, 0.0};
    float leftShieldwz[] = {50.0, 50.0, 50.0, 50.0, 50.0};

    /* Draw building of wheelhouse */
    glColor3fv(white);
    glPushMatrix();         // 1F of wheelhouse building
        glTranslatef(0, 7.5, 40.0);
        glScalef(36.0, 15.0, 30.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();         // 2F of wheelhouse building
        glTranslatef(0.0, 22.5, 42.5);
        glScalef(36, 15, 25);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();     // 3F of wheelhouse building, Wall1
        glTranslatef(0.0, 37.5, 32.5);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, txId[6]);
        glBegin(GL_QUADS);
            glNormal3f(0.0, 0.0, 1.0);
            glTexCoord2f(0.0, 2.0);
            glVertex3f(-15.0, 7.5, 0.126);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-15.0, -7.5, 0.126);
            glTexCoord2f(4.0, 0.0);
            glVertex3f(15.0, -7.5, 0.126);
            glTexCoord2f(4.0, 2.0);
            glVertex3f(15.0, 7.5, 0.126);
        glEnd();
        glScalef(30.0, 15.0, 0.25);
        glutSolidCube(1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();     // 3F of wheelhouse building, Wall2
        glTranslatef(14.5, 37.5, 39.0);
        glScalef(0.25, 15.0, 13.0);
        glutSolidCube(1.0);
    glPopMatrix();

//    glPushMatrix();     // 3F of wheelhouse building, Wall3
//        glTranslatef(-14.5, 37.5, 39.0);
//        glScalef(0.25, 15, 13);
//        glutSolidCube(1.0);
//    glPopMatrix();

    glBegin(GL_QUAD_STRIP);     // Roof of 3F of wheelhouse using quadratic strip
    for (int i = 0; i < n; i++) {
    glVertex3f(roofvx[i], roofvy[i], roofvz[i]);
    if(i > 0) normal(roofvx[i-1], roofvy[i-1], roofvz[i-1],
                     roofwx[i], roofwy[i], roofwz[i],
                     roofvx[i], roofvy[i], roofvz[i]);
    glVertex3f(roofwx[i], roofwy[i], roofwz[i]);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < n; i++) {
    glVertex3f(shieldavx[i], shieldavy[i], shieldavz[i]);
    if(i > 0) normal(shieldawx[i-1], shieldawy[i-1], shieldawz[i-1],
                    shieldavx[i], shieldavy[i], shieldavz[i],
                    shieldawx[i], shieldawy[i], shieldawz[i]);
    glVertex3f(shieldawx[i], shieldawy[i], shieldawz[i]);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < n; i++) {
    glVertex3f(shieldbvx[i], shieldbvy[i], shieldbvz[i]);
    if(i > 0) normal(shieldbwx[i-1], shieldbwy[i-1], shieldbwz[i-1],
                    shieldbvx[i], shieldbvy[i], shieldbvz[i],
                    shieldbwx[i], shieldbwy[i], shieldbwz[i]);
    glVertex3f(shieldbwx[i], shieldbwy[i], shieldbwz[i]);
    }
    glEnd();

    glColor3fv(grey);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < n; i++) {
    glVertex3f(thirdFloorvx[i], thirdFloorvy[i], thirdFloorvz[i]);
    if(i > 0) normal(thirdFloorwx[i-1], thirdFloorwy[i-1], thirdFloorwz[i-1],
                    thirdFloorvx[i], thirdFloorvy[i], thirdFloorvz[i],
                    thirdFloorwx[i], thirdFloorwy[i], thirdFloorwz[i]);
    glVertex3f(thirdFloorwx[i], thirdFloorwy[i], thirdFloorwz[i]);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < n; i++) {
    glVertex3f(secondFloorvx[i], secondFloorvy[i], secondFloorvz[i]);
    if(i > 0) normal(secondFloorwx[i-1], secondFloorwy[i-1], secondFloorwz[i-1],
                    secondFloorvx[i], secondFloorvy[i], secondFloorvz[i],
                    secondFloorwx[i], secondFloorwy[i], secondFloorwz[i]);
    glVertex3f(secondFloorwx[i], secondFloorwy[i], secondFloorwz[i]);
    }
    glEnd();

    glColor3fv(white);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < m; i++) {
    glVertex3f(rightShieldvx[i], rightShieldvy[i], rightShieldvz[i]);
    if(i > 0) normal(rightShieldwx[i-1], rightShieldwy[i-1], rightShieldwz[i-1],
                    rightShieldvx[i], rightShieldvy[i], rightShieldvz[i],
                    rightShieldwx[i], rightShieldwy[i], rightShieldwz[i]);
    glVertex3f(rightShieldwx[i], rightShieldwy[i], rightShieldwz[i]);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < m; i++) {
    glVertex3f(leftShieldvx[i], leftShieldvy[i],leftShieldvz[i]);
    if(i > 0) normal(leftShieldwx[i-1], leftShieldwy[i-1], leftShieldwz[i-1],
                    leftShieldvx[i], leftShieldvy[i], leftShieldvz[i],
                    leftShieldwx[i], leftShieldwy[i], leftShieldwz[i]);
    glVertex3f(leftShieldwx[i], leftShieldwy[i], leftShieldwz[i]);
    }
    glEnd();


    /* Draw main building on the deck */
    glPushMatrix();     // Front wall1
        glTranslatef(0.0, 25.0, -30);
        glScalef(70.0, 10.0, 1.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();     // Front wall2
        glTranslatef(21.25, 10.0, -30.0);
        glScalef(27.5, 20.0, 1.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();     // Front wall3
        glTranslatef(-21.25, 10.0, -30.0);
        glScalef(27.5, 20.0, 1.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();     // Right wall
        glTranslatef(34.5, 15.0, -90.0);
        glScalef(1.0, 30.0, 120.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();     // Left wall
        glTranslatef(-34.5, 15.0, -90.0);
        glScalef(1.0, 30.0, 120.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();     // Rear wall
        glTranslatef(0.0, 15.0, -149.5);
        glScalef(70.0, 30.0, 1.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();     // Partition wall
        glTranslatef(7.0, 15.0, -134.0);
        glScalef(54.0, 30.0, 1.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glColor3fv(brown);
    glPushMatrix();     // Roof and ceiling
        glTranslatef(0.0, 29.5, -90.0);
        glScalef(70.0, 1.0, 120.0);
        glutSolidCube(1.0);
    glPopMatrix();

    /* Draw chimney */
    glPushMatrix();
    glTranslatef(0.0, 30.0, -75.0);
    glRotatef(-90.0, 1, 0, 0);

        glPushMatrix();
            glColor3fv(black);
            glutSolidCylinder(20.0, 23.0, 18.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(white);
            glTranslatef(0.0, 0.0, 23.0);
            glutSolidCylinder(20.0, 2.0, 18.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(red);
            glTranslatef(0.0, 0.0, 25.0);
            glutSolidCylinder(20.0, 6.0, 18.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(white);
            glTranslatef(0.0, 0.0, 31.0);
            glutSolidCylinder(20.0, 6.0, 18.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(red);
            glTranslatef(0.0, 0.0, 37.0);
            glutSolidCylinder(20.0, 6.0, 18.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(white);
            glTranslatef(0.0, 0.0, 43.0);
            glutSolidCylinder(20.0, 2.0, 18.0, 1.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(black);
            glTranslatef(0.0, 0.0, 45.0);
            glutSolidCylinder(20.0, 5.0, 18.0, 1.0);
        glPopMatrix();
    glPopMatrix();

}

void skybox(void)
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, txId[0]);  // front
    glBegin(GL_QUADS);
       glTexCoord2f(0., 1.);   glVertex3f(300, 300, 300);
       glTexCoord2f(0., 0.);   glVertex3f(300, -300, 300);
       glTexCoord2f(1., 0.);  glVertex3f(-300, -300, 300);
       glTexCoord2f(1., 1.);  glVertex3f(-300, 300, 300);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, txId[1]);  // up
    glBegin(GL_QUADS);
        glTexCoord2f(0., 1.);  glVertex3f(300, 300, -300);
        glTexCoord2f(0., 0.);  glVertex3f(300, 300, 300);
        glTexCoord2f(1., 0.); glVertex3f(-300, 300, 300);
        glTexCoord2f(1., 1.); glVertex3f(-300, 300, -300);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, txId[2]);  // down
    glBegin(GL_QUADS);
        glTexCoord2f(0., 1.);  glVertex3f(300, -300, 300);
        glTexCoord2f(0., 0.);  glVertex3f(300, -300, -300);
        glTexCoord2f(1., 0.); glVertex3f(-300, -300, -300);
        glTexCoord2f(1., 1.); glVertex3f(-300, -300, 300);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, txId[3]);  // right
    glBegin(GL_QUADS);
        glTexCoord2f(0., 1.);  glVertex3f(300, 300, -300);
        glTexCoord2f(0., 0.);  glVertex3f(300, -300, -300);
        glTexCoord2f(1., 0.); glVertex3f(300, -300, 300);
        glTexCoord2f(1., 1.); glVertex3f(300, 300, 300);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, txId[4]);  // back
    glBegin(GL_QUADS);
        glTexCoord2f(0., 1.);  glVertex3f(-300, 300, -300);
        glTexCoord2f(0., 0.);  glVertex3f(-300, -300, -300);
        glTexCoord2f(1., 0.); glVertex3f(300, -300, -300);
        glTexCoord2f(1., 1.); glVertex3f(300, 300, -300);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, txId[5]);  // left
    glBegin(GL_QUADS);
        glTexCoord2f(0., 1.);  glVertex3f(-300, 300, 300);
        glTexCoord2f(0., 0.);  glVertex3f(-300, -300, 300);
        glTexCoord2f(1., 0.); glVertex3f(-300, -300, -300);
        glTexCoord2f(1., 1.); glVertex3f(-300, 300, -300);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawEngineRoom(void)
{
    glBegin(GL_QUADS);
        glColor3fv(white);
        glVertex3f(-34.0, 0.0, -31.0);
        glVertex3f(-34.0, -40.0, -31.0);
        glVertex3f(34.0, -40.0, -31.0);
        glVertex3f(34.0, 0.0, -31.0);

        glVertex3f(34.0, 0.0, -31.0);
        glVertex3f(34.0, -40.0, -31.0);
        glVertex3f(34.0, -40.0, -150.0);
        glVertex3f(34.0, 0.0, -150.0);

        glVertex3f(34.0, 0.0, -150.0);
        glVertex3f(34.0, -40.0, -150.0);
        glVertex3f(-34.0, -40.0, -150.0);
        glVertex3f(-34.0, 0.0, -150.0);

        glVertex3f(-34.0, 0.0, -150.0);
        glVertex3f(-34.0, -40.0, -150.0);
        glVertex3f(-34.0, -40.0, -31.0);
        glVertex3f(-34.0, 0.0, -31.0);

        glVertex3f(34.0, -40.0, -31.0);
        glVertex3f(-34.0, -40.0, -31.0);
        glVertex3f(-34.0, -40.0, -150.0);
        glVertex3f(34.0, -40.0, -150.0);

        glVertex3f(-15.0, -30.0, -31.0);
        glVertex3f(-15.0, -30.0, -120.0);
        glVertex3f(15.0, -30.0, -120.0);
        glVertex3f(15.0, -30.0, -31.0);

        glVertex3f(-34.0, -30.0, -120.0);
        glVertex3f(-34.0, -30.0, -150.0);
        glVertex3f(34.0, -30.0, -150.0);
        glVertex3f(34.0, -30.0, -120.0);
    glEnd();
}


void drawStairsSide(float stairsHeight, float stairsDist)
{
    float stairsAngle;
    float halfHeight = (stairsHeight + 1.5) / 2.0;
    float halfDist = (stairsDist + 1.5) / 2.0;
    float handRailLen = sqrt(pow(stairsHeight + 1.5, 2.0) + pow(stairsDist + 1.5, 2.0));

    glBegin(GL_POLYGON);        // Side of the stairs
        glVertex3f(0.0, 0.0, 0.75);
        glVertex3f(0.0, stairsHeight, stairsDist+0.75);
        glVertex3f(0.0, stairsHeight+1.5, stairsDist+0.75);
        glVertex3f(0.0, 0.0, -0.75);
    glEnd();

    stairsAngle = atan(stairsHeight/stairsDist) * 180/PI;
    glPushMatrix();
        glTranslatef(0.0, 5.5, -0.75);
        glRotatef(-stairsAngle, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.25, handRailLen, 8.0, 2.0);
    glPopMatrix();

    for (int i=0; i<3; i++) {
        glPushMatrix();
            glTranslatef(0.0, i*halfHeight, i*halfDist - 0.75);
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glutSolidCylinder(0.2, 5.5, 8.0, 2.0);
        glPopMatrix();
    }
}
void drawStairs(float stairsHeight, float stairsDist, float stairsWidth)
{
    int nStep = 15;
    float deltaHeight = stairsHeight/nStep;
    float deltaStep = stairsDist/nStep;


    for (int i=1; i<=nStep; i++) {               // Draw the steps of stairs
        glPushMatrix();
            glTranslatef(0.0, i*deltaHeight, i*deltaStep);
            glScalef(stairsWidth - 0.1, 1.0, 1.5);
            glutSolidCube(1.0);
        glPopMatrix();
    }


    glPushMatrix();
        glTranslatef(stairsWidth/2.0, 0.0, 0.0);
        drawStairsSide(stairsHeight, stairsDist);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-stairsWidth/2.0, 0.0, 0.0);
        drawStairsSide(stairsHeight, stairsDist);
    glPopMatrix();
}


void allocateStairs(void)
{
    glPushMatrix();
        glColor3fv(white);
        glPushMatrix();
            glTranslatef(-25.0, 0.0, 16.25);
            drawStairs(15.0, 8.0, 6.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-35.0, 15.0, 32.75);
            drawStairs(15.0, 8.0, 6.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(1.0, -30.0, -140.0);
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            drawStairs(30.0, 20.0, 12.0);
        glPopMatrix();
    glPopMatrix();
}

void drawFence(float fenceHeight, float fenceWidth)
{
    float halfHeight = fenceHeight / 2.0;
    float halfWidth = fenceWidth / 2.0;

    glPushMatrix();     // Right pillar of fence
        glColor3fv(white);
        glTranslatef(-halfWidth, 0.0, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.25, fenceHeight, 8.0, 2.0);
    glPopMatrix();

    glPushMatrix();     // Left pillar of fence
        glColor3fv(white);
        glTranslatef(halfWidth, 0.0, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.25, fenceHeight, 8.0, 2.0);
    glPopMatrix();


    glPushMatrix();
        glColor3fv(white);
        glTranslatef(-halfWidth, halfHeight, 0.0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        glutSolidCylinder(0.2, fenceWidth, 8.0, 2.0);
    glPopMatrix();


    glPushMatrix();
        glColor3fv(black);
        glTranslatef(-halfWidth, fenceHeight - 0.25, 0.0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        glutSolidCylinder(0.25, fenceWidth, 8.0, 2.0);
    glPopMatrix();

}

void allocateFence(void)
{
    int nSideFence = 12;

    glPushMatrix();
    for (int i=0; i<nSideFence; i++) {      // Fence on the right side
        glPushMatrix();
            glTranslatef(-50.0, 0.0, -15.0*i+34.0);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            drawFence(7.0, 15.0);
        glPopMatrix();
    }

    for (int i=0; i<nSideFence; i++) {      //Fence on the left side
        glPushMatrix();
            glTranslatef(50.0, 0.0, -15.0*i+34.0);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            drawFence(7.0, 15.0);
        glPopMatrix();
    }

    glPushMatrix();           // Right side fence of 2F
        glTranslatef(-50.0, 15.0, 33.25);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        drawFence(7.0, 16.5);
    glPopMatrix();

    glPushMatrix();             // Left side fence of 2F
        glTranslatef(50.0, 15.0, 33.25);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        drawFence(7.0, 16.5);
    glPopMatrix();

    for (int i=0; i<6; i++) {       // Front left side fence of 2F
        glPushMatrix();
            glTranslatef(-12.0*i+44.0, 15.0, 25.0);
            drawFence(7.0, 12.0);
        glPopMatrix();
    }

    for (int i=0; i<2; i++) {       // Front right side fence of 2F
        glPushMatrix();
            glTranslatef(-11.0*i-33.5, 15.0, 25.0);
            drawFence(7.0, 11.0);
        glPopMatrix();
    }

    glPushMatrix();
        glTranslatef(-44.0, 30.0, 41.5);
        drawFence(7.0, 12.0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-30.0, 30.0, 35.75);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        drawFence(7.0, 11.5);
    glPopMatrix();

    for (int i=0; i<6; i++) {
        glPushMatrix();
            glTranslatef(-10.0*i+25.0, 30.0, 30.0);
            drawFence(7.0, 10.0);
        glPopMatrix();
    }

    glPushMatrix();
        glTranslatef(30.0, 30.0, 35.75);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        drawFence(7.0, 11.5);
    glPopMatrix();

    for (int i=0; i<2; i++) {
        glPushMatrix();
            glTranslatef(-10.0*i+45.0, 30.0, 41.5);
            drawFence(7.0, 10.0);
        glPopMatrix();
    }

    for (int i=0; i<9; i++) {
        glPushMatrix();
            glTranslatef(15.0, -30.0, -10.0*i-36.0);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            drawFence(7.0, 10.0);
        glPopMatrix();
    }

    for (int i=0; i<9; i++) {
        glPushMatrix();
            glTranslatef(-15.0, -30.0, -10.0*i-36.0);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            drawFence(7.0, 10.0);
        glPopMatrix();
    }

    for (int i=0; i<2; i++) {
        glPushMatrix();
            glTranslatef(-10.0*i-20.0, -30.0, -120.0);
            drawFence(7.0, 10.0);
        glPopMatrix();
    }

    for (int i=0; i<2; i++) {
        glPushMatrix();
            glTranslatef(-10.0*i+30.0, -30.0, -120.0);
            drawFence(7.0, 10.0);
        glPopMatrix();
    }

}

void drawPiston(void)
{
    glPushMatrix();
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.75, 1.5, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        GLUquadric *q;
        q = gluNewQuadric();
        gluCylinder(q, 0.75, 0.25, 0.5, 20.0, 2.0);
        gluQuadricDrawStyle(q, GLU_FILL);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, -3.5, 0.0);
        glScalef(0.4, 6.0, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();
}

void drawConnectingRod(void)
{
    glPushMatrix();
        glScalef(1.5, 2.0, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, -3.5, 0.0);
        glScalef(0.5, 5.0, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, -7.25, 0.0);
        glScalef(2.0, 2.5, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();
}

void drawCrankShaft(void)
{
    glPushMatrix();
        glTranslatef(0.0, crankRadius, crankRadius);
        glutSolidCylinder(0.5, 0.3, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glRotatef(-45.0, 10.0, 0.0, 0.0);
        glTranslatef(0.0, 0.0, -0.5/sqrt(2.0));
        glutSolidCylinder(0.5/sqrt(2.0), 2.5*sqrt(2.0), 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, 0.0, -0.5);
        glutSolidCylinder(0.5, 0.5, 20.0, 2.0);
    glPopMatrix();
}
void drawEngine(void)
{

    glPushMatrix();     // Piston
        glColor3fv(silver);
        glTranslatef(0.0, pistonHeight-21.1, crankRadius-34.4);
        drawPiston();
    glPopMatrix();

    glPushMatrix();         // Connecting rod
        glColor3fv(lightGreen);
        glTranslatef(0.0, rodPosition-21.1, crankRadius-34.6);
        glRotatef(rodAngle, 0.0, 0.0, 1.0);
        drawConnectingRod();
    glPopMatrix();

    glPushMatrix();     // Crankshaft
        glColor3fv(silver);
        glTranslatef(0.0, -21.1 ,-35.0);
        glRotatef(-crankAngle, 0.0, 0.0, 1.0);
        drawCrankShaft();
    glPopMatrix();

}

void drawTable(void)
{
    glPushMatrix();
        glColor3fv(brown);
        glPushMatrix();
            glTranslatef(0.0, 9.0, 0.0);
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glutSolidCylinder(12.5, 0.75, 20.0, 2.0);
        glPopMatrix();

        glPushMatrix();
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glutSolidCylinder(1.0, 9.0, 20.0, 2.0);
        glPopMatrix();

        glPushMatrix();
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glutSolidCylinder(7.5, 1.5, 20.0, 2.0);
        glPopMatrix();
    glPopMatrix();
}

void drawChair(void)
{
    float orange[] = {1.00, 0.46, 0.22};

    glPushMatrix();         // backrest
        glColor3fv(orange);
        glRotatef(-85.0, 1.0, 0.0, 0.0);
        glTranslatef(0.0, 0.0, 4.0);
        glScalef(5.0, 0.75, 8.0);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, txId[7]);
        glBegin(GL_QUADS);
            glNormal3f(0.0, -1.0, 0.0);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(-0.5, -0.5, 0.5);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-0.5, -0.5, -0.5);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(0.5, -0.5, -0.5);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(0.5, -0.5, 0.5);
        glEnd();
        glutSolidCube(1.0);
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();


    glPushMatrix();     // seat
        glColor3fv(orange);
        glTranslatef(0.0, 0.0, -2.0);
        glScalef(5.0, 2.0, 5.0);
        glutSolidCube(1.0);
    glPopMatrix();

    /* legs */
    glPushMatrix();
        glColor3fv(brown);
        glPushMatrix();
            glTranslatef(-2.0, -3.0, 0.0);
            glScalef(1.0, 4.0, 1.0);
            glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-2.0, -3.0, -4.0);
            glScalef(1.0, 4.0, 1.0);
            glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(2.0, -3.0, -4.0);
            glScalef(1.0, 4.0, 1.0);
            glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(2.0, -3.0, 0.0);
            glScalef(1.0, 4.0, 1.0);
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();

}

void drawTableChairs(void)
{
    glPushMatrix();
        drawTable();
        for (int i=0; i<4; i++) {
            glPushMatrix();
                glRotatef(i*90.0, 0.0, 1.0, 0.0);
                glTranslatef(0.0, 5.0, 15.0);
                drawChair();
            glPopMatrix();
        }
    glPopMatrix();
}

void allocateTablesChairs(void)
{
    glPushMatrix();
        glTranslatef(17.0, 0.01, -56.0);
        glScalef(0.6, 0.6, 0.7);
        drawTableChairs();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-17.0, 0.01, -82.0);
        glScalef(0.6, 0.6, 0.7);
        glRotatef(45.0, 0.0, 1.0, 0.0);
        drawTableChairs();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(17.0, 0.01, -108.0);
        glScalef(0.6, 0.6, 0.7);
        glRotatef(20.0, 0.0, 1.0, 0.0);
        drawTableChairs();
    glPopMatrix();
}


void drawCeilingFan(void)
{
    int n = 6;
    float x[] = {0.5, 1.0, 0.5, -0.5, -1.0, -0.5};
    float y[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float z[] = {0.0, 6.0, 8.1, 8.1, 6.0, 0.0};
    float lightYellow[] = {1.0, 1.0, 0.5};

    glPushMatrix();
    glTranslatef(0.0, 21.5, -75.0);

    glPushMatrix();
        glColor3fv(brown);
        glTranslatef(0.0, 6.5, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(1.5, 1.5, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(silver);
        glColor3f(0.12f, 0.1f, 0.1f);
        glTranslatef(0.0, 1.5, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.1, 5.0, 4.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(silver);
        glTranslatef(0.0, 0.5, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        GLUquadric *q2;
        q2 = gluNewQuadric();
        gluCylinder(q2, 2.0, 0.75, 1.0, 20.0, 2.0);
        gluQuadricDrawStyle(q2, GLU_FILL);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(brown);
        glTranslatef(0.0, -0.5, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(2.0, 1.0, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(lightYellow);
        glTranslatef(0.0, -0.5, 0.0);
        glutSolidSphere(1.8, 20.0, 20.0);
    glPopMatrix();

    glPushMatrix();
    glColor3fv(brown);
    glRotatef(fanAngle, 0.0, 1.0, 0.0);
    for (int i=0; i<3; i++) {
        glPushMatrix();
        glRotatef(i*120.0, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 0.0, 1.9);
        glBegin(GL_POLYGON);
            for (int j=0; j<n; j++)
                glVertex3f(x[j], y[j], z[j]);
        glEnd();
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix();

}

void drawHelm(void)
{
    int N = 13;     // Number of points on the handle's curve
    int nSlice = 36;
    float theta = 10 * PI/180;
    float vx[] = {0.0, 0.2, 0.25, 0.2, 0.1, 0.25, 0.25, 0.1, 0.1, 0.15, 0.25, 0.2, 0.1};
    float vy[] = {3.5, 3.25, 3.0, 2.75, 2.5, 2.25, 1.75, 1.5, 1.25, 1.15, 0.5, 0.25, 0.0};
    float vz[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float wx[N], wy[N], wz[N];
    float darkBrown[] = {0.3, 0.05, 0.0};

        glPushMatrix();
            glColor3fv(darkBrown);
            glutSolidTorus(0.25, 3.0, 20.0, 20.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(darkBrown);
            glTranslatef(0.0, 0.0, -0.25);
            glutSolidCylinder(1.0, 1.5, 20.0, 2.0);
        glPopMatrix();

        glPushMatrix();
            glColor3fv(brown);
            glTranslatef(0.0, 0.0, -0.26);
            glutSolidCylinder(0.5, 0.2, 10.0, 2.0);
        glPopMatrix();

        glPushMatrix();
        glColor3fv(darkBrown);
        for(int k=0; k<8; k++) {
            glPushMatrix();
            glRotatef(k*45.0, 0.0, 0.0, 1.0);
            glTranslatef(0.0, 1.0, 0.0);
            for (int j = 0; j < nSlice; j++) {
                for (int i = 0; i < N; i++) {
                    wx[i] = vx[i]*cos(theta) + vz[i]*sin(theta);
                    wy[i] = vy[i];
                    wz[i] = -vx[i]*sin(theta) + vz[i]*cos(theta);
                }

                glBegin(GL_TRIANGLE_STRIP);
                for (int i = 0; i < N; i++) {
                    //glTexCoord2f((float)j/nSlice, (float)i/(N-1));
                    glVertex3f(vx[i], vy[i], vz[i]);
                    if (i > 0) {
                        normal(wx[i-1], wy[i-1], wz[i-1],
                            vx[i], vy[i], vz[i],
                            wx[i], wx[i], wz[i]);
                        }
                    //glTexCoord2f((float)(j+1)/nSlice, (float)i/(N-1));
                    glVertex3f(wx[i], wy[i], wz[i]);
               }
                glEnd();

                for (int i = 0; i < N; i++) {
                    vx[i] = wx[i];
                    vy[i] = wy[i];
                    vz[i] = wz[i];
                }
            }
        glPopMatrix();
        }
        glPopMatrix();
}

void drawControls(void)
{
    float greenBrown[] = {0.17, 0.2, 0.0};

    glPushMatrix();
        glTranslatef(0.0, 35.2, 47.0);
        glScalef(0.8, 0.8, 0.8);

    glPushMatrix();
        glColor3fv(silver);
        glTranslatef(0.0, -4.0, 0.65);
        glScalef(8.0/3.0, 0.5, 0.8);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(silver);
        glRotatef(18.4, 0.0, 0.0, 1.0);
        glTranslatef(0.0, -sqrt(40.0)/2, 0.65);
        glScalef(0.5, sqrt(40.0), 0.8);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(silver);
        glRotatef(-18.4, 0.0, 0.0, 1.0);
        glTranslatef(0.0, -sqrt(40.0)/2, 0.65);
        glScalef(0.5, sqrt(40.0), 0.8);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(lightGreen);
        glTranslatef(0.0, 0.0, 1.5);
        glutSolidCylinder(2.0, 0.5, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(lightGreen);
        glRotatef(-15.9, 0.0, 0.0, 1.0);
        glTranslatef(3.5, 0.0, 1.5);
        glBegin(GL_QUADS);
            glVertex3f(3.5, -1.0, -0.25);
            glVertex3f(-3.5, -2.0, -0.25);
            glVertex3f(-3.5, 2.0, -0.25);
            glVertex3f(3.5, 1.0, -0.25);

            glVertex3f(3.5, 1.0, -0.25);
            glVertex3f(-3.5, 2.0, -0.25);
            glVertex3f(-3.5, 2.0, 0.25);
            glVertex3f(3.5, 1.0, 0.25);

            glVertex3f(3.5, 1.0, 0.25);
            glVertex3f(-3.5, 2.0, 0.25);
            glVertex3f(-3.5, -2.0, 0.25);
            glVertex3f(3.5, -1.0, 0.25);

            glVertex3f(3.5, -1.0, 0.25);
            glVertex3f(-3.5, -2.0, 0.25);
            glVertex3f(-3.5, -2.0, -0.25);
            glVertex3f(3.5, -1.0, -0.25);
        glEnd();
    glPopMatrix();

    glPushMatrix();
        glColor3fv(lightGreen);
        glTranslatef(sqrt(45.0), -2.0, 1.5);
        glutSolidCylinder(1.0, 0.5, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(lightGreen);
        glTranslatef(sqrt(45.0), -2.0, 2.0);
        glutSolidCylinder(1.0, 1.0, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();
        glColor3fv(lightGreen);
        glTranslatef(sqrt(45.0), -2.0, 5.0);
        glScalef(4.0, 9.0, 4.0);
//        glEnable(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, txId[7]);
//        glBegin(GL_QUADS);
//            glNormal3f(0.0, 0.0, -1.0);
//            glTexCoord2f(0.0, 1.0);
//            glVertex3f(0.5, 0.5, -0.5);
//            glTexCoord2f(0.0, 0.0);
//            glVertex3f(0.5, 0.16, -0.5);
//            glTexCoord2f(1.0, 0.0);
//            glVertex3f(-0.5, 0.16, -0.5);
//            glTexCoord2f(1.0, 1.0);
//            glVertex3f(-0.5, 0.5, -0.5);
//        glEnd();
        glutSolidCube(1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, txId[8]);
    glBegin(GL_QUADS);
        glNormal3f(0.0, -1.0, 0.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-0.5, -0.5, 0.5);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(0.5, -0.5, -0.5);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(0.5, -0.5, 0.5);
    glEnd();

    glPushMatrix();
        glColor3fv(silver);
        glTranslatef(0.0, -6.25, 5.0);
        glScalef(5.0, 0.5, 11.0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();         // black cylinder
        glColor3fv(greenBrown);
        glTranslatef(0.0, -6.0, 6.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(1.5, 9.0, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();         // meter
        glColor3fv(greenBrown);
        glTranslatef(2.0, 4.5, 5.25);
        glutSolidCylinder(1.5, 1.5, 20.0, 2.0);
    glPopMatrix();

    glPushMatrix();         // rod
        glColor3fv(greenBrown);
        glTranslatef(2.0, -6.0, 6.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.25, 11.0, 4.0, 2.0);
    glPopMatrix();

    drawHelm();

    glPopMatrix();
}

void drawBuoy(void)
{
    float vermilion[] = {1.00, 0.16, 0.00};

    glPushMatrix();
    glPushMatrix();
        glColor3fv(vermilion);
        glutSolidTorus(0.5, 2.0, 20.0, 20.0);
    glPopMatrix();

    for (int i=0; i<4; i++) {
        glRotatef(i*90.0, 0.0, 0.0, 1.0);
        glPushMatrix();
            glTranslatef(0.0, 2.0, 0.0);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glColor3fv(white);
            glutSolidTorus(0.15, 0.5, 10.0, 10.0);
        glPopMatrix();
    }
    glPopMatrix();
}

void throwBuoy(void)
{
    glPushMatrix();
        glTranslatef(buoy_x, buoy_y, 0.0);
        glPushMatrix();
            glTranslatef(50.0, 8.0, 0.0);
            glRotatef(90.0, 1.0, 0.0, 0.0);
            drawBuoy();
        glPopMatrix();
    glPopMatrix();
}



void normal(float x1, float y1, float z1,
            float x2, float y2, float z2,
              float x3, float y3, float z3 )
{
      float nx, ny, nz;
      nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
      ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
      nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

      glNormal3f(nx, ny, nz);
}
