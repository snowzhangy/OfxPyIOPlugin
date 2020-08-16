#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

// the one OFX header we need, it includes the others necessary
//#include <ofxImageEffect.h>
#include <ofxsImageEffect.h>
#include <ofxsProcessing.h>
//#include "common.h"
//python
#include "CTPython.h"
///////////////
static CTPython* myPython = NULL;

static CTPython *getCTPython()
{
	return myPython;
}
///////////////
#define kPluginName "PyIOPlugin"
#define kPluginGrouping "Mumbear"
#define kPluginDescription \
"------------------------------------------------------------------------------------------------------------------ \n" \
"PyIOPlugin is a openfx plugin to use python for image processing"

#define kPluginIdentifier "Mumbear.PyIOPlugin"
#define kPluginVersionMajor 1
#define kPluginVersionMinor 0
#define kSupportsTiles false
#define kSupportsMultiResolution false
#define kSupportsMultipleClipPARs false
#define kPyScript "C:\\openfx\\plugins\\PythonIO\\pysample\\pyiotest.py"
#define kPyInitFunction "py_init"
#define kPyUpdateFunction "py_update"
#define kPyShutdownFunction "py_shutdown"
/////////////////
inline int getImageComponentsCount(OFX::PixelComponentEnum _pixelComponents) {
	switch (_pixelComponents) {
	case OFX::ePixelComponentAlpha:
		return 1;
	case OFX::ePixelComponentNone:
		return 0;
	case OFX::ePixelComponentRGB:
		return 3;
	case OFX::ePixelComponentRGBA:
		return 4;
	case OFX::ePixelComponentCustom:
	default:
		return 0;
	}
}
inline int getImageDepthBytes(OFX::BitDepthEnum _pixelDepth) {
	switch (_pixelDepth) {
	case OFX::eBitDepthNone:
		return 0;
	case OFX::eBitDepthUByte:
		return 8;
	case OFX::eBitDepthUShort:
		return 16;
	case OFX::eBitDepthHalf:
		return 16;
	case OFX::eBitDepthFloat:
		return 32;
	default:
		return 0;
	}
}

/////////////////////////////
class PyIO : public OFX::ImageProcessor
{
public:
	explicit PyIO(OFX::ImageEffect& p_Instance);

	virtual void processImagesCUDA() {};
	virtual void processImagesOpenCL() {};
	virtual void multiThreadProcessImages(OfxRectI p_ProcWindow);

	void setSrcImg(OFX::Image* p_SrcImg);

private:
	OFX::Image* _srcImg;
};

class  PyIOPlugin : public OFX::ImageEffect
{
public:
	explicit PyIOPlugin(OfxImageEffectHandle p_Handle);
	~PyIOPlugin();
	virtual void render(const OFX::RenderArguments& p_Args);
	virtual bool isIdentity(const OFX::IsIdentityArguments& p_Args, OFX::Clip*& p_IdentityClip, double& p_IdentityTime);
	virtual void changedParam(const OFX::InstanceChangedArgs& p_Args, const std::string& p_ParamName);
	void setupAndProcess(PyIO& p_PyIO, const OFX::RenderArguments& p_Args);

private:
	OFX::Clip* m_DstClip;
	OFX::Clip* m_SrcClip;
	OFX::DoubleParam* m_Scale;
	OFX::StringParam* m_PyPath;
};
/////////////////////////////
class PyIOPluginFactory : public OFX::PluginFactoryHelper<PyIOPluginFactory>
{
public:
	PyIOPluginFactory();
	virtual void load() {}
	virtual void unload() {}
	virtual void describe(OFX::ImageEffectDescriptor& p_Desc);
	virtual void describeInContext(OFX::ImageEffectDescriptor& p_Desc, OFX::ContextEnum p_Context);
	virtual OFX::ImageEffect* createInstance(OfxImageEffectHandle p_Handle, OFX::ContextEnum p_Context);
};
