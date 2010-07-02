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

static struct sample_probe probes1[] = {
  {SAMPLE_PROBE_RGBA, 10, D3DCOLOR_ARGB(255, 0, 255, 0)},
  {SAMPLE_PROBE_RGBA, 16, D3DCOLOR_ARGB(255, 0, 255, 0)},
  {SAMPLE_PROBE_RGBA, 22, D3DCOLOR_ARGB(255, 0, 255, 0)}, 
};

struct pixel_shader_test_probe vec4_indexing_test1 =
{
	{PIXEL_SHADER_TEST_PROBE, "vec4_indexing_test1",
	 "float4 test(): COLOR\n"
	 "{\n"
	 "  float4 color;\n"
	 "  color[0] = 0.0;\n"
	 "  color[1] = 1.0;\n"
	 "  color[2] = 0.0;\n"
	 "  color[3] = 1.0;\n"
	 "  return color;\n"
	 "}\n",
	 "ps_3_0", 0, },
	32, 3, probes1
};

static struct sample_probe probes2[] = {
	{SAMPLE_PROBE_RGBA, 10, D3DCOLOR_ARGB(255, 0, 255, 0)},
	{SAMPLE_PROBE_RGBA, 16, D3DCOLOR_ARGB(255, 0, 255, 0)},
	{SAMPLE_PROBE_RGBA, 22, D3DCOLOR_ARGB(255, 0, 255, 0)}, 
};

struct constant_table table2[] =
{
	INT_CONSTANT("i", 2),
	{NULL, 0, 0}
};

struct pixel_shader_test_probe vec4_indexing_test2 =
{
	{PIXEL_SHADER_TEST_PROBE, "vec4_indexing_test2",
	 "uniform int i;\n" /* We have this uniform here so the compiler can't optimize */
	 "float4 test(): COLOR\n"
	 "{\n"
	 "  float4 color = float4(0, 0, 1, 1);\n"
	 "  color.g = color[i];\n"
	 "  color.b = 0;\n"
	 "  return color;\n"
	 "}\n",
	 "ps_3_0", 0, table2},
	32, 3, probes2
};

