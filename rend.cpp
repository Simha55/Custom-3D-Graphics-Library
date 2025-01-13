/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include <cmath> 
#include <cstdlib> 
#define PI (float) 3.14159265358979323846


// Additional Functions Start


// Function to convert degrees to radians
float degreesToRadians(float degrees) {
	return degrees * (PI / 180.0f);
}

// Function to generate a random float between min and max
float randomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Function to rotate a vector by given angles around x, y, z axes
void rotateNormal(GzCoord& normal) {
	// Generate random rotation angles between -10 and 10 degrees
	float angleX = randomFloat(-5.0f, 5.0f);
	float angleY = randomFloat(-5.0f, 5.0f);
	float angleZ = randomFloat(-5.0f, 5.0f);

	// Convert angles to radians
	angleX = degreesToRadians(angleX);
	angleY = degreesToRadians(angleY);
	angleZ = degreesToRadians(angleZ);

	// Rotation matrices
	float rotX[3][3] = {
		{1, 0, 0},
		{0, cos(angleX), -sin(angleX)},
		{0, sin(angleX), cos(angleX)}
	};

	float rotY[3][3] = {
		{cos(angleY), 0, sin(angleY)},
		{0, 1, 0},
		{-sin(angleY), 0, cos(angleY)}
	};

	float rotZ[3][3] = {
		{cos(angleZ), -sin(angleZ), 0},
		{sin(angleZ), cos(angleZ), 0},
		{0, 0, 1}
	};

	// Apply the rotations to the normal vector
	GzCoord tempNormal = { normal[0], normal[1], normal[2] };

	// Rotate around X
	normal[0] = rotX[0][0] * tempNormal[0] + rotX[0][1] * tempNormal[1] + rotX[0][2] * tempNormal[2];
	normal[1] = rotX[1][0] * tempNormal[0] + rotX[1][1] * tempNormal[1] + rotX[1][2] * tempNormal[2];
	normal[2] = rotX[2][0] * tempNormal[0] + rotX[2][1] * tempNormal[1] + rotX[2][2] * tempNormal[2];

	// Store the rotated normal temporarily
	tempNormal[0] = normal[0];
	tempNormal[1] = normal[1];
	tempNormal[2] = normal[2];

	// Rotate around Y
	normal[0] = rotY[0][0] * tempNormal[0] + rotY[0][1] * tempNormal[1] + rotY[0][2] * tempNormal[2];
	normal[1] = rotY[1][0] * tempNormal[0] + rotY[1][1] * tempNormal[1] + rotY[1][2] * tempNormal[2];
	normal[2] = rotY[2][0] * tempNormal[0] + rotY[2][1] * tempNormal[1] + rotY[2][2] * tempNormal[2];

	// Store the rotated normal temporarily
	tempNormal[0] = normal[0];
	tempNormal[1] = normal[1];
	tempNormal[2] = normal[2];

	// Rotate around Z
	normal[0] = rotZ[0][0] * tempNormal[0] + rotZ[0][1] * tempNormal[1] + rotZ[0][2] * tempNormal[2];
	normal[1] = rotZ[1][0] * tempNormal[0] + rotZ[1][1] * tempNormal[1] + rotZ[1][2] * tempNormal[2];
	normal[2] = rotZ[2][0] * tempNormal[0] + rotZ[2][1] * tempNormal[1] + rotZ[2][2] * tempNormal[2];

	// Normalize the normal vector to maintain its length
	float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	if (length > 0) {
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
	}
}

void crossProduct(GzCoord v0, GzCoord v1, GzCoord normal) {
	normal[0] = v0[1] * v1[2] - v0[2] * v1[1]; // A component
	normal[1] = v0[2] * v1[0] - v0[0] * v1[2]; // B component
	normal[2] = v0[0] * v1[1] - v0[1] * v1[0]; // C component
}

float dotProduct(const GzCoord vec1, const GzCoord vec2) {
	float result = 0.0f;
	for (int i = 0; i < 3; i++) {
		result += vec1[i] * vec2[i];
	}
	return result;
}

// Function to compute the Z value at a given pixel (x, y)
float interpolate(GzCoord v0, GzCoord v1, GzCoord v2, float x, float y) {
	GzCoord edgeVec1, edgeVec2, normal;

	// Compute two edge vectors
	edgeVec1[0] = v1[0] - v0[0];
	edgeVec1[1] = v1[1] - v0[1];
	edgeVec1[2] = v1[2] - v0[2];

	edgeVec2[0] = v2[0] - v0[0];
	edgeVec2[1] = v2[1] - v0[1];
	edgeVec2[2] = v2[2] - v0[2];

	// Compute the normal vector to the plane
	crossProduct(edgeVec1, edgeVec2, normal);

	// Coefficients A, B, and C of the plane equation
	float A = normal[0];
	float B = normal[1];
	float C = normal[2];

	// Solve for D using any vertex (let's use v0)
	float D = -(A * v0[0] + B * v0[1] + C * v0[2]);

	// Compute Z at the pixel (x, y) using the plane equation
	float z = -(A * x + B * y + D) / C;

	return z;
}

bool isTopLeft(GzCoord start, GzCoord end) {
	GzCoord direction;
	direction[0] = end[0] - start[0];
	direction[1] = end[1] - start[1];
	bool is_top_edge = (direction[1] == 0 && direction[0] > 0);
	bool is_left_edge = (direction[1] < 0);

	return is_top_edge || is_left_edge;
}


void normalize(GzCoord v) {
	float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (length != 0) {
		v[0] /= length;
		v[1] /= length;
		v[2] /= length;
	}
}

void shadingEquation(GzColor color, GzColor ka, GzColor kd, GzColor ks, GzLight* lights, GzLight ambientlight, int numlights, float spec, GzCoord normal, GzCoord eye) {
	// Resultant color

	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f; // Initialize color as black

	// Normalize the normal and eye vectors
	GzCoord shading_normal = { normal[0], normal[1], normal[2] };
	normalize(shading_normal);
	normalize(eye);

	// Ambient lighting (constant for all lights)
	color[0] += ka[0] * ambientlight.color[0];
	color[1] += ka[1] * ambientlight.color[1];
	color[2] += ka[2] * ambientlight.color[2];

	// Loop through each light source and calculate the diffuse and specular components
	for (int i = 0; i < numlights; i++) {
		// normalize the light direction
		GzCoord l = { lights[i].direction[0], lights[i].direction[1], lights[i].direction[2] };
		normalize(l);

		// compute dot products
		float nl = dotProduct(shading_normal, l);  // n dot l
		float ne = dotProduct(shading_normal, eye); // n dot e
		// check the cases based on the signs of nl and ne
		if ((nl * ne > 0)) { // case 1 and 2: both positive or negative , compute lighting as usual
			if (nl < 0 && ne < 0) {
				shading_normal[0] = -normal[0];
				shading_normal[1] = -normal[1];
				shading_normal[2] = -normal[2];
				//recalculating the dot products after flipping the normals
				nl = dotProduct(shading_normal, l);  // n dot l
				ne = dotProduct(shading_normal, eye); // n dot e
			}
			// diffuse reflection (lambert's law)
			color[0] += kd[0] * lights[i].color[0] * nl;
			color[1] += kd[1] * lights[i].color[1] * nl;
			color[2] += kd[2] * lights[i].color[2] * nl;

			// compute the reflection vector r = 2(n . l)n - l
			GzCoord r = {
				2.0f * nl * shading_normal[0] - l[0],
				2.0f * nl * shading_normal[1] - l[1],
				2.0f * nl * shading_normal[2] - l[2]
			};
			normalize(r);  // normalize the reflection vector

			// compute the dot product of r and the view vector v (eye direction)
			float rv = dotProduct(r, eye);
			rv = max(0.0f, rv);  // clamp to 0 or positive

			// specular reflection (phong model)
			float specularfactor = pow(rv, spec);
			color[0] += ks[0] * lights[i].color[0] * specularfactor;
			color[1] += ks[1] * lights[i].color[1] * specularfactor;
			color[2] += ks[2] * lights[i].color[2] * specularfactor;
		}
		else {
			// case 3: light and eye are on opposite sides, skip this light (light contributes zero)
			continue;
		}
	}

	// Clamp the color values to [0, 1]
	color[0] = min(1.0f, color[0]);
	color[1] = min(1.0f, color[1]);
	color[2] = min(1.0f, color[2]);

}


//Additional functions End



int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
	/* HW 3.1
	// Create rotate matrix : rotate along x axis
	// Pass back the matrix using mat value
	*/
	// Converting degree to radians
	float radians = degree * PI / 180.0;
	// Creating RotX matrix
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	mat[0][0] = 1.0;
	mat[1][1] = (float)cos(radians);
	mat[1][2] = -1.0 * (float)sin(radians);
	mat[2][1] = (float)sin(radians);
	mat[2][2] = (float)cos(radians);
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
	/* HW 3.2
	// Create rotate matrix : rotate along y axis
	// Pass back the matrix using mat value
	*/
	// Converting degree to radians
	float radians = degree * PI / 180.0;
	// Creating RotY matrix
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	mat[0][0] = (float)cos(radians);
	mat[0][2] = (float)sin(radians);
	mat[1][1] = 1.0;
	mat[2][0] = -1.0 * (float)sin(radians);
	mat[2][2] = (float)cos(radians);
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
	/* HW 3.3
	// Create rotate matrix : rotate along z axis
	// Pass back the matrix using mat value
	*/
	// Converting degree to radians
	float radians = degree * PI / 180.0;
	// Creating RotZ matrix
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}

	mat[0][0] = (float)cos(radians);
	mat[0][1] = -1.0 * (float)sin(radians);
	mat[1][0] = (float)sin(radians);
	mat[1][1] = (float)cos(radians);
	mat[2][2] = 1.0;
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
	/* HW 3.4
	// Create translation matrix
	// Pass back the matrix using mat value
	*/
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	mat[0][0] = 1.0;
	mat[1][1] = 1.0;
	mat[2][2] = 1.0;
	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
	/* HW 3.5
	// Create scaling matrix
	// Pass back the matrix using mat value
	*/
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	 -- set display resolution
	 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	 -- allocate memory for pixel buffer
	 */
	 //framebuffer = (char*) malloc (3 * sizeof(char) * xRes * yRes);
	xres = (unsigned short)xRes;
	yres = (unsigned short)yRes;
	// Intialized framebuffer with specified res
	framebuffer = new char[3 * xres * yres];
	// As GzPixel struct containes different variables on rgbaz values, I only initialized xRes * yRes raather than multiplying with 3
	pixelbuffer = new GzPixel[xres * yres];

	/* HW 3.6
	- setup Xsp and anything only done once
	- init default camera
	*/
	//Initializing Camera Params
	m_camera.position[0] = DEFAULT_IM_X;
	m_camera.position[1] = DEFAULT_IM_Y;
	m_camera.position[2] = DEFAULT_IM_Z;

	m_camera.lookat[0] = 0;
	m_camera.lookat[1] = 0;
	m_camera.lookat[2] = 0;

	m_camera.worldup[0] = 0;
	m_camera.worldup[1] = 1;
	m_camera.worldup[2] = 0;

	m_camera.FOV = DEFAULT_FOV;

	// Initializin stack level
	matlevel = -1;
	// Initialize num lights
	numlights = 0;
	//Setting up Xsp
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Xsp[i][j] = 0;
		}
	}
	Xsp[0][0] = (float)xres / 2.0;
	Xsp[0][3] = (float)xres / 2.0;
	Xsp[1][1] = -1.0 * (float)yres / 2.0;
	Xsp[1][3] = (float)yres / 2.0;
	Xsp[2][2] = (float)MAXINT;
	Xsp[3][3] = 1.0;

}

GzRender::~GzRender()
{
	/* HW1.2 clean up, free buffer memory */
	delete[] framebuffer;
	delete[] pixelbuffer;
}

int GzRender::GzDefault()
{
	/* HW1.3 set pixel buffer to some default values - start a new frame */
	GzPixel defaultValue;
	defaultValue.red = 700;
	defaultValue.blue = 700;
	defaultValue.green = 700;
	defaultValue.alpha = 1;
	defaultValue.z = INT_MAX;

	int resolution = xres * yres;

	for (int i = 0; i < resolution; i++) {
		pixelbuffer[i] = defaultValue;
		framebuffer[3 * i] = (char)700;
		framebuffer[3 * i + 1] = (char)700;
		framebuffer[3 * i + 2] = (char)700;
	}
	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
	/* HW 3.7
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/
	// Calculating Xiw
	// Step-1: computing Camera Z-axis
	GzCoord cam_Z_axis, center_lookat;
	for (int i = 0; i < 3; i++) {
		center_lookat[i] = m_camera.lookat[i] - m_camera.position[i];
	}
	float cl_magnitude = 0.0f;
	for (int i = 0; i < 3; i++) {
		cl_magnitude += center_lookat[i] * center_lookat[i];
	}
	cl_magnitude = sqrt(cl_magnitude);
	for (int i = 0; i < 3; i++) {
		cam_Z_axis[i] = center_lookat[i] / cl_magnitude;
	}
	// Step-2: Computing new up' vector
	GzCoord new_worldup;
	float up_Z_dotProduct = dotProduct(m_camera.worldup, cam_Z_axis);
	for (int i = 0; i < 3; i++) {
		new_worldup[i] = m_camera.worldup[i] - up_Z_dotProduct * cam_Z_axis[i];
	}
	// Step-3: Computing the Camera Y axis
	GzCoord cam_Y_axis;
	float new_worldup_magnitude = 0.0f;
	for (int i = 0; i < 3; i++) {
		new_worldup_magnitude += new_worldup[i] * new_worldup[i];
	}
	new_worldup_magnitude = sqrt(new_worldup_magnitude);
	for (int i = 0; i < 3; i++) {
		cam_Y_axis[i] = new_worldup[i] / new_worldup_magnitude;
	}
	// Step-4: Computing Camera X axis
	GzCoord cam_X_axis;
	crossProduct(cam_Y_axis, cam_Z_axis, cam_X_axis);
	// Computing Xiw
	m_camera.Xiw[0][0] = cam_X_axis[0];
	m_camera.Xiw[0][1] = cam_X_axis[1];
	m_camera.Xiw[0][2] = cam_X_axis[2];
	m_camera.Xiw[1][0] = cam_Y_axis[0];
	m_camera.Xiw[1][1] = cam_Y_axis[1];
	m_camera.Xiw[1][2] = cam_Y_axis[2];
	m_camera.Xiw[2][0] = cam_Z_axis[0];
	m_camera.Xiw[2][1] = cam_Z_axis[1];
	m_camera.Xiw[2][2] = cam_Z_axis[2];
	m_camera.Xiw[0][3] = -1.0 * (cam_X_axis[0] * m_camera.position[0] + cam_X_axis[1] * m_camera.position[1] + cam_X_axis[2] * m_camera.position[2]);
	m_camera.Xiw[1][3] = -1.0 * (cam_Y_axis[0] * m_camera.position[0] + cam_Y_axis[1] * m_camera.position[1] + cam_Y_axis[2] * m_camera.position[2]);
	m_camera.Xiw[2][3] = -1.0 * (cam_Z_axis[0] * m_camera.position[0] + cam_Z_axis[1] * m_camera.position[1] + cam_Z_axis[2] * m_camera.position[2]);
	m_camera.Xiw[3][0] = 0;
	m_camera.Xiw[3][1] = 0;
	m_camera.Xiw[3][2] = 0;
	m_camera.Xiw[3][3] = 1;


	// Calculating Xpi
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m_camera.Xpi[i][j] = 0;
		}
	}
	double FOV_to_radians = (double)m_camera.FOV * PI / 180.0;
	float xpi_value = (float)tan(FOV_to_radians / 2.0);
	m_camera.Xpi[0][0] = 1;
	m_camera.Xpi[1][1] = 1;
	m_camera.Xpi[2][2] = xpi_value;
	m_camera.Xpi[3][3] = 1;
	m_camera.Xpi[3][2] = xpi_value;

	int check = 0;
	check = GzPushMatrix(Xsp);
	if (check) {
		return GZ_FAILURE;
	}
	check = GzPushMatrix(m_camera.Xpi);
	if (check) {
		return GZ_FAILURE;
	}
	check = GzPushMatrix(m_camera.Xiw);
	if (check) {
		return GZ_FAILURE;
	}


	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
	/* HW 3.8
	/*- overwrite renderer camera structure with new camera definition
	*/
	m_camera.FOV = camera.FOV;

	for (int i = 0; i < 3; i++) {
		m_camera.lookat[i] = camera.lookat[i];
	}
	for (int i = 0; i < 3; i++) {
		m_camera.worldup[i] = camera.worldup[i];
	}
	for (int i = 0; i < 3; i++) {
		m_camera.position[i] = camera.position[i];
	}

	return GZ_SUCCESS;
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
	/* HW 3.9
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	matlevel++;
	if (matlevel >= MATLEVELS) {
		return GZ_FAILURE;
	}
	else {
		// For Ximage stack
		if (matlevel == 0) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					Ximage[matlevel][i][j] = matrix[i][j];
				}
			}
		}
		else {
			GzMatrix new_matrix;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					new_matrix[i][j] = 0;
					for (int k = 0; k < 4; k++) {
						new_matrix[i][j] += Ximage[matlevel - 1][i][k] * matrix[k][j];
					}
				}
			}
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					Ximage[matlevel][i][j] = new_matrix[i][j];
				}
			}
		}




		// for Xnorm stack

		GzMatrix identity_matrix;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					identity_matrix[i][j] = 1;
				}
				else {
					identity_matrix[i][j] = 0;
				}
			}
		}

		if (matlevel == 0 || matlevel == 1) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					Xnorm[matlevel][i][j] = identity_matrix[i][j];
				}
			}
		}

		else if (matlevel == 2) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					Xnorm[matlevel][i][j] = matrix[i][j];
				}
			}
			// Set translational part of Xiw to 0
			Xnorm[matlevel][0][3] = 0;
			Xnorm[matlevel][1][3] = 0;
			Xnorm[matlevel][2][3] = 0;
		}

		else {
			GzMatrix new_matrix;
			GzMatrix copy_matrix; // creating a copy of the matrix , just for the sake of not altering it

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					copy_matrix[i][j] = matrix[i][j];
				}
			}
			//Setting the translation to zero, cause normals dont need translations
			copy_matrix[0][3] = 0;
			copy_matrix[1][3] = 0;
			copy_matrix[2][3] = 0;
			copy_matrix[3][3] = 1;
			// Finding the scale of each row
			float scale[4];
			for (int i = 0; i < 4; i++) {
				scale[i] = sqrt(matrix[i][0] * matrix[i][0] +
					matrix[i][1] * matrix[i][1] +
					matrix[i][2] * matrix[i][2]);
			}
			// normalizing each row with the scale, cause we only need rotations
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (scale[i] != 0) {  // Avoid division by zero
						copy_matrix[i][j] = copy_matrix[i][j] / scale[i];
					}
					else {
						copy_matrix[i][j] = copy_matrix[i][j];  // If scale is 0, leave it unchanged
					}
				}
			}

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					new_matrix[i][j] = 0;
					for (int k = 0; k < 4; k++) {
						new_matrix[i][j] += Xnorm[matlevel - 1][i][k] * copy_matrix[k][j];
					}
				}
			}
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					Xnorm[matlevel][i][j] = new_matrix[i][j];
				}
			}
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
	/* HW 3.10
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/
	matlevel--;
	if (matlevel < 0) {
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	/* HW1.4 write pixel values into the buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		// Z value checking
		if (z < pixelbuffer[index].z) {
			//clipping values
			if (r > 4095) {
				r = 4095;
			}
			if (g > 4095) {
				g = 4095;
			}
			if (b > 4095) {
				b = 4095;
			}
			if (r < 0) {
				r = 0;
			}
			if (g < 0) {
				g = 0;
			}
			if (b < 0) {
				b = 0;
			}
			pixelbuffer[index] = GzPixel{ r, g, b, a, z };
		}
	}
	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity* r, GzIntensity* g, GzIntensity* b, GzIntensity* a, GzDepth* z)
{
	/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		*r = pixelbuffer[index].red;
		*g = pixelbuffer[index].green;
		*b = pixelbuffer[index].blue;
		*a = pixelbuffer[index].alpha;
		*z = pixelbuffer[index].z;
	}
	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
	/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\r", xres, yres);
	for (int i = 0; i < xres * yres; ++i) {
		unsigned char r = static_cast<unsigned char>((pixelbuffer[i].red >> 4) & 0xff);
		unsigned char g = static_cast<unsigned char>((pixelbuffer[i].green >> 4) & 0xff);
		unsigned char b = static_cast<unsigned char>((pixelbuffer[i].blue >> 4) & 0xff);


		fwrite(&r, sizeof(unsigned char), 1, outfile);
		fwrite(&g, sizeof(unsigned char), 1, outfile);
		fwrite(&b, sizeof(unsigned char), 1, outfile);
	}
	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
	/* HW1.7 write pixels to framebuffer:
		- put the pixels into the frame buffer
		- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red
		- NOT red, green, and blue !!!
	*/
	for (int i = 0; i < xres * yres; ++i) {
		unsigned char r = static_cast<unsigned char>((pixelbuffer[i].red >> 4) & 0xff);
		unsigned char g = static_cast<unsigned char>((pixelbuffer[i].green >> 4) & 0xff);
		unsigned char b = static_cast<unsigned char>((pixelbuffer[i].blue >> 4) & 0xff);
		framebuffer[3 * i] = b;
		framebuffer[3 * i + 1] = g;
		framebuffer[3 * i + 2] = r;
	}
	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken* nameList, GzPointer* valueList)
{
	/* HW 2.1
	-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	-- In later homeworks set shaders, interpolaters, texture maps, and lights
	*/
	GzColor* color = nullptr;
	for (int i = 0; i < numAttributes; i++) {
		switch (nameList[i]) {
		case GZ_RGB_COLOR:
			color = (GzColor*)valueList[i];
			flatcolor[0] = (*color)[0];
			flatcolor[1] = (*color)[1];
			flatcolor[2] = (*color)[2];
			break;

		case GZ_INTERPOLATE: {
			int* temp = (int*)valueList[i];
			interp_mode = *temp;
			break;
		}

		case GZ_DIRECTIONAL_LIGHT: {
			GzLight* temp_light = (GzLight*)valueList[i];
			lights[numlights].direction[0] = temp_light->direction[0];
			lights[numlights].direction[1] = temp_light->direction[1];
			lights[numlights].direction[2] = temp_light->direction[2];

			lights[numlights].color[0] = temp_light->color[0];
			lights[numlights].color[1] = temp_light->color[1];
			lights[numlights].color[2] = temp_light->color[2];

			numlights++;
			break;
		}

		case GZ_AMBIENT_LIGHT: {
			GzLight* temp_amb_light = (GzLight*)valueList[i];
			ambientlight.direction[0] = temp_amb_light->direction[0];
			ambientlight.direction[1] = temp_amb_light->direction[1];
			ambientlight.direction[2] = temp_amb_light->direction[2];

			ambientlight.color[0] = temp_amb_light->color[0];
			ambientlight.color[1] = temp_amb_light->color[1];
			ambientlight.color[2] = temp_amb_light->color[2];
			break;
		}

		case GZ_AMBIENT_COEFFICIENT: {
			GzColor* temp_amb = (GzColor*)valueList[i];
			Ka[0] = (*temp_amb)[0];
			Ka[1] = (*temp_amb)[1];
			Ka[2] = (*temp_amb)[2];
			break;
		}

		case GZ_DIFFUSE_COEFFICIENT: {
			GzColor* temp_diff = (GzColor*)valueList[i];
			Kd[0] = (*temp_diff)[0];
			Kd[1] = (*temp_diff)[1];
			Kd[2] = (*temp_diff)[2];
			break;
		}

		case GZ_SPECULAR_COEFFICIENT: {
			GzColor* temp_spec = (GzColor*)valueList[i];
			Ks[0] = (*temp_spec)[0];
			Ks[1] = (*temp_spec)[1];
			Ks[2] = (*temp_spec)[2];
			break;
		}

		case GZ_DISTRIBUTION_COEFFICIENT: {
			float* spec_power = (float*)valueList[i];
			spec = *spec_power;
			break;
		}

		case GZ_TEXTURE_MAP: {
			GzTexture texture_func = (GzTexture)valueList[i];
			tex_fun = texture_func;
			break;
		}
		case GZ_AASHIFTX: {
			float* shiftX = (float*)valueList[i];
			Xoffset = *shiftX;
		}
						break;
		case GZ_AASHIFTY: {
			float* shiftY = (float*)valueList[i];
			Yoffset = *shiftY;
		}
						break;

		default:
			fprintf(stderr, "Error: Unsupported token %d\n", nameList[i]);
			break;
		}
	}
	return GZ_SUCCESS;
}




int GzRender::GzPutTriangle(int numParts, GzToken* nameList, GzPointer* valueList)
/* numParts - how many names and values */
{
	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
		  GZ_NULL_TOKEN:		do nothing - no values
		  GZ_POSITION:		3 vert positions in model space
	-- Invoke the rastrizer/scanline framework
	-- Return error code
	*/
	GzCoord* normals = nullptr; // Declare a pointer for normals
	GzCoord* vertex = nullptr;
	GzTextureIndex* uv_index = nullptr;
	GzCoord eye = { 0.0f, 0.0f, -1.0f };  // View direction
	// Process input tokens
	for (int i = 0; i < numParts; i++) {
		if (nameList[i] == GZ_NULL_TOKEN) continue;

		if (nameList[i] == GZ_NORMAL) {
			normals = (GzCoord*)valueList[i];  // Read normals
		}

		if (nameList[i] == GZ_POSITION) {
			vertex = (GzCoord*)valueList[i];   // Read vertex positions
		}

		if (nameList[i] == GZ_TEXTURE_INDEX) {
			uv_index = (GzTextureIndex*)valueList[i];   // Read UV coords
		}
	}

	// Ensure both vertex positions and normals are available
	if (!vertex || !normals || !uv_index) {
		return GZ_FAILURE;
	}


	//Converting the 3D vertices in model space to 4D and same goes for normals as well
	float vertex_4D[3][4], normals_4D[3][4];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			if (j != 3) {
				vertex_4D[i][j] = vertex[i][j];
				normals_4D[i][j] = normals[i][j];
			}
			else {
				vertex_4D[i][j] = 1.0;
				normals_4D[i][j] = 1.0;
			}

		}
	}
	// Multiplying with Ximage with vertices and Xnorm with normals
	float transformed_vertex_4D[3][4], transformed_normals_4D[3][4];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			float vertex_sum = 0, normals_sum = 0;
			for (int k = 0; k < 4; k++) {
				vertex_sum += Ximage[matlevel][j][k] * vertex_4D[i][k];
				normals_sum += Xnorm[matlevel][j][k] * normals_4D[i][k];
			}
			transformed_vertex_4D[i][j] = vertex_sum;
			transformed_normals_4D[i][j] = normals_sum;
		}
	}
	// Ignoring negaive z vertices
	for (int i = 0; i < 3; i++) {
		if (transformed_vertex_4D[i][2] < 0) {
			return GZ_FAILURE;
		}
	}
	// transforming 4D vertices and normals back to 3D
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			vertex[i][j] = transformed_vertex_4D[i][j] / transformed_vertex_4D[i][3];
			normals[i][j] = transformed_normals_4D[i][j] / transformed_normals_4D[i][3];

		}
		vertex[i][0] -= Xoffset;
		vertex[i][1] -= Yoffset;
	}
	// Computing the bounding box
	int minX = floor(min(min(vertex[0][0], vertex[1][0]), vertex[2][0]));
	int maxX = ceil(max(max(vertex[0][0], vertex[1][0]), vertex[2][0]));
	int minY = floor(min(min(vertex[0][1], vertex[1][1]), vertex[2][1]));
	int maxY = ceil(max(max(vertex[0][1], vertex[1][1]), vertex[2][1]));

	// Clipping within frame
	minX = max(minX, 0);
	minY = max(minY, 0);
	maxX = min(maxX, xres - 1);
	maxY = min(maxY, yres - 1);

	//Initializing sorted_normals
	GzCoord sorted_normals[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			sorted_normals[i][j] = normals[i][j];
		}
	}

	GzTextureIndex sorted_uv_index[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			sorted_uv_index[i][j] = uv_index[i][j];
		}
	}
	// Sorting vertices based on Y values (and normals too)
	GzCoord temp_vertex, temp_normal;
	GzTextureIndex temp_uv_index;
	for (int i = 0; i < 2; i++) {
		for (int j = i + 1; j < 3; j++) {
			if (vertex[j][1] < vertex[i][1]) { // Sort based on Y values
				// Swapping vertices
				for (int k = 0; k < 3; k++) {
					temp_vertex[k] = vertex[i][k];
					vertex[i][k] = vertex[j][k];
					vertex[j][k] = temp_vertex[k];

					// Swapping normals
					temp_normal[k] = sorted_normals[i][k];
					sorted_normals[i][k] = sorted_normals[j][k];
					sorted_normals[j][k] = temp_normal[k];
				}

				// Swapping uv_index (u, v are 2 components, so loop over 2)
				for (int k = 0; k < 2; k++) {
					temp_uv_index[k] = sorted_uv_index[i][k];
					sorted_uv_index[i][k] = sorted_uv_index[j][k];
					sorted_uv_index[j][k] = temp_uv_index[k];
				}
			}
		}
	}
	// L/R relationship
	// Mid-Y vertex (V1)
	float x1 = vertex[1][0];
	float y1 = vertex[1][1];

	//// Long edge (V2-V3)
	float x2 = vertex[0][0];
	float y2 = vertex[0][1];
	float x3 = vertex[2][0];
	float y3 = vertex[2][1];

	//// Compute line equation for edge V2-V3
	float dX = x2 - x3;
	float dY = y2 - y3;
	float A = dY;                // Coefficient A
	float B = -dX;               // Coefficient B
	float C = dX * y3 - dY * x3; // Coefficient C

	//// Compute Xp for y = Y1
	float Xp = -(B * y1 + C) / A;
	bool right = false;
	if (Xp < x1) right = true;

	auto edgeFunction = [](GzCoord v1, GzCoord v0, GzCoord p) {
		// Always make sure that the dY, dX defines the direction of vector
		// If there is a vector v1->v2 then dY = y2-y1 and dX = x2 - x1. 
		return (p[0] - v0[0]) * (v1[1] - v0[1]) - (p[1] - v0[1]) * (v1[0] - v0[0]);
		};


	//Gouraud Shading
	GzColor rgb_normal_0;
	GzColor rgb_normal_1;
	GzColor rgb_normal_2;

	if (tex_fun != nullptr) {
		GzColor tex_Ka = { 1.0, 1.0, 1.0 };
		GzColor tex_Kd = { 1.0, 1.0, 1.0 };
		GzColor tex_Ks = { 1.0, 1.0, 1.0 };
		shadingEquation(rgb_normal_0, tex_Ka, tex_Kd, tex_Ks, lights, ambientlight, numlights, spec, sorted_normals[0], eye);
		shadingEquation(rgb_normal_1, tex_Ka, tex_Kd, tex_Ks, lights, ambientlight, numlights, spec, sorted_normals[1], eye);
		shadingEquation(rgb_normal_2, tex_Ka, tex_Kd, tex_Ks, lights, ambientlight, numlights, spec, sorted_normals[2], eye);
	}
	else {
		shadingEquation(rgb_normal_0, Ka, Kd, Ks, lights, ambientlight, numlights, spec, sorted_normals[0], eye);
		shadingEquation(rgb_normal_1, Ka, Kd, Ks, lights, ambientlight, numlights, spec, sorted_normals[1], eye);
		shadingEquation(rgb_normal_2, Ka, Kd, Ks, lights, ambientlight, numlights, spec, sorted_normals[2], eye);
	}

	// Red color interpolation
	GzCoord red_vertex[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			red_vertex[i][j] = vertex[i][j];
		}
	}
	// overwriting the 3rd index of all vertices with red color
	red_vertex[0][2] = rgb_normal_0[0];
	red_vertex[1][2] = rgb_normal_1[0];
	red_vertex[2][2] = rgb_normal_2[0];

	// Green color interpolation
	GzCoord green_vertex[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {

			green_vertex[i][j] = vertex[i][j];

		}
	}
	// overwriting the 3rd index of all vertices with green color
	green_vertex[0][2] = rgb_normal_0[1];
	green_vertex[1][2] = rgb_normal_1[1];
	green_vertex[2][2] = rgb_normal_2[1];

	// Blue color interpolation
	GzCoord blue_vertex[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {

			blue_vertex[i][j] = vertex[i][j];

		}
	}
	// overwriting the 3rd index of all vertices with blue color
	blue_vertex[0][2] = rgb_normal_0[2];
	blue_vertex[1][2] = rgb_normal_1[2];
	blue_vertex[2][2] = rgb_normal_2[2];

	// Perspective z interpolation factor --> z_prime

	float z_prime_1 = vertex[0][2] / (INT_MAX - vertex[0][2]); // for perspective interpolation of 1st coordinate
	float z_prime_2 = vertex[1][2] / (INT_MAX - vertex[1][2]); // for perspective interpolation of 2nd coordinate
	float z_prime_3 = vertex[2][2] / (INT_MAX - vertex[2][2]); // for perspective interpolation of 3rd coordinate

	//Phong Shading
	// Normal x interpolation
	GzCoord phong_vertex_normal_x[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			phong_vertex_normal_x[i][j] = vertex[i][j];
		}
	}
	// overwriting 3rd index with normal x coordinates
	phong_vertex_normal_x[0][2] = sorted_normals[0][0] / (z_prime_1 + 1);
	phong_vertex_normal_x[1][2] = sorted_normals[1][0] / (z_prime_2 + 1);
	phong_vertex_normal_x[2][2] = sorted_normals[2][0] / (z_prime_3 + 1);

	// Normal y interpolation
	GzCoord phong_vertex_normal_y[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			phong_vertex_normal_y[i][j] = vertex[i][j];
		}
	}
	// overwriting 3rd index with normal y coordinates
	phong_vertex_normal_y[0][2] = sorted_normals[0][1] / (z_prime_1 + 1);
	phong_vertex_normal_y[1][2] = sorted_normals[1][1] / (z_prime_2 + 1);
	phong_vertex_normal_y[2][2] = sorted_normals[2][1] / (z_prime_3 + 1);

	// Normal z interpolation
	GzCoord phong_vertex_normal_z[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			phong_vertex_normal_z[i][j] = vertex[i][j];
		}
	}
	// overwriting 3rd index with normal z coordinates
	phong_vertex_normal_z[0][2] = sorted_normals[0][2] / (z_prime_1 + 1);
	phong_vertex_normal_z[1][2] = sorted_normals[1][2] / (z_prime_2 + 1);
	phong_vertex_normal_z[2][2] = sorted_normals[2][2] / (z_prime_3 + 1);

	// Seting colors based on UV values
	GzCoord uv_vertex_u[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			uv_vertex_u[i][j] = vertex[i][j];
		}
	}
	// Overwriting 3rd index with u-coordinates of texture
	uv_vertex_u[0][2] = sorted_uv_index[0][0] / (z_prime_1 + 1); // u for vertex 0
	uv_vertex_u[1][2] = sorted_uv_index[1][0] / (z_prime_2 + 1); // u for vertex 1
	uv_vertex_u[2][2] = sorted_uv_index[2][0] / (z_prime_3 + 1); // u for vertex 2

	// v coordinate interpolation
	GzCoord uv_vertex_v[3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			uv_vertex_v[i][j] = vertex[i][j];
		}
	}
	// Overwriting 3rd index with v-coordinates of texture
	uv_vertex_v[0][2] = sorted_uv_index[0][1] / (z_prime_1 + 1); // v for vertex 0
	uv_vertex_v[1][2] = sorted_uv_index[1][1] / (z_prime_2 + 1); // v for vertex 1
	uv_vertex_v[2][2] = sorted_uv_index[2][1] / (z_prime_3 + 1); // v for vertex 2

	float w0, w1, w2;
	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			GzCoord p = { static_cast<float>(x), static_cast<float>(y), INT_MAX };

			// Compute w0, w1, w2 based on the 'right' flag
			if (right) {
				w0 = edgeFunction(vertex[1], vertex[0], p);
				w1 = edgeFunction(vertex[2], vertex[1], p);
				w2 = edgeFunction(vertex[0], vertex[2], p);
			}
			else {
				w0 = edgeFunction(vertex[2], vertex[0], p);
				w1 = edgeFunction(vertex[1], vertex[2], p);
				w2 = edgeFunction(vertex[0], vertex[1], p);
			}

			// Check if the point is inside the triangle
			if (w0 <= 0 && w1 <= 0 && w2 <= 0) {
				// Check top-left rules
				if ((w0 == 0 && isTopLeft(vertex[0], vertex[right ? 1 : 2])) ||
					(w1 == 0 && isTopLeft(vertex[right ? 1 : 2], vertex[right ? 2 : 1])) ||
					(w2 == 0 && isTopLeft(vertex[right ? 2 : 1], vertex[0]))) {
					continue;
				}


				// Calculate z value and render pixel
				float z = interpolate(vertex[0], vertex[1], vertex[2], x, y);
				float z_prime = z / (INT_MAX - z);
				int roundedZ = static_cast<int>(round(z));

				// Set pixel color
				GzIntensity r = ctoi(flatcolor[0]);
				GzIntensity g = ctoi(flatcolor[1]);
				GzIntensity b = ctoi(flatcolor[2]);
				GzIntensity a = 1;

				// Seting colors based on normal values

				//if (normals != nullptr) {
				//	r = ctoi(normals[0][0]);  // convert normal x to red
				//	g = ctoi(normals[0][1]);  // convert normal y to green
				//	b = ctoi(normals[0][2]);  // convert normal z to blue
				//	a = 1;
				//}
				// 

				//flat shading
				if (interp_mode == GZ_FLAT) {
					GzColor shaded_flat_color;
					shadingEquation(shaded_flat_color, Ka, Kd, Ks, lights, ambientlight, numlights, spec, normals[0], eye);

					r = ctoi(shaded_flat_color[0]);  // Red channel
					g = ctoi(shaded_flat_color[1]);  // Green channel
					b = ctoi(shaded_flat_color[2]);  // Blue channel
					a = 1;  // Alpha channel is fully opaque

					// Texture coloring
					GzColor uv_color;
					if (tex_fun != nullptr) {
						float interpolated_u = interpolate(uv_vertex_u[0], uv_vertex_u[1], uv_vertex_u[2], x, y);
						float interpolated_v = interpolate(uv_vertex_v[0], uv_vertex_v[1], uv_vertex_v[2], x, y);
						tex_fun(interpolated_u * (z_prime + 1), interpolated_v * (z_prime + 1), uv_color);

						GzColor Kt;
						for (int i = 0; i < 3; i++) {
							Kt[i] = 1;
						}
						shadingEquation(shaded_flat_color, Kt, Kt, Kt, lights, ambientlight, numlights, spec, normals[0], eye);
						
						r = ctoi(min(1.0, shaded_flat_color[0] * uv_color[0]));  // Red channel
						g = ctoi(min(1.0, shaded_flat_color[1] * uv_color[1]));  // Green channel
						b = ctoi(min(1.0, shaded_flat_color[2] * uv_color[2])); // Blue channel
						a = 1;  // Alpha channel is fully opaque

					}

				}

				//Gouraud shading
				GzColor Kt;
				if (interp_mode == GZ_COLOR) {
			
					float red = interpolate(red_vertex[0], red_vertex[1], red_vertex[2], x, y);


					float green = interpolate(green_vertex[0], green_vertex[1], green_vertex[2], x, y);

					float blue = interpolate(blue_vertex[0], blue_vertex[1], blue_vertex[2], x, y);
					r = ctoi(red);  // Red channel
					g = ctoi(green);  // Green channel
					b = ctoi(blue); // Blue channel
					a = 1;  // Alpha channel is fully opaque

					// Texture coloring
					GzColor uv_color;
					if (tex_fun != nullptr) {
						float interpolated_u = interpolate(uv_vertex_u[0], uv_vertex_u[1], uv_vertex_u[2], x, y);
						float interpolated_v = interpolate(uv_vertex_v[0], uv_vertex_v[1], uv_vertex_v[2], x, y);
						tex_fun(interpolated_u * (z_prime + 1), interpolated_v * (z_prime + 1), uv_color);

						/*r = ctoi(uv_color[0]);
						g = ctoi(uv_color[1]);
						b = ctoi(uv_color[2]);
						a = 1;*/
						for (int i = 0; i < 3; i++) {
							Kt[i] = uv_color[i];
						}
						r = ctoi(min(1.0, red * Kt[0]));  // Red channel
						g = ctoi(min(1.0, green * Kt[1]));  // Green channel
						b = ctoi(min(1.0, blue * Kt[2])); // Blue channel
						a = 1;  // Alpha channel is fully opaque

					}

				}

				//phong shading
				if (interp_mode == GZ_NORMALS) {
					
					float final_normal_x = interpolate(phong_vertex_normal_x[0], phong_vertex_normal_x[1], phong_vertex_normal_x[2], x, y);
					
					float final_normal_y = interpolate(phong_vertex_normal_y[0], phong_vertex_normal_y[1], phong_vertex_normal_y[2], x, y);
					
					float final_normal_z = interpolate(phong_vertex_normal_z[0], phong_vertex_normal_z[1], phong_vertex_normal_z[2], x, y);

					GzCoord interpolated_normal = { final_normal_x * (z_prime + 1) , final_normal_y * (z_prime + 1), final_normal_z * (z_prime + 1) };
					/*
					This is a fun experiment, to create a rough surface on teapot
					Uncomment the code below
					*/
					//rotateNormal(interpolated_normal);
					
					// Texture coloring
					GzColor uv_color;
					if (tex_fun != nullptr) {
						float interpolated_u = interpolate(uv_vertex_u[0], uv_vertex_u[1], uv_vertex_u[2], x, y);
						float interpolated_v = interpolate(uv_vertex_v[0], uv_vertex_v[1], uv_vertex_v[2], x, y);
						tex_fun(interpolated_u * (z_prime + 1), interpolated_v * (z_prime + 1), uv_color);

						/*r = ctoi(uv_color[0]);
						g = ctoi(uv_color[1]);
						b = ctoi(uv_color[2]);
						a = 1;*/
						for (int i = 0; i < 3; i++) {
							Kd[i] = uv_color[i];
							Ka[i] = uv_color[i];
						}

					}
					GzColor phong_color;
					// normals will be normalized in shading function
					shadingEquation(phong_color, Ka, Kd, Ks, lights, ambientlight, numlights, spec, interpolated_normal, eye);
					r = ctoi(phong_color[0]);
					g = ctoi(phong_color[1]);
					b = ctoi(phong_color[2]);
					a = 1;
				}
				
				 

				GzPut(x, y, r, g, b, a, roundedZ);
			}

		}
	}


	return GZ_SUCCESS;
}


