#include "olcConsoleGameEngine.h";
#include <fstream>
#include <strstream>
using namespace std;

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d vectors[3];
	wchar_t symb;
	short col;
};

struct mesh
{
	vector<triangle> triangles;

	bool LoadFromObjectFile(string sFilename)
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		vector<vec3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char temp;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> temp >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> temp >> f[0] >> f[1] >> f[2];
				triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}
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

		vec3d vCamera;
		//Multiplies 4x4 projection matrix with 3x1 Vector.
		void MatMul(vec3d& i, vec3d& o, mat4x4& m)
		{
			//Epsilon to prevent division by zero errors.
			float fEpsilon = 0.000001f;
			o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
			o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
			o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
			float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3] + fEpsilon;

			o.x /= w; o.y /= w; o.z /= w;
		}

		CHAR_INFO CalculateColor(float lum) {
			short bg_col, fg_col;
			wchar_t sym;

			int pixel_bw = (int)(13.0f * lum);

			switch (pixel_bw)
			{
				case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

				case 1: 
					bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
				case 2: 
					bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
				case 3: 
					bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
				case 4: 
					bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

				case 5: 
					bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
				case 6: 
					bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
				case 7: 
					bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
				case 8: 
					bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

				case 9:  
					bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
				case 10: 
					bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
				case 11: 
					bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
				case 12: 
					bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
				default:
					bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
			}
			CHAR_INFO c;
			c.Attributes = bg_col | fg_col;
			c.Char.UnicodeChar = sym;
			return c;
		}

	public:
		bool OnUserCreate() override
		{
			//cubeMesh.triangles = {

			//	// SOUTH FACE
			//	{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			//	{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			//	// EAST FACE                                                 
			//	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			//	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			//	// NORTH FACE                                                 
			//	{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			//	{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			//	// WEST FACE                                                
			//	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			//	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			//	// TOP FACE                                                
			//	{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			//	{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			//	// BOTTOM FACE                                                 
			//	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			//	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			//};
			cubeMesh.LoadFromObjectFile("SpaceShip.obj");


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


			//Rotation Matrice Z and X 
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
				//Rotation
				MatMul(tri.vectors[0], triRotatedZ.vectors[0], matRotZ);
				MatMul(tri.vectors[1], triRotatedZ.vectors[1], matRotZ);
				MatMul(tri.vectors[2], triRotatedZ.vectors[2], matRotZ);
				MatMul(triRotatedZ.vectors[0], triRotatedZX.vectors[0], matRotX);
				MatMul(triRotatedZ.vectors[1], triRotatedZX.vectors[1], matRotX);
				MatMul(triRotatedZ.vectors[2], triRotatedZX.vectors[2], matRotX);
				//Offset to screen
				triTranslated = triRotatedZX;
				triTranslated.vectors[0].z = triRotatedZX.vectors[0].z + 13.0f;
				triTranslated.vectors[1].z = triRotatedZX.vectors[1].z + 13.0f;
				triTranslated.vectors[2].z = triRotatedZX.vectors[2].z + 13.0f;

				//Calculate surface normal.
				vec3d normal, line1, line2;
				line1.x = triTranslated.vectors[1].x - triTranslated.vectors[0].x;
				line1.y = triTranslated.vectors[1].y - triTranslated.vectors[0].y;
				line1.z = triTranslated.vectors[1].z - triTranslated.vectors[0].z;
				line2.x = triTranslated.vectors[2].x - triTranslated.vectors[0].x;
				line2.y = triTranslated.vectors[2].y - triTranslated.vectors[0].y;
				line2.z = triTranslated.vectors[2].z - triTranslated.vectors[0].z;

				normal.x = line1.y * line2.z - line1.z * line2.y;
				normal.y = line1.z * line2.x - line1.x * line2.z;
				normal.z = line1.x * line2.y - line1.y * line2.x;

				float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

				normal.x /= l;
				normal.y /= l;
				normal.z /= l;


				//Projection
				if (	normal.x * (triTranslated.vectors[0].x - vCamera.x) +
						normal.y * (triTranslated.vectors[0].y - vCamera.y) +
						normal.z * (triTranslated.vectors[0].z - vCamera.z) < 0.0f){

					vec3d light_direction = { 0.0f, 0.0f, -1.0f };

					float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);

					light_direction.x /= l;
					light_direction.y /= l;
					light_direction.z /= l;

					float lDp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
					CHAR_INFO c = CalculateColor(lDp);
					triTranslated.col = c.Attributes;
					triTranslated.symb = c.Char.UnicodeChar;




					MatMul(triTranslated.vectors[0], triProjected.vectors[0], projMat);
					MatMul(triTranslated.vectors[1], triProjected.vectors[1], projMat);
					MatMul(triTranslated.vectors[2], triProjected.vectors[2], projMat);

					//Scale
					triProjected.vectors[0].x += 1.0f; triProjected.vectors[0].y += 1.0f;
					triProjected.vectors[1].x += 1.0f; triProjected.vectors[1].y += 1.0f;
					triProjected.vectors[2].x += 1.0f; triProjected.vectors[2].y += 1.0f;
					triProjected.vectors[0].x *= 0.5f * (float)ScreenWidth();
					triProjected.vectors[0].y *= 0.5f * (float)ScreenHeight();
					triProjected.vectors[1].x *= 0.5f * (float)ScreenWidth();
					triProjected.vectors[1].y *= 0.5f * (float)ScreenHeight();
					triProjected.vectors[2].x *= 0.5f * (float)ScreenWidth();
					triProjected.vectors[2].y *= 0.5f * (float)ScreenHeight();

					triProjected.col = triTranslated.col;
					triProjected.symb = triTranslated.symb;

					FillTriangle(triProjected.vectors[0].x, triProjected.vectors[0].y,
						triProjected.vectors[1].x, triProjected.vectors[1].y,
						triProjected.vectors[2].x, triProjected.vectors[2].y,
						triProjected.symb, triProjected.col);

					/*DrawTriangle(triProjected.vectors[0].x, triProjected.vectors[0].y,
						triProjected.vectors[1].x, triProjected.vectors[1].y,
						triProjected.vectors[2].x, triProjected.vectors[2].y,
						PIXEL_SOLID, FG_WHITE);*/
				}
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

