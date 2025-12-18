import sympy as sp
from pathlib import Path

# ============================================================
# 0) Symbole – jawnie z "_" w nazwach
# ============================================================
q_0, q_1, q_2, q_3, q_4, q_5 = sp.symbols(
    "q_0 q_1 q_2 q_3 q_4 q_5", real=True
)
d_1, d_2, d_5 = sp.symbols("d_1 d_2 d_5", real=True)
a_3, a_4, a_6 = sp.symbols("a_3 a_4 a_6", real=True)

pi = sp.pi

# ============================================================
# 1) Standard DH
# ============================================================
def dh_A(theta, d, a, alpha):
    ct = sp.cos(theta)
    st = sp.sin(theta)
    ca = sp.cos(alpha)
    sa = sp.sin(alpha)
    return sp.Matrix([
        [ct, -st*ca,  st*sa, a*ct],
        [st,  ct*ca, -ct*sa, a*st],
        [0,      sa,     ca,    d],
        [0,       0,      0,    1],
    ])

# ============================================================
# 2) Macierze A_i^{i-1}
# ============================================================
A1_0 = dh_A(theta=0,            d=d_1, a=0,   alpha=0)
A2_1 = dh_A(theta=q_0,          d=d_2, a=0,   alpha=-pi/2)
A3_2 = dh_A(theta=q_1 - pi/2,   d=0,   a=a_3, alpha=0)
A4_3 = dh_A(theta=q_2,          d=0,   a=a_4, alpha=0)
A5_4 = dh_A(theta=-pi/2,        d=0,   a=0,   alpha=-pi/2)
A6_5 = dh_A(theta=q_3,          d=d_5, a=0,   alpha=pi/2)
A7_6 = dh_A(theta=q_4 + pi/2,   d=0,   a=a_6, alpha=0)
A8_7 = dh_A(theta=-pi/2,        d=0,   a=0,   alpha=-pi/2)
A9_8 = dh_A(theta=q_5,          d=0,   a=0,   alpha=0)

mats = [A1_0, A2_1, A3_2, A4_3, A5_4, A6_5, A7_6, A8_7, A9_8]

# ============================================================
# 3) Mnożenie do T_0^9 (bez simplify)
# ============================================================
T = sp.eye(4)
for A in mats:
    T = sp.expand(T * A)

R = T[:3, :3]
p = T[:3, 3]

# ============================================================
# 4) Wypisywanie
# ============================================================
names_R = [
    ("R_xx", R[0, 0]), ("R_xy", R[0, 1]), ("R_xz", R[0, 2]),
    ("R_yx", R[1, 0]), ("R_yy", R[1, 1]), ("R_yz", R[1, 2]),
    ("R_zx", R[2, 0]), ("R_zy", R[2, 1]), ("R_zz", R[2, 2]),
]
names_T = [("T_x", p[0]), ("T_y", p[1]), ("T_z", p[2])]

def print_assignments():
    print("=== ROTATION (R) ===")
    for name, expr in names_R:
        print(f"{name} = {sp.sstr(expr)}")
    print("\n=== TRANSLATION (T) ===")
    for name, expr in names_T:
        print(f"{name} = {sp.sstr(expr)}")

if __name__ == "__main__":
    print_assignments()
