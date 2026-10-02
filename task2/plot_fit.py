import pandas as pd
import matplotlib.pyplot as plt


data = pd.read_csv(
    "result/task2/fit.csv"
)


plt.figure(figsize=(8,5))

plt.plot(
    data["time"],
    data["real_angle"],
    label="Measured angle"
)

plt.plot(
    data["time"],
    data["fit_angle"],
    label="Linear fit"
)


plt.xlabel("Time (s)")
plt.ylabel("Angle (rad)")

plt.title(
    "Rotation Angle Fitting"
)

plt.legend()

plt.grid()

plt.savefig(
    "result/task2/fit_curve.png",
    dpi=300
)


plt.figure(figsize=(8,4))

plt.plot(
    data["time"],
    data["error"]
)


plt.xlabel("Time (s)")
plt.ylabel("Error (rad)")

plt.title(
    "Fitting Error"
)

plt.grid()

plt.savefig(
    "result/task2/error_curve.png",
    dpi=300
)