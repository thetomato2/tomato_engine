#include "graphics.hh"

namespace tom
{

void d3d_init(HWND hwnd, gfx_state *state)
{
    D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };

    ID3D11Device *base_device;
    ID3D11DeviceContext *base_device_context;
    D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                      feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, &base_device,
                      nullptr, &base_device_context);

    ID3D11Device1 *device;
    base_device->QueryInterface(__uuidof(ID3D11Device1), rcast(void **, &device));
    ID3D11DeviceContext1 *device_context;
    base_device_context->QueryInterface(__uuidof(ID3D11DeviceContext1),
                                        rcast(void **, &device_context));
    state->device_context = device_context;
    state->device         = device;

    IDXGIDevice1 *dxgi_device;
    device->QueryInterface(__uuidof(IDXGIDevice1), rcast(void **, &dxgi_device));
    IDXGIAdapter *dxgi_adapter;
    dxgi_device->GetAdapter(&dxgi_adapter);
    IDXGIFactory2 *dxgi_factory;
    dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), rcast(void **, &dxgi_factory));

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    swap_chain_desc.Width              = 0;  // use window width
    swap_chain_desc.Height             = 0;  // use window height
    swap_chain_desc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    swap_chain_desc.Stereo             = FALSE;
    swap_chain_desc.SampleDesc.Count   = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount        = 2;
    swap_chain_desc.Scaling            = DXGI_SCALING_STRETCH;
    swap_chain_desc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_desc.Flags              = 0;

    IDXGISwapChain1 *swap_chain;
    dxgi_factory->CreateSwapChainForHwnd(device, hwnd, &swap_chain_desc, nullptr, nullptr,
                                         &swap_chain);
    state->swap_chain = swap_chain;

    ID3D11Texture2D *frame_buf;
    swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&frame_buf));

    ID3D11RenderTargetView *frame_buf_view;
    device->CreateRenderTargetView(frame_buf, nullptr, &frame_buf_view);
    state->frame_buf_view = frame_buf_view;

    D3D11_TEXTURE2D_DESC depth_buf_desc;
    frame_buf->GetDesc(&depth_buf_desc);  // base on framebuffer properties

    depth_buf_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buf_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D *depth_buf;
    device->CreateTexture2D(&depth_buf_desc, nullptr, &depth_buf);

    ID3D11DepthStencilView *depth_buf_view;
    device->CreateDepthStencilView(depth_buf, nullptr, &depth_buf_view);
    state->depth_buf_view = depth_buf_view;

    D3D11_RASTERIZER_DESC1 rasterizer_desc = {};
    rasterizer_desc.FillMode               = D3D11_FILL_SOLID;
    rasterizer_desc.CullMode               = D3D11_CULL_BACK;

    ID3D11RasterizerState1 *rasterizer_state;
    device->CreateRasterizerState1(&rasterizer_desc, &rasterizer_state);
    state->rasterizer_state = rasterizer_state;

    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter             = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampler_desc.AddressU           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.ComparisonFunc     = D3D11_COMPARISON_NEVER;

    ID3D11SamplerState *sampler_state;
    device->CreateSamplerState(&sampler_desc, &sampler_state);
    state->sampler_state = sampler_state;

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
    depth_stencil_desc.DepthEnable              = TRUE;
    depth_stencil_desc.DepthWriteMask           = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc                = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState *depth_stencil_state;
    device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
    state->depth_stencil_state = depth_stencil_state;
}

}  // namespace tom
