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
#define COBJMACROS
#include <d3d9.h>
#include <d3dx9.h>
#include <assert.h>
#include <stdio.h>

#include "hlsl.h"

#define ok(pred, message, ...) if(!(pred)) { char buf[1024]; sprintf_s(buf, 1024, message, __VA_ARGS__); MessageBox(NULL, buf, "Error", MB_OK); assert(0); }
#define skip(message, ...)  { char buf[1024]; sprintf_s(buf, 1024, message, __VA_ARGS__); MessageBox(NULL, buf, "Error", MB_OK); assert(0); }
#define trace(message, ...) { char buf[1024]; sprintf_s(buf, 1024, message, __VA_ARGS__); MessageBox(NULL, buf, "Error", MB_OK);}

static HMODULE d3d9_handle = 0;
static LPDIRECT3DVERTEXBUFFER9 geometry = 0;
static LPDIRECT3DVERTEXSHADER9 vshader = 0;
static LPDIRECT3DVERTEXDECLARATION9 vdeclaration = 0;
static LPDIRECT3DDEVICE9 device = 0;

#ifdef GEN_EXPECTED
static HANDLE output = 0;
#endif

extern struct pixel_shader_test_linear cos_test, sin_test;
extern struct pixel_shader_test_probe if_greater_test, vec4_indexing_test1, vec4_indexing_test2;

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
static struct pixel_shader_test* pixel_shader_tests[] = {
  (struct pixel_shader_test*) &cos_test,
  (struct pixel_shader_test*) &sin_test,
  (struct pixel_shader_test*) &if_greater_test,
  (struct pixel_shader_test*) &vec4_indexing_test1,
  (struct pixel_shader_test*) &vec4_indexing_test2,
  NULL
};

/* const struct vertex */
/* { */
/* 	float x, y, z; */
/* } geometry_vertices[4] = { */
/* 	{-1, -1, 0.0}, */
/* 	{-1, 1, 0.0}, */
/* 	{1, -1, 0.0}, */
/* 	{1, 1, 0.0}, */
/* }; */

const struct vertex
{
	float x, y, z;
} geometry_vertices[4] = {
	{-1, -1, 0.0},
	{-1, 1, 0.0},
	{1, -1, 0.0},
	{1, 1, 0.0},
};

const D3DVERTEXELEMENT9 vdeclelements[] = {
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	D3DDECL_END()
};

const char* vshader_hlsl =
	"float4 vshader(float4 pos: POSITION): POSITION\n"
	"{\n"
	"  return pos;\n"
	"}\n";

static HWND create_window(void)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = "d3d9_test_wc";
    RegisterClass(&wc);

    return CreateWindow("d3d9_test_wc", "d3d9_test",
						0, 0, 0, 0, 0, 0, 0, 0, 0);
}

static int get_refcount(IUnknown* o)
{
	IUnknown_AddRef(o);
	return IUnknown_Release(o);
}

static IDirect3DDevice9 *init_d3d9(void)
{
    IDirect3D9 * (__stdcall * d3d9_create)(UINT SDKVersion) = 0;
    IDirect3D9 *d3d9_ptr = 0;
    IDirect3DDevice9 *device_ptr = 0;
    D3DPRESENT_PARAMETERS present_parameters;
	D3DXMATRIX projection_matrix;

	void* temp_geometry_vertices;
	
	LPD3DXBUFFER compiled;
	LPD3DXBUFFER errors;
	
    HRESULT hres;

    d3d9_create = (void *)GetProcAddress(d3d9_handle, "Direct3DCreate9");
    ok(d3d9_create != NULL, "Failed to get address of Direct3DCreate9\n");
    if (!d3d9_create) return NULL;

    d3d9_ptr = d3d9_create(D3D_SDK_VERSION);
    if (!d3d9_ptr)
    {
        skip("could not create D3D9\n");
        return NULL;
    }
	
    ZeroMemory(&present_parameters, sizeof(present_parameters));
    present_parameters.Windowed = TRUE;
    present_parameters.hDeviceWindow = create_window();
    present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	
    hres = IDirect3D9_CreateDevice(d3d9_ptr, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present_parameters, &device_ptr);
	ok(SUCCEEDED(hres), "Could not create device, IDirect3D9_CreateDevice returned: %08x", hres);

	/* Create the geometry on which our pixel shader will run */
	hres = IDirect3DDevice9_CreateVertexBuffer(device_ptr, 4 * sizeof(struct vertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &geometry, NULL);
	ok(SUCCEEDED(hres), "Could not create vertex buffer, IDirect3DDevice9_CreateVertexBuffer returned: %08x\n", hres);
	
	hres = IDirect3DVertexBuffer9_Lock(geometry, 0, sizeof(geometry_vertices), (void**)&temp_geometry_vertices, 0);
	ok(SUCCEEDED(hres), "IDirect3DVertexBuffer9_Lock returned: %08x\n", hres);
	memcpy(temp_geometry_vertices, geometry_vertices, sizeof(geometry_vertices));
	IDirect3DVertexBuffer9_Unlock(geometry);

	hres = IDirect3DDevice9_CreateVertexDeclaration(device_ptr, vdeclelements, &vdeclaration);
	ok(SUCCEEDED(hres), "Could not create vertex declaration: IDirect3DDevice9_CreateVertexDeclaration returned: %08x\n", hres);
	
	/* Set up projection */
	D3DXMatrixOrthoLH(&projection_matrix, 2.0, 2.0, 0, 1.0);
	IDirect3DDevice9_SetTransform(device_ptr, D3DTS_PROJECTION, &projection_matrix);
	
	hres = D3DXCompileShader(vshader_hlsl, strlen(vshader_hlsl),
							 NULL, NULL, "vshader", "vs_1_1", 0,
							 &compiled, &errors, 0);
	ok(SUCCEEDED(hres), "Vertex shader compilation failed: %s\n", errors->lpVtbl->GetBufferPointer(errors));
	
	hres = IDirect3DDevice9_CreateVertexShader(device_ptr, (DWORD*) compiled->lpVtbl->GetBufferPointer(compiled), &vshader);
	ok(SUCCEEDED(hres), "IDirect3DDevice9_CreateVertexxShader returned: %08x\n", hres);
	IUnknown_Release(compiled);	
	
    return device_ptr;
}

static void set_constant(LPD3DXCONSTANTTABLE constants, struct constant_table* constant)
{
	switch(constant->type) {
	case CONSTANT_TYPE_INT:
		constants->lpVtbl->SetInt(constants, device, constant->name, (int)constant->value[0]);
		break;
	case CONSTANT_TYPE_FLOAT:
		constants->lpVtbl->SetFloat(constants, device, constant->name, constant->value[0]);
		break;
	case CONSTANT_TYPE_FLOAT4:
		break;
	}
}

#ifdef GEN_EXPECTED
static int fmt_results_linear(void* actual, struct pixel_shader_test_linear* ps)
{
  const int per_line = 4;
  static char buf[2 * 1024 * 1024]; /* Give us 2 megs to work with */
  int result = 0;
  int i = 0, j = 0;
  int index = 0, written = 0;
  float* _actual = (float*) actual;
  index += sprintf(&buf[index], "float %s_expected[] = {\n", ps->head.name);
  for(i = 0;i < ps->samples;i += per_line) {
	  for(j = 0;j < per_line && (i + j) < ps->samples;j++) 
		  index += sprintf(&buf[index], " %.20ff,", _actual[i + j]);
	  index += sprintf(&buf[index], "\n");
  }
  index += sprintf(&buf[index], "};\n\n");
  WriteFile(output, (void*)buf, index,  &written, NULL);
  return result;
}
#endif

/* For an explanation of why this works, please see
   www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm */
int ULPS(float A, float B)
{
	int bInt;
	int aInt = *(int*)&A;
	if (aInt < 0)
		aInt = 0x80000000 - aInt;
	bInt = *(int*)&B;
	if (bInt < 0)
		bInt = 0x80000000 - bInt;
	return abs(aInt - bInt);
}

/* Compare floating point arrays using units in the last place (ULPS)
   keeping a count of how many erroneous values we have found */
static int cmp_results_linear(struct pixel_shader_test_linear* ps, float* actual)
{
	int result = 0;
	int i = 0;
	for(i = 0;i < ps->samples;i++)
	  if(ULPS(ps->expected[i], actual[i]) >= ps->max_ulps)
		result ++;
	return result;
}


LPDIRECT3DPIXELSHADER9 compile_pixel_shader(struct pixel_shader_test* ps, LPD3DXCONSTANTTABLE* constants)
{
	LPD3DXBUFFER compiled;
	LPD3DXBUFFER errors;
	LPDIRECT3DPIXELSHADER9 pshader;
	HRESULT hr;
	
	/* Compile the pixel shader for this particular test and create the pixel
	   shader object */
	hr = D3DXCompileShader(ps->shader, strlen(ps->shader),
						   NULL, NULL, "test", ps->version,
						   0, &compiled, &errors, constants);
	ok (hr == D3D_OK, "Pixel shader %s compilation failed: %s\n", ps->name, (char*) errors->lpVtbl->GetBufferPointer(errors));
	
	hr = IDirect3DDevice9_CreatePixelShader(device, (DWORD*) compiled->lpVtbl->GetBufferPointer(compiled), &pshader);
	ok(SUCCEEDED(hr), "IDirect3DDevice9_CreatePixelShader returned: %08x\n", hr);
	IUnknown_Release(compiled);

	return pshader;
}

LPDIRECT3DSURFACE9 compute_shader_linear(struct pixel_shader_test* ps, int samples, D3DFORMAT format)
{
	LPDIRECT3DPIXELSHADER9 pshader;
	LPDIRECT3DSURFACE9 render_target;
	LPDIRECT3DSURFACE9 readback;
	LPD3DXCONSTANTTABLE constants;
	
	HRESULT hr;
	
	pshader = compile_pixel_shader(ps, &constants);
	
	/* Create the render target surface*/
	hr = IDirect3DDevice9_CreateRenderTarget(device, samples, 1, format,
											 D3DMULTISAMPLE_NONE, 0, FALSE, &render_target, NULL);
	ok(hr == D3D_OK, "IDirect3DDevice9_CreateRenderTarget returned: %08x\n", hr);

	/* The Direct3D 9 docs state that we cannot lock a render target surface,
	   instead we must copy the render target onto this surface to lock it */
	hr = IDirect3DDevice9_CreateOffscreenPlainSurface(device, samples, 1, format,
													  D3DPOOL_SYSTEMMEM, &readback, NULL);
	ok(hr == D3D_OK, "IDirect3DDevice9_CreateOffscreenPlainSurface returned: %08x\n", hr);

	hr = IDirect3DDevice9_SetRenderTarget(device, 0, render_target);
	ok(hr == D3D_OK, "IDirect3DDevice9_SetRenderTarget returned: %08x\n", hr);
	
	/* Clear the backbuffer */
	hr = IDirect3DDevice9_Clear(device, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	ok(hr == D3D_OK, "IDirect3DDevice9_Clear returned: %08x\n", hr);

	/* Start drawing the geometry which will fill the entire screen */
	hr = IDirect3DDevice9_BeginScene(device);
	ok(hr == D3D_OK, "IDirect3DDevice9_BeginScene returned: %08x\n", hr);

	/* This vertex shader is required, because pixel shaders 3.0 must have either
	   a vertex shader or pretransformed vertices */
	hr = IDirect3DDevice9_SetVertexShader(device, vshader);
	ok(hr == D3D_OK, "IDirect3DDevice9_SetVertexShader returned: %08x\n", hr);
	hr = IDirect3DDevice9_SetPixelShader(device, pshader);
	ok(hr == D3D_OK, "IDirect3DDevice9_SetPixelShader returned: %08x\n", hr);
	/* The VPOS semantic gives us whole numbered values in the range of our screen
	   coordinates (e.g. on a 800x600 screen, VPOS would range from (0,0) to
	   (800.0, 600.0) in (1.0, 1.0) increments. However, our shader tests
	   operate on values in the range [0, 1]. By setting this constant here,
	   we let our shaders convert between VPOS and the clamped values.
	*/
	constants->lpVtbl->SetFloat(constants, device, "samples", (float)samples);
	if(ps->constants) {
		int i = 0;
		for(;ps->constants[i].name;i++)
			set_constant(constants, &ps->constants[i]);
	}
	hr = IDirect3DDevice9_SetVertexDeclaration(device, vdeclaration);
	ok(hr == D3D_OK, "IDirect3DDevice9_SetVertexDeclaration returned: %08x\n", hr);
	IDirect3DDevice9_SetStreamSource(device, 0, geometry, 0, sizeof(struct vertex));
	ok(hr == D3D_OK, "IDirect3DDevice9_SetStreamSource returned: %08x\n", hr);
	IDirect3DDevice9_DrawPrimitive(device, D3DPT_TRIANGLESTRIP, 0, 2);
	ok(hr == D3D_OK, "IDirect3DDevice9_DrawPrimitive returned: %08x\n", hr);

	hr = IDirect3DDevice9_EndScene(device);
	ok(hr == D3D_OK, "IDirect3DDevice9_EndScene returned: %08x\n", hr);

	IDirect3DDevice9_Present(device, NULL, NULL, NULL, NULL);

	IDirect3DDevice9_GetRenderTargetData(device, render_target, readback);
	
	/* release everything */
	IUnknown_Release(pshader);
	IUnknown_Release(render_target);
	IUnknown_Release(constants);

	return readback;
}

LPDIRECT3DSURFACE9 compute_shader(struct pixel_shader_test* ps)
{
	switch(ps->type) {
	case PIXEL_SHADER_TEST_LINEAR:
		return compute_shader_linear(ps, ((struct pixel_shader_test_linear*) ps)->samples, D3DFMT_R32F);
	case PIXEL_SHADER_TEST_PROBE:
		return compute_shader_linear(ps, ((struct pixel_shader_test_probe*) ps)->samples, D3DFMT_A8R8G8B8);
	default:
		trace("Unknown pixel shader type: %08x\n", ps->type);
	}
	return 0;
}

/* The pixel shader tests are designed to run using pixel shader version
   ps_3_0's VPOS semantic. This semantic was chosen, because texture coordinate
   calculations do not have to be the same on every platform, as per the
   Direct3D 9 specification. The VPOS semantic however, holds screen position,
   and since screen pixels are specified in whole numbers, the VPOS semantic
   should the same accross all hardware configurations. In this function, all we
   have to do is ensure that our geometry fills the entire screen and that our
   pixel shader is set. */
static void test_ps_linear(void* data, struct pixel_shader_test_linear* ps)
{
	int error_cnt;

#ifdef GEN_EXPECTED
	fmt_results_linear((float*) data, ps);
#else
	error_cnt =  cmp_results_linear(ps, (float*) data);
	ok(error_cnt == 0, "Results for %s not expected: %d errors\n", ps->head.name, error_cnt);
#endif
}

static void test_ps_probe(void* _data, struct pixel_shader_test_probe* ps)
{
	int i = 0;
	DWORD* data = (DWORD*) _data;
	
	for(;i < ps->probe_cnt;i++)
 		ok((data[ps->probes[i].sample] & ps->probes[i].mask) == (ps->probes[i].color & ps->probes[i].mask), "%s: Probing sample %d turned up the wrong color %08x (should have been %08x)\n", ps->head.name, ps->probes[i].sample, data[ps->probes[i].sample], ps->probes[i].color);
}

static void hlsl_suite(IDirect3DDevice9* device)
{
	LPDIRECT3DSURFACE9 readback;
	D3DLOCKED_RECT lr;
	HRESULT hr;
	struct pixel_shader_test** cur_shader = pixel_shader_tests;
	
	for(;*cur_shader;cur_shader++) {
		readback = compute_shader(*cur_shader);
		hr = IDirect3DSurface9_LockRect(readback, &lr, NULL, D3DLOCK_READONLY);
		ok(hr == D3D_OK, "IDirect3DSurface9_LockRect returned: %08x\n", hr);
		
		switch((*cur_shader)->type) {
		case PIXEL_SHADER_TEST_LINEAR:
			test_ps_linear(lr.pBits, (struct pixel_shader_test_linear*) *cur_shader);
			break;
		case PIXEL_SHADER_TEST_PROBE:
			test_ps_probe(lr.pBits, (struct pixel_shader_test_probe*) *cur_shader);
			break;
		default:
			trace("Unknown type: %08x\n", (*cur_shader)->type);
		}
		hr = IDirect3DSurface9_UnlockRect(readback);
		ok(hr == D3D_OK, "IDirect3DSurface9_UnlockRect returned: %08x\n", hr);
		
		IUnknown_Release(readback);
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	D3DCAPS9 caps;
	ULONG refcount;
  
	d3d9_handle = LoadLibraryA("d3d9.dll");
	if (!d3d9_handle) {
		skip("Could not read d3d9.dll\n");
		return 0;
	}

	device = init_d3d9();
	if (!device) return 0;

#ifdef GEN_EXPECTED
	output = CreateFile("expected.c", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
	
	IDirect3DDevice9_GetDeviceCaps(device, &caps);
	if(caps.PixelShaderVersion & 0xffff) {
		hlsl_suite(device);
	} else skip("No pixel shader support\n");

	refcount = IDirect3DVertexShader9_Release(vshader);
	ok(!refcount, "Vertex shader has %u references left\n", refcount);

	refcount = IDirect3DVertexBuffer9_Release(geometry);
	ok(!refcount, "Vertex buffer has %u references left\n", refcount);

	refcount = IDirect3DVertexDeclaration9_Release(vdeclaration);
	ok(!refcount, "Vertex declaration has %u references left\n", refcount);
	
	refcount = IDirect3DDevice9_Release(device);
	ok(!refcount, "Device has %u references left\n", refcount);
	return 0;
}
