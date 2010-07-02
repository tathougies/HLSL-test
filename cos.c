/*
 * Copyright (C) 2010 - Travis Athougies
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#define CINTERFACE
#include <d3d9.h>
#include <d3dx9.h>
#include <assert.h>

#include "hlsl.h"

static float expected[] = {
	1.00000000000000000000f, 0.99039268493652344000f, 0.96193975210189819000f, 0.91573476791381836000f,
	0.85355335474014282000f, 0.77778506278991699000f, 0.69134169816970825000f, 0.59754514694213867000f,
	0.50000000000000000000f, 0.40245485305786133000f, 0.30865830183029175000f, 0.22221493721008301000f,
	0.14644661545753479000f, 0.08426526188850402800f, 0.03806024789810180700f, 0.00960737466812133790f,
	0.00000000000000000000f, 0.00960737466812133790f, 0.03806024789810180700f, 0.08426526188850402800f,
	0.14644661545753479000f, 0.22221493721008301000f, 0.30865830183029175000f, 0.40245485305786133000f,
	0.50000000000000000000f, 0.59754514694213867000f, 0.69134169816970825000f, 0.77778506278991699000f,
	0.85355335474014282000f, 0.91573476791381836000f, 0.96193975210189819000f, 0.99039268493652344000f,
};

struct pixel_shader_test_linear cos_test =
{
	{PIXEL_SHADER_TEST_LINEAR, "cos",
	 "uniform float samples;\n"											
	 "float4 test(float2 pos: VPOS): COLOR\n"								
	 "{\n"																	
	 "  const float pi2 = 6.2831853071795862;\n"							
	 "  float calcd = (cos((pos.x/samples) * pi2) + 1)/2;"				
	 "  return calcd;\n"													
	 "}\n",
	 "ps_3_0", 0, NULL},
	32, 64, expected
};

