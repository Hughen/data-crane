from distutils.core import setup, Extension

myclient = Extension("myclient", sources=["client.cc", "python.cc"])

setup(ext_modules=[myclient], name="myclient", description="my c++ module for python3", version="0.1")
