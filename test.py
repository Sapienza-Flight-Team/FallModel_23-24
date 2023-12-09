#!/usr/bin/python3

from libsft_fall_model import dropPoints, trajectories
import numpy as np
from scipy.optimize import minimize
import pandas as pd
import matplotlib.pyplot as plt
from math import atan2, degrees, radians, cos, sin

# read file with all data
file_name = "test2.csv"
empirical_data = pd.read_csv(file_name, sep='\s+')

R_E = 6378100
def micro_diff(origin, target):
    dLat = radians(target[0] - origin[0])
    dLong = radians(target[1] - origin[1])
    N = dLat * R_E
    E = dLong * R_E * cos(radians(origin[0]))
    return np.array([N,E])

def micro_sum(origin, vector):
    dLat = degrees(vector[0]/R_E)
    dLong = degrees(vector[1]/(R_E*cos(radians(origin[0]))))
    return np.array([origin[0]+dLat, origin[1]+dLong])

""" optimize CdS"""
def cost(CdS, data):
    # convert data in list
    altitude = data['H'].tolist()
    mass = data['mass'].tolist()
    target = list(zip(data['Lat_land'], data['Long_land']))
    vel = list(zip(data['Head'], data['Vel'], data['V_down']))
    wind = data['wind'].tolist()
    res = dropPoints(wind, vel, target, altitude, mass, CdS, 10, 2, 'rk4')
    # compute loss
    sum = 0
    for launch in range(len(res)):
        theoretical_launch_point = res[launch]
        empirical_launch_point = (data['Lat_drop'][launch], data['Long_drop'][launch])
        distance = micro_diff(theoretical_launch_point, empirical_launch_point)
        sum += np.linalg.norm(distance)**2
    loss = (sum/len(res))**0.5
    return loss
CdS_initial_guess = 0.85*0.4*0.4
CdS_bounds = (0, None)
result = minimize(cost, CdS_initial_guess, args=(empirical_data,), method='Nelder-Mead', bounds=[CdS_bounds])

CdS_optimized = result.x[0]
loss_optimized = result.fun

print(f"CdS ottimizzato: {CdS_optimized}")
print(f"Perdita minima: {loss_optimized}")

def get_trajectories(CdS, data):
    # convert data in list
    altitude = data['H'].tolist()
    mass = data['mass'].tolist()
    target = list(zip(data['Lat_land'], data['Long_land']))
    vel = list(zip(data['Head'], data['Vel'], data['V_down']))
    wind = data['wind'].tolist()
    return trajectories(wind, vel, target, altitude, mass, CdS, 10, 2, 'rk4')

solutions = get_trajectories(CdS_optimized, empirical_data)

# computo i vettori differenza tra punto di lancio e punto di atterraggio empirici
LatDrop = empirical_data['Lat_drop'].tolist()
LongDrop = empirical_data['Long_drop'].tolist()
dropPoints = list(zip(LatDrop, LongDrop))
LatLand = empirical_data['Lat_land'].tolist()
LongLand = empirical_data['Long_land'].tolist()
landPoints = list(zip(LatLand, LongLand))

NEDPoints = []

for i in range(len(solutions)):
    distance = micro_diff(dropPoints[i], landPoints[i])
    NEDPoints += [[distance[0],distance[1],0]]

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlim([-40, 40])
ax.set_ylim([-40, 40])
ax.set_zlim([-10, 70])

for trajectory in solutions:
    N = [coord[0] for coord in trajectory]
    E = [coord[1] for coord in trajectory]
    D = [coord[2] for coord in trajectory]

    ax.plot(N, E, D)

land_N = [coord[0] for coord in NEDPoints]
land_E = [coord[1] for coord in NEDPoints]
land_D = [coord[2] for coord in NEDPoints]

ax.scatter(land_N, land_E, land_D)

plt.show()

# calcolo l'errore relativo nel punto di landing
relative_error = []
for i in range(len(solutions)):
    theoretical_land = solutions[i][-1][:2]
    empirical_land = NEDPoints[i][:2]
    relative_error.append(np.array(list(theoretical_land))-np.array(list(empirical_land)))

error_N = [error[0] for error in relative_error]
error_E = [error[1] for error in relative_error]

fig, ax = plt.subplots()  # Crea una nuova figura e un set di assi

ax.set_xlim([-40,40])
ax.set_ylim([-40,40])
ax.axhline(0, color='black')  # Disegna l'asse y
ax.axvline(0, color='black')  # Disegna l'asse x
ax.scatter(error_N, error_E)  # Disegna i punti di errore sul grafico
ax.set_xlabel('Errore N')  # Imposta l'etichetta dell'asse x
ax.set_ylabel('Errore E')  # Imposta l'etichetta dell'asse y
ax.set_title('Errore relativo nel punto di landing')  # Imposta il titolo del grafico
plt.show()

# calcolo bias
bias = np.linalg.norm(np.cumsum(relative_error, axis=1)/len(relative_error))
print(bias)