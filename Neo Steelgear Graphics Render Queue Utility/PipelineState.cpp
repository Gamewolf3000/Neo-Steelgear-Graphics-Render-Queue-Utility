#include "PipelineState.h"

#include <fstream>

#include <d3dcompiler.h>

ID3DBlob* PipelineState::LoadBlob(const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("Could not open blob file");

	file.seekg(0, std::ios_base::end);
	size_t size = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios_base::beg);

	ID3DBlob* toReturn = nullptr;
	HRESULT hr = D3DCreateBlob(size, &toReturn);

	if (FAILED(hr))
		throw std::runtime_error("Could not create blob when loading blob file");

	file.read(static_cast<char*>(toReturn->GetBufferPointer()), size);
	file.close();

	return toReturn;
}

void PipelineState::CachePSO(const std::string& path)
{
	ID3DBlob* blob;
	HRESULT hr = pipelineState->GetCachedBlob(&blob);

	if (FAILED(hr))
		throw std::runtime_error("Could not create cached blob for vertex pipeline state");

	std::ofstream file(path, std::ios::binary);
	file.write(static_cast<char*>(blob->GetBufferPointer()), blob->GetBufferSize());
}

ID3D12PipelineState* PipelineState::GetPipelineState()
{
	return pipelineState;
}
