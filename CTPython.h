/*
  Author : Yang Zhang

  This plugin let you using python to process image using openfx image.
  
*/
#pragma once
#include <string>
#include <pybind11.h>
#include <embed.h>
#include <numpy.h>
//openfx
#include <ofxImageEffect.h>
#include <ofxMemory.h>
//
#include "common.h"

typedef unsigned char uchar;
namespace py = pybind11;
using namespace py::literals;
using namespace std;

/////////////////////////////////
inline string dirname(const string& str)
{
	size_t found;
	found = str.find_last_of("/\\");
	return str.substr(0, found);
}
inline string basename(const string& str)
{
	size_t found;
	found = str.find_last_of("/\\");
	str.substr(0, found);
	return str.substr(found + 1);
}
inline string replace(string str, string fromstr, string tostr)
{
	size_t index = 0;
	while (true) {
		/* Locate the substring to replace. */
		index = str.find("fromstr", index);
		if (index == std::string::npos) break;

		/* Make the replacement. */
		str.replace(index, fromstr.size(), tostr);

		/* Advance index forward so the next iteration doesn't pick it up as well. */
		index += 3;
	}
	return str;
}
inline vector<string> split(string str, string token) {
	vector<string>result;
	while (str.size()) {
		int index = str.find(token);
		if (index != string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0)result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

////////////////////////
class CTPython
{
public:
	CTPython();
	~CTPython();

	void py_import(string file_name);
	void py_call(string py_func);
	void py_call(string py_func, void* srcImg, void* dstImg, int srcbitdepth, int dstbitdepth, int height, int width, int nComp);

protected:
	py::scoped_interpreter m_guard;
	py::module pyOSModule;
	py::module pyModule;
	py::object pyReturn;

private:

	string engine_cwd = string("");
	string python_cwd = string("");
	string old_module_name = string("");
	bool is_initialized = false;

};

