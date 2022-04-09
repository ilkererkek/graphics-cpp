#include "olcConsoleGameEngine.h";
using namespace std;

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d vectors[3];
};

struct mesh
{
	vector<triangle> triangles;
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine
{
	public:
		olcEngine3D()
		{
			m_sAppName = L"3D Demo";
		}


	private:
		mesh cubeMesh;
		mat4x4 projMat;

		float fTheta;
		//Multiplies 4x4 projection matrix with 3x1 Vector.
		void ProjMul(vec3d& i, vec3d& o, mat4x4& m)
		{
			//Epsilon to prevent division by zero errors.
			float fEpsilon = 0.000001f;
			o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
			o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
			o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
			float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3] + fEpsilon;

			o.x /= w; o.y /= w; o.z /= w;
		}

	public:
		bool OnUserCreate() override
		{
			cubeMesh.triangles = {

				// SOUTH FACE
				{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

				// EAST FACE                                                 
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

				// NORTH FACE                                                 
				{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

				// WEST FACE                                                
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

				// TOP FACE                                                
				{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

				// BOTTOM FACE                                                 
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			};
			//Defining Projection Matrix.
			float fNear = 0.1f;
			float fFar = 1000.0f;
			float fFov = 90.0f;
			float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
			float fFovRadian = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

			projMat.m[0][0] = fAspectRatio * fFovRadian;
			projMat.m[1][1] = fFovRadian;
			projMat.m[2][2] = fFar / (fFar - fNear);
			projMat.m[3][2] = - projMat.m[2][2] *  fNear;
			projMat.m[2][3] = 1.0f;

			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override
		{
			//Fill the screen.
			Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
			//Rotation Matrices.
			mat4x4 matRotZ, matRotX;
			fTheta += 1.0f * fElapsedTime;

			matRotZ.m[0][0] = cosf(fTheta);
			matRotZ.m[0][1] = sinf(fTheta);
			matRotZ.m[1][0] = -sinf(fTheta);
			matRotZ.m[1][1] = cosf(fTheta);
			matRotZ.m[2][2] = 1;
			matRotZ.m[3][3] = 1;
			matRotX.m[0][0] = 1;
			matRotX.m[1][1] = cosf(fTheta * 0.5f);
			matRotX.m[1][2] = sinf(fTheta * 0.5f);
			matRotX.m[2][1] = -sinf(fTheta * 0.5f);
			matRotX.m[2][2] = cosf(fTheta * 0.5f);
			matRotX.m[3][3] = 1;

			for (auto tri : cubeMesh.triangles)
			{
				triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

				ProjMul(tri.vectors[0], triRotatedZ.vectors[0], matRotZ);
				ProjMul(tri.vectors[1], triRotatedZ.vectors[1], matRotZ);
				ProjMul(tri.vectors[2], triRotatedZ.vectors[2], matRotZ);
				ProjMul(triRotatedZ.vectors[0], triRotatedZX.vectors[0], matRotX);
				ProjMul(triRotatedZ.vectors[1], triRotatedZX.vectors[1], matRotX);
				ProjMul(triRotatedZ.vectors[2], triRotatedZX.vectors[2], matRotX);

				triTranslated = triRotatedZX;
				triTranslated.vectors[0].z = triRotatedZX.vectors[0].z + 3.0f;
				triTranslated.vectors[1].z = triRotatedZX.vectors[1].z + 3.0f;
				triTranslated.vectors[2].z = triRotatedZX.vectors[2].z + 3.0f;

				ProjMul(triTranslated.vectors[0], triProjected.vectors[0], projMat);
				ProjMul(triTranslated.vectors[1], triProjected.vectors[1], projMat);
				ProjMul(triTranslated.vectors[2], triProjected.vectors[2], projMat);

				triProjected.vectors[0].x += 1.0f; triProjected.vectors[0].y += 1.0f;
				triProjected.vectors[1].x += 1.0f; triProjected.vectors[1].y += 1.0f;
				triProjected.vectors[2].x += 1.0f; triProjected.vectors[2].y += 1.0f;
				triProjected.vectors[0].x *= 0.5f * (float)ScreenWidth();
				triProjected.vectors[0].y *= 0.5f * (float)ScreenHeight();
				triProjected.vectors[1].x *= 0.5f * (float)ScreenWidth();
				triProjected.vectors[1].y *= 0.5f * (float)ScreenHeight();
				triProjected.vectors[2].x *= 0.5f * (float)ScreenWidth();
				triProjected.vectors[2].y *= 0.5f * (float)ScreenHeight();

				DrawTriangle(triProjected.vectors[0].x, triProjected.vectors[0].y,
					triProjected.vectors[1].x, triProjected.vectors[1].y,
					triProjected.vectors[2].x, triProjected.vectors[2].y,
					PIXEL_SOLID, FG_WHITE);

			}


			return true;
		}

};

int main()
{
	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 4, 4)) {
		demo.Start();
	}
	else {
		std::cerr << "Error starting demo" << std::endl;
		return 1;
	}
	return 0;
}

