#ifndef TOMATO_GRAPHICS_HPP_
#define TOMATO_GRAPHICS_HPP_

#include "core.hpp"

namespace tom
{

struct graphics_state
{
    ID3D11Device1 *device;
    ID3D11DeviceContext1 *device_context;
    ID3D11RenderTargetView *frame_buf_view;
    ID3D11DepthStencilView *depth_buf_view;
    ID3D11DepthStencilState *depth_stencil_state;
    ID3D11RasterizerState1 *rasterizer_state;
    ID3D11SamplerState *sampler_state;
    IDXGISwapChain1 *swap_chain;
    ID3D11PixelShader *pixel_shader;
    ID3D11VertexShader *vertex_shader;
    ID3D11InputLayout *input_layout;
    ID3D11Buffer *const_buf;
    ID3D11Buffer *vert_buf;
    ID3D11Buffer *ind_buf;
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *texture_view;
    D3D11_VIEWPORT viewport;
    u32 stride;
    u32 offset;
};

void d3d_init(HWND hwnd, graphics_state *state);

}  // namespace tom

#endif  // ! TOMATO_GRAPHICS_HPP_