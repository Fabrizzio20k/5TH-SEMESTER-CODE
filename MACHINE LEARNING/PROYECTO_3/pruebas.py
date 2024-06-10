import pickle as pkl
import numpy as np

path = "video_features/vector_features"

fl = "train_10"

with open(f"{path}/{fl}_features.pkl", "rb") as f:
    results = pkl.load(f)

print(len(results))

# counter_vector_with_zeros = 0
# for i in range(len(results)):
#     feature_vector = results[i]["feature_vector"]
#     if np.all(feature_vector == 0):  # Verificar si todos los elementos del vector son ceros
#         counter_vector_with_zeros += 1
#         print(results[i]["file_path"], results[i]["label"], feature_vector)

# print(
#     f"Number of vectors that only contain zeros: {counter_vector_with_zeros}")
