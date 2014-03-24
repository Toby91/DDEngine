#include "ShaderHolder.h"
#include "RenderContext.h"
#include "Config.h"
#include "D3DUtils.h"
#include "DDEUtils.h"

using namespace DDEngine;

ShaderHolder::ShaderHolder(Config& config, RenderContext& renderContext) : config(config), renderContext(renderContext) {
	this->config = config;
	this->isRealTimeCompilerRunning = false;
	this->compileListenOnType = ShaderType::VS;
	this->isAllCompile = false;

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	SystemTimeToFileTime(&sysTime, &(this->lastFileTime));
}

ShaderHolder::~ShaderHolder() {

}

void ShaderHolder::load() {

	// Here you can load engine built-in shaders

}

void ShaderHolder::addVertexShader(string name, wstring path) {
	ID3D11VertexShader* vs;
	HRESULT result = DXUtils::createAndCompileVertexShader(renderContext.device, &path[0], config.CFG_VERTEX_SHADER_ENTRY_POINT.c_str(), config.CFG_VERTEX_SHADER_MODEL.c_str(), &vs);
	
	if (!FAILED(result)) {
		VertexShaderEnvelope envelope;
		envelope.name = name;
		envelope.path = path;
		envelope.vertexShader = vs;
		vertexShaders.insert(VertexShaders::value_type(name, envelope));
	}
}

void ShaderHolder::addPixelShader( string name, wstring path ) {
	ID3D11PixelShader* ps;
	HRESULT result = DXUtils::createAndCompilePixelShader(renderContext.device, &path[0], config.CFG_PIXEL_SHADER_ENTRY_POINT.c_str(), config.CFG_PIXEL_SHADER_MODEL.c_str(), &ps);

	if (!FAILED(result)) {
		PixelShaderEnvelope envelope;
		envelope.name = name;
		envelope.path = path;
		envelope.pixelShader = ps;
		pixelShaders.insert(PixelShaders::value_type(name, envelope));
	}
}

void ShaderHolder::addVertexShaderBinary(string name, wstring path) {
	ID3D11VertexShader* vs;
	HRESULT result = DXUtils::createVertexShaderFromBinary(renderContext.device, &path[0], &vs);
	
	if (!FAILED(result)) {
		VertexShaderEnvelope envelope;
		envelope.name = name;
		envelope.path = path;
		envelope.vertexShader = vs;
		vertexShaders.insert(VertexShaders::value_type(name, envelope));
	}
}

void ShaderHolder::addPixelShaderBinary( string name, wstring path ) {
	ID3D11PixelShader* ps;
	HRESULT result = DXUtils::createPixelShaderFromBinary(renderContext.device, &path[0], &ps);

	if (!FAILED(result)) {
		PixelShaderEnvelope envelope;
		envelope.name = name;
		envelope.path = path;
		envelope.pixelShader = ps;
		pixelShaders.insert(PixelShaders::value_type(name, envelope));
	}
}

void ShaderHolder::addInputLayout(string name, string shaderName, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements) {
	ID3D11InputLayout* inputLayout;
	wstring path;

	VertexShaders::iterator it = vertexShaders.find(shaderName);
	if(it != vertexShaders.end()) {
		path = it->second.path;
	}

	if (path.empty()) return;
	
	HRESULT result;
	
	result = DXUtils::createInputLayout(renderContext.device, &path[0], config.CFG_VERTEX_SHADER_ENTRY_POINT.c_str(), config.CFG_VERTEX_SHADER_MODEL.c_str(), &inputLayout, layout, numElements);

	if (!FAILED(result)) {
		inputLayouts.insert(InputLayouts::value_type(name, inputLayout));
	}
}

void ShaderHolder::addInputLayoutBinary( string name, string shaderName, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements ) {
	ID3D11InputLayout* inputLayout;
	wstring path;

	VertexShaders::iterator it = vertexShaders.find(shaderName);
	if(it != vertexShaders.end()) {
		path = it->second.path;
	}

	if (path.empty()) return;
	
	HRESULT result;
	
	result = DXUtils::createInputLayoutFromBinary(renderContext.device, &path[0], &inputLayout, layout, numElements);

	if (!FAILED(result)) {
		inputLayouts.insert(InputLayouts::value_type(name, inputLayout));
	}
}

void ShaderHolder::addConstantBuffer( string name, UINT byteWidth ) {
	ID3D11Buffer* constantBuffer;
	HRESULT result = DXUtils::createConstatnBuffer(renderContext.device, byteWidth, &constantBuffer);

	if (!FAILED(result)) {
		constantBuffers.insert(ConstantBuffers::value_type(name, constantBuffer));
	}
}

void ShaderHolder::cleanUp() {
	for (VertexShaders::iterator it = vertexShaders.begin(); it != vertexShaders.end(); ++it) {
		it->second.vertexShader->Release();
	}

	for (PixelShaders::iterator it = pixelShaders.begin(); it != pixelShaders.end(); ++it) {
		it->second.pixelShader->Release();
	}

	for (InputLayouts::iterator it = inputLayouts.begin(); it != inputLayouts.end(); ++it) {
		it->second->Release();
	}

	for (ConstantBuffers::iterator it = constantBuffers.begin(); it != constantBuffers.end(); ++it) {
		it->second->Release();
	}
}

ID3D11VertexShader* ShaderHolder::getVertexShader( string name ) {
	VertexShaders::iterator it = vertexShaders.find(name);
	if(it != vertexShaders.end()) {
		return it->second.vertexShader;
	}

	return NULL;
}

ID3D11PixelShader* ShaderHolder::getPixelShader( string name ) {
	PixelShaders::iterator it = pixelShaders.find(name);
	if(it != pixelShaders.end()) {
		return it->second.pixelShader;
	}

	return NULL;
}

ID3D11Buffer* ShaderHolder::getConstatnBuffer( string name )
{
	ConstantBuffers::iterator it = constantBuffers.find(name);
	if(it != constantBuffers.end()) {
		return it->second;
	}

	return NULL;
}

ID3D11InputLayout* ShaderHolder::getInputLayout( string name )
{
	InputLayouts::iterator it = inputLayouts.find(name);
	if(it != inputLayouts.end()) {
		return it->second;
	}

	return NULL;
}

void ShaderHolder::activateVS( string name ) {
		renderContext.context->VSSetShader(getVertexShader(name), NULL, 0);
		activeVS = name;
}

void ShaderHolder::activatePS( string name ) {
		renderContext.context->PSSetShader(getPixelShader(name), NULL, 0);
		activePS = name;
}

void ShaderHolder::activateIL( string name ) {
		renderContext.context->IASetInputLayout(getInputLayout(name));
		activeIL = name;
}

void ShaderHolder::updateConstantBufferVS( string name, const void* bufferData, UINT startSlot ) {
	ID3D11Buffer* constantBuffer = getConstatnBuffer(name);
	renderContext.context->UpdateSubresource(constantBuffer, 0, NULL, bufferData, 0, 0);
	renderContext.context->VSSetConstantBuffers( startSlot, 1, &constantBuffer );
}

void ShaderHolder::updateConstantBufferPS( string name, const void* bufferData, UINT startSlot ) {
	ID3D11Buffer* constantBuffer = getConstatnBuffer(name);
	renderContext.context->UpdateSubresource(constantBuffer, 0, NULL, bufferData, 0, 0);
	renderContext.context->PSSetConstantBuffers( startSlot, 1, &constantBuffer );
}

std::string ShaderHolder::getActiveVS() {
	return this->activeVS;
}

std::string ShaderHolder::getActivePS() {
	return this->activePS;
}

std::string ShaderHolder::getActiveIL() {
	return this->activeIL;
}

void ShaderHolder::refreshShaders() {
	activateVS(activeVS);
	activatePS(activePS);
	activateIL(activeIL);
}

void ShaderHolder::recompileVertexShader( string name ) {
	VertexShaders::iterator it = vertexShaders.find(name);
	if(it != vertexShaders.end()) {
		ID3D11VertexShader* vs;
		
		if (!FileUtils::isFileLocked(it->second.path) && (CompareFileTime(&FileUtils::getLastWriteTime(it->second.path), &this->lastFileTime) == 1)) {
	
			HRESULT result = DXUtils::createAndCompileVertexShader(renderContext.device, &it->second.path[0], 
				config.CFG_VERTEX_SHADER_ENTRY_POINT.c_str(), config.CFG_VERTEX_SHADER_MODEL.c_str(), &vs);

			if (!FAILED(result)) {
				ID3D11VertexShader* old = it->second.vertexShader;
				it->second.vertexShader = vs;
				old->Release();
				this->lastFileTime = FileUtils::getLastWriteTime(it->second.path);
			}

		}
	}
}

void ShaderHolder::recompilePixelShader( string name ) {
	PixelShaders::iterator it = pixelShaders.find(name);
	if(it != pixelShaders.end()) {
		ID3D11PixelShader* ps;

		if (!FileUtils::isFileLocked(it->second.path) && (CompareFileTime(&FileUtils::getLastWriteTime(it->second.path), &this->lastFileTime) == 1)) {
			this->lastFileTime = FileUtils::getLastWriteTime(it->second.path);
			
			HRESULT result = DXUtils::createAndCompilePixelShader(renderContext.device, &it->second.path[0], 
				config.CFG_PIXEL_SHADER_ENTRY_POINT.c_str(), config.CFG_PIXEL_SHADER_MODEL.c_str(), &ps);
		
			if (!FAILED(result)) {
				ID3D11PixelShader* old = it->second.pixelShader;
				it->second.pixelShader = ps;
				old->Release();
			}
		}
	}
}

void ShaderHolder::realTimeCompile() {
	if (isAllCompile) {
		
		for (VertexShaders::iterator it = vertexShaders.begin(); it != vertexShaders.end(); ++it) {
			recompileVertexShader(it->first);
		}

		for (PixelShaders::iterator it = pixelShaders.begin(); it != pixelShaders.end(); ++it) {
			recompilePixelShader(it->first);
		}

	} else if (isRealTimeCompilerRunning) {
		switch (compileListenOnType) {
			
			case VS:
				recompileVertexShader(compileListenOn);
				break;
			
			case PS:
				recompilePixelShader(compileListenOn);
				break;
			
		}	
	}
}

void ShaderHolder::runRealTimeCompiler(string name, string type) {
	this->compileListenOn = name;
	this->isRealTimeCompilerRunning = true;
	if(type == "VS") this->compileListenOnType = ShaderType::VS;
	if(type == "PS") this->compileListenOnType = ShaderType::PS;
}

void ShaderHolder::stopRealTimeCompiler() {
	this->isRealTimeCompilerRunning = false;
	this->isAllCompile = false;
}

void ShaderHolder::runRealTimeCompilerAll() {
	this->isAllCompile = true;
}