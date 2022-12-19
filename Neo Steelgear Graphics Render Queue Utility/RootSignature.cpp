#include "RootSignature.h"

#include <string>
#include <stdexcept>

UINT RootSignature::AddRootBuffer(const RootParameter& rootParameter)
{
	D3D12_ROOT_PARAMETER toAdd;
	toAdd.ParameterType = rootParameter.viewType;
	toAdd.ShaderVisibility = rootParameter.visibility;
	toAdd.Descriptor.ShaderRegister = rootParameter.shaderRegister;
	toAdd.Descriptor.RegisterSpace = rootParameter.registerSpace;

	rootParameters.push_back(toAdd);
	return static_cast<UINT>(rootParameters.size() - 1);
}

void RootSignature::AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& samplerDesc)
{
	staticSamplers.push_back(samplerDesc);
}

void RootSignature::Finalize(ID3D12Device* deviceToUse)
{
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = static_cast<unsigned int>(rootParameters.size());
	desc.pParameters = rootParameters.size() != 0 ? &rootParameters[0] : nullptr;
	desc.NumStaticSamplers = static_cast<UINT>(staticSamplers.size());
	desc.pStaticSamplers = staticSamplers.size() > 0 ? &staticSamplers[0] : nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

	ID3DBlob* serialized;
	ID3DBlob* error;
	HRESULT hr = D3D12SerializeRootSignature(&desc,
		D3D_ROOT_SIGNATURE_VERSION_1_0, &serialized, &error);

	if (FAILED(hr))
	{
		std::string blobMessage =
			std::string(static_cast<char*>(error->GetBufferPointer()));
		throw std::runtime_error("Could not serialize root signature: " + blobMessage);
	}

	hr = deviceToUse->CreateRootSignature(0, serialized->GetBufferPointer(),
		serialized->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	if (FAILED(hr))
		throw std::runtime_error("Could not create root signature");

	serialized->Release();
}

ID3D12RootSignature* RootSignature::GetRootSignature() const
{
	return rootSignature;
}