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
#define FLOAT4(name, v1, v2, v3, v4) {name, CONSTANT_TYPE_FLOAT4, {v1,v2,v3,v4}},
#define CONSTANT_TABLE_END() {NULL, 0, 0} };

struct constant_table
{
  char* name;
  int type;
  float value[4];
};

/* Probes tell the testing framework to examine the color output of certain pixels,
   and raise an error if they're not what we expected */
struct sample_probe
{
  /* Lets us specify a mask so we can specify which bits we want to check
	 You'd normally want to use SAMPLE_PROBE_RGB and SAMPLE_PROBE_RGBA here*/
  DWORD mask;
  int sample;
  DWORD color;
};

/* This is the base structure for writing a pixel shader test.
   This structure is never used plain. Instead, use the specific
   structure for the type of test you're writing. */
struct pixel_shader_test
{
  UINT type; /* The type of this test. So the engine knows how to treat the struct */
  char* name; /* The name of the test */
  const char* shader; /* The shader code to run */
  LPCSTR version; /* The pixel shader profile to run under */
  UINT flags; /* Any flags for the test. Currently unused */
  /* A table to an array of struct constant_table's. Using this
	 you can set constants for the shader. If you leave this as
	 NULL, no constants will be set. Construct the constant table
	 using the appropriate macros. For example.
	 CONSTANT_TABLE_BEGIN(name)
	 INT(name, 8)
	 CONTANT_TABLE_END(); */
  struct constant_table* constants;
};

/* A pixel shader test that makes sure that the output of our
   shader matches an array of expected values. The pixel
   shader is drawn in a samples x 1 area and should use
   the VPOS semantic's x component to determine its position.

   Note: The use of VPOS was decided upon because texture coordinates
   seemed to give very different results on different platforms.
   Screen position was perfectly consistent.
*/
struct pixel_shader_test_linear
{
  struct pixel_shader_test head; /* The parent class */
  int samples; /* How many samples to take */
  /* How many Units in the Last Place errors we should tolerate.
	 You should determine this after examining the output of the
	 shader on a few other platforms */
  int max_ulps;
  /* An array of floating point numbers of sample length that represent our expected values */
  float* expected; 
};

/* Specifies a test where we want to examine the value of only some
   pixels and ensure that their colors match up exactly to what
   we expected.

   Once again use the VPOS semantic.
*/
struct pixel_shader_test_probe
{
  struct pixel_shader_test head; /* see struct pixel_shader_test_linear */
  int samples; /* See struct pixel_shader_test_linear */
  int probe_cnt; /* The number of probes to do */
  struct sample_probe* probes; /* A pointer to an array of struct sample_probe's of length probe_cnt */
};

#endif

