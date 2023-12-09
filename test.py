#!/usr/bin/python3

from libsft_fall_model import dropPoints, trajectories
import numpy as np
from scipy.optimize import minimize
import pandas as pd
from geopy.distance import geodesic

# read file with all data
file_name = "test2.csv"
empirical_data = pd.read_csv(file_name, sep='\s+')

"""
# algorithm
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
        distance = geodesic(theoretical_launch_point, empirical_launch_point).meters
        sum += distance**2
    loss = (sum/len(res))**0.5
    return loss

CdS_initial_guess = 0.85*0.4*0.4
CdS_bounds = (0.001, None)
result = minimize(cost, CdS_initial_guess, args=(empirical_data,), method='Nelder-Mead', bounds=[CdS_bounds])

CdS_optimized = result.x[0]
loss_optimized = result.fun

print(f"CdS ottimizzato: {CdS_optimized}")
print(f"Perdita minima: {loss_optimized}")
"""