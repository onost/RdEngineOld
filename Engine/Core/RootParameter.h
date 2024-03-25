#pragma once
#include <cstdint>
#include <d3d12.h>

class RootParameter
{
	friend class RootSignature;

public:
	RootParameter()
		: mRootParameter()
	{}

	~RootParameter()
	{
		if (mRootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			delete[] mRootParameter.DescriptorTable.pDescriptorRanges;
		}
	}

	void InitConstant(uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		mRootParameter.Constants.ShaderRegister = shaderRegister;
		mRootParameter.Constants.RegisterSpace = 0;
		mRootParameter.Constants.Num32BitValues = 0;
		mRootParameter.ShaderVisibility = shaderVisibility;
	}

	void InitDescriptorTable(uint32_t numDescRanges, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		mRootParameter.DescriptorTable.NumDescriptorRanges = numDescRanges;
		mRootParameter.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[numDescRanges];
		mRootParameter.ShaderVisibility = shaderVisibility;
	}

	void InitDescriptorRange(uint32_t index, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescs, uint32_t baseShaderRegister)
	{
		D3D12_DESCRIPTOR_RANGE* range = (D3D12_DESCRIPTOR_RANGE*)(mRootParameter.DescriptorTable.pDescriptorRanges + index);
		range->RangeType = rangeType;
		range->NumDescriptors = numDescs;
		range->BaseShaderRegister = baseShaderRegister;
		range->RegisterSpace = 0;
		range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

private:
	D3D12_ROOT_PARAMETER mRootParameter;
};
