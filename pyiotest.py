import numpy as np
def py_init(paras = None):
    # print('py_init(): called')
    return 1
def py_update(paras = None):
    new_image = paras[0]
    blank_image = new_image*(0.8,0.2,0.2,1.0)
    # print('py_update(): called')
    return blank_image
def py_shutdown(paras = None):
    # print('py_shutdown(): called')
    return 1