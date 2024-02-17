#pragma once
#include <cstdint>
#include <d3d12.h>

// ルートパラメータ
class RootParameter
{
	friend class RootSignature;
public:
	~RootParameter()
	{
		if (mRootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			delete[] mRootParam.DescriptorTable.pDescriptorRanges;
		}
	}

	// 定数バッファビュー
	void InitConstant(
		uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		mRootParam.Constants.ShaderRegister = shaderRegister;
		mRootParam.Constants.RegisterSpace = 0;
		mRootParam.Constants.Num32BitValues = 0;
		mRootParam.ShaderVisibility = shaderVisibility;
	}

	// デスクリプタテーブル
	void InitDescTable(
		uint32_t numDescRanges, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		mRootParam.DescriptorTable.NumDescriptorRanges = numDescRanges;
		mRootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[numDescRanges];
		mRootParam.ShaderVisibility = shaderVisibility;
	}

	// デスクリプタレンジ
	void SetDescRange(
		uint32_t i, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescs, uint32_t baseShaderRegister)
	{
		D3D12_DESCRIPTOR_RANGE* range =
			(D3D12_DESCRIPTOR_RANGE*)(mRootParam.DescriptorTable.pDescriptorRanges + i);
		range->RangeType = rangeType;
		range->NumDescriptors = numDescs;
		range->BaseShaderRegister = baseShaderRegister;
		range->RegisterSpace = 0;
		range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

private:
	// ルートパラメータ
	D3D12_ROOT_PARAMETER mRootParam;
};
