"# OfxPyIOPlugin"

How to install

1:create an folder struct like this
  openfx
  -----include  (put openfx 1.4 headers here)
  -----plugins
  ------------PythonIO  (put this reposit here)
  -----pybind11
  -----Support  (put openfx support library here)
  
 2:open sln with vs2017 and compile
 3:add a environment variable OFX_PLUGIN_PATH and point to YOURPATH\PythonIO\x64\YOURPROFILE
 4:start davinci resolve and you should able to see and use the default python file.
 have fun.
