# -*- coding: utf-8 -*-

import numpy as np
import fall_model as fm
import simulation as sim

""" parametri del modello """
class my_model:
    def __init__(self, quota, rho, flyer_mass, wind, flyer):
        self.quota = quota
        self.rho = rho
        self.fly_mass = flyer_mass
        self.wind = wind
        self.flyer = flyer


    def hit(self, vel):
        init_state = fm.state(np.array([0, 0, -self.quota], dtype=float),
                            np.array(vel, dtype=float))
        mod = fm.model(self.wind, self.flyer)
        my_sim = sim.simulation(mod)
        ret = my_sim.run(init_state, [0, 10], 2000)

        for i in range(len(ret)):
            if ret[i].pos[2] > 0:
                break

        pos_x = (ret[i].pos[0]*ret[i].pos[2] + ret[i+1].pos[0]*ret[i+1].pos[2])/(ret[i].pos[2] + ret[i+1].pos[2])
        pos_y = (ret[i].pos[1]*ret[i].pos[2] + ret[i+1].pos[1]*ret[i+1].pos[2])/(ret[i].pos[2] + ret[i+1].pos[2])
        impact_vel = np.linalg.norm(ret[i].vel)
        return [pos_x, pos_y, impact_vel]