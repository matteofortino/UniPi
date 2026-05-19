import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn.datasets import load_iris
from sklearn.metrics import silhouette_score

# Load the Iris dataset (dropping the true labels for unsupervised clustering)
iris = load_iris()
X = iris.data

inertia = []
silhouette_scores = []
k_range = range(2, 11)

for k in k_range:
    kmeans = KMeans(n_clusters=k, random_state=42, n_init=10)
    kmeans.fit(X)

    inertia.append(kmeans.inertia_)
    silhouette_scores.append(silhouette_score(X, kmeans.labels_))

# Plotting the Elbow Method
plt.figure(figsize=(12, 5))

plt.subplot(1, 2, 1)
plt.plot(range(2, 11), inertia, marker="o", color="b")
plt.title("Elbow")

# Plotting the Silhouette Method
plt.subplot(1, 2, 2)
plt.plot(k_range, silhouette_scores, marker="o", color="r")
plt.title("Silhouette ")

plt.tight_layout()
plt.show()
