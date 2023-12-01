#!/usr/bin/python3

import libsft_fall_model as ad
import pandas as pd
import geopy.distance as gp_dist

# read file with all data
file_name = "test2.csv"
empirical_data = pd.read_csv(file_name, sep='\s+')

# algorithm
def compute_loss(CdS, data):
    # convert data in list
    altitude = empirical_data['H'].tolist()
    mass = empirical_data['mass'].tolist()
    target = list(zip(empirical_data['Lat_land'], empirical_data['Long_land']))
    vel = list(zip(empirical_data['Head'], empirical_data['Vel'], empirical_data['V_down']))
    wind = empirical_data['wind'].tolist()
    res = ad.run(wind, vel, target, altitude, mass, CdS, 10, 2, 'rk4')
    # compute loss
    sum = 0
    for launch in range(len(res)):
        theoretical_launch_point = res[launch]
        empirical_launch_point = (empirical_data['Lat_drop'][launch], empirical_data['Long_drop'][launch])
        distance = gp_dist.geodesic(theoretical_launch_point, empirical_launch_point).meters
        sum += distance**2
    loss = (sum/len(res))**0.5
    return loss

initial_CdS = 2
initial_step = 1
directions = [1, -1]
final_step = 0.0001

CdS = initial_CdS
step = initial_step
curr_loss = compute_loss(CdS, empirical_data)
while step > final_step:
    loss = []
    for i in range(len(directions)):
        loss.append(compute_loss(CdS+directions[i]*step, empirical_data))
    best_loss_index = 0
    for i in range(1, len(loss)):
        if loss[i] < loss[best_loss_index]:
            best_loss_index = i
    if loss[best_loss_index] < curr_loss:
        CdS = CdS+directions[best_loss_index]*step
    else:
        step = step/2

print("CdS trovato: ")
print(CdS)
print("Errore quadratico medio: ")
print(curr_loss)