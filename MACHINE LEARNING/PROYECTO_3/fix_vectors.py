import pickle
import numpy as np

fl = "train_10_fixed"

# Función para cargar un archivo .pkl


def cargar_pkl(file_path):
    with open(file_path, 'rb') as f:
        data = pickle.load(f)
    return data


# open the pickle file and load the features
with open('video_features/vector_features/' + fl + '_features.pkl', 'rb') as f:
    results = pickle.load(f)

print(results[0].keys())

shapes_features = set()
count = 0

for i in range(0, len(results)):
    if (results[i]["feature_vector"] is not None):
        shapes_features.add(results[i]["feature_vector"].shape)

        if (results[i]["feature_vector"].shape == (0,)):
            print(results[i]["file_path"], results[i]["feature_vector"])
            count += 1

print(count)
print(shapes_features)


fl = "train_10"

# Cargar los datos
file_path = 'video_features/vector_features/' + fl + '_features.pkl'
data = cargar_pkl(file_path)

# Preprocesar los datos para manejar None y dimensiones 0
for video in data:
    feature_vector = video['feature_vector']
    if feature_vector is None or len(feature_vector) == 0:

        video['feature_vector'] = np.zeros((1, 1024))
    elif len(feature_vector) > 1:

        video['feature_vector'] = np.mean(
            feature_vector, axis=0).reshape(1, -1)


for video in data:
    assert video['feature_vector'].shape == (1, 1024)


def procesar_videos(data):
    for video in data:
        feature_vector = video['feature_vector']
        if len(feature_vector.shape) > 1 and feature_vector.shape[0] > 1:
            # Si hay múltiples vectores, calculamos la media
            video['feature_vector'] = np.mean(feature_vector, axis=0)
        else:
            # Si hay un solo vector, lo usamos tal cual
            video['feature_vector'] = feature_vector.reshape(-1)
    return data


data = procesar_videos(data)

for video in data:
    assert video['feature_vector'].shape == (1024,)

with open('video_features/vector_features/' + fl + '_fixed_features.pkl', 'wb') as f:
    pickle.dump(data, f)
