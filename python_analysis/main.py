from uncertainties import *
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import minimize


def fit_split_normal_linear(x, y, yerr_minus, yerr_plus, x_for_line=None):
    x = np.asarray(x)
    y = np.asarray(y)
    sm = np.asarray(yerr_minus)
    sp = np.asarray(yerr_plus)

    # 負の対数尤度
    def nll(theta):
        a, b = theta
        r = y - (a * x + b)
        sigma = np.where(r >= 0.0, sp, sm)
        return 0.5 * np.sum((r / sigma) ** 2 + np.log(2.0 * np.pi * sigma**2))

    # 初期値: 対称化した重みでざっくり直線当てとく
    s0 = 0.5 * (sm + sp)
    w = 1.0 / s0**2
    X = np.vstack([x, np.ones_like(x)]).T
    beta0 = np.linalg.lstsq((X * np.sqrt(w)[:, None]), y * np.sqrt(w), rcond=None)[
        0
    ]  # [a0, b0]

    res = minimize(nll, x0=beta0, method="Powell")  # 微分不要で頑丈

    a_hat, b_hat = res.x

    # 近似共分散（最尤点での片側σを固定した線形化近似）
    r = y - (a_hat * x + b_hat)
    sigma = np.where(r >= 0.0, sp, sm)
    W = np.diag(1.0 / sigma**2)
    XtW = X.T @ W
    cov = np.linalg.inv(XtW @ X)
    a_se, b_se = np.sqrt(np.diag(cov))

    # 可視化用の直線
    if x_for_line is None:
        x_for_line = np.linspace(x.min(), x.max(), 200)
    y_fit = a_hat * x_for_line + b_hat

    return (a_hat, b_hat, a_se, b_se, cov, x_for_line, y_fit)


def weighted_corr(x, y, w):
    xw = np.sum(w * x) / np.sum(w)
    yw = np.sum(w * y) / np.sum(w)
    cov_xy = np.sum(w * (x - xw) * (y - yw))
    var_x = np.sum(w * (x - xw) ** 2)
    var_y = np.sum(w * (y - yw) ** 2)
    return cov_xy / np.sqrt(var_x * var_y)


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
    R_plus_err = []
    R_minus_err = []

    with open("gaussian_fitting.csv", "r", encoding="utf-8") as f:
        lines = f.readlines()[1:]  # Skip header line
        for line in lines:
            parts = line.strip().split(",")
            element = parts[0]
            A = float(parts[1])
            Z = int(parts[2])
            E_c_value = float(parts[3])
            E_c_uncertainty = float(parts[4])
            dR_plus = float(parts[6]) * 1e-15
            dR_minus = float(parts[7]) * 1e-15

            E_c = ufloat(E_c_value, E_c_uncertainty)
            R = calc_atom_radius_from_E_c(E_c, Z)

            print(
                f"Element: {element}, Atomic Number: {Z}, Coulombic Energy: {E_c} MeV, Calculated Radius: {R.n:.4e} \pm {R.s:.4e} m"
            )
            A_values.append(A)
            R_values.append(R)
            R_plus_err.append(np.sqrt(R.s**2 + dR_plus**2))
            R_minus_err.append(np.sqrt(R.s**2 + dR_minus**2))

    print(R_plus_err, print(R_minus_err))

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

    ## include other error
    plt.errorbar(
        A_sqrt,
        R_magnitudes,
        yerr=[R_minus_err, R_plus_err],
        fmt="o",
        label="Calculated Radius",
        color="black",
    )
    # add fitted line
    x = np.array(A_sqrt)
    y = np.array(R_magnitudes)
    sm = np.array(R_minus_err)
    sp = np.array(R_plus_err)
    a, b, a_se, b_se, cov, x_fit, y_fit = fit_split_normal_linear(x, y, sm, sp)
    plt.plot(x_fit, y_fit, "--", label="fitted line", color="gray")

    plt.xlabel("Cube Root of Mass Number ($A^{1/3}$)")
    plt.ylabel("Calculated Nucleus Radius (m)")
    plt.title("Atomic Nucleus Radius vs Cube Root of Mass Number")
    plt.legend()
    plt.grid()
    plt.show()
    plt.show()
