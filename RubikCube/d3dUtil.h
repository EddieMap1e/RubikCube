#pragma once

// D3D���߼�

#include <d3d11_1.h>			// �Ѱ���Windows.h
#include <DirectXCollision.h>	// �Ѱ���DirectXMath.h
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


// ����������غ���
ComPtr<ID3D11ShaderResourceView> CreateDDSTexture2DArrayFromFile(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	const std::vector<std::wstring>& filenames,
	UINT maxMipMapSize = 0);