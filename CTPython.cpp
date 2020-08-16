#include "CTPython.h"
/*
  Author : Yang Zhang

  This plugin let you using python to process image using openfx image.
  
*/
//////////////////////////////
//pyobject vector conversion 
template<typename T>
py::array_t<T> type_convert(void* image, int height, int width, int numChannels)
{	
	return py::array_t<T>({ height, width, numChannels }, (T *)image);

}
template<typename T>
void pyObjConvert(py::object& pyReturn,void* dstImg,int size)
{
	py::array_t<T> ret_arr = py::array_t<T>(pyReturn);
	memcpy(dstImg, ret_arr.data(), size*sizeof(T));
}
//
py::object Image2PyObject(void* image, int dstBitDepth, int height,int width,int nComp)
{
	if (image)
	{
		switch (dstBitDepth)
		{
		case 8: return type_convert<unsigned char>(image, height,width,nComp); break;
		case 16: return type_convert<unsigned short>(image, height, width, nComp);break;
		case 32: return type_convert<float>(image, height, width, nComp); break;
		default: MESSAGE("CTPython::Image2PyObject(): Wrong dstBitDepth format!\n");
		}
	}

}

void PyObject2Image(py::object pyReturn, void* image,int size,int dstBitDepth) {
	if (pyReturn)
	{
		switch (dstBitDepth)
		{
		case 8: pyObjConvert<unsigned char>(pyReturn,image, size); break;
		case 16:pyObjConvert<unsigned short>(pyReturn,image, size); break;
		case 32:pyObjConvert<float>(pyReturn,image, size); break;
		default: MESSAGE("CTPython::PyObject2Image(): Wrong dstBitDepth format!\n");
		}
	}

}
CTPython::CTPython()
{
}

CTPython::~CTPython()
{
	pyOSModule.release();
	pyModule.release();
	pyReturn.release();
}

void CTPython::py_import(string file_name)
{
	if (!py::isinstance<py::module>(pyOSModule))
		pyOSModule = py::module::import("os");

	engine_cwd = string(string(py::str(pyOSModule.attr("getcwd")())).c_str());
	python_cwd = dirname(file_name);

	// change current work directory for python
	pyOSModule.attr("chdir")(python_cwd.c_str());

	try {
		string base_name = basename(file_name);
		string module_name = base_name.substr(0, base_name.find("."));
		if (old_module_name == module_name && py::isinstance<py::module>(pyModule))
		{
			pyModule.reload();
			MESSAGE("CTPython::py_import(): Python file '%s' is reloaded successfully!\n", file_name.c_str());
			py::dict pyDict = py::getModuleDict();
			for (auto it = pyDict.begin(); it != pyDict.end(); it++)
			{
				//MESSAGE("%s: %s\n", std::string(py::str(it->first)).c_str(), std::string(py::str(it->second)).c_str());
				string desc = replace(std::string(py::str(it->second)),"\\\\", "/");
				//MESSAGE("desc: %s\n", desc.get());
				vector<string> strs = split(desc.substr(1, desc.size() - 2),string("'"));
				string py_module_name = strs[strs.size() - 1];
				//MESSAGE("py_module_name.dirname(): %s\n", py_module_name.dirname().get());
				if (py_module_name.substr(0, 2) == string("./"))
				{
					//MESSAGE("%s\n", std::string(py::str(it->first)).c_str());
					py::module(pyDict[it->first]).reload();
					MESSAGE("CTPython::py_import(): Python file '%s' is reloaded successfully!\n", py_module_name.c_str());
				}
			}
		}
		else
		{
			pyModule = py::module::import(module_name.c_str());
			MESSAGE("CTPython::py_import(): Python file '%s' is imported successfully!\n", file_name.c_str());
		}

		old_module_name = module_name;
	}
	catch (const py::error_already_set &e) {
		MESSAGE("CTPython::py_import(): Error: %s\n", e.what());
		is_initialized = false;
	}

	// restore current work directory to engine
	pyOSModule.attr("chdir")(engine_cwd.c_str());
	if (pyModule.ptr()) {
		is_initialized = true;
	}
}
void CTPython::py_call(string py_func) {
	if (!is_initialized) return;
	// change current work directory for python
	pyOSModule.attr("chdir")(python_cwd.c_str());
	try {
		if (py::hasattr(pyModule, py_func.c_str()))
		{
			pyReturn = pyModule.attr(py_func.c_str()).call();
		}
	}
	catch (const py::error_already_set &e) {
		MESSAGE("CTPython::py_call(): Error '%s' when calling '%s'!\n", e.what(), py_func.c_str());
		is_initialized = false;
	}

	// restore current work directory to engine
	pyOSModule.attr("chdir")(engine_cwd.c_str());
}
void CTPython::py_call(string py_func, void* srcImg,void* dstImg, int srcbitdepth, int dstbitdepth, int height, int width,int nComp)
{
	if (!is_initialized) return;

	// change current work directory for python
	pyOSModule.attr("chdir")(python_cwd.c_str());

	try {
		if (py::hasattr(pyModule, py_func.c_str()))
		{
			py::tuple py_paras(1);
			py_paras[0] = Image2PyObject(srcImg,srcbitdepth,height,width,nComp);
			pyReturn = pyModule.attr(py_func.c_str()).call(py_paras);
			PyObject2Image(pyReturn, dstImg, height*width*nComp, dstbitdepth);
		}
	}
	catch (const py::error_already_set &e) {
		MESSAGE("CTPython::py_call(): Error '%s' when calling '%s'!\n", e.what(), py_func.c_str());
		is_initialized = false;
	}

	// restore current work directory to engine
	pyOSModule.attr("chdir")(engine_cwd.c_str());
}