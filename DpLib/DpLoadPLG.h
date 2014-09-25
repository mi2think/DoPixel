/********************************************************************
	created:	2014/06/28
	created:	28:6:2014   22:05
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpLoadPLG.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpLoadPLG
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	PLG File Load
*********************************************************************/

#ifndef __DP_LOAD_PLG__
#define __DP_LOAD_PLG__

#include "DpFileStream.h"
#include "DpObject.h"
#include "DpScopeGuard.h"
#include "DpColor.h"

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

		#define PLX_RGB_MASK		0x8000
		#define PLX_SHADE_MODE_MASK	0x6000
		#define PLX_2SIDE_MASK		0x1000
		#define PLX_COLOR_MASK		0x0fff

		#define PLX_COLOR_MODE_RGB_FLAG			0x8000
		#define PLX_COLOR_MODE_INDEXED_FLAG		0x0000

		#define PLX_2SIDE_FLAG					0x1000
		#define PLX_1SIDE_FLAG					0x0000

		#define PLX_SHADE_MODE_PURE_FLAG		0x0000
		#define PLX_SHADE_MODE_FLAT_FLAG		0x2000
		#define PLX_SHADE_MODE_GUARD_FLAG		0x4000
		#define PLX_SHADE_MODE_PHONG_FLAG		0x6000

		//////////////////////////////////////////////////////////////////////////

		// Skip space & comments, until find a valid line for read data
		// Buffer will be split by line
		inline char* GetLineFromBufferPLG(unsigned int& next, char* buffer)
		{
			char* p = buffer;
			char* ret = NULL;
	
			while (*p != 0)
			{
				if (isspace(*p))
				{
					++p;
					continue;
				}
				else if (*p == '#')
				{
					// Skip to next line
					while (*p != 0 && *p != '\n')
						++p;

					if (*p != 0)
						++p;
				}
				else
					break;
			}

			if (*p != 0)
			{
				ret = p;

				// Clac next
				while (*p != 0 && *p != '\n')
					++p;

				if (*p != 0)
				{
					*p = 0;	// for char*
					++p;
				}

				next = p - ret;
			}

			return ret;
		}

		inline void CalcObjectRadius(Object& obj)
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


		// Load Object from .PLG file
		// Success return 0, else return -1
		// scale: initial scale
		// pos: initial pos
		inline int LoadObjectFromPLG(Object& obj, const char* fileName, const Vector4f& scale, const Vector4f& pos)
		{
			FileStream fileStream(fileName, FileStream::TextRead);
			
			unsigned int fileSize = (unsigned int)fileStream.Size();
			assert(fileSize != -1);
			char* buffer = new char[fileSize + 1];
			fileStream.Read(buffer, fileSize);
			fileStream.Close();
			*(buffer + fileSize) = 0;
			ON_SCOPE_EXIT([& buffer]{ delete buffer; });

			memset(&obj, 0, sizeof(obj));
			obj.state = Object::STATE_ACTIVE | Object::STATE_VISIBLE;
			obj.worldPos = pos;

			unsigned int next = 0;
			char* p = GetLineFromBufferPLG(next, buffer);
			if (! p)
				return -1;

			sscanf_s(p, "%s %d %d", obj.name, sizeof(obj.name), &obj.numVertices, &obj.numPolys);

			// Load vertex list
			p += next;
			for (int i = 0; i < obj.numVertices; ++i, p += next)
			{
				p = GetLineFromBufferPLG(next, p);
				if (! p)
					return -1;

				sscanf_s(p, "%f %f %f", &obj.vListLocal[i].x, &obj.vListLocal[i].y, &obj.vListLocal[i].z);
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
			for (int i = 0; i < obj.numPolys; ++i, p += next)
			{
				p = GetLineFromBufferPLG(next, p);
				if (! p)
					return -1;

				// Assume each ploy is triangle
				sscanf_s(p, "%s %d %d %d %d", tmp, sizeof(tmp), &polyNumVerts, &obj.pList[i].vert[0], &obj.pList[i].vert[1], &obj.pList[i].vert[2]);
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
	}
}

#endif