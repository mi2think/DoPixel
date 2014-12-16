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

namespace DoPixel
{
	namespace Core
	{
		void CalcObjectRadius(Object& obj)
		{
			obj.avgRadius = 0;
			obj.maxRadius = 0;

			float totalRadius = 0;
			for (int i = 0; i < obj.numVertices; ++i)
			{
				float distVertex = obj.vListLocal[i].Length();
				if (distVertex >= obj.maxRadius)
					obj.maxRadius = int(distVertex);
				totalRadius += distVertex;
			}

			obj.avgRadius = int(totalRadius / obj.numVertices);
		}

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

		bool LoadObjectFromPLG(Object& obj, const char* fileName, const Vector4f& scale, const Vector4f& pos)
		{
			FileParser fileParser;
			fileParser.Open(fileName);

			memset(&obj, 0, sizeof(obj));
			obj.state = Object::STATE_ACTIVE | Object::STATE_VISIBLE;
			obj.worldPos = pos;

			std::string strLine;
			if (! fileParser.GetLine(strLine))
				return false;

			sscanf_s(strLine.c_str(), "%s %d %d", obj.name, sizeof(obj.name), &obj.numVertices, &obj.numPolys);

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
			}

			// Calc avg radius and max radius
			CalcObjectRadius(obj);

			// Load ploy list
			int polySurfaceDesc = 0;
			int polyNumVerts = 0;
			char tmp[16] = { 0 };
			for (int i = 0; i < obj.numPolys; ++i)
			{
				if (!fileParser.GetLine(strLine))
					return false;

				// Assume each ploy is triangle
				sscanf_s(strLine.c_str(), "%s %d %d %d %d", tmp, sizeof(tmp), &polyNumVerts, &obj.pList[i].vert[0], &obj.pList[i].vert[1], &obj.pList[i].vert[2]);
				if (tmp[0] == '0' && toupper(tmp[1]) == 'X')
					sscanf_s(tmp, "%x", &polySurfaceDesc);
				else
					polySurfaceDesc = atoi(tmp);

				// Let ploy vertex list be object vertex list 
				obj.pList[i].vlist = obj.vListLocal;

				// Analyze ploy surface desc

				// Side
				if (polySurfaceDesc & PLX_2SIDE_FLAG)
					obj.pList[i].attr |= POLY_ATTR_2SIDE;

				// Color
				if (polySurfaceDesc & PLX_COLOR_MODE_RGB_FLAG)
				{
					obj.pList[i].attr |= POLY_ATTR_RGB24;

					int red = ((polySurfaceDesc & 0x0f00) >> 8);
					int green = ((polySurfaceDesc & 0x00f0) >> 4);
					int blue = (polySurfaceDesc & 0x000f);

					// In file. RGB is 4.4.4, in virtual color system convert 8.8.8 to 5.5.5 or 5.5.6
					// So, 4.4.4 -> 8.8.8
					obj.pList[i].color = RGB24(red, green, blue);
				}
				else
				{
					obj.pList[i].attr |= POLY_ATTR_8BITCOLOR;
					obj.pList[i].color = Color(polySurfaceDesc & 0x00ff);
					obj.pList[i].shadeColor = obj.pList[i].color;
				}

				int shadeMode = (polySurfaceDesc & PLX_SHADE_MODE_MASK);
				switch (shadeMode)
				{
				case PLX_SHADE_MODE_PURE_FLAG:
					obj.pList[i].attr |= POLY_ATTR_SHADE_PURE;
					break;
				case PLX_SHADE_MODE_FLAT_FLAG:
					obj.pList[i].attr |= POLY_ATTR_SHADE_FLAT;
					break;
				case PLX_SHADE_MODE_GUARD_FLAG:
					obj.pList[i].attr |= POLY_ATTR_SHADE_GOURAUD;
					break;
				case PLX_SHADE_MODE_PHONG_FLAG:
					obj.pList[i].attr |= POLY_ATTR_SHADE_PHONG;
					break;
				default:
					break;
				}

				obj.pList[i].state = POLY_STATE_ACTIVE;
			}
			return 0;
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
			obj.worldPos = pos;

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

					obj.vListLocal[i] = Vector4f(x, y, z, 1);
					++i;
				}
			}

			CalcObjectRadius(obj);

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

						poly.color = Color((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
						poly.shadeColor = poly.color;
						poly.attr = POLY_ATTR_SHADE_FLAT;
						poly.state = POLY_STATE_ACTIVE;
					}

					++i;
				}
			}

			return true;
		}
	}
}