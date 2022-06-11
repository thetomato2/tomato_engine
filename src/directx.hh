#ifndef TOMATO_DIRECTX_HH_
#define TOMATO_DIRECTX_HH_

#include "core.hh"

namespace tom::d3d
{
struct d3d_context
{
    ID3D11Device1 *device;
    ID3D11DeviceContext1 *device_context;
    ID3D11RenderTargetView *frame_buf_view;
    ID3D11DepthStencilView *depth_buf_view;
    ID3D11DepthStencilState *depth_stencil_state;
    ID3D11RasterizerState1 *rasterizer_state;
    ID3D11SamplerState *sampler_state;
    IDXGISwapChain1 *swap_chain;
};
}  // namespace tom::d3d

#endif  // ! TOMATO_DIRECTX_HPP_