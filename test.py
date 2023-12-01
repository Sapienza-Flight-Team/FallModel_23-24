#!/usr/bin/python3

import libsft_fall_model as ad
import pandas as pd
"""
# read file with all data
file_name = "test2.csv"
empirical_data = pd.read_csv(file_name, sep='\s+')

# algorithm
def compute_loss(CdS, data):
    altitude = empirical_data['H'].tolist()
    mass = empirical_data['mass'].tolist()
    target = list(zip(empirical_data['Lat_land'], empirical_data['Long_land']))
    vel = list(zip(empirical_data['Head'], empirical_data['Vel'], empirical_data['V_down']))
    wind = empirical_data['wind'].tolist()
    res = ad.run(wind, vel, target, altitude, mass, CdS, 10, 2, 'rk4')
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
"""
wind_str = "18003KT"            # il vento si descrive dicendo da dove viene e si indica con una stringa "{gradi}{nodi}KT" (es. "24010KT": il vento viene da 240° a velocità 10 nodi)
vel_list = (2.0, 22.0, -2)      # velocità vettoriale descritta come direzione (in radianti N/NE) e velocità in m/s
pos = (41.8317465, 12.3334762)  # posizione del target espressa come latitudine e longitudine
h = 33.9                        # quota di lancio in metri
m = 0.560                       # massa in chilogrammi del paracadute+payload
Cd = 1.5                        # cd del paracadute
S = 0.28                        # superficie del paracadute in m2
time = 10                       # il tempo in secondi
step = 2                        # lo step in millisecondi
integrator = "rk4"              # integrator (es. "rk4", "rk45", "euler")

res = ad.run([wind_str], [vel_list], [pos], [h], [m], Cd*S, time, step, integrator)

print(res)