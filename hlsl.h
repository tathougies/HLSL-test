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
#ifndef __hlsl_H__
#define __hlsl_H__

#define PIXEL_SHADER_TEST_LINEAR       1
#define PIXEL_SHADER_TEST_PROBE        2

#define SAMPLE_PROBE_RGB      0x00FFFFFF
#define SAMPLE_PROBE_RGBA     0xFFFFFFFF

#define CONSTANT_TYPE_FLOAT            1
#define CONSTANT_TYPE_INT              2
#define CONSTANT_TYPE_FLOAT4           3

#define CONSTANT_TABLE_BEGIN(name) struct constant_table name[] = {
#define INT(name, value)   {name, CONSTANT_TYPE_INT, {value, 0, 0, 0}},
#define FLOAT(name, value) {name, CONSTANT_TYPE_FLOAT, {value, 0, 0, 0}},
#define FLOAT4(name, v1, v2, v3, v4) {name, CONSTANT_TYPE_FLOAT4, {v1,v2,v3,v4}}poka
#define CONSTANT_TABLE_END() {NULL, 0, 0} }

struct constant_table
{
  char* name;
  int type;
  float value[4];
};

struct pixel_shader_test
{
  UINT type;
  char* name; /* The name of the test */
  const char* shader; /* The shader code to run */
  LPCSTR version;
  UINT flags;
  struct constant_table* constants;
};

struct pixel_shader_test_linear
{
  struct pixel_shader_test head;
  int samples;
  int max_ulps;
  float* expected;
};

struct sample_probe
{
  DWORD mask;
  int sample;
  DWORD color;
};

struct pixel_shader_test_probe
{
  struct pixel_shader_test head;
  int samples;
  int probe_cnt;
  struct sample_probe* probes;
};

#endif

