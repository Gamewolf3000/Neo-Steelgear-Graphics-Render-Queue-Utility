#include "ComputePipelineState.h"

#include <stdexcept>

ComputePipelineState::ComputePipelineState()
{
	ZeroMemory(&desc, sizeof(desc));
	desc.NodeMask = 0;
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
}

void ComputePipelineState::SetRootSignature(
	ID3D12RootSignature* rootSignature)
{
	desc.pRootSignature = rootSignature;
}

void ComputePipelineState::SetShader(const std::string& path)
{
	shaderBlob = LoadBlob(path);
	desc.CS.BytecodeLength = shaderBlob->GetBufferSize();
	desc.CS.pShaderBytecode = shaderBlob->GetBufferPointer();
}

void ComputePipelineState::SetCachedPSO(const std::string& path)
{
	cachedPSO = LoadBlob(path);
	desc.CachedPSO.CachedBlobSizeInBytes = shaderBlob->GetBufferSize();
	desc.CachedPSO.pCachedBlob = shaderBlob->GetBufferPointer();
}

void ComputePipelineState::Finalize(ID3D12Device* deviceToUse)
{
	HRESULT hr = S_OK;
	hr = deviceToUse->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipelineState));

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
			throw std::runtime_error("Could not create compute pipeline state");
		}
	}
}