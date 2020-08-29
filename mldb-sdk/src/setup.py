from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import os, pathlib
from os.path import isfile, join, isdir
import re
import pybind11

def find_all_cc(files = [], fdir = pathlib.Path(__file__).parent):
    for fn in os.listdir(fdir):
        abs_path = join(fdir, fn)
        if re.search(r'test[s]?$|^build$', fn, re.I):
            continue
        if isfile(abs_path) and re.search(r'\.(c|cc|cpp|cxx|hpp)$', fn, re.I):
            files.append(abs_path)
        elif isdir(abs_path):
            find_all_cc(files=files, fdir=abs_path)

def get_sources():
    files = []
    find_all_cc(files)
    print(files)
    return files

def get_libraries():
    return [
        "protobuf",
        "pthread",
        "grpc++",
        "absl_bad_optional_access",
        "absl_str_format_internal",
        "absl_time",
        "absl_time_zone",
        "absl_civil_time",
        "absl_strings",
        "absl_strings_internal",
        "absl_throw_delegate",
        "absl_int128",
        "absl_base",
        "absl_spinlock_wait",
        "absl_raw_logging_internal",
        "absl_log_severity",
        "absl_dynamic_annotations",
        "grpc",
        "address_sorting",
        "upb",
        "cares",
        "z",
        "absl_bad_optional_access",
        "absl_str_format_internal",
        "absl_time",
        "absl_time_zone",
        "absl_civil_time",
        "absl_strings",
        "absl_strings_internal",
        "absl_throw_delegate",
        "absl_int128",
        "absl_base",
        "absl_spinlock_wait",
        "absl_raw_logging_internal",
        "absl_log_severity",
        "absl_dynamic_annotations",
        "gpr",
        "absl_str_format_internal",
        "absl_time",
        "absl_time_zone",
        "absl_civil_time",
        "absl_strings",
        "absl_strings_internal",
        "absl_throw_delegate",
        "absl_int128",
        "absl_base",
        "absl_spinlock_wait",
        "absl_raw_logging_internal",
        "absl_log_severity",
        "absl_dynamic_annotations",
        "ssl",
        "crypto",
        "dl",
        "grpc++_reflection",
        "re2",
    ]

ext_modules = [
    Extension(
        "mldb",
        sources=get_sources(),
        include_dirs=[".", pybind11.get_include()],
        library_dirs=["/usr/local/lib"],
        libraries=get_libraries(),
        extra_compile_args=[
            "-pthread",
            "-Wl,--no-as-needed",
            "-Wl,--as-needed",
            "-std=c++11",
        ],
        language="c++"
    )
]

setup(
    name="mldb",
    version="1.0",
    description="Machine Learning Database SDK",
    long_description="",
    ext_modules=ext_modules,
    setup_requires=["pybind11>=2.5.0"],
    zip_safe=False,
)
