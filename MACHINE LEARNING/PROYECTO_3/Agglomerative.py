import numpy as np
import heapq


class AgglomerativeClustering:
    def __init__(self, n_clusters=2, linkage='ward'):
        self.n_clusters = n_clusters
        self.linkage = linkage
        self.labels_ = None

    def fit(self, X):
        n_samples = X.shape[0]
        distances = self._compute_distances(X)
        clusters = {i: [i] for i in range(n_samples)}
        cluster_sizes = {i: 1 for i in range(n_samples)}
        heap = self._create_heap(distances, n_samples)

        while len(clusters) > self.n_clusters:
            while True:
                min_dist, i, j = heapq.heappop(heap)
                if i in clusters and j in clusters:
                    break

            clusters = self._merge_clusters(clusters, i, j)
            distances = self._update_distances(
                distances, clusters, X, cluster_sizes, i, j)
            cluster_sizes[i] += cluster_sizes[j]
            del cluster_sizes[j]

        self.labels_ = self._get_labels(clusters, n_samples)
        return self

    def predict(self, X):
        self.fit(X)
        return self.labels_

    def _compute_distances(self, X):
        n_samples = X.shape[0]
        distances = np.full((n_samples, n_samples), np.inf)
        for i in range(n_samples):
            for j in range(i + 1, n_samples):
                distances[i, j] = np.linalg.norm(X[i] - X[j])
                distances[j, i] = distances[i, j]
        return distances

    def _create_heap(self, distances, n_samples):
        heap = []
        for i in range(n_samples):
            for j in range(i + 1, n_samples):
                heapq.heappush(heap, (distances[i, j], i, j))
        return heap

    def _merge_clusters(self, clusters, i, j):
        clusters[i].extend(clusters[j])
        del clusters[j]
        return clusters

    def _update_distances(self, distances, clusters, X, cluster_sizes, i, j):
        for k in clusters:
            if k != i and k != j:
                if self.linkage == 'single':
                    distances[min(i, k), max(i, k)] = np.min(
                        [distances[min(i, k), max(i, k)], distances[min(j, k), max(j, k)]])
                elif self.linkage == 'complete':
                    distances[min(i, k), max(i, k)] = np.max(
                        [distances[min(i, k), max(i, k)], distances[min(j, k), max(j, k)]])
                elif self.linkage == 'average':
                    d_i_k = distances[min(i, k), max(i, k)]
                    d_j_k = distances[min(j, k), max(j, k)]
                    distances[min(i, k), max(i, k)] = (
                        d_i_k * cluster_sizes[i] + d_j_k * cluster_sizes[j]) / (cluster_sizes[i] + cluster_sizes[j])
                elif self.linkage == 'ward':
                    d_i_k = distances[min(i, k), max(i, k)]
                    d_j_k = distances[min(j, k), max(j, k)]
                    size_i = cluster_sizes[i]
                    size_j = cluster_sizes[j]
                    size_k = cluster_sizes[k]
                    new_dist = np.sqrt(((size_i + size_k) * d_i_k ** 2 + (size_j + size_k) * d_j_k **
                                       2 - size_k * distances[min(i, j), max(i, j)] ** 2) / (size_i + size_j + size_k))
                    distances[min(i, k), max(i, k)] = new_dist
        distances[i, j] = np.inf
        distances[j, :] = np.inf
        distances[:, j] = np.inf
        return distances

    def _get_labels(self, clusters, n_samples):
        labels = np.empty(n_samples, dtype=int)
        for cluster_id, points in clusters.items():
            for point in points:
                labels[point] = cluster_id
        return labels
