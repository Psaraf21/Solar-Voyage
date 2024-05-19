#include<iostream>
#include<gl/glut.h>            
#include <gl/glu.h>
#include<math.h>
#include <stdio.h>

using namespace std;
float tx = 0, ty = 0, tz = 0, Trans_y = 0, alpha = 0, T_z = 200, z_viewvol = -120, saturn_tx = 0;
unsigned char mainkey = 's';      

// 0:Sun, 1:Jupiter, 2:RedPlanet, 3:Saturn, 4:BluePlanet, 5: AnyPlanet 6: LastPlanet  {R, G, B, Tx, Ty, Tz, Radius}....eachplanet: SolidSphere
float planets[6][7] = { {0.96,0.96,0.5,0,65,-2400,60}, {1,0.1,0.1,80,-30,-3600,50}, {0.96,0.33,0.33,-300,0,-1800,30},
  {0.96,0.9,1,-440,20,-4200,50}, {0.67,0.95,0.365,100,0,-800,30}, {0.16,0.7,0.96,10,-70,-1600,50} };

// {Base_Radius, Top_Radius, height, all-color}   
float BsterCylTypes[9][4] = { {1, 0.1, 3, 0.6}, {1, 0.9, 0.5, 0}, {2, 1, 3, 0.6}, {2, 2, 6, 0.6}, {2.05, 2.05, 0.3, 0},
	  {2.2, 1.7, 1, 0}, {2.2, 1.7, 1, 0.4}, {2.7, 2.2, 1.5, 0.4}, {2.4, 1.8, 2, 0.6} };                        // total 9 rings

int BsterCylOrder[] = { 0, 1, 2, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 5, 6, 5, 6, 5, 6, 7, 8 };             // Total 19

float BlackStripPlate[7][4] = { {1.9,-6,13,10.31},  {11.7,-6,-13,10.31}, {-2.7,-11,43,7}, {16.2,-11,-42,7},
		  {-3.8,-14,19,3.17}, {17.2,-14,-19,3.17}, {-3.8,-14,90,21} };

float Shuttle_Plates[3][4] = { {-6,10,1,0.5}, {-11,5,1.9,1}, {-14,3,2.1,1.9} };

GLUquadricObj* leftbooster, * maintank, * shuttle;
GLfloat pos[] = { -10, 20, -60, 1 };						//light position         //{-2,4,5,1}
GLfloat amb[] = { 0.7, 0.7, 0.7, 1.0 };				//Ambient intensity 
GLfloat front_amb_diff[] = { 0.8, 0.7, 0.7,1.0 };	//Front side property
GLfloat back_amb_diff[] = { 0.4,0.7,0.1,1.0 };		//Back side property
GLfloat spe[] = { 0.25,0.25,0.25,1.0 };			//Property for front and back
GLfloat spe2[] = { 80 };			            //Property for front and back
GLfloat theta = 0, dt = 0.7, axes[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
int axis = 0; 					         	//0:x, 1:Y, 2:Z, 3:light aroundz

/* tank - fuel tank*/
void shuttle_main_tank() {
	glPushMatrix();
	maintank = gluNewQuadric();
	gluQuadricDrawStyle(maintank, GLU_FILL);
	gluQuadricNormals(maintank, GLU_SMOOTH);
	gluQuadricOrientation(maintank, GLU_OUTSIDE);

	glColor3f(0.9, 0.9, 0.4);
	// Top of Tank pointing part
	float h = 0;
	for (float r = 4.7; r >= 0; r -= 0.5) {

		glPushMatrix();
		glTranslated(7.5, 22 + h, 0);         // Flexible to take it up and down        
		glTranslated(0, 0, z_viewvol);          
		glRotated(-90, 1, 0, 0);             
		gluCylinder(maintank, r, r, 1, 100, 100);
		h++;
		glPopMatrix();
	}

	glTranslated(7.5, -18, z_viewvol);
	glRotated(-90, 1, 0, 0);
	gluCylinder(maintank, 4.7, 4.7, 40, 32, 32);                  // Main Tank
	glPopMatrix();

	glPushMatrix();                          // for spherical bottom 
	glTranslated(7.5, -18, z_viewvol);
	glRotated(-90, 1, 0, 0);                    
	glutSolidSphere(4.7, 32, 32);
	glPopMatrix();
}

/wings of shuttle./

void shuttle_plate() {               
	glColor3f(1, 1, 1);
	for (int i = 0; i < 3; i++) {         // ShuttlePlates: 0: Top Plate, 1: Middle Plate, 2: Bottom Plate
		glPushMatrix();
		glTranslated(6.7, Shuttle_Plates[i][0], 6 + z_viewvol);
		glScalef(5, Shuttle_Plates[i][1], 0);
		glRotated(-90, 1, 0, 0);
		gluCylinder(shuttle, Shuttle_Plates[i][2], Shuttle_Plates[i][3], 1, 32, 32);
		glPopMatrix();
	}
	glColor3f(0.2, 0.2, 0.2);
}

float random(int min, int max) {            // throw flames
again: int i = (rand() % 100) / 10;
	if (max < 7) {
		if (i == 8 || i == 7)
			i = -1;
		else if (i == 9)
			i = -2;
	}
	if (i >= min && i <= max)
		return i;
	else
		goto again;
}

float random2(int min, int max) {                // needed to throw flames
again: int i = (rand() % 100);
	if (i >= min && i <= max)
		return i;
	else
		goto again;
}

/*  fire*/
void fire(int j) {
	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	float flamethrower[5][3] = { {7,-20, 8.5}, {6,-20,7.5}, {8,-20,7.5}, {0,-25,0}, {15,-25,0} };    // {T_x, T_y, T_z} 
	for (int i = 0; i < 200; i++) {
		glPushMatrix();
		glTranslated(flamethrower[j][0] + random(-3, 3), flamethrower[j][1] - random2(0, 20), flamethrower[j][2] + random(0, 5));
		glTranslated(0, 0, z_viewvol);                

		if (i % 3 == 0) {
			glColor3f(1, 0.46, 0);
			glutWireSphere(0.5, 32, 32);
		}
		else if (i % 2 == 0)
		{
			glColor3f(1, 0.64, 0);
			glutWireTetrahedron();
		}
		else {
			glColor3f(0.95, 0.95, 0.2);
			glutSolidOctahedron();
		}
		glPopMatrix();
	}  
	glDisable(GL_BLEND);
	glPopMatrix();
}

/* tail shuttle*/
void shuttleTail() {
	glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslated(6.7, -11, 10 + z_viewvol);
	glRotated(50, 1, 0, 0);
	glScalef(1, 4, 7);
	gluCylinder(shuttle, 0.8, 0.5, 1, 32, 32);
	glPopMatrix();

	glPushMatrix();                     // Tail...help
	glTranslated(6.7, -16.5, 13.3 + z_viewvol);
	glScalef(1, 4, 2.5);
	gluCylinder(shuttle, 0.3, 0.5, 1, 32, 32);
	glPopMatrix();

	glPushMatrix();                     // Tail...help
	glTranslated(6.7, -15.30, 14 + z_viewvol);
	glRotated(90, 1, 0, 0);
	glScalef(1, 4, 2.5);
	gluCylinder(shuttle, 0.3, 0.3, 1, 32, 32);                 
	glPopMatrix();
}
/*  jet exhaust 3 where which help in launch */
void shuttle_exhaust() {                    // Use 7 cylindrical rings 2 cones for each exhaust of Shuttle
	float r = 1, T_y = -16, height;
	float exhaust[3][2] = { {6.7, 12}, {5, 7}, {9, 7} };             // {T_x, T_y}
	for (int j = 0; j < 3; j++) {
		glColor3f(0, 0, 0); //ring color
		r = 1;
		T_y = -16;

		for (int i = 0; i < 7; i++) {
			glPushMatrix();                             
			glTranslated(exhaust[j][0], T_y, exhaust[j][1] + z_viewvol);
			glRotated(-90, 1, 0, 0);                      
			gluCylinder(shuttle, r, r, 0.40, 32, 32);                  
			glPopMatrix();
			T_y = T_y - 0.5;
			r = r + 0.15;
		}
		height = 0;
		glColor3f(0.3, 0.3, 0.3); //cone color top

		for (int i = 0; i < 2; i++) {
			glPushMatrix();                             
			glTranslated(exhaust[j][0], T_y, exhaust[j][1] + z_viewvol);
			glRotated(-90, 1, 0, 0);                     
			height = -T_y - 16;
			glutSolidCone(r, height, 32, 32);                  
			glPopMatrix();

			glColor3f(.92, 0.75, 0.05);//cone color bottom inside
			T_y = T_y - 0.2;
		}
	}
}
/*  Body space shuttel where austronaut are present*/
void shuttle_Discovery() {
	glTranslated(0, -3, 0);
	shuttle = gluNewQuadric();
	gluQuadricDrawStyle(shuttle, GLU_FILL);
	gluQuadricNormals(shuttle, GLU_SMOOTH);
	gluQuadricOrientation(shuttle, GLU_OUTSIDE);

	glColor3f(0, 0, 0);
	glPushMatrix();                                // topmost front
	glTranslated(6.7, 13.5, 9 + z_viewvol);
	glutSolidSphere(1.4, 32, 32);
	glPopMatrix();

	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();                               // windscreen 
	glTranslated(6.7, 7, 9 + z_viewvol);
	glRotated(-90, 1, 0, 0);
	gluCylinder(shuttle, 3.2, 1.4, 6.5, 32, 32);
	glPopMatrix();

	glColor3f(0.3, 0.3, 0.3);                  // Color for Windscreen
	// 0: middle windscreen, 1: left windscreen, 2: right windscreen 3: Rotation along y-axis 
	float windscreens[3][4] = { {6.7, 9, 10.9, 0}, {5.3, 8.4, 10.7, 1}, {8, 8.4, 10.7, -1} };

	for (int i = 0; i < 3; i++) {
		glPushMatrix();                    
		glTranslated(windscreens[i][0], windscreens[i][1], windscreens[i][2] + z_viewvol);
		glRotated(45, 1, windscreens[i][3], 0);              
		gluCylinder(shuttle, 1, 0.5, 1, 32, 32);
		glPopMatrix();
	}

	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();                        
	glTranslated(6.7, -12, 9 + z_viewvol);
	glRotated(-90, 1, 0, 0);
	gluCylinder(shuttle, 3.2, 3.2, 19, 32, 32);     
	glPopMatrix();

	glColor3f(0, 0, 0);                   // Window color
	int windowsY[5] = { 3, 0, -3, -6, -9 };                
	for (int i = 0; i < 5; i++) {
		glPushMatrix();                     
		glTranslated(6.7, windowsY[i], 10 + z_viewvol);      
		glScalef(6.1, 1, 1);
		glutSolidCube(1);
		glPopMatrix();
	}

	glColor3f(0.6, 0.6, 0.6); //Last Ring 

	glPushMatrix();                     
	glTranslated(6.7, -15, 9 + z_viewvol);
	glRotated(-90, 1, 0, 0);
	gluCylinder(shuttle, 3.2, 3.2, 3, 32, 32);
	glPopMatrix();

	glPushMatrix();                    
	glTranslated(6.7, -15, 10 + z_viewvol);
	glRotated(-90, 1, 0, 0);
	gluCylinder(shuttle, 3.2, 3.2, 3, 32, 32);
	glPopMatrix();

	// Render exhaust
	shuttle_exhaust();
	glColor3f(0.4, 0.4, 0.4);
	shuttleTail();
	shuttle_plate();                
}
/* left solid booster of shuttle*/
void shuttle_leftBooster(bool callfromRight) {               

	leftbooster = gluNewQuadric();
	gluQuadricDrawStyle(leftbooster, GLU_FILL);
	gluQuadricNormals(leftbooster, GLU_SMOOTH);
	gluQuadricOrientation(leftbooster, GLU_OUTSIDE);

	int j, height = 0;       
	for (int i = 0; i < 21; i++) {
		j = BsterCylOrder[i];
		height = height + BsterCylTypes[j][2];         
		glPushMatrix();
		glColor3f(BsterCylTypes[j][3], BsterCylTypes[j][3], BsterCylTypes[j][3]);
		glTranslated(0, 20 - height, 0);               
		glTranslated(0, 0, z_viewvol);
		glRotated(-90, 1, 0, 0);              
		gluCylinder(leftbooster, BsterCylTypes[j][0], BsterCylTypes[j][1], BsterCylTypes[j][2], 40, 100);
		glPopMatrix();
	}
	if (callfromRight == true)
		return;
	fire(3);
}
/*  right solid booster */

void shuttle_rightBooster() {              
	glPushMatrix();
	glTranslated(15, 0, 0);
	shuttle_leftBooster(true);
	glPopMatrix();
	fire(4);                         
}

// Assemble Space Shuttle 
void shuttle_main() {
	shuttle_leftBooster(false);        
	shuttle_rightBooster();
	shuttle_main_tank();
	shuttle_Discovery();
}

/*  shuttle seperating */
void shuttle_seperate() {
	theta = theta - 45;
	glPushMatrix(); 				    //left booster
	glRotated(theta, 1, 1, 0);
	shuttle_leftBooster(false);
	glPopMatrix();

	glPushMatrix(); 				    //right booster
	glRotated(theta, 1, -1, 0);
	shuttle_rightBooster();
	glPopMatrix();

	glPushMatrix(); 				   //Fuel Tank
	glRotated(theta, -1, 0, 1);
	shuttle_main_tank();
	glPopMatrix();

	glPushMatrix(); 				       //for shuttle
	glRotated(theta / 3, 1, 0, 0);
	shuttle_Discovery();
	glPopMatrix();

	theta = theta + 45;                   // Resetting theta

}

void BackGroundSphere() {
	glColor3f(1, 1,1);
	glutWireSphere(4000, 12, 12);                      // Big Wire Sphere
}

//Moon of Jupiter Planet
void Jupiter_Moon() {
	int moon[3][2] = { {20, -3600}, {80, -3540}, {80, 3660} };
	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glColor3f(1, 0.8, 0.8);
		glTranslated(moon[i][0], -20, moon[i][1]);                     
		glutSolidSphere(5, 32, 32);                    
		glPopMatrix();
	}
}

void AllPlanets() {                              // All planets and Sun
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glColor3f(planets[i][0], planets[i][1], planets[i][2]);
		if (i == 3) {
			glTranslated(planets[i][3] + saturn_tx, planets[i][4], planets[i][5]);
			saturn_tx += 1;
		}
		else {
			glTranslated(planets[i][3], planets[i][4], planets[i][5]);              
		}
		glutSolidSphere(planets[i][6], 32, 32);
		glPopMatrix();
	}
}

void Display_Contol_Menu() {
	fprintf(stdout, "Space Shuttle Project");
	cout << "\n\nPress 's' to Start Main Project....\nPress 'o' to see only Shuttle...\nPress 'r' to see shuttle in different view..." << endl;
	cout << "Press 'd' to see shuttle only on different view along different axis...\nPress 'g' to see Solar System or Galaxy..." << endl;
	cout << "Press 'p' to seperate shuttle....like what happens during its launch\nPress 'l' to see booster only\n";
	cout << "Press 'k' to see shuttle discovery only\nPress 'm' to see orange main tank only\n";
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 's' || key == 'o' || key == 'r' || key == 'd' || key == 'g' || key == 'p' || key == 'l' || key == 'k' || key == 'm')
	{
		mainkey = key;              
		if (key == 'p')
			theta = 45;
		else if (key == 's')
		{
			theta = 0;
			T_z = 200;
		}
	}
	else {
		mainkey = 's';
	}
	glutPostRedisplay();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();    
	glLightfv(GL_LIGHT0, GL_POSITION, pos);                 
	switch (mainkey) {

	case 's': {
		// Displays all Planets 
		glPushMatrix();
		glTranslated(0, 0, T_z);
		BackGroundSphere();
		AllPlanets();
		Jupiter_Moon();
		glPopMatrix();

		T_z = T_z + 10;                               // To Move along z-axis towards origin

		if (theta > 359 && theta < 360)             
		{
			T_z = 0;
			z_viewvol = -120;
		}

		if (theta < 46)
		{
			glPushMatrix();               
			glTranslated(tx, ty, 0);
			glTranslated(6, 0, -120);
			glRotated(-theta * 1.3, 1, 0, 0);
			glTranslated(-6, 0, 120);
			shuttle_main();              
			glPopMatrix();

		}
		else if (theta >= 46 && theta < 80) {
			glPushMatrix();
			glTranslated(6.7, 0, -120);
			glRotated(-62, 1, 0, 0);   
			glTranslated(-6.7, 0, 120);
			shuttle_seperate();         
			glPopMatrix();
		}
		else {
			glPushMatrix();

			glTranslated(random(-1, 1) * 0.05, Trans_y + random(-1, 1) * 0.05, random(-1, 1) * 0.05);

			glTranslated(6.7, 0, -120);
			glRotated(-48, 1, 0, 0);
			glTranslated(-6.7, 0, 120);

			glPushMatrix();
			glTranslated(6.7, 0, -120);
			glRotated(alpha, 0, 1, 0);
			alpha = alpha + 1;               
			glTranslated(-6.7, 0, 120);

			shuttle_Discovery();        
			fire(1);
			if (T_z > 3600)
				z_viewvol -= 0.20;
			glPopMatrix();

			glPopMatrix();
		}
		break;       
	}
	case 'o': shuttle_main();                  
		break;
	case 'r': glTranslated(6.7, 0, -100);       
		glRotated(-40, 1, 0, 0);
		glTranslated(-6.7, 0, 100);
		shuttle_main();
		break;
	case 'd': glTranslated(6.7, 0, -100);         
		glRotated(-40, 1, 0, 1);
		glTranslated(-6.7, 0, 100);
		shuttle_main();
		break;
	case 'g':  BackGroundSphere();                
		AllPlanets();
		Jupiter_Moon();
		break;
	case 'p': shuttle_seperate();                
		break;
	case 'l': shuttle_leftBooster(false);
		break;
	case 'k': shuttle_Discovery();
		break;
	case 'm': shuttle_main_tank();
		break;
	}

	glPopMatrix();                                 
	glutSwapBuffers();
}

void idle(void) {
	theta = (theta < 360) ? theta + dt : dt;
	tx = tx - 0.05;
	ty = ty + 0.08;
	glutPostRedisplay();
}

void main(int argc, char** argv) {
	pos[2] = z_viewvol + 20;               
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("3D Animation");
	glClearColor(0, 0, 0, 0);       //sets background color for the window.
	glEnable(GL_DEPTH_TEST);		 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				
	gluPerspective(45, 1, 20, 9000);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, front_amb_diff);    //Front side
	glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, back_amb_diff);      //Back side
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);                //Front and back
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, spe2);              //Front and back

	glLightfv(GL_LIGHT0, GL_AMBIENT, amb); 	//light source
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); 	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();				//Identity matrix for Modelview 

	glLightfv(GL_LIGHT0, GL_POSITION, pos); //light postion is affected by current modelview
	glEnable(GL_LIGHTING); 	//enable light
	glEnable(GL_LIGHT0); 		//enable gl_light0
	glEnable(GL_COLOR_MATERIAL);             // Color Material
	Display_Contol_Menu();

	glutDisplayFunc(display);        // display 
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();                     // event-processing loop
}
