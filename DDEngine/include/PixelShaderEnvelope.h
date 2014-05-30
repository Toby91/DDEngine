#pragma once

#include <string>

struct ID3D11PixelShader;

namespace DDEngine
{

struct PixelShaderEnvelope
{
	std::string name;
	std::string entryPoint;
	std::wstring path;
	ID3D11PixelShader* pixelShader;
};
}