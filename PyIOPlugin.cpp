/*
Software License :

Copyright (c) 2014, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name The Open Effects Association Ltd, nor the names of its
	  contributors may be used to endorse or promote products derived from this
	  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Author : Yang Zhang

  This plugin let you using python to process image using openfx image.
  
*/
#include "PyIOPlugin.h"
// anonymous namespace to hide our symbols in

///////////////////////////////////////////

PyIO::PyIO(OFX::ImageEffect& p_Instance)
	: OFX::ImageProcessor(p_Instance)
{
}
void PyIO::multiThreadProcessImages(OfxRectI p_ProcWindow)
{
	
}
void PyIO::setSrcImg(OFX::Image* p_SrcImg)
{
	_srcImg = p_SrcImg;
}

//////////////////////////////
PyIOPlugin::PyIOPlugin(OfxImageEffectHandle p_Handle)
	: ImageEffect(p_Handle)
{
	m_DstClip = fetchClip(kOfxImageEffectOutputClipName);
	m_SrcClip = fetchClip(kOfxImageEffectSimpleSourceClipName);
	if (!myPython)
		myPython = new CTPython();
	myPython->py_import(kPyScript);
	myPython->py_call(kPyInitFunction);

	m_Scale = fetchDoubleParam("Scale");
	m_PyPath = fetchStringParam("PythonPath");
}

void PyIOPlugin::render(const OFX::RenderArguments& p_Args)
{
	if ((m_DstClip->getPixelDepth() == OFX::eBitDepthFloat) && (m_DstClip->getPixelComponents() == OFX::ePixelComponentRGBA)) {
		PyIO mPyIO(*this);
		setupAndProcess(mPyIO, p_Args);
		////////////////

	}
	else {
		OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
	}
}

PyIOPlugin::~PyIOPlugin()
{
	if (myPython)
		myPython->py_call(kPyShutdownFunction);
}

bool PyIOPlugin::isIdentity(const OFX::IsIdentityArguments& p_Args, OFX::Clip*& p_IdentityClip, double& p_IdentityTime)
{
	if (m_SrcClip) {
		p_IdentityClip = m_SrcClip;
		p_IdentityTime = p_Args.time;
		return true;
	}
	return false;
}

void PyIOPlugin::changedParam(const OFX::InstanceChangedArgs& p_Args, const std::string& p_ParamName)
{
	if (p_ParamName == "Scale") {
		
	}
	if (p_ParamName == "PythonPath") {
		string PATH;
		m_PyPath->getValue(PATH);
		if (myPython)
		{
			myPython->py_import(PATH);
			myPython->py_call(kPyInitFunction);
		}
			
	}
}

void PyIOPlugin::setupAndProcess(PyIO& p_PyIO, const OFX::RenderArguments& p_Args)
{
	std::auto_ptr<OFX::Image> dst(m_DstClip->fetchImage(p_Args.time));
	OFX::BitDepthEnum dstBitDepth = dst->getPixelDepth();
	OFX::PixelComponentEnum dstComponents = dst->getPixelComponents();

	std::auto_ptr<OFX::Image> src(m_SrcClip->fetchImage(p_Args.time));
	OFX::BitDepthEnum srcBitDepth = src->getPixelDepth();
	OFX::PixelComponentEnum srcComponents = src->getPixelComponents();

	if ((srcBitDepth != dstBitDepth) || (srcComponents != dstComponents))
	{
		OFX::throwSuiteStatusException(kOfxStatErrValue);
	}

	p_PyIO.setDstImg(dst.get());
	p_PyIO.setSrcImg(src.get());

	// Setup GPU Render arguments
	// Set the render window
	p_PyIO.setRenderWindow(p_Args.renderWindow);

	// Set the scales
	//p_PyIO.setScales(_convolve, _display, _adjust, _matrix);

	// Call the base class process member, this will call the derived templated process code

	float* input = static_cast<float*>(src->getPixelData());
	float* output = static_cast<float*>(dst->getPixelData());
	int imgSizeY = (p_Args.renderWindow.y2 - p_Args.renderWindow.y1);
	int imgSizeX = (p_Args.renderWindow.x2 - p_Args.renderWindow.x1);
	myPython->py_call(kPyUpdateFunction, input, output, getImageDepthBytes(srcBitDepth),getImageDepthBytes(dstBitDepth),
		imgSizeY,imgSizeX,getImageComponentsCount(srcComponents));

	//p_PyIO.process();
	/////////////////////////////
	//python 
	
}
//////////////////////////////////////////////////////////////////////////////////////////
//

PyIOPluginFactory::PyIOPluginFactory()
	: OFX::PluginFactoryHelper<PyIOPluginFactory>(kPluginIdentifier, kPluginVersionMajor, kPluginVersionMinor)
{
}

void PyIOPluginFactory::describe(OFX::ImageEffectDescriptor& p_Desc)
{
	// basic labels
	p_Desc.setLabels(kPluginName, kPluginName, kPluginName);
	p_Desc.setPluginGrouping(kPluginGrouping);
	p_Desc.setPluginDescription(kPluginDescription);

	// add the supported contexts
	p_Desc.addSupportedContext(OFX::eContextFilter);
	p_Desc.addSupportedContext(OFX::eContextGeneral);

	// add supported pixel depths
	p_Desc.addSupportedBitDepth(OFX::eBitDepthFloat);

	// set a few flags
	p_Desc.setSingleInstance(false);
	p_Desc.setHostFrameThreading(false);
	p_Desc.setSupportsMultiResolution(kSupportsMultiResolution);
	p_Desc.setSupportsTiles(kSupportsTiles);
	p_Desc.setTemporalClipAccess(false);
	p_Desc.setRenderTwiceAlways(false);
	p_Desc.setSupportsMultipleClipPARs(kSupportsMultipleClipPARs);
}
void PyIOPluginFactory::describeInContext(OFX::ImageEffectDescriptor& p_Desc, OFX::ContextEnum /*p_Context*/)
{
	// Source clip only in the filter context
   // create the mandated source clip
	OFX::ClipDescriptor *srcClip = p_Desc.defineClip(kOfxImageEffectSimpleSourceClipName);

	srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
	srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
	srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
	srcClip->setTemporalClipAccess(true);
	srcClip->setSupportsTiles(kSupportsTiles);
	srcClip->setIsMask(false);
	// create the mandated output clip
	OFX::ClipDescriptor *dstClip = p_Desc.defineClip(kOfxImageEffectOutputClipName);
	dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
	dstClip->setSupportsTiles(kSupportsTiles);


	// make some pages and to things in
	OFX::PageParamDescriptor *page = p_Desc.definePageParam("Controls");
	////////////////
	OFX::DoubleParamDescriptor* param = p_Desc.defineDoubleParam("Scale");
	param->setLabel("Scale");
	param->setHint("adjust scale");
	param->setDefault(0.0);
	param->setRange(0.0, 1.0);
	param->setIncrement(0.001);
	param->setDisplayRange(0.0, 1.0);
	page->addChild(*param);

	OFX::StringParamDescriptor*stringparam = p_Desc.defineStringParam("PythonPath");
	stringparam->setLabel("Python File");
	stringparam->setHint("make sure it's the absolute path");
	stringparam->setStringType(OFX::eStringTypeFilePath);
	stringparam->setDefault(kPyScript);
	stringparam->setFilePathExists(true);
	page->addChild(*stringparam);

}


OFX::ImageEffect* PyIOPluginFactory::createInstance(OfxImageEffectHandle p_Handle, OFX::ContextEnum /*p_Context*/)
{
	return new PyIOPlugin(p_Handle);
}

void OFX::Plugin::getPluginIDs(PluginFactoryArray& p_FactoryArray)
{
	static PyIOPluginFactory PyIOPluginf;
	p_FactoryArray.push_back(&PyIOPluginf);
}