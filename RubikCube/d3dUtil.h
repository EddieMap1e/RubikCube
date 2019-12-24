#pragma once

// D3D工具集

#include <d3d11_1.h>			// 已包含Windows.h
#include <DirectXCollision.h>	// 已包含DirectXMath.h
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <wincodec.h>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include "DDSTextureLoader.h"	
#include "WICTextureLoader.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std::experimental;
using namespace std;


// 纹理数组相关函数
ComPtr<ID3D11ShaderResourceView> CreateDDSTexture2DArrayFromFile(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	const std::vector<std::wstring>& filenames,
	UINT maxMipMapSize = 0);