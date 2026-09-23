import os
import numpy as np
import matplotlib.pyplot as plt

folder = "test_results"

labels = [10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 4, 6, 8]

all_results = []
performance_scores = []

for i in range(10):
    filepath = os.path.join(folder, f"test{i}.txt")

    # load all numbers directly
    values = np.loadtxt(filepath)

    if len(values) != 16:
        raise ValueError(f"{filepath} should contain 16 numbers")

    timings = values[:15]
    perf_score = values[15]

    all_results.append(timings)
    performance_scores.append(perf_score)

data = np.array(all_results)

# --- SORT by board size ---
sorted_idx = np.argsort(labels)
sorted_labels = np.array(labels)[sorted_idx]
sorted_data = data[:, sorted_idx]

# --- STATS ---
mean_values = np.mean(sorted_data, axis=0)
std_values = np.std(sorted_data, axis=0)

# --- LINEAR REGRESSION ---
coeffs = np.polyfit(sorted_labels, mean_values, 1)
trend_line = np.poly1d(coeffs)

# --- PLOT 1: timings ---
plt.figure()

# data with error bars
plt.errorbar(
    sorted_labels,
    mean_values,
    yerr=std_values,
    marker='o',
    capsize=5,
    label="Mean time ± std dev"
)

# regression line
plt.plot(
    sorted_labels,
    trend_line(sorted_labels),
    linestyle='--',
    label=f"Linear trend (slope={coeffs[0]:.2f})"
)

plt.xlabel("Board Size (N)")
plt.ylabel("Time (μs)")
plt.title("N-Queens Performance Trend")
plt.legend()
plt.grid()


# --- PLOT 2: normalized performance ---
plt.figure()

performance_scores = np.array(performance_scores)

# normalize to [0, 1]
normalized_perf = (performance_scores - performance_scores.min()) / (
    performance_scores.max() - performance_scores.min()
)

plt.plot(
    range(len(normalized_perf)),
    normalized_perf,
    marker='o',
    label="Normalized performance score"
)

plt.xlabel("Test Run")
plt.ylabel("Normalized Score")
plt.title("Performance Score Trend Across Runs")
plt.legend()
plt.grid()

plt.show()
