#include "VertexPipelineState.h"

#include <fstream>

#include <d3dcompiler.h>

VertexPipelineState::VertexPipelineState()
{
	ZeroMemory(&desc, sizeof(desc));

	desc.SampleMask = UINT(-1);

	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	desc.RasterizerState.DepthBias = 0;
	desc.RasterizerState.DepthBiasClamp = 0.0f;
	desc.RasterizerState.SlopeScaledDepthBias = 0.0f;
	desc.RasterizerState.DepthClipEnable = true;

	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 0;
	desc.SampleDesc.Count = 1;
	desc.NodeMask = 0;

	desc.DepthStencilState.DepthEnable = true;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	desc.DepthStencilState.StencilEnable = false;
	desc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	desc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	desc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	desc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	desc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
}

void VertexPipelineState::SetBasedOnWindow(unsigned int width, unsigned int height)
{
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.top = 0;
	scissorRect.bottom = height;
	scissorRect.left = 0;
	scissorRect.right = width;
}

void VertexPipelineState::SetRootSignature(ID3D12RootSignature* rootSignature)
{
	desc.pRootSignature = rootSignature;
}

void VertexPipelineState::SetShader(ShaderStage stage, const std::string& path)
{
	switch (stage)
	{
	case ShaderStage::VS:
		shaderBlobs[0] = LoadBlob(path);
		desc.VS.BytecodeLength = shaderBlobs[0]->GetBufferSize();
		desc.VS.pShaderBytecode = shaderBlobs[0]->GetBufferPointer();
		break;
	case ShaderStage::HS:
		shaderBlobs[1] = LoadBlob(path);
		desc.HS.BytecodeLength = shaderBlobs[1]->GetBufferSize();
		desc.HS.pShaderBytecode = shaderBlobs[1]->GetBufferPointer();
		break;
	case ShaderStage::DS:
		shaderBlobs[2] = LoadBlob(path);
		desc.DS.BytecodeLength = shaderBlobs[2]->GetBufferSize();
		desc.DS.pShaderBytecode = shaderBlobs[2]->GetBufferPointer();
		break;
	case ShaderStage::GS:
		shaderBlobs[3] = LoadBlob(path);
		desc.GS.BytecodeLength = shaderBlobs[3]->GetBufferSize();
		desc.GS.pShaderBytecode = shaderBlobs[3]->GetBufferPointer();
		break;
	case ShaderStage::PS:
		shaderBlobs[4] = LoadBlob(path);
		desc.PS.BytecodeLength = shaderBlobs[4]->GetBufferSize();
		desc.PS.pShaderBytecode = shaderBlobs[4]->GetBufferPointer();
		break;
	default:
		throw std::runtime_error("Incorrect shader stage type when setting shader in vertex pipeline state");
		break;
	}
}

void VertexPipelineState::SetFormatDSV(DXGI_FORMAT format)
{
	desc.DSVFormat = format;
}

void VertexPipelineState::AddRenderTarget(RenderTarget& renderTarget)
{
	UINT currentRenderTarget = desc.NumRenderTargets++;
	desc.RTVFormats[currentRenderTarget] = renderTarget.format;
	desc.BlendState.RenderTarget[currentRenderTarget] = renderTarget.blendDesc;
}

void VertexPipelineState::SetCachedPSO(const std::string& path)
{
	cachedPSO = LoadBlob(path);
	desc.CachedPSO.CachedBlobSizeInBytes = cachedPSO->GetBufferSize();
	desc.CachedPSO.pCachedBlob = cachedPSO->GetBufferPointer();
}

D3D12_RASTERIZER_DESC& VertexPipelineState::RasterizerDesc()
{
	return desc.RasterizerState;
}

D3D12_BLEND_DESC& VertexPipelineState::BlendDesc()
{
	return desc.BlendState;
}

D3D12_DEPTH_STENCIL_DESC& VertexPipelineState::DepthStencilDesc()
{
	return desc.DepthStencilState;
}

D3D12_STREAM_OUTPUT_DESC& VertexPipelineState::StreamOutputDesc()
{
	return desc.StreamOutput;
}

D3D12_VIEWPORT& VertexPipelineState::Viewport()
{
	return viewport;
}

D3D12_RECT& VertexPipelineState::ScissorRect()
{
	return scissorRect;
}

D3D_PRIMITIVE_TOPOLOGY& VertexPipelineState::PrimitiveTopology()
{
	return topology;
}

void VertexPipelineState::Finalize(ID3D12Device* deviceToUse)
{
	HRESULT hr = S_OK;
	hr = deviceToUse->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));

	if (FAILED(hr))
	{
		if (hr == D3D12_ERROR_DRIVER_VERSION_MISMATCH || hr == D3D12_ERROR_ADAPTER_NOT_FOUND)
		{
			desc.CachedPSO.CachedBlobSizeInBytes = 0;
			desc.CachedPSO.pCachedBlob = nullptr;
			Finalize(deviceToUse);
		}
		else
		{
			throw std::runtime_error("Could not create graphics pipeline state");
		}
	}
}