from ctypes import CDLL
import importlib.resources


def load_hpipm():
    lib_name = "libhpipm.so"
    if not importlib.resources.is_resource("hpipm_python", lib_name):
        return CDLL(lib_name)
    with importlib.resources.path("hpipm_python", lib_name) as p:
        return CDLL(p)
