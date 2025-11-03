from uncertainties import *
import numpy as np
import matplotlib.pyplot as plt


def calc_atom_radius_from_E_c(E_c, Z):
    """,
    Calculate the atomic radius from the coulombic energy.

    Parameters
    ----------
    E_c : ufloat
        Coulombic energy in MeV.
    Z : int
        Atomic number.

    Returns
    -------
    R : ufloat
        Calculated atomic radius in meters.
    """
    e = 1.602176634e-19  # Elementary charge in Coulombs
    epsilon_0 = 8.854187817e-12  # Vacuum permittivity in F/m
    Mev2J = 1.60218e-13  # Conversion factor from MeV to Joules

    E_c = E_c * Mev2J  # Convert E_c to Joules

    R = (3 * e**2 * (2 * Z + 1)) / (20 * np.pi * epsilon_0 * E_c)

    return R


if __name__ == "__main__":

    A_values = []
    R_values = []

    with open("gaussian_fitting.csv", "r", encoding="utf-8") as f:
        lines = f.readlines()[1:]  # Skip header line
        for line in lines:
            parts = line.strip().split(",")
            element = parts[0]
            A = float(parts[1])
            Z = int(parts[2])
            E_c_value = float(parts[3])
            E_c_uncertainty = float(parts[4])

            E_c = ufloat(E_c_value, E_c_uncertainty)
            R = calc_atom_radius_from_E_c(E_c, Z)

            print(
                f"Element: {element}, Atomic Number: {Z}, Coulombic Energy: {E_c} MeV, Calculated Radius: {R.n:.4e} \pm {R.s:.4e} m"
            )
            A_values.append(A)
            R_values.append(R)

    # plot \sqrt{3}{A} vs Radius with error bars
    A_sqrt = [np.cbrt(A) for A in A_values]
    R_magnitudes = [R.n for R in R_values]
    R_std_devs = [R.s for R in R_values]
    plt.errorbar(
        A_sqrt,
        R_magnitudes,
        yerr=R_std_devs,
        fmt="o",
        label="Calculated Radius",
        color="black",
    )
    # add fitted line
    coeffs = np.polyfit(A_sqrt, R_magnitudes, 1, w=1 / np.array(R_std_devs))
    fit_line = np.poly1d(coeffs)
    x_fit = np.linspace(min(A_sqrt), max(A_sqrt), 100)
    y_fit = fit_line(x_fit)
    rvalue = np.corrcoef(A_sqrt, R_magnitudes)[0, 1]
    plt.plot(
        x_fit,
        y_fit,
        "r--",
        label=f"fitted line (r={rvalue:.2f})",
        color="gray",
    )

    plt.xlabel("Cube Root of Mass Number ($A^{1/3}$)")
    plt.ylabel("Calculated Nucleus Radius (m)")
    plt.title("Atomic Nucleus Radius vs Cube Root of Mass Number")
    plt.legend()
    plt.grid()
    plt.show()
    plt.savefig("nucleus_radius_vs_A_cbrt.png", dpi=300)
