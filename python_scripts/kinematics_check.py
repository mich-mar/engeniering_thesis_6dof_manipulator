import numpy as np
import numpy as np
import matplotlib.pyplot as plt

def rot_z(theta: float) -> np.ndarray:
    c, s = np.cos(theta), np.sin(theta)
    return np.array([
        [ c, -s, 0, 0],
        [ s,  c, 0, 0],
        [ 0,  0, 1, 0],
        [ 0,  0, 0, 1],
    ], dtype=float)

def rot_x(alpha: float) -> np.ndarray:
    c, s = np.cos(alpha), np.sin(alpha)
    return np.array([
        [1, 0,  0, 0],
        [0, c, -s, 0],
        [0, s,  c, 0],
        [0, 0,  0, 1],
    ], dtype=float)

def trans_z(d: float) -> np.ndarray:
    return np.array([
        [1, 0, 0, 0],
        [0, 1, 0, 0],
        [0, 0, 1, d],
        [0, 0, 0, 1],
    ], dtype=float)

def trans_x(a: float) -> np.ndarray:
    return np.array([
        [1, 0, 0, a],
        [0, 1, 0, 0],
        [0, 0, 1, 0],
        [0, 0, 0, 1],
    ], dtype=float)

def dh_step(theta: float, d: float, a: float, alpha: float) -> np.ndarray:
    return rot_z(theta) @ trans_z(d) @ trans_x(a) @ rot_x(alpha)

def fk_end_effector(q0, q1, q2, q3, q4, q5, units="mm"):
    # Stałe z tabeli (mm)
    d1 = 63.0
    d2 = 72.5
    d3 = 169.4  # w tabeli jest jako TransX (a_i)
    d4 = 71.0   # w tabeli jest jako TransX (a_i)
    d5 = 40.5
    d6 = 52.5   # w tabeli jest jako TransX (a_i)

    T = np.eye(4)

    # 0 -> 1
    T = T @ dh_step(theta=q0, d=d1, a=0.0, alpha=0.0)
    # 1 -> 2
    T = T @ dh_step(theta=0.0, d=d2, a=0.0, alpha=-np.pi/2)
    # 2 -> 3
    T = T @ dh_step(theta=q1 - np.pi/2, d=0.0, a=d3, alpha=0.0)
    # 3 -> 4
    T = T @ dh_step(theta=q2, d=0.0, a=d4, alpha=0.0)
    # 4 -> 5
    T = T @ dh_step(theta=-np.pi/2, d=0.0, a=0.0, alpha=-np.pi/2)
    # 5 -> 6
    T = T @ dh_step(theta=q3, d=d5, a=0.0, alpha= np.pi/2)
    # 6 -> 7
    T = T @ dh_step(theta=q4 + np.pi/2, d=0.0, a=d6, alpha=0.0)
    # 7 -> 8
    T = T @ dh_step(theta=-np.pi/2, d=0.0, a=0.0, alpha=-np.pi/2)
    # 8 -> 9
    T = T @ dh_step(theta=q5, d=0.0, a=0.0, alpha=0.0)

    p = T[:3, 3].copy()
    return T, p

def print_matrix(mat):
    for row in mat:
        print(" ".join(f"{v:8.2f}" for v in row))

def print_vector(vec):
    print(" ".join(f"{v:8.2f}" for v in vec))

def fk_all_frames(q0, q1, q2, q3, q4, q5, degrees=True):
    q0,q1,q2,q3,q4,q5 = [to_rad(q, degrees) for q in (q0,q1,q2,q3,q4,q5)]
    d1 = 63.0
    d2 = 72.5
    d3 = 169.4
    d4 = 71.0
    d5 = 40.5
    d6 = 52.5

    steps = [
        (0.0,          d1, 0.0, 0.0),         # 0->1
        (q0,         d2, 0.0, -np.pi/2),    # 1->2
        (q1-np.pi/2,  0.0, d3, 0.0),         # 2->3
        (q2,          0.0, d4, 0.0),         # 3->4
        (-np.pi/2,    0.0, 0.0, -np.pi/2),   # 4->5
        (q3,          d5, 0.0,  np.pi/2),    # 5->6
        (q4+np.pi/2,  0.0, d6, 0.0),         # 6->7
        (-np.pi/2,    0.0, 0.0, -np.pi/2),   # 7->8
        (q5,          0.0, 0.0, 0.0),        # 8->9
    ]

    T = np.eye(4)
    Ts = [T.copy()]  # T0_0
    for (th, d, a, al) in steps:
        T = T @ dh_step(theta=th, d=d, a=a, alpha=al)
        Ts.append(T.copy())
    return Ts  # długość 10: od 0 do 9

def _axis_equal_3d(ax, X, Y, Z):
    x_min, x_max = np.min(X), np.max(X)
    y_min, y_max = np.min(Y), np.max(Y)
    z_min, z_max = np.min(Z), np.max(Z)

    cx, cy, cz = (x_min + x_max)/2, (y_min + y_max)/2, (z_min + z_max)/2
    r = max(x_max - x_min, y_max - y_min, z_max - z_min) / 2
    if r == 0:
        r = 1.0
    ax.set_xlim(cx - r, cx + r)
    ax.set_ylim(cy - r, cy + r)
    ax.set_zlim(cz - r, cz + r)

def plot_manipulator(q0, q1, q2, q3, q4, q5, frame_scale=25.0):
    Ts = fk_all_frames(q0, q1, q2, q3, q4, q5)
    pts = np.array([T[:3, 3] for T in Ts])  # (10,3)

    fig = plt.figure()
    ax = fig.add_subplot(111, projection="3d")

    # --- CZŁONY MANIPULATORA (czarne) ---
    ax.plot(
        pts[:, 0], pts[:, 1], pts[:, 2],
        color="k", marker="o", linewidth=2, label="Człony manipulatora"
    )

    # --- OSIE WSZYSTKICH UKŁADÓW 0..9 ---
    for T in Ts:
        p = T[:3, 3]
        R = T[:3, :3]

        ex = R[:, 0]  # oś X
        ez = R[:, 2]  # oś Z

        # oś X – niebieska
        ax.plot(
            [p[0], p[0] + frame_scale * ex[0]],
            [p[1], p[1] + frame_scale * ex[1]],
            [p[2], p[2] + frame_scale * ex[2]],
            color="b", linewidth=2
        )

        # oś Z – czerwona
        ax.plot(
            [p[0], p[0] + frame_scale * ez[0]],
            [p[1], p[1] + frame_scale * ez[1]],
            [p[2], p[2] + frame_scale * ez[2]],
            color="r", linewidth=2
        )

    # --- LEGENDA (ręcznie, żeby się nie powielała) ---
    from matplotlib.lines import Line2D
    legend_elements = [
        Line2D([0], [0], color='k', lw=2, marker='o', label='Człony manipulatora'),
        Line2D([0], [0], color='b', lw=2, label='Oś X układu'),
        Line2D([0], [0], color='r', lw=2, label='Oś Z układu'),
    ]
    ax.legend(handles=legend_elements, loc="upper right")

    ax.set_xlabel("X [mm]")
    ax.set_ylabel("Y [mm]")
    ax.set_zlabel("Z [mm]")
    ax.set_title("Manipulator – osie X (niebieskie) i Z (czerwone) dla wszystkich ramek")

    _axis_equal_3d(ax, pts[:, 0], pts[:, 1], pts[:, 2])
    plt.show()


def to_rad(x, degrees: bool):
    return np.deg2rad(x) if degrees else x

def angle_deg(u, v):
    u = u / np.linalg.norm(u)
    v = v / np.linalg.norm(v)
    return np.rad2deg(np.arccos(np.clip(np.dot(u, v), -1.0, 1.0)))

def debug_wrist_axes(q0,q1,q2,q3,q4,q5, degrees=True):
    Ts = fk_all_frames(q0,q1,q2,q3,q4,q5, degrees=degrees)

    z5 = Ts[5][:3,2]
    z6 = Ts[6][:3,2]
    z8 = Ts[8][:3,2]

    print(f"Z5 = [{z5[0]: .3f}, {z5[1]: .3f}, {z5[2]: .3f}]")
    print(f"Z6 = [{z6[0]: .3f}, {z6[1]: .3f}, {z6[2]: .3f}]")
    print(f"Z8 = [{z8[0]: .3f}, {z8[1]: .3f}, {z8[2]: .3f}]")

    print(f"angle(Z5,Z6) = {angle_deg(z5,z6):.2f} deg")
    print(f"angle(Z6,Z8) = {angle_deg(z6,z8):.2f} deg")
    print(f"angle(Z5,Z8) = {angle_deg(z5,z8):.2f} deg")



if __name__ == "__main__":
    # przykład (radiany)
    plot_manipulator(
        q0=30, q1=30, q2=0,
        q3=0, q4=0, q5=0,
        frame_scale=25.0
    )


