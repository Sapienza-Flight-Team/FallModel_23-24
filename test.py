import libsft_fall_model as ad

vel_list = [2, 22]              # velocità vettoriale descritta come direzione (in radianti N/NE) e velocità in m/s
wind_str = "18003KT"            # il vento si descrive dicendo da dove viene e si indica con una stringa "{gradi}{nodi}KT" (es. "24010KT": il vento viene da 240° a velocità 10 nodi)
pos = [41.8317465, 12.3334762]  # posizione del target espressa come latitudine e longitudine
h = 33.9                        # quota di lancio in metri
Cd = 1.5                        # cd del paracadute
S = 0.28                        # superficie del paracadute in m2
m = 0.560                       # massa in chilogrammi del paracadute+payload
time = 10                       # il tempo in secondi
step = 2                        # lo step in millisecondi
integrator = ''                 # integrator (es. "rk4", "rk45", "euler")

ad.run(vel_list, wind_str, pos, h, Cd, S, m, time, step, integrator)