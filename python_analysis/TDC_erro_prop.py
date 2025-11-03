from uncertainties import ufloat
import numpy as np
import matplotlib.pyplot as plt


if __name__ == "__main__":
    cc = 0.299792458  # Speed of light [m/ns]
    mp = 938.272013  # Proton mass [MeV/c2]
    mn = 939.565346  # Neutron mass [MeV/c2]
    ns2ch = ufloat(22.2105, 0.01681)
    ch2ns = 1 / ns2ch
