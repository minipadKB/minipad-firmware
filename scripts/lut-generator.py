import numpy as np
import os
import math as m


def generate_lut():
    range = 400  # Range for output release to press
    x = np.arange(0, 0.004, 0.0000001)
    y = np.zeros(4096)
    press = lookup(x[0])
    y[:press] = range
    for i in x:
        a = lookup(i)
        if y[a] != 0:
            continue
        else:
            y[a] = round((1 - i / x[len(x) - 1]) * range)
    y = range - y
    print(os.getcwd())
    save_array_to_file(y, "lut.txt")


def lookup(x):
    return m.floor(adc(gauss_to_volts(magnet_distance_to_gauss(x))))


def magnet_distance_to_gauss(x):
    # Wooting Lekker magnet
    b = 1280  # Residual induction (mT)
    l = 0.002  # Length of Magnet (m)
    r = 0.002  # Radius of Magnet (m)
    offset_dist = 0.00049
    x += offset_dist
    g = (((l + x) / (((r ** 2) + ((l + x) ** 2)) ** (1 / 2))) - (x / (((r ** 2) + (x ** 2)) ** (1 / 2)))) * -b / 2
    return g


def gauss_to_volts(g):
    k = 3.3 / 5
    return (2.5 + ((4 - 2.5) / 460 * g)) * k  # SLSS49E datasheet


def adc(i):
    v_ref = 3.3  # Reference voltage (V)
    res = 12  # Resolution (bits)
    return (i / v_ref) * (2 ** res)


def save_array_to_file(array, filename):
    with open(filename, 'w') as file:
        for i, element in enumerate(array):
            file.write(str(element))
            if (i + 1) % 8 != 0:
                file.write(', ')
            else:
                file.write(',\n')


generate_lut()
