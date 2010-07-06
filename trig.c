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

/* These values were chosen after comparing the results of both NVIDIA and
   ATI graphics cards on Windows and the results of an NVIDIA card on Linux,
   using the provided gen-expected.c. It was found that the ULPS between
   most values were fairly low, but for a certain number of values (with
   seemingly no correlation), the ULPs error was very, very high (as high
   as 65536). It also seems that the incidence of the higher numbers was
   much lower than that of the lower numbers. Since Direct3D doesn't require
   every renderer to produce exactly the same results, these values were
   chosen to be loose enough so that the test will succeed on a proper
   implementation but strict enough so that an improper implementation will
   cause the test to fail */
static float sin_expected[] = {
	0.50000000000000000000f, 0.59754514694213867000f, 0.69134163856506348000f, 0.77778506278991699000f,
	0.85355341434478760000f, 0.91573476791381836000f, 0.96193975210189819000f, 0.99039268493652344000f,
	1.00000000000000000000f, 0.99039268493652344000f, 0.96193975210189819000f, 0.91573476791381836000f,
	0.85355335474014282000f, 0.77778506278991699000f, 0.69134163856506348000f, 0.59754508733749390000f,
	0.50000000000000000000f, 0.40245491266250610000f, 0.30865836143493652000f, 0.22221493721008301000f,
	0.14644664525985718000f, 0.08426526188850402800f, 0.03806024789810180700f, 0.00960734486579895020f,
	0.00000000000000000000f, 0.00960734486579895020f, 0.03806024789810180700f, 0.08426523208618164100f,
	0.14644661545753479000f, 0.22221490740776062000f, 0.30865836143493652000f, 0.40245485305786133000f,
};

struct pixel_shader_test_linear sin_test = 
{
	{PIXEL_SHADER_TEST_LINEAR, "sin", /* Shader test type and name */										
	 "uniform float samples;\n"
	 "float4 test(float2 pos: VPOS): COLOR\n"
	 "{\n"
	 "  const float pi2 = 6.2831853071795862;\n"
	 "  float calcd = (sin((pos.x/samples) * pi2) + 1)/2;"
	 "  return calcd;\n"
	 "}\n",
	 "ps_3_0", 0, NULL}, /* Shader profile, Flags (unused for now), and constant table */
	32, 64, sin_expected /* Samples, Max ULPS, and expected values */
};

static float cos_expected[] = {
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
	{PIXEL_SHADER_TEST_LINEAR, "cos", /* Shader test type and name */
	 "uniform float samples;\n"											
	 "float4 test(float2 pos: VPOS): COLOR\n"								
	 "{\n"																	
	 "  const float pi2 = 6.2831853071795862;\n"							
	 "  float calcd = (cos((pos.x/samples) * pi2) + 1)/2;"				
	 "  return calcd;\n"													
	 "}\n",
	 "ps_3_0", 0, NULL}, /* Version, flags (unused), and constant table */
	32, 64, cos_expected /* Samples, max ULPs, and expected values */
};

