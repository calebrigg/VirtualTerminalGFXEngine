#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <strstream>
#include <algorithm>
#include <unistd.h>
#include <string>


 using namespace std;

struct vec2d
{
	float u = 0;
	float v = 0;
	float w = 1;
};
 
struct vec3d
{
  
float x = 0;
   
float y = 0;
   
float z = 0;
   
float w = 1;
 
};

 
struct triangle
{
  
vec3d p[3];
vec2d t[3];
  
wchar_t sym;
  
short col;
 
};

 
 
struct mesh 
{
  
vector < triangle > tris;
  

bool LoadFromObjectFile (string sFilename, bool bHasTexture = false)
  {
    
ifstream f (sFilename);
    
if (!f.is_open ())
      
return false;
    

      // Local cache of verts
    vector < vec3d > verts;
    vector <vec2d> texs;
    

	while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					vec2d v;
					s >> junk >> junk >> v.u >> v.v;
					texs.push_back(v);
				}
				else
				{
					vec3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
			}

			if (!bHasTexture)
			{
				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}
			else
			{
				if (line[0] == 'f')
				{
					s >> junk;

					string tokens[6];
					int nTokenCount = -1;


					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();


					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

				}

			}
		}
return true;
  
}

};


 
 
struct mat4x4
{
  
float m[4][4] = { 0 };

};


 
class olcEngine3D:public olc::PixelGameEngine
{

public:
olcEngine3D ()
  {
    
sAppName = "3D Demo";

} 
 
private:
mesh meshCube;
  
mat4x4 matProj;		// Matrix that converts from view space to screen space
  vec3d
    vCamera;			// Location of camera in world space
  vec3d
    vLookDir;			// Direction vector along the direction camera points
  float
    fYaw;			// FPS Camera rotation in XZ plane
  float
    fTheta;			// Spins World transform
  olc::Sprite *sprTex1;
  
  
vec3d Matrix_MultiplyVector (mat4x4 & m, vec3d & i) 
  {
    
vec3d v;
    
v.x =
      i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    
v.y =
      i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    
v.z =
      i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    
v.w =
      i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    
return v;
  
}
  
 
mat4x4 Matrix_MakeIdentity () 
  {
    
mat4x4 matrix;
    
matrix.m[0][0] = 1.0f;
    
matrix.m[1][1] = 1.0f;
    
matrix.m[2][2] = 1.0f;
    
matrix.m[3][3] = 1.0f;
    
return matrix;
  
}
  
 
mat4x4 Matrix_MakeRotationX (float fAngleRad) 
  {
    
mat4x4 matrix;
    
matrix.m[0][0] = 1.0f;
    
matrix.m[1][1] = cosf (fAngleRad);
    
matrix.m[1][2] = sinf (fAngleRad);
    
matrix.m[2][1] = -sinf (fAngleRad);
    
matrix.m[2][2] = cosf (fAngleRad);
    
matrix.m[3][3] = 1.0f;
    
return matrix;
  
}
  
 
mat4x4 Matrix_MakeRotationY (float fAngleRad) 
  {
    
mat4x4 matrix;
    
matrix.m[0][0] = cosf (fAngleRad);
    
matrix.m[0][2] = sinf (fAngleRad);
    
matrix.m[2][0] = -sinf (fAngleRad);
    
matrix.m[1][1] = 1.0f;
    
matrix.m[2][2] = cosf (fAngleRad);
    
matrix.m[3][3] = 1.0f;
    
return matrix;
  
}
  
 
mat4x4 Matrix_MakeRotationZ (float fAngleRad) 
  {
    
mat4x4 matrix;
    
matrix.m[0][0] = cosf (fAngleRad);
    
matrix.m[0][1] = sinf (fAngleRad);
    
matrix.m[1][0] = -sinf (fAngleRad);
    
matrix.m[1][1] = cosf (fAngleRad);
    
matrix.m[2][2] = 1.0f;
    
matrix.m[3][3] = 1.0f;
    
return matrix;
  
}
  
 
mat4x4 Matrix_MakeTranslation (float x, float y, float z) 
  {
    
mat4x4 matrix;
    
matrix.m[0][0] = 1.0f;
    
matrix.m[1][1] = 1.0f;
    
matrix.m[2][2] = 1.0f;
    
matrix.m[3][3] = 1.0f;
    
matrix.m[3][0] = x;
    
matrix.m[3][1] = y;
    
matrix.m[3][2] = z;
    
return matrix;
  
}
  
 
mat4x4 Matrix_MakeProjection (float fFovDegrees, float fAspectRatio,
				   float fNear, float fFar) 
  {
    
float
      fFovRad = 1.0f / tanf (fFovDegrees * 0.5f / 180.0f * 3.14159f);
    
mat4x4 matrix;
    
matrix.m[0][0] = fAspectRatio * fFovRad;
    
matrix.m[1][1] = fFovRad;
    
matrix.m[2][2] = fFar / (fFar - fNear);
    
matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    
matrix.m[2][3] = 1.0f;
    
matrix.m[3][3] = 0.0f;
    
return matrix;
  
}
  
 
mat4x4 Matrix_MultiplyMatrix (mat4x4 & m1, mat4x4 & m2) 
  {
    
mat4x4 matrix;
    
for (int c = 0; c < 4; c++)
      
for (int r = 0; r < 4; r++)
	
matrix.m[r][c] =
	  m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] +
	  m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    
return matrix;
  
}
  
 
mat4x4 Matrix_PointAt (vec3d & pos, vec3d & target, vec3d & up) 
  {
    
      // Calculate new forward direction
      vec3d newForward = Vector_Sub (target, pos);
    
newForward = Vector_Normalise (newForward);
    
 
      // Calculate new Up direction
      vec3d a = Vector_Mul (newForward, Vector_DotProduct (up, newForward));
    
vec3d newUp = Vector_Sub (up, a);
    
newUp = Vector_Normalise (newUp);
    
 
      // New Right direction is easy, its just cross product
      vec3d newRight = Vector_CrossProduct (newUp, newForward);
    
 
      // Construct Dimensioning and Translation Matrix        
      mat4x4 matrix;
    
matrix.m[0][0] = newRight.x;
    matrix.m[0][1] = newRight.y;
    matrix.m[0][2] = newRight.z;
    matrix.m[0][3] = 0.0f;
    
matrix.m[1][0] = newUp.x;
    matrix.m[1][1] = newUp.y;
    matrix.m[1][2] = newUp.z;
    matrix.m[1][3] = 0.0f;
    
matrix.m[2][0] = newForward.x;
    matrix.m[2][1] = newForward.y;
    matrix.m[2][2] = newForward.z;
    matrix.m[2][3] = 0.0f;
    
matrix.m[3][0] = pos.x;
    matrix.m[3][1] = pos.y;
    matrix.m[3][2] = pos.z;
    matrix.m[3][3] = 1.0f;
    
return matrix;
  
 
}
  
 
mat4x4 Matrix_QuickInverse (mat4x4 & m)	// Only for Rotation/Translation Matrices
  {
    
mat4x4 matrix;
    
matrix.m[0][0] = m.m[0][0];
    matrix.m[0][1] = m.m[1][0];
    matrix.m[0][2] = m.m[2][0];
    matrix.m[0][3] = 0.0f;
    
matrix.m[1][0] = m.m[0][1];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[1][3] = 0.0f;
    
matrix.m[2][0] = m.m[0][2];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[2][3] = 0.0f;
    
matrix.m[3][0] =
      -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] +
	m.m[3][2] * matrix.m[2][0]);
    
matrix.m[3][1] =
      -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] +
	m.m[3][2] * matrix.m[2][1]);
    
matrix.m[3][2] =
      -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] +
	m.m[3][2] * matrix.m[2][2]);
    
matrix.m[3][3] = 1.0f;
    
return matrix;
  
}
  
 
vec3d Vector_Add (vec3d & v1, vec3d & v2) 
  {
    
return
    {
    v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
  
}
  
 
vec3d Vector_Sub (vec3d & v1, vec3d & v2) 
  {
    
return
    {
    v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
  
}
  
 
vec3d Vector_Mul (vec3d & v1, float k) 
  {
    
return
    {
    v1.x * k, v1.y * k, v1.z * k};
  
}
  
 
vec3d Vector_Div (vec3d & v1, float k) 
  {
    
return
    {
    v1.x / k, v1.y / k, v1.z / k};
  
}
  
 
float
  Vector_DotProduct (vec3d & v1, vec3d & v2) 
  {
    
return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  
}
  
 
float
  Vector_Length (vec3d & v) 
  {
    
return sqrtf (Vector_DotProduct (v, v));
  
}
  
 
vec3d Vector_Normalise (vec3d & v) 
  {
    
float
      l = Vector_Length (v);
    
return
    {
    v.x / l, v.y / l, v.z / l};
  
}
  
 
vec3d Vector_CrossProduct (vec3d & v1, vec3d & v2) 
  {
    
vec3d v;
    
v.x = v1.y * v2.z - v1.z * v2.y;
    
v.y = v1.z * v2.x - v1.x * v2.z;
    
v.z = v1.x * v2.y - v1.y * v2.x;
    
return v;
  
}
  
 
vec3d Vector_IntersectPlane (vec3d & plane_p, vec3d & plane_n, vec3d & lineStart, vec3d & lineEnd, float &t) 
  {
    
plane_n = Vector_Normalise (plane_n);
    
float
      plane_d = -Vector_DotProduct (plane_n, plane_p);
    
float
      ad = Vector_DotProduct (lineStart, plane_n);
    
float
      bd = Vector_DotProduct (lineEnd, plane_n);
    
t = (-plane_d - ad) / (bd - ad);
    
vec3d lineStartToEnd = Vector_Sub (lineEnd, lineStart);
    
vec3d lineToIntersect = Vector_Mul (lineStartToEnd, t);
    
return Vector_Add (lineStart, lineToIntersect);
  
}
  
 
int
  Triangle_ClipAgainstPlane (vec3d plane_p, vec3d plane_n, triangle & in_tri,
			     triangle & out_tri1, triangle & out_tri2) 
  {
    
      // Make sure plane normal is indeed normal
      plane_n = Vector_Normalise (plane_n);
    
 
      // Return signed shortest distance from point to plane, plane normal must be normalised
      auto dist =[&](vec3d & p) 
    {
      
vec3d n = Vector_Normalise (p);
      
return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z -
	       Vector_DotProduct (plane_n, plane_p));
    
};
    
 
      // Create two temporary storage arrays to classify points either side of plane
      // If distance sign is positive, point lies on "inside" of plane
    vec3d * inside_points[3]; int nInsidePointCount = 0;
    
	vec3d * outside_points[3]; int nOutsidePointCount = 0;
	
	vec2d* inside_tex[3]; int nInsideTexCount = 0;
    
    vec2d* outside_tex[3]; int nOutsideTexCount = 0;
    
 
      // Get signed distance of each point in triangle to plane
      float
      d0 = dist (in_tri.p[0]);
    
float
      d1 = dist (in_tri.p[1]);
    
float
      d2 = dist (in_tri.p[2]);
    
 
if (d0 >= 0)
      {
	inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0];
      }
    
    else
      {
	outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
      }
    
if (d1 >= 0)
      {
	inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
      }
    
    else
      {
	outside_points[nOutsidePointCount++] = &in_tri.p[1]; outside_tex[nOutsideTexCount++] = &in_tri.t[1];
      }
    
if (d2 >= 0)
      {
	inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
      }
    
    else
      {
	outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
      }
    
 
      // Now classify triangle points, and break the input triangle into 
      // smaller output triangles if required. There are four possible
      // outcomes...
      
if (nInsidePointCount == 0)
      
      {
	
	  // All points lie on the outside of plane, so clip whole triangle
	  // It ceases to exist
	  
return 0;		// No returned triangles are valid
      }
    
 
if (nInsidePointCount == 3)
      
      {
	
	  // All points lie on the inside of plane, so do nothing
	  // and allow the triangle to simply pass through
	  out_tri1 = in_tri;
	
 
return 1;		// Just the one returned original triangle is valid
      }
    
 
if (nInsidePointCount == 1 && nOutsidePointCount == 2)
      
      {
	
	  // Triangle should be clipped. As two points lie outside
	  // the plane, the triangle simply becomes a smaller triangle
	  
	  // Copy appearance info to new triangle
	  out_tri1.col = in_tri.col;
	
out_tri1.sym = in_tri.sym;
	
 
	  // The inside point is valid, so keep that...
	  out_tri1.p[0] = *inside_points[0];
	  out_tri1.t[0] = *inside_tex[0];
	
 
	  // but the two new points are at the locations where the 
	  // original sides of the triangle (lines) intersect with the plane
	float t;
	
	out_tri1.p[1] = Vector_IntersectPlane (plane_p, plane_n, *inside_points[0], *outside_points[0], t);
	out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
	out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
	
	
	out_tri1.p[2] = Vector_IntersectPlane (plane_p, plane_n, *inside_points[0], *outside_points[1], t);
	out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
	out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
	
 
return 1;		// Return the newly formed single triangle
      }
    
 
if (nInsidePointCount == 2 && nOutsidePointCount == 1)
      
      {
	
	  // Triangle should be clipped. As two points lie inside the plane,
	  // the clipped triangle becomes a "quad". Fortunately, we can
	  // represent a quad with two new triangles
	  
	  // Copy appearance info to new triangles
	  out_tri1.col = in_tri.col;
	
out_tri1.sym = in_tri.sym;
	
 
out_tri2.col = in_tri.col;
	
out_tri2.sym = in_tri.sym;
	
 
	  // The first triangle consists of the two inside points and a new
	  // point determined by the location where one side of the triangle
	  // intersects with the plane
	out_tri1.p[0] = *inside_points[0];
	out_tri1.p[1] = *inside_points[1];
	out_tri1.t[0] = *inside_tex[0];
	out_tri1.t[1] = *inside_tex[1];
	
	
	float t;
	
	out_tri1.p[2] = Vector_IntersectPlane (plane_p, plane_n, *inside_points[0], *outside_points[0], t);
	out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
	out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
 
	  // The second triangle is composed of one of he inside points, a
	  // new point determined by the intersection of the other side of the 
	  // triangle and the plane, and the newly created point above
	  out_tri2.p[0] = *inside_points[1];
	
out_tri2.p[1] = out_tri1.p[2];
	
out_tri2.p[2] = Vector_IntersectPlane (plane_p, plane_n, *inside_points[1], *outside_points[0], t);
out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
	
 
return 2;		// Return two newly formed triangles which form a quad
      }
  
}
  
 
 
void
  GetColour (float x1, float y1, float x2, float y2, float x3, float y3,
	     float lum) 
  {
    
int
      pixel_bw = (int) (13.0f * lum);
    
switch (pixel_bw)
      
      {
      
case 0:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::VERY_DARK_GREY);
	break;
      
case 1:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::VERY_DARK_GREY);
	break;
      
case 2:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::VERY_DARK_GREY);
	break;
      
case 3:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::VERY_DARK_GREY);
	break;
      
case 4:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::VERY_DARK_GREY);
	break;
      
 
case 5:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::DARK_GREY);
	break;
      
case 6:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::DARK_GREY);
	break;
      
case 7:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::DARK_GREY);
	break;
      
case 8:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::DARK_GREY);
	break;
      
 
case 9:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::GREY);
	break;
      
case 10:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::GREY);
	break;
      
case 11:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::GREY);
	break;
      
case 12:
	FillTriangle (x1, y1, x2, y2, x3, y3, olc::GREY);
	break;
      
default:
	
FillTriangle (x1, y1, x2, y2, x3, y3, olc::BLACK);
	break;
      
}
  
}

 
 
 
 float *pDepthBuffer = nullptr;
 
 
public:
bool OnUserCreate ()override
  {
    
 pDepthBuffer = new float[ScreenWidth() * ScreenHeight()];
//meshCube.LoadFromObjectFile ("table.obj");
    
   meshCube.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,}, 
		{ 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
						  																			   
		// EAST           																			   
		{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
		{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
						   																			   
		// NORTH           																			   
		{ 1.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
		{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
						   																			   
		// WEST            																			   
		{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
		{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
						   																			   
		// TOP             																			   
		{ 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
		{ 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
						   																			  
		// BOTTOM          																			  
		{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
		{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

};
    
    sprTex1 = new olc::Sprite("alternative.PNG");
 
      //PROJECTION MATRIX
      matProj = Matrix_MakeProjection (65.0f, (float) ScreenHeight () / (float) ScreenWidth (), 0.1f, 1000.0f);
      vCamera.y+=0.5f;
    
 
return true;
  
}

bool cli=true;
vector<string> lines;
string cmd;
bool begin=false;


bool OnUserUpdate (float fElapsedTime) override
  {
 
//LEFT OFF HERE AT 26:40 on VIDEO #3
if (!cli) FillRect (0, 0, ScreenWidth (), ScreenHeight (), olc::BLACK);
if (cli) FillRect (0, 0, ScreenWidth (), ScreenHeight (), olc::BLACK);
    
if (GetKey (olc::Key::E).bHeld&&cli)
cli=false;
 
if (GetKey (olc::UP).bHeld&&cli)
      
vCamera.y += 2.0f * fElapsedTime;
    
 
if (GetKey (olc::DOWN).bHeld&&cli)
      
vCamera.y -= 2.0f * fElapsedTime;
    
 
if (GetKey (olc::RIGHT).bHeld&&cli)
      
vCamera.x -= 2.0f * fElapsedTime;
    
 
if (GetKey (olc::LEFT).bHeld&&cli)
      
vCamera.x += 2.0f * fElapsedTime;

if (cli){    
    
 float glbldp;
 
 
vec3d vForward = Vector_Mul (vLookDir, 3.0f * fElapsedTime);
    
 
      // Standard FPS Control scheme, but turn instead of strafe
      if (GetKey (olc::Key::W).bHeld&&cli)
      
vCamera = Vector_Add (vCamera, vForward);
    
 
if (GetKey (olc::Key::S).bHeld&&cli)
      
vCamera = Vector_Sub (vCamera, vForward);
    
 
if (GetKey (olc::Key::A).bHeld&&cli)
      
fYaw -= 1.0f * fElapsedTime;
    
 
if (GetKey (olc::Key::D).bHeld&&cli)
      
fYaw += 1.0f * fElapsedTime;
    
 

		

		// Set up "World Tranmsform" though not updating theta 
		// makes this a bit redundant
		mat4x4 matRotZ, matRotX;
		//fTheta += 1.0f * fElapsedTime; // Uncomment to spin me right round baby right round
		matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
		matRotX = Matrix_MakeRotationX(fTheta);

		mat4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);

		mat4x4 matWorld;
		matWorld = Matrix_MakeIdentity();	// Form World Matrix
		matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX); // Transform by rotation
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

		// Create "Point At" Matrix for camera
		vec3d vUp = { 0,1,0 };
		vec3d vTarget = { 0,0,1 };
		mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
		vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
		vTarget = Vector_Add(vCamera, vLookDir);
		mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

		// Make view matrix from camera
		mat4x4 matView = Matrix_QuickInverse(matCamera);

		// Store triagles for rastering later
		vector<triangle> vecTrianglesToRaster;

		// Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
			triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
			triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);
			triTransformed.t[0] = tri.t[0];
			triTransformed.t[1] = tri.t[1];
			triTransformed.t[2] = tri.t[2];

			// Calculate triangle Normal
			vec3d normal, line1, line2;

			// Get lines either side of triangle
			line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
			line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

			// Take cross product of lines to get normal to triangle surface
			normal = Vector_CrossProduct(line1, line2);

			// You normally need to normalise a normal!
			normal = Vector_Normalise(normal);
			
			// Get Ray from triangle to camera
			vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

			// If ray is aligned with normal, then triangle is visible
			if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Illumination
				vec3d light_direction = { 0.0f, 1.0f, -1.0f };
				light_direction = Vector_Normalise(light_direction);

				// How "aligned" are light direction and triangle surface normal?
				float dp = max(0.1f, Vector_DotProduct(light_direction, normal));
				glbldp = dp;
				// Choose console colours as required (much easier with RGB)
	

				// Convert World Space --> View Space
				triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
				triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
				triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
				triViewed.sym = triTransformed.sym;
				triViewed.col = triTransformed.col;
				triViewed.t[0] = triTransformed.t[0];
				triViewed.t[1] = triTransformed.t[1];
				triViewed.t[2] = triTransformed.t[2];

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles. 
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				// We may end up with multiple triangles form the clip, so project as
				// required
				for (int n = 0; n < nClippedTriangles; n++)
				{
					// Project triangles from 3D --> 2D
					triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
					triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
					triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
					triProjected.col = clipped[n].col;
					triProjected.sym = clipped[n].sym;
					triProjected.t[0] = clipped[n].t[0];
					triProjected.t[1] = clipped[n].t[1];
					triProjected.t[2] = clipped[n].t[2];


					triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
					triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
					triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

					triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
					triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
					triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

					triProjected.t[0].w = 1.0f / triProjected.p[0].w;
					triProjected.t[1].w = 1.0f / triProjected.p[1].w;
					triProjected.t[2].w = 1.0f / triProjected.p[2].w;


					// Scale into view, we moved the normalising into cartesian space
					// out of the matrix.vector function from the previous videos, so
					// do this manually
					triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
					triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
					triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

					// X/Y are inverted so put them back
					triProjected.p[0].x *= -1.0f;
					triProjected.p[1].x *= -1.0f;
					triProjected.p[2].x *= -1.0f;
					triProjected.p[0].y *= -1.0f;
					triProjected.p[1].y *= -1.0f;
					triProjected.p[2].y *= -1.0f;

					// Offset verts into visible normalised space
					vec3d vOffsetView = { 1,1,0 };
					triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
					triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
					triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
					triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
					triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
					triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}			
			}
		}

		// Sort triangles from back to front
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});

	for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
			pDepthBuffer[i] = 0.0f;


		// Clear Screen
		//FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::CYAN);

		// Clear Depth Buffer


		// Loop through all transformed, viewed, projected, and sorted triangles
		for (auto &triToRaster : vecTrianglesToRaster)
		{

			triangle clipped[2];
			list<triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					switch (p)
					{
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

	
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}


			for (auto &t : listTriangles)
			{
				TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
					t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
					t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w, sprTex1);
				
				DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, olc::WHITE);
			}
		}
}

if (!cli){
	if (!begin && GetKey (olc::Key::ENTER).bPressed){
		begin=true;
		lines.clear();
	}
	if (!begin)	{
	DrawString(30,ScreenHeight () / 2,"Welcome to the Caleb OS Simulation. Press Enter to Begin!",olc::GREEN);
	}
	
	if (begin){
		
		DrawString(1,ScreenHeight()-9,cmd,olc::GREEN);
		for (int i=0; i<lines.size(); i++){
			DrawString(0,((i*9)+1),lines[i],olc::GREEN);
		}	
			getKeyPress(cmd);
			if (cmd==">exit" && GetKey (olc::Key::ENTER).bHeld){cmd=">"; begin=false; cli=true; lines.clear();}
			if (GetKey (olc::Key::ENTER).bPressed && (lines.size()<(ScreenHeight()-9)/9)){
				lines.push_back(cmd);
				string z = Parse(cmd);
				lines.push_back(z);
				cmd=">";
			}
	}
}

		return true;
	}

string exec(string cmd) {

    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1"); // Do we want STDERR?

    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
    return data;
}
 
string Parse(string s){
	int pos=0;
	string a,b,c,d;
	if(s.length()>0){
	d=s.substr(1,s.length());}
	a=s.substr(0,s.find(" "));
	s.erase(0, s.find(" ")+1);
	b=s.substr(0,s.find(" "));
	s.erase(0, s.find(" ")+1);
	c=s.substr(0,s.find(" "));
	
	if(a==">ADD" || a==">add"){int x = stoi(b) + stoi(c); return to_string(x);}
	else if(a==">ls"){return exec("ls");}
	else if(a==">echo"){string y = d; return exec(y);}
	else if(a==">clear"){lines.clear();return("Lines Cleared.");}
	else if(a==">SUB" || a==">sub"){int x = stoi(b) - stoi(c); return to_string(x);}
	else if(a==">DIV" || a==">div"){int x = stoi(b) / stoi(c); return to_string(x);}
	else if(a==">MUL" || a==">mul"){int x = stoi(b) * stoi(c); return to_string(x);}
	else if(a==">MOD" || a==">mod"){int x = stoi(b) % stoi(c); return to_string(x);}
	else {return "Syntax Error: Command not recognized.";}
}


void getKeyPress(string& s){
	
	if (GetKey (olc::Key::F1).bPressed){s+=".";}
	else if (GetKey (olc::Key::F2).bPressed){s+="/";}
    else if (GetKey (olc::Key::A).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="a";}
    else if (GetKey (olc::Key::B).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="b";}
    else if (GetKey (olc::Key::C).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="c";}
    else if (GetKey (olc::Key::D).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="d";}
    else if (GetKey (olc::Key::E).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="e";}
    else if (GetKey (olc::Key::F).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="f";}
    else if (GetKey (olc::Key::G).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="g";}
    else if (GetKey (olc::Key::H).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="h";}
    else if (GetKey (olc::Key::I).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="i";}
    else if (GetKey (olc::Key::J).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="j";}
    else if (GetKey (olc::Key::K).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="k";}
    else if (GetKey (olc::Key::L).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="l";}
    else if (GetKey (olc::Key::M).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="m";}
    else if (GetKey (olc::Key::N).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="n";}
    else if (GetKey (olc::Key::O).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="o";}
    else if (GetKey (olc::Key::P).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="p";}
    else if (GetKey (olc::Key::Q).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="q";}
    else if (GetKey (olc::Key::R).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="r";}
    else if (GetKey (olc::Key::S).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="s";}
    else if (GetKey (olc::Key::T).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="t";}
    else if (GetKey (olc::Key::U).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="u";}
    else if (GetKey (olc::Key::V).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="v";}
    else if (GetKey (olc::Key::W).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="w";}
    else if (GetKey (olc::Key::X).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="x";}
    else if (GetKey (olc::Key::Y).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="y";}
    else if (GetKey (olc::Key::Z).bPressed && !(GetKey (olc::Key::SHIFT).bHeld)){s+="z";}
    else if (GetKey (olc::Key::K0).bPressed){s+="0";}
    else if (GetKey (olc::Key::K1).bPressed){s+="1";}
    else if (GetKey (olc::Key::K2).bPressed){s+="2";}
    else if (GetKey (olc::Key::K3).bPressed){s+="3";}
    else if (GetKey (olc::Key::K4).bPressed){s+="4";}
    else if (GetKey (olc::Key::K5).bPressed){s+="5";}
    else if (GetKey (olc::Key::K6).bPressed){s+="6";}
    else if (GetKey (olc::Key::K7).bPressed){s+="7";}
    else if (GetKey (olc::Key::K8).bPressed){s+="8";}
    else if (GetKey (olc::Key::K9).bPressed){s+="9";}
    else if (GetKey (olc::Key::SPACE).bPressed){s+=" ";}
	else if (GetKey (olc::Key::BACK).bPressed && s.length()>1){s.pop_back();}
	
	
	else if (GetKey (olc::Key::A).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="A";}
    else if (GetKey (olc::Key::B).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="B";}
    else if (GetKey (olc::Key::C).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="C";}
    else if (GetKey (olc::Key::D).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="D";}
    else if (GetKey (olc::Key::E).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="E";}
    else if (GetKey (olc::Key::F).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="F";}
    else if (GetKey (olc::Key::G).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="G";}
    else if (GetKey (olc::Key::H).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="H";}
    else if (GetKey (olc::Key::I).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="I";}
    else if (GetKey (olc::Key::J).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="J";}
    else if (GetKey (olc::Key::K).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="K";}
    else if (GetKey (olc::Key::L).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="L";}
    else if (GetKey (olc::Key::M).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="M";}
    else if (GetKey (olc::Key::N).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="N";}
    else if (GetKey (olc::Key::O).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="O";}
    else if (GetKey (olc::Key::P).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="P";}
    else if (GetKey (olc::Key::Q).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="Q";}
    else if (GetKey (olc::Key::R).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="R";}
    else if (GetKey (olc::Key::S).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="S";}
    else if (GetKey (olc::Key::T).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="T";}
    else if (GetKey (olc::Key::U).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="U";}
    else if (GetKey (olc::Key::V).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="V";}
    else if (GetKey (olc::Key::W).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="W";}
    else if (GetKey (olc::Key::X).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="X";}
    else if (GetKey (olc::Key::Y).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="Y";}
    else if (GetKey (olc::Key::Z).bPressed && GetKey (olc::Key::SHIFT).bHeld){s+="Z";}
    else{s+="";}

	
}


void TexturedTriangle(	int x1, int y1, float u1, float v1, float w1,
							int x2, int y2, float u2, float v2, float w2,
							int x3, int y3, float u3, float v3, float w3,
		olc::Sprite *tex)
	{
		if (y2 < y1)
		{
			swap(y1, y2);
			swap(x1, x2);
			swap(u1, u2);
			swap(v1, v2);
			swap(w1, w2);
		}

		if (y3 < y1)
		{
			swap(y1, y3);
			swap(x1, x3);
			swap(u1, u3);
			swap(v1, v3);
			swap(w1, w3);
		}

		if (y3 < y2)
		{
			swap(y2, y3);
			swap(x2, x3);
			swap(u2, u3);
			swap(v2, v3);
			swap(w2, w3);
		}

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;
		float dv1 = v2 - v1;
		float du1 = u2 - u1;
		float dw1 = w2 - w1;

		int dy2 = y3 - y1;
		int dx2 = x3 - x1;
		float dv2 = v3 - v1;
		float du2 = u3 - u1;
		float dw2 = w3 - w1;

		float tex_u, tex_v, tex_w;

		float dax_step = 0, dbx_step = 0,
			du1_step = 0, dv1_step = 0,
			du2_step = 0, dv2_step = 0,
			dw1_step=0, dw2_step=0;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy2) du2_step = du2 / (float)abs(dy2);
		if (dy2) dv2_step = dv2 / (float)abs(dy2);
		if (dy2) dw2_step = dw2 / (float)abs(dy2);

		if (dy1)
		{
			for (int i = y1; i <= y2; i++)
			{
				int ax = x1 + (float)(i - y1) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u1 + (float)(i - y1) * du1_step;
				float tex_sv = v1 + (float)(i - y1) * dv1_step;
				float tex_sw = w1 + (float)(i - y1) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					swap(ax, bx);
					swap(tex_su, tex_eu);
					swap(tex_sv, tex_ev);
					swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i*ScreenWidth() + j]){
						Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
						pDepthBuffer[i*ScreenWidth() + j] = tex_w;
					}
					t += tstep;
				}

			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;
		dw1 = w3 - w2;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				int ax = x2 + (float)(i - y2) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u2 + (float)(i - y2) * du1_step;
				float tex_sv = v2 + (float)(i - y2) * dv1_step;
				float tex_sw = w2 + (float)(i - y2) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					swap(ax, bx);
					swap(tex_su, tex_eu);
					swap(tex_sv, tex_ev);
					swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i*ScreenWidth() + j]){
						Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
						pDepthBuffer[i*ScreenWidth() + j] = tex_w;
					}
					t += tstep;
				}
			}	
		}		
	}
};
  
 
int
  main ()
  {
    
olcEngine3D demo;
    
if (demo.Construct (510, 340, 1, 1))
      
demo.Start ();
    
return 0;
  
}
  
