import libsft_fall_model as ad

wind_str = "18003KT"            # il vento si descrive dicendo da dove viene e si indica con una stringa "{gradi}{nodi}KT" (es. "24010KT": il vento viene da 240° a velocità 10 nodi)
vel_list = [2.0, 22.0, -2]      # velocità vettoriale descritta come direzione (in radianti N/NE) e velocità in m/s
pos = [41.8317465, 12.3334762]  # posizione del target espressa come latitudine e longitudine
h = 33.9                        # quota di lancio in metri
m = 0.560                       # massa in chilogrammi del paracadute+payload
Cd = 1.5                        # cd del paracadute
S = 0.28                        # superficie del paracadute in m2
time = 10                       # il tempo in secondi
step = 2                        # lo step in millisecondi
integrator = "rk4"              # integrator (es. "rk4", "rk45", "euler")

res = ad.run([wind_str], [vel_list], [pos], [h], [m], Cd*S, time, step, integrator)

print(res)