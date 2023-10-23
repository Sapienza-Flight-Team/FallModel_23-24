from my_model import my_model as mm
import numpy as np
from math import exp, cos, sin, pi
import matplotlib.pyplot as plt

flyer_mass = 1.266
rho0 = 1.2250
quota = 45

""" creo il vento come una sorta di moto browniano. """
""" Tanto più il vento si allontana dal suo punto di equilibrio (velocità media) tanto più l'incremento porta verso l'equilibrio
    nel moto browniano quindi l'incremento ha pendenza:
    norm(mean_wind)*(0,0,1)*N(0,v_var2) + (-b,a,0)*N(0, h_var2) + (a,b,0)*N(1, r_var2)
    il correttore esegue una media tra la pendenza e mean_wind-current_wind"""

h_var = 3
v_var = 8
r_var = 15

a = 20  # nord wind
b = 0  # est wind
mean_wind = np.array([a,b,0])
mean_wind_vel = np.linalg.norm(mean_wind)

wind_list = [mean_wind]
T = 10
steps = 1000
for t in np.linspace(0, T, steps):
    N = np.random.randn(3)
    pendenza = np.array([0,0,mean_wind_vel])*N[0]*v_var + np.array([-b,a,0])*N[1]*h_var + mean_wind*N[2]*r_var
    current_wind = wind_list[-1]
    new_vel = current_wind + T/steps * np.mean([pendenza, mean_wind - current_wind], axis=0)
    wind_list.append(new_vel)

print(np.cov(np.array(wind_list).T))

def var_wind(t, pos): return wind_list[int(t/T*steps)]
def mid_wind(t, pos): return mean_wind

def flyer(t, state, r_vel):
    rho = rho0*exp(-10000*(state.pos[2]+quota))
    cds = 0.47*0.1257 + 1.75*0.3491
    return 0.5 * np.linalg.norm(r_vel) * cds * rho / flyer_mass

var_mod = mm(quota, rho0, flyer_mass, var_wind, flyer)
mid_mod = mm(quota, rho0, flyer_mass, mid_wind, flyer)

var_data_x = []
var_data_y = []
for theta in np.linspace(0, 2*pi, 20):
    for ray in np.linspace(0, 10, 15):
        vel = ray*np.array([cos(theta), sin(theta), 0])
        ret = var_mod.hit(vel)
        var_data_x.append(ret[0])
        var_data_y.append(ret[1])

mid_data_x = []
mid_data_y = []
for theta in np.linspace(0, 2*pi, 20):
    for ray in np.linspace(0, 10, 15):
        vel = ray*np.array([cos(theta), sin(theta), 0])
        ret = mid_mod.hit(vel)
        mid_data_x.append(ret[0])
        mid_data_y.append(ret[1])

plt.figure()
plt.scatter(var_data_x, var_data_y, c='red')
plt.scatter(mid_data_x, mid_data_y, c='green')
plt.show()