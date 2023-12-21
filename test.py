import AirDrop as ad

    """
    
    int cxx_trajectories(int size, double* wind, double* vel, double* target, double* h,
            double* m, double CdS, double time, double step,
            const char* integrator, double** result, int* lengths) {
    std::string integrator_i = integrator;
    Simulation s(step, time, integrator_i);

    for (int i = 0; i < size; i++) {
        double wind_head = wind[2 * i + 0];
        double wind_speed = wind[2 * i + 1];

        double vel_head = vel[3 * i + 0];
        double vel_speed = vel[3 * i + 1];
        double vel_down = vel[3 * i + 2];

        double target_lat = target[2 * i + 0];
        double target_lon = target[2 * i + 1];

        double h_i = h[i];

        double mass_i = m[i];

        GPS gps_target = {target_lat, target_lon};

        // Wind law
        std::function<VReal3(const State<3>&, const VReal<3>&, double)>
            wind_law = [wind_head, wind_speed](const State<3>& state,
                                               const VReal<3>& pos, double t) {
                return VReal<3>{wind_speed * cos(wind_head),
                                wind_speed * sin(wind_head), 0};
            };
        // CdS
        std::function<double(const State<3>&, double)> cds_payload =
            [CdS](const State<3>& s, double t) {
                if (t <= 1) {
                    return CdS * (1- cos(3 * t));
                } else {
                    return CdS;
                }
            };

        // Create the paychute and wind
        PayChute<3> pc(cds_payload, mass_i);
        Wind<3> Vw(wind_law);

        // Create the ballistic model
        BallisticModel bm(pc, Vw);
        // Create simulation object

        State<3> S0 = {0,
                       0,
                       -h_i,
                       vel_speed * cos(vel_head),
                       vel_speed * sin(vel_head),
                       vel_down};

        // Run the simulation
        Results<3> res = s.run(bm, S0);

        // Alloc memory
        lengths[i] = res[0].size();
        result[i] = (double*)malloc(lengths[i] * 7 * sizeof(double));
        if (!result[i]) return 1;

        for (int k = 0; k < lengths[i]; ++k) {
            State<3> S = res[0][k];
            result[i][7 * k + 0] = S.X()[0];
            result[i][7 * k + 1] = S.X()[1];
            result[i][7 * k + 2] = S.X()[2];
            result[i][7 * k + 3] = S.X_dot()[0];
            result[i][7 * k + 4] = S.X_dot()[1];
            result[i][7 * k + 5] = S.X_dot()[2];
            result[i][7 * k + 6] = S.get_t();
        }
    }

    return 0;
}

    """



sim = ad.Simulation()

risultato = sim.run(modello, stato_iniziale)