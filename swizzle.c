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
	{SAMPLE_PROBE_RGB, 0, D3DCOLOR_XRGB(0, 255, 255)}
};

CONSTANT_TABLE_BEGIN(table);
FLOAT4("color", 1, 0, 0, 1);
CONSTANT_TABLE_END();

struct pixel_shader_test_probe swizzle_test1 =
{
	{PIXEL_SHADER_TEST_PROBE, "swizzle_test1",
	 "uniform float4 color;\n" /* Once again this is to prevent compiler optimization. For tests with optimization, look in optimizations.c */
	 "float4 test(): COLOR\n"
	 "{\n"
	 "  float4 ret = color;\n"
	 "  ret.gb = ret.ra;\n"
	 "  ret.ra = float2(0, 0);\n"
	 "  return ret;\n"
	 "}\n",
	 "ps_3_0", 0, constants
	},
	1, 1, probes
};
