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

static struct sample_probe probes[] = {
	{SAMPLE_PROBE_RGB, 15, D3DCOLOR_XRGB(0, 255, 0)},
	{SAMPLE_PROBE_RGB, 25, D3DCOLOR_XRGB(0, 0, 255)},
};

struct pixel_shader_test_probe if_greater_test =
{
	{PIXEL_SHADER_TEST_PROBE, "if_greater",
	 "uniform float samples;\n"
	 "float4 test(float2 pos: VPOS): COLOR\n"
	 "{\n"
	 "  if(pos.x > 20.0)\n"
	 "     return float4(0, 0, 1, 0);\n"
	 "  else\n"
	 "     return float4(0, 1, 0, 0);\n"
	 "}\n",
	 "ps_3_0", 0, NULL},
	32, 2, probes
};
