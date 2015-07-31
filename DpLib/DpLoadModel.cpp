/********************************************************************
	created:	2014/12/08
	created:	8:12:2014   22:59
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpLoadModel.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpLoadModel
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Load 3D model file
*********************************************************************/

#include "DpObject.h"
#include "DpColor.h"
#include "DpFileParser.h"
#include "DpLoadModel.h"
#include "DpCore.h"
#include "DpMath.h"
#include "DpMaterial.h"

#include <map>

namespace DoPixel
{
	namespace Core
	{
		/* For .PLG file, format.

		# This is comments
		object_name  num_vertices num_polygons

		# Vertex list, each vertex using format: x y z
		x0 y0 z0
		x1 y1 z1
		x2 y2 z2

		# Poly list, each poly using format as follow:
		surface_desc num_vertices v0 v1 .. vn
		surface_desc num_vertices v0 v1 .. vn

		# v0 v1 is num of poly vertex

		For surface_desc, defined as:
		CSSD | RRRR | GGGG | BBBB

		a) C	: RGB/Indexed Color
		b) SS	: shade mode
		c) D	: single face
		d) RRRR GGGG BBBB	: Red Green Blue value in RGB mode
		e) GGGG BBBB	: color index in 8 bits indexed mode
		//////////////////////////////////////////////////////////////////////////

		follow is a .plg file for cube

		# PLG header

		# object
		tri 8 12

		# Vertex list
		5 5 5
		-5 5 5
		...

		# Poly list
		0xd0f0 3 0 1 2
		0xd0f0 3 0 2 3
		...

		*/

		bool LoadObjectFromPLG(Object& obj, const char* fileName, const Vector4f& scale, const Vector4f& pos, const Vector4f& rot, int vertexFlag)
		{
			memset(&obj, 0, sizeof(obj));
			obj.state = Object::STATE_ACTIVE | Object::STATE_VISIBLE;
			obj.worldPos = pos;
			obj.attr = Object::ATTR_SINGLE_FRAME;
			obj.numFrames = 1;

			FileParser fileParser;
			if (!fileParser.Open(fileName))
				return false;

			std::string strLine;
			if (! fileParser.GetLine(strLine))
				return false;

			sscanf_s(strLine.c_str(), "%s %d %d", obj.name, sizeof(obj.name), &obj.numVertices, &obj.numPolys);

			obj.Init(obj.numVertices, obj.numPolys, obj.numFrames);

			// Load vertex list
			for (int i = 0; i < obj.numVertices; ++i)
			{
				if (!fileParser.GetLine(strLine))
					return false;

				sscanf_s(strLine.c_str(), "%f %f %f", &obj.vListLocal[i].x, &obj.vListLocal[i].y, &obj.vListLocal[i].z);
				obj.vListLocal[i].w = 1;
				obj.vListLocal[i].x *= scale.x;
				obj.vListLocal[i].y *= scale.y;
				obj.vListLocal[i].z *= scale.z;

				obj.vListLocal[i].attr |= Vertex::Attr_Point;
			}

			// Calc avg radius and max radius
			obj.UpdateRadius();

			// Load ploy list
			int polySurfaceDesc = 0;
			int polyNumVerts = 0;
			char tmp[16] = { 0 };
			for (int i = 0; i < obj.numPolys; ++i)
			{
				if (!fileParser.GetLine(strLine))
					return false;

				Poly& poly = obj.pList[i];

				// Assume each ploy is triangle
				sscanf_s(strLine.c_str(), "%s %d %d %d %d", tmp, sizeof(tmp), &polyNumVerts, &poly.vert[0], &poly.vert[1], &poly.vert[2]);
				if (tmp[0] == '0' && toupper(tmp[1]) == 'X')
					sscanf_s(tmp, "%x", &polySurfaceDesc);
				else
					polySurfaceDesc = atoi(tmp);

				// Let ploy vertex list be object vertex list 
				poly.vlist = obj.vListLocal;

				poly.clist = obj.coordlist;

				// Analyze ploy surface desc

				// Side
				if (polySurfaceDesc & PLX_2SIDE_FLAG)
					poly.attr |= POLY_ATTR_2SIDE;

				// Color
				Color color;
				if (polySurfaceDesc & PLX_COLOR_MODE_RGB_FLAG)
				{
					poly.attr |= POLY_ATTR_RGB32;

					int red = ((polySurfaceDesc & 0x0f00) >> 8);
					int green = ((polySurfaceDesc & 0x00f0) >> 4);
					int blue = (polySurfaceDesc & 0x000f);

					// In file. RGB is 4.4.4, in virtual color system convert 8.8.8 to 5.5.5 or 5.5.6
					// So, 4.4.4 -> 8.8.8
					color = Color((unsigned char)red, (unsigned char)green, (unsigned char)blue);
				}
				else
				{
					//assert(false && "no support for 8-bit color");
					poly.attr |= POLY_ATTR_8BITCOLOR;
					color = Color(polySurfaceDesc & 0x00ff);
				}

				// set poly color to vertex color
				Vertex& v0 = obj.vListLocal[poly.vert[0]];
				Vertex& v1 = obj.vListLocal[poly.vert[1]];
				Vertex& v2 = obj.vListLocal[poly.vert[2]];
				v0.color = color;
				v1.color = color;
				v2.color = color;

				int shadeMode = (polySurfaceDesc & PLX_SHADE_MODE_MASK);
				switch (shadeMode)
				{
				case PLX_SHADE_MODE_PURE_FLAG:
					poly.attr |= POLY_ATTR_SHADE_PURE;
					break;
				case PLX_SHADE_MODE_FLAT_FLAG:
					poly.attr |= POLY_ATTR_SHADE_FLAT;
					break;
				case PLX_SHADE_MODE_GUARD_FLAG:
					{
						poly.attr |= POLY_ATTR_SHADE_GOURAUD;
						// set vertex need normals
						v0.attr |= Vertex::Attr_Normal;
						v1.attr |= Vertex::Attr_Normal;
						v2.attr |= Vertex::Attr_Normal;
					}
					break;
				case PLX_SHADE_MODE_PHONG_FLAG:
					{
						poly.attr |= POLY_ATTR_SHADE_PHONG;
						// set vertex need normals
						v0.attr |= Vertex::Attr_Normal;
						v1.attr |= Vertex::Attr_Normal;
						v2.attr |= Vertex::Attr_Normal;
					}
					break;
				default:
					break;
				}

				poly.state = POLY_STATE_ACTIVE;
			}

			obj.ComputeVertexNormals();
			return true;
		}

		/* For .ASC file, format

		Ambient light color: Red=0.3 Green=0.3 Blue=0.3

		Named object: "Cube"
		Tri-mesh, Vertices: 8     Faces: 12
		Vertex list:
		Vertex 0:  X:-1.013187     Y:-0.999930     Z:-0.000001
		Vertex 1:  X:-1.013187     Y:-0.999930     Z:2.000000
		Vertex 2:  X:0.986814     Y:-0.999930     Z:-0.000001
		Vertex 3:  X:0.986814     Y:-0.999930     Z:2.000000
		Vertex 4:  X:-1.013187     Y:1.000070     Z:-0.000001
		Vertex 5:  X:0.986814     Y:1.000070     Z:-0.000001
		Vertex 6:  X:0.986814     Y:1.000070     Z:2.000000
		Vertex 7:  X:-1.013187     Y:1.000070     Z:2.000000
		Face list:
		Face 0:    A:2 B:3 C:1 AB:1 BC:1 CA:1
		Material:"r6g16b255a0"
		Smoothing:  1
		Face 1:    A:2 B:1 C:0 AB:1 BC:1 CA:1
		Material:"r6g16b255a0"
		Smoothing:  1
		*/

		bool LoadObjectFrom3DSASC(Object& obj, const char* fileName, const Vector4f& scale, const Vector4f& pos, const Vector4f& rot, int vertexFlag)
		{
			memset(&obj, 0, sizeof(obj));
			obj.state = Object::STATE_ACTIVE | Object::STATE_VISIBLE;
			obj.worldPos = pos;
			obj.attr = Object::ATTR_SINGLE_FRAME;
			obj.numFrames = 1;

			FileParser parser;
			if (!parser.Open(fileName))
				return false;

			auto fnGetLine = [&parser](std::string& strLine) -> bool
			{
				while (true)
				{
					if (!parser.GetLine(strLine))
						return false;
					break;
				}
				return true;
			};

			std::string strLine;
			FileParser::RegexInfo regexInfo;
		
			// Find object name
			regexInfo.GenRegexInfo("['Named'] ['object:'] ['\"'] [s] ['\"']");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					// the 4th is name
					auto val = parser.GetMatchedVal<std::string>(3);
					strncpy_s(obj.name, val.c_str(), sizeof(obj.name));
					break;
				}
			}

			// Get number of vertices and polys in object
			regexInfo.GenRegexInfo("['Tri-mesh,'] ['Vertices:'] [i] ['Faces:'] [i]");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					obj.numVertices = parser.GetMatchedVal<int>(0);
					obj.numPolys = parser.GetMatchedVal<int>(1);
					break;
				}
			}

			obj.Init(obj.numVertices, obj.numPolys, obj.numFrames);

			// Get vertex list
			regexInfo.GenRegexInfo("['Vertex'] [i] ['X'] [f] ['Y'] [f] ['Z'] [f]");
			for (int i = 0; i < obj.numVertices;)
			{
				fnGetLine(strLine);

				std::string s;
				StrUtility::StrReplace(s, strLine, ":", " ");
				if (parser.RegexPatternMatch(s, regexInfo))
				{
					auto x = parser.GetMatchedVal<float>(0);
					auto y = parser.GetMatchedVal<float>(1);
					auto z = parser.GetMatchedVal<float>(2);

					// invert sign?
					if ((vertexFlag & VERTEX_FLAGS_INVERT_X) != 0)
						x = -x;
					if ((vertexFlag & VERTEX_FLAGS_INVERT_Y) != 0)
						y = -y;
					if ((vertexFlag & VERTEX_FLAGS_INVERT_Z) != 0)
						z = -z;

					// swap any axes?
					if ((vertexFlag & VERTEX_FLAGS_SWAP_XY) != 0)
						std::swap(x, y);
					if ((vertexFlag & VERTEX_FLAGS_SWAP_YZ) != 0)
						std::swap(y, z);
					if ((vertexFlag & VERTEX_FLAGS_SWAP_XZ) != 0)
						std::swap(x, z);

					// scale
					x *= scale.x;
					y *= scale.y;
					z *= scale.z;

					obj.vListLocal[i].p = Vector4f(x, y, z, 1);
					obj.vListLocal[i].attr |= Vertex::Attr_Point;
					++i;
				}
			}

			obj.UpdateRadius();

			// Get polygons
			
			// Face ddd: A:ddd B:ddd C:ddd AB:1|0 BC:1|0 CA:1|
			// Material:"rdddgdddbddda0"
			// Smoothing:0|1
			// the A, B, C part is vertex 0,1,2 but the AB, BC, CA part, vertex index start by 0
			// has to do with the edges and the vertex ordering
			// the material indicates the color, and has an 'a0' tacked on the end???
			// Smoothing indicates whether average vertex normals
			
			FileParser::RegexInfo regexInfoFace("['Face'] [i] ['A'] [i] ['B'] [i] ['C'] [i] ['AB'] [i] ['BC'] [i] ['CA'] [i]");
			for (int i = 0; i < obj.numPolys;)
			{
				// Get face info
				fnGetLine(strLine);

				std::string s;
				StrUtility::StrReplace(s, strLine, ":", " ");
				if (parser.RegexPatternMatch(s, regexInfoFace))
				{
					Poly& poly = obj.pList[i];
					
					poly.vlist = obj.vListLocal;
					poly.vert[0] = parser.GetMatchedVal<int>(1);
					poly.vert[1] = parser.GetMatchedVal<int>(2);
					poly.vert[2] = parser.GetMatchedVal<int>(3);
					
					// Winding order invert 
					if ((vertexFlag & VERTEX_FLAGS_INVERT_WINDING_ORDER) != 0)
						std::swap(poly.vert[0], poly.vert[2]);

					poly.vlist = obj.vListLocal;
					poly.clist = obj.coordlist;

					// Get material
					fnGetLine(strLine);
					
					StrUtility::StrReplace(s, strLine, "\"", " ");
					FileParser::RegexInfo regexMaterial("['Material:'] ['r'] [i] ['g'] [i] ['b'] [i] ['a'] [i]");
					if (parser.RegexPatternMatch(s, regexMaterial))
					{
						auto r = parser.GetMatchedVal<int>(0);
						auto g = parser.GetMatchedVal<int>(1);
						auto b = parser.GetMatchedVal<int>(2);
						auto a = parser.GetMatchedVal<int>(3);

						a = 255;

						Color color((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);

						// set poly color to vertex color
						Vertex& v0 = obj.vListLocal[poly.vert[0]];
						Vertex& v1 = obj.vListLocal[poly.vert[1]];
						Vertex& v2 = obj.vListLocal[poly.vert[2]];
						v0.color = color;
						v1.color = color;
						v2.color = color;

						poly.attr |= POLY_ATTR_RGB32;
					}

					// Vertex overrides
					int vertexOverrides = vertexFlag & VERTEX_FLAGS_OVERRIDE_MASK;	

					if ((vertexOverrides & VERTEX_FLAGS_OVERRIDE_PURE) != 0)
						poly.attr |= POLY_ATTR_SHADE_PURE;					
					else if ((vertexOverrides & VERTEX_FLAGS_OVERRIDE_FLAT) != 0)
						poly.attr |= POLY_ATTR_SHADE_FLAT;
					else if ((vertexOverrides & VERTEX_FLAGS_OVERRIDE_GOURAUD) != 0)
					{
						poly.attr |= POLY_ATTR_SHADE_GOURAUD;
						// need normals
						poly.vlist[poly.vert[0]].attr |= Vertex::Attr_Normal;
						poly.vlist[poly.vert[1]].attr |= Vertex::Attr_Normal;
						poly.vlist[poly.vert[2]].attr |= Vertex::Attr_Normal;
					}
					else if ((vertexOverrides & VERTEX_FLAGS_OVERRIDE_TEXTURE) != 0)
						poly.attr |= POLY_ATTR_SHADE_TEXTURE;
					else
						// default
						poly.attr |= POLY_ATTR_SHADE_FLAT;

					poly.state = POLY_STATE_ACTIVE;

					++i;
				}
			}

			// compute vertex normals for any gouraud shaded polys
			obj.ComputeVertexNormals();

			return true;
		}

		/* For .COB file, format

		Name Cube,1
		center 0 0 0
		x axis 1 0 0
		y axis 0 1 0
		z axis 0 0 1
		Transform
		0.953972 0 0.299895 0
		0 1 0 0
		- 0.299895 0 0.953972 0
		0 0 0 1
		World Vertices 8
		- 1.000000 - 1.000000 - 1.000000
		- 1.000000 - 1.000000 1.000000
		1.000000 - 1.000000 - 1.000000
		1.000000 - 1.000000 1.000000
		- 1.000000 1.000000 - 1.000000
		1.000000 1.000000 - 1.000000
		1.000000 1.000000 1.000000
		- 1.000000 1.000000 1.000000
		Texture Vertices 6
		0.000000 0.000000
		0.000000 1.000000
		0.000000 0.000000
		0.000000 1.000000
		1.000000 0.000000
		1.000000 1.000000
		Faces 12
		Face verts 3 flags 0 mat 4
		< 0, 0 > <1, 1> <3, 5>
		Face verts 3 flags 0 mat 4
		< 0, 0 > <3, 5> <2, 4>
		...
		*/
	
		bool LoadObjectFromCOB(Object& object, const char* fileName, const Vector4f& scale, const Vector4f& pos, const Vector4f& rot, int vertexFlag)
		{
			memset(&object, 0, sizeof(object));
			object.state = Object::STATE_ACTIVE | Object::STATE_VISIBLE;
			object.worldPos = pos;
			object.attr = Object::ATTR_SINGLE_FRAME;
			object.numFrames = 1;

			FileParser parser;
			parser.SetComment("");
			if (!parser.Open(fileName))
				return false;

			auto fnGetLine = [&parser](std::string& strLine) -> bool
			{
				while (true)
				{
					if (!parser.GetLine(strLine))
						return false;
					break;
				}
				return true;
			};

			std::string strLine;
			FileParser::RegexInfo regexInfo;
			
			//Find object name
			regexInfo.GenRegexInfo("['Name'] [s]");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;
				
				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					auto name = parser.GetMatchedVal<std::string>(1);
					strncpy_s(object.name, name.c_str(), sizeof(object.name));
					break;
				}
			}
			
			// local matrix
			// center 0 0 0
			// x axis 1 0 0
			// y axis 0 1 0
			// z axis 0 0 1
			Matrix44f matrixLocal;
			matrixLocal.Identity();
			regexInfo.GenRegexInfo("['center'] [f] [f] [f]");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;
				
				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					// the "center" holds the translation factors, so place in
					// last row of homogeneous matrix, note that these are row vectors
					// that we need to drop in each column of matrix
					matrixLocal.m41 = -parser.GetMatchedVal<float>(0);	// center x
					matrixLocal.m42 = -parser.GetMatchedVal<float>(1);	// center y
					matrixLocal.m43 = -parser.GetMatchedVal<float>(2);	// center z

					// x axis
					fnGetLine(strLine);
					regexInfo.GenRegexInfo("['x'] ['axis'] [f] [f] [f]");
					bool b = parser.RegexPatternMatch(strLine, regexInfo);
					assert(b);
					matrixLocal.m11 = parser.GetMatchedVal<float>(0);
					matrixLocal.m12 = parser.GetMatchedVal<float>(1);
					matrixLocal.m13 = parser.GetMatchedVal<float>(2);

					// y axis
					fnGetLine(strLine);
					regexInfo.GenRegexInfo("['y'] ['axis'] [f] [f] [f]");
					b = parser.RegexPatternMatch(strLine, regexInfo);
					assert(b);
					matrixLocal.m21 = parser.GetMatchedVal<float>(0);
					matrixLocal.m22 = parser.GetMatchedVal<float>(1);
					matrixLocal.m23 = parser.GetMatchedVal<float>(2);

					// z axis
					fnGetLine(strLine);
					regexInfo.GenRegexInfo("['z'] ['axis'] [f] [f] [f]");
					b = parser.RegexPatternMatch(strLine, regexInfo);
					assert(b);
					matrixLocal.m31 = parser.GetMatchedVal<float>(0);
					matrixLocal.m32 = parser.GetMatchedVal<float>(1);
					matrixLocal.m33 = parser.GetMatchedVal<float>(2);
					break;
				}
			}

			//transforms
			Matrix44f matrixWorld;
			matrixWorld.Identity();
			regexInfo.GenRegexInfo("['Transform']");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					// x axis
					fnGetLine(strLine);
					regexInfo.GenRegexInfo("[f] [f] [f] [f]");
					bool b = parser.RegexPatternMatch(strLine, regexInfo);
					assert(b);
					matrixWorld.m11 = parser.GetMatchedVal<float>(0);
					matrixWorld.m12 = parser.GetMatchedVal<float>(1);
					matrixWorld.m13 = parser.GetMatchedVal<float>(2);
					matrixWorld.m14 = parser.GetMatchedVal<float>(3);

					// y axis
					fnGetLine(strLine);
					regexInfo.GenRegexInfo("[f] [f] [f] [f]");
					b = parser.RegexPatternMatch(strLine, regexInfo);
					assert(b);
					matrixWorld.m21 = parser.GetMatchedVal<float>(0);
					matrixWorld.m22 = parser.GetMatchedVal<float>(1);
					matrixWorld.m23 = parser.GetMatchedVal<float>(2);
					matrixWorld.m24 = parser.GetMatchedVal<float>(3);

					// z axis
					fnGetLine(strLine);
					regexInfo.GenRegexInfo("[f] [f] [f] [f]");
					b = parser.RegexPatternMatch(strLine, regexInfo);
					assert(b);
					matrixWorld.m31 = parser.GetMatchedVal<float>(0);
					matrixWorld.m32 = parser.GetMatchedVal<float>(1);
					matrixWorld.m33 = parser.GetMatchedVal<float>(2);
					matrixWorld.m34 = parser.GetMatchedVal<float>(3);

					//transform
					fnGetLine(strLine);
					regexInfo.GenRegexInfo("[f] [f] [f] [f]");
					b = parser.RegexPatternMatch(strLine, regexInfo);
					assert(b);
					matrixWorld.m41 = parser.GetMatchedVal<float>(0);
					matrixWorld.m42 = parser.GetMatchedVal<float>(1);
					matrixWorld.m43 = parser.GetMatchedVal<float>(2);
					matrixWorld.m44 = parser.GetMatchedVal<float>(3);
					break;
				}
			}

			// Number of vertex
			regexInfo.GenRegexInfo("['World'] ['Vertices'] [i]");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					object.numVertices = parser.GetMatchedVal<int>(0);
					break;
				}
			}

			// Init vertices
			object.InitVertices(object.numVertices, object.numFrames);

			// Load vertex
			regexInfo.GenRegexInfo("[f] [f] [f]");
			for (int i = 0; i < object.numVertices;)
			{
				fnGetLine(strLine);

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					auto x = parser.GetMatchedVal<float>(0);
					auto y = parser.GetMatchedVal<float>(1);
					auto z = parser.GetMatchedVal<float>(2);

					object.vListLocal[i].p = Vector4f(x, y, z, 1);
				
					// Since trueSpace do not change vertex position when move or rotation for keep precision,
					// We need to apply transform vertex		
					
					// transform
					if ((vertexFlag & VERTEX_FLAGS_TRANSFORM_LOCAL) != 0)
						object.vListLocal[i].p *= matrixLocal;

					if ((vertexFlag & VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD) != 0)
						object.vListLocal[i].p *= matrixWorld;

					// invert
					if ((vertexFlag & VERTEX_FLAGS_INVERT_X) != 0)
						object.vListLocal[i].x = -object.vListLocal[i].x;

					if ((vertexFlag & VERTEX_FLAGS_INVERT_Y) != 0)
						object.vListLocal[i].y = -object.vListLocal[i].y;

					if ((vertexFlag & VERTEX_FLAGS_INVERT_Z) != 0)
						object.vListLocal[i].z = -object.vListLocal[i].z;

					// swap axes
					if ((vertexFlag & VERTEX_FLAGS_SWAP_YZ) != 0)
						Swap(object.vListLocal[i].y, object.vListLocal[i].z);

					if ((vertexFlag & VERTEX_FLAGS_SWAP_XZ) != 0)
						Swap(object.vListLocal[i].x, object.vListLocal[i].z);

					if ((vertexFlag & VERTEX_FLAGS_SWAP_XY) != 0)
						Swap(object.vListLocal[i].x, object.vListLocal[i].y);

					// scale vertices
					object.vListLocal[i].x *= scale.x;
					object.vListLocal[i].y *= scale.y;
					object.vListLocal[i].z *= scale.z;

					object.vListLocal[i].attr |= Vertex::Attr_Point;

					++i;
				}
			}

			// compute average and max radius
			object.UpdateRadius();

			// get texture vertices
			int numUVs = 0;
			regexInfo.GenRegexInfo("['Texture'] ['Vertices'] [i]");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					numUVs = parser.GetMatchedVal<int>(0);
					break;
				}
			}

			// load UV list
			object.InitCoordList(numUVs);
			regexInfo.GenRegexInfo("[f] [f]");
			for (int i = 0; i < numUVs;)
			{
				fnGetLine(strLine);

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					object.coordlist[i].x = parser.GetMatchedVal<float>(0);
					object.coordlist[i].y = parser.GetMatchedVal<float>(1);

					++i;
				}
			}

			// load faces num
			regexInfo.GenRegexInfo("['Faces'] [i]");
			while (true)
			{
				if (!parser.GetLine(strLine))
					return false;

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					object.numPolys = parser.GetMatchedVal<int>(0);
					break;
				}
			}

			// load faces
			// format:
			// Face verts nn flags ff mat mm
			// the nn is the number of vertices, always 3
			// the ff is the flags, unused for now, has to do with holes
			// the mm is the material index number 

			object.InitPolys(object.numPolys);

			std::map<int, int> polyMaterials;
			std::map<int, int> materialsRef;
			int totalMaterialsRef = 0;

			regexInfo.GenRegexInfo("['Face'] ['verts'] [i] ['flags'] [i] ['mat'] [i]");
			FileParser::RegexInfo regexInfo2("[i] [i] [i] [i] [i] [i]");
			for (int i = 0; i < object.numPolys; ++i)
			{
				fnGetLine(strLine);

				if (parser.RegexPatternMatch(strLine, regexInfo))
				{
					// at this point we have the number of vertices for the polygon, the flags, and it's material index
					auto materialId = parser.GetMatchedVal<int>(2);
					polyMaterials[i] = materialId;

					// update materials reference
					if (materialsRef[materialId] == 0)
					{
						// mark as referenced
						materialsRef[materialId] = 1;

						// increment total number of materials for this object
						++totalMaterialsRef;
					}

					// test if number of vertices is 3
					assert(parser.GetMatchedVal<int>(0) == 3);

					// read vertex indices
					// <vindex0, coordindex0>  <vindex1, coordindex1> <vindex2,coordindex2> 
					fnGetLine(strLine);
					std::string strLineFormat;
					StrUtility::StrReplaceAnychar(strLineFormat, strLine, ",<>", " ");
					if (parser.RegexPatternMatch(strLineFormat, regexInfo2))
					{
						// 0,2,4 holds vertex indices
						// 1,3,5 holds texture indices

						auto vIndex0 = parser.GetMatchedVal<int>(0);
						auto vIndex1 = parser.GetMatchedVal<int>(2);
						auto vIndex2 = parser.GetMatchedVal<int>(4);

						auto coordIndex0 = parser.GetMatchedVal<int>(1);
						auto coordIndex1 = parser.GetMatchedVal<int>(3);
						auto coordIndex2 = parser.GetMatchedVal<int>(5);

						auto& poly = object.pList[i];
						if ((vertexFlag & VERTEX_FLAGS_INVERT_WINDING_ORDER) != 0)
						{
							poly.vert[0] = vIndex2;
							poly.vert[1] = vIndex1;
							poly.vert[2] = vIndex0;

							poly.coord[0] = coordIndex2;
							poly.coord[1] = coordIndex1;
							poly.coord[2] = coordIndex0;
						}
						else
						{
							poly.vert[0] = vIndex0;
							poly.vert[1] = vIndex1;
							poly.vert[2] = vIndex2;

							poly.coord[0] = coordIndex0;
							poly.coord[1] = coordIndex1;
							poly.coord[2] = coordIndex2;
						}
						poly.vlist = object.vListLocal;
						poly.clist = object.coordlist;
						poly.state = POLY_STATE_ACTIVE;
					}
				}
			}

			// find materials
			regexInfo.GenRegexInfo("['mat#'] [i]");
			regexInfo2.GenRegexInfo("['rgb'] [f] [f] [f]");
			FileParser::RegexInfo regexInfo3("['Shader'] ['class:'] ['color']");
			FileParser::RegexInfo regexInfo4("['alpha'] [f] ['ka'] [f] ['ks'] [f] ['exp'] [f]");
			FileParser::RegexInfo regexInfo5("['Shader'] ['name:'] ['\"'] [s] ['\"']");
			FileParser::RegexInfo regexInfo6("['Shader'] ['name:'] ['plain'] ['color']");
			FileParser::RegexInfo regexInfo7("['Shader'] ['name:'] ['texture'] ['map']");
			FileParser::RegexInfo regexInfo8("['file'] ['name:'] ['string'] ['\"'] [s] ['\"']");
			FileParser::RegexInfo regexInfo9("['Shader'] ['class:'] ['reflectance']");

			for (int i = 0; i < totalMaterialsRef; ++i)
			{
				while (true)
				{
					// hunt for material header "mat# ddd"
					fnGetLine(strLine);

					if (parser.RegexPatternMatch(strLine, regexInfo))
					{
						// generate actual material
						int materialId = parser.GetMatchedVal<int>(0);
						int genMaterialId = materialId;
						Material& material = MaterialManager::GetInstance().GenMaterial(genMaterialId);
						if (genMaterialId != materialId)
						{
							// repeat material id in globe material manager, generate new id
							// and change map between poly index - material id
							for (auto& kv : polyMaterials)
							{
								if (kv.second == materialId)
									kv.second = genMaterialId;
							}
							// No need to change materialsRef
							
							materialId = genMaterialId;
						}

						// color of poly
						while (true)
						{
							fnGetLine(strLine);

							std::string formatStrLine;
							StrUtility::StrReplaceAnychar(formatStrLine, strLine, ",", " ");
							if (parser.RegexPatternMatch(formatStrLine, regexInfo2))
							{
								auto r = Math::Clamp(parser.GetMatchedVal<float>(0) * 255 + 0.5f, 0.0f, 255.0f);
								auto g = Math::Clamp(parser.GetMatchedVal<float>(1) * 255 + 0.5f, 0.0f, 255.0f);
								auto b = Math::Clamp(parser.GetMatchedVal<float>(2) * 255 + 0.5f, 0.0f, 255.0f);

								material.color.Set((unsigned char)r, (unsigned char)g, (unsigned char)b);
								break;
							}
						}

						// "alpha float ka float ks float exp float ior float"
						// alpha is transparency           0 - 1
						// ka is ambient coefficient       0 - 1
						// ks is specular coefficient      0 - 1
						// exp is highlight power exponent 0 - 1
						// ior is index of refraction (unused)
						while (true)
						{
							fnGetLine(strLine);
							if (parser.RegexPatternMatch(strLine, regexInfo4))
							{
								auto alpha = Math::Clamp(parser.GetMatchedVal<float>(0) * 255 + 0.5f, 0.0f, 255.0f);
								auto ka = parser.GetMatchedVal<float>(1);
								auto kd = 1.0f;	// hard code for now
								auto ks = parser.GetMatchedVal<float>(2);
								auto power = parser.GetMatchedVal<float>(3);

								material.color.a = (unsigned char)Math::Clamp(alpha * 255 + 0.5f, 0.0f, 255.0f);
								material.kambient = ka;
								material.kdiffuse = kd;
								material.kspecular = ks;
								material.power = power;

								// compute material reflectivity in pre-multiplied format to help engine
								material.rambient *= ka;
								material.rdiffuse *= kd;
								material.rspecular *= ks;
								
								break;
							}

						}

						// now we need to know the shading model, it's a bit tricky, we need to look for the lines
						// "Shader class: color" first, then after this line is:
						// "Shader name: "xxxxxx" (xxxxxx) "
						// where the xxxxx part will be "plain color" and "plain" for colored polys 
						// or "texture map" and "caligari texture"  for textures
						// THEN based on that we hunt for "Shader class: reflectance" which is where the type
						// of shading is encoded, we look for the "Shader name: "xxxxxx" (xxxxxx) " again, 
						// and based on it's value we map it to our shading system as follows:
						// "constant" -> MATV1_ATTR_SHADE_MODE_CONSTANT 
						// "matte"    -> MATV1_ATTR_SHADE_MODE_FLAT
						// "plastic"  -> MATV1_ATTR_SHADE_MODE_GOURAUD
						// "phong"    -> MATV1_ATTR_SHADE_MODE_FASTPHONG 
						// and in the case that in the "color" class, we found a "texture map" then the "shading mode" is
						// "texture map" -> MATV1_ATTR_SHADE_MODE_TEXTURE 
						// which must be logically or'ed with the other previous modes

						//  look for the "shader class: color"
						while (true)
						{
							fnGetLine(strLine);
							if (parser.RegexPatternMatch(strLine, regexInfo3))
							{
								break;
							}
						}

						// now look for the shader name for this class
						// Shader name: "plain color" or Shader name: "texture map"
						while (true)
						{
							fnGetLine(strLine);

							std::string formatStrLine;
							StrUtility::StrReplaceAnychar(formatStrLine, strLine, "\"", " ");

							// is this a "plain color" poly?
							if (parser.RegexPatternMatch(formatStrLine, regexInfo6))
							{
								// not much to do this is default, we need to wait for the reflectance type
								// to tell us the shading mode
								break;
							}

							// is this a "texture map" poly?
							if (parser.RegexPatternMatch(formatStrLine, regexInfo7))
							{
								// set the texture mapping flag in material
								material.SetAttr(material.GetAttr() | Material::ATTR_SHADE_TEXTURE);

								// for find texture file name
								// e.g.: 
								// file name: string "D:\WINXPP Program Files\ts5\Textures\metal04.bmp"
								while (true)
								{
									fnGetLine(strLine);
									if (parser.RegexPatternMatch(strLine, regexInfo8))
									{
										auto fileName = parser.GetMatchedVal<std::string>(4);
										material.SetTextureFileName(fileName.c_str());

										//just load file & missed attribute:
										//S repeat : float 1
										//T repeat : float 1
										//S offset : float 0
										//T offset : float 0
										//animate : bool 0
										//filter : bool 0

										object.texture = material.GetTexture();
										object.attr |= Object::ATTR_TEXTURES;
										break;
									}
								}
								break;
							}
						}
						
						//  look for the "Shader class: reflectance"
						while (true)
						{
							fnGetLine(strLine);
							if (parser.RegexPatternMatch(strLine, regexInfo9))
							{
								break;
							}
						}

						// looking for "Shader name: "xxxxxx" (xxxxxx)"
						while (true)
						{
							fnGetLine(strLine);

							if (parser.RegexPatternMatch(strLine, regexInfo5))
							{
								auto shaderName = parser.GetMatchedVal<std::string>(3);
								if (shaderName == "constant")
									material.SetAttr(material.GetAttr() | Material::ATTR_SHADE_PURE);
								else if (shaderName == "matte")
									material.SetAttr(material.GetAttr() | Material::ATTR_SHADE_FLAT);
								else if (shaderName == "plastic")
									material.SetAttr(material.GetAttr() | Material::ATTR_SHADE_GOURAUD);
								else if (shaderName == "phong")
									material.SetAttr(material.GetAttr() | Material::ATTR_SHADE_PHONG);
								else
									material.SetAttr(material.GetAttr() | Material::ATTR_SHADE_FLAT);

								break;
							}
						}

						break;
					}
				}
			}
			
			// at this point polyMaterials holds all the indices for the polygon materials (zero based, so they need fix up)
			// and we must access the materials array to fill in each polygon with the polygon color, etc.
			// now that we finally have the material library loaded
			for (int i = 0; i < object.numPolys; ++i)
			{
				Poly& poly = object.pList[i];
				Vertex& v0 = object.vListLocal[poly.vert[0]];
				Vertex& v1 = object.vListLocal[poly.vert[1]];
				Vertex& v2 = object.vListLocal[poly.vert[2]];
				
				poly.attr |= POLY_ATTR_RGB32;
				
				// Find material
				int materialId = polyMaterials[i];
				const Material* material = MaterialManager::GetInstance().GetMaterial(materialId);
				assert(material != nullptr);
	
				// set poly color to vertex color
				Color color;				
				if ((material->attr & Material::ATTR_SHADE_TEXTURE) != 0)
					color.Set(255, 255, 255);
				else
					color = material->color;
				v0.color = color;
				v1.color = color;
				v2.color = color;
				
				// Fill shade model
				if ((material->attr & Material::ATTR_SHADE_PURE))
					poly.attr |= POLY_ATTR_SHADE_PURE;
				else if ((material->attr & Material::ATTR_SHADE_FLAT))
					poly.attr |= POLY_ATTR_SHADE_FLAT;
				else if ((material->attr & Material::ATTR_SHADE_GOURAUD))
				{
					poly.attr |= POLY_ATTR_SHADE_GOURAUD;
					v0.attr |= Vertex::Attr_Normal;
					v1.attr |= Vertex::Attr_Normal;
					v2.attr |= Vertex::Attr_Normal;
				}
				else if ((material->attr & Material::ATTR_SHADE_PHONG))
				{
					poly.attr |= POLY_ATTR_SHADE_PHONG;
					v0.attr |= Vertex::Attr_Normal;
					v1.attr |= Vertex::Attr_Normal;
					v2.attr |= Vertex::Attr_Normal;
				}
				else if ((material->attr & Material::ATTR_SHADE_TEXTURE))
				{
					poly.attr |= POLY_ATTR_SHADE_TEXTURE;
					poly.texture = object.texture;
					v0.attr |= Vertex::Attr_Texture;
					v1.attr |= Vertex::Attr_Texture;
					v2.attr |= Vertex::Attr_Texture;
				}
			}

			object.ComputeVertexNormals();

			return true;
		}
	}
}