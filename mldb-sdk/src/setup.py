from setuptools import find_packages, setup, Extension
from setuptools.command.build_ext import build_ext
import os, pathlib, sys
from os.path import isfile, join, isdir
import re
import pybind11

def find_all_cc(files = [], fdir = pathlib.Path(__file__).parent):
    fdir = str(fdir)
    for fn in os.listdir(fdir):
        abs_path = join(fdir, fn)
        if re.search(r'test[s]?$|^build$', fn, re.I):
            continue;
        if isfile(abs_path) and re.search(r'\.(c|cc|cpp|cxx|hpp)$', fn, re.I):
            files.append(abs_path)
        elif isdir(abs_path):
            find_all_cc(files=files, fdir=abs_path)

def get_sources():
    files = []
    find_all_cc(files)
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
        "re2"
    ]

def get_current_dir():
    return str(pathlib.Path(__file__).parent)

def is_anaconda():
    arr = sys.executable.split("/")
    if len(arr) < 3:
        return False
    elif arr[-3] == "conda":
        return True

def get_library_dirs():
    if is_anaconda():
        return ["/usr/local/lib", join(sys.prefix, "lib")]
    else:
        return ["/usr/local/lib"]

def find_mldb_packages():
    return find_packages(where=join(get_current_dir(), "..", "python"))

class sdk_build_ext(build_ext):
    def build_extensions(self):
        if "-Wstrict-prototypes" in self.compiler.compiler_so:
            self.compiler.compiler_so.remove("-Wstrict-prototypes")
        if is_anaconda():
            conda_lib_flags = "-L" + join(sys.prefix, "lib")
            if conda_lib_flags in self.compiler.linker_so:
                self.compiler.linker_so.remove(conda_lib_flags)
        super().build_extensions()

ext_modules = [
    Extension(
        ".mldb_sdk",
        sources=get_sources(),
        include_dirs=[
            get_current_dir(),
            pybind11.get_include(),
        ],
        library_dirs=get_library_dirs(),
        libraries=get_libraries(),
        extra_link_args=[
            "-pthread",
            "-Wl,--no-as-needed",
            "-lgrpc++_reflection",
            "-Wl,--as-needed",
            "-ldl",
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
    cmdclass={"build_ext": sdk_build_ext},
    packages=find_mldb_packages(),
    package_dir={"": join(get_current_dir(), "..", "python")},
    setup_requires=["pybind11>=2.5.0"],
    zip_safe=False,
    platforms="any",
)
