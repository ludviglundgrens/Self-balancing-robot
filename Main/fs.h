class SP {       
  public:             
    int new_time = 0;
    int last_time = 0;
    int elapsed_time = 0;
    int last_val = 0;

    int x0 [4];
    int xp [4];
    int yh;
    int u;

    int xh[4];
    int xph[4];
    int dt = 0;
    int y0 = 0;

    int A[4][4];
    int B[4];
    int C[4];
    int D[4];
    int L[4];
    int K[4];


    SP(int *a, int *b, int *c, int *d, int *l, int *k, int last_time) {
        int A = *a;
        int B = *b;
        int C = *c;
        int D = *d;
        int L = *l;
        int K = *k;
        //last_time = millis();
        last_time = 1;
    }

    double update(float y, float u) {
        //new_time = millis();                
        new_time = 1;
        dt = new_time - last_time;        

        // xp  = A*x  + Bu
        // xph = A*xh + Bu + L(y - yh)
        // yh  = C*xh
        // u   = -K*xh
        xph[0] = A[0][0]*xh[0] + A[0][1]*xh[1] + A[0][2]*xh[2] + A[0][3]*xh[3] + B[0]*u;
        xph[1] = A[1][0]*xh[0] + A[1][1]*xh[1] + A[1][2]*xh[2] + A[1][3]*xh[3] + B[1]*u;
        xph[2] = A[2][0]*xh[0] + A[2][1]*xh[1] + A[2][2]*xh[2] + A[2][3]*xh[3] + B[2]*u;
        xph[3] = A[3][0]*xh[0] + A[3][1]*xh[1] + A[3][2]*xh[2] + A[3][3]*xh[3] + B[3]*u;

        // Estimate xh linearly
        //xh = calx_xh(xph, dt);
        xh[0] = xh[0] + xph[0]*dt;
        xh[1] = xh[1] + xph[1]*dt;
        xh[2] = xh[2] + xph[2]*dt;
        xh[3] = xh[3] + xph[3]*dt;

        yh = (C[0]*xh[0] + C[1]*xh[1] + C[2]*xh[2] + C[3]*xh[3]);
        u = (K[0]*xh[0] + K[1]*xh[1] + K[2]*xh[2] + K[3]*xh[3]);                           
        return u;                 
    }

    // int calc_xp (int A[4][4], int x[4], int B[4], int u){
    //     xp = {A[0][0]*x[0] + A[0][1]*x[1] + A[0][2]*x[2] + A[0][3]*x[3] + B[0]*u,
    //         A[1][0]*x[0] + A[1][1]*x[1] + A[1][2]*x[2] + A[1][3]*x[3] + B[1]*u,
    //         A[2][0]*x[0] + A[2][1]*x[1] + A[2][2]*x[2] + A[2][3]*x[3] + B[2]*u,
    //         A[3][0]*x[0] + A[3][1]*x[1] + A[3][2]*x[2] + A[3][3]*x[3] + B[3]*u
    //     };
    //     return xp;
    // }
    // int calc_xph(int A[4][4], int xh[4], int B[4], int u, int L[4]){
    //     xph = calc_xp(A, xh, B, u);
    //     xph[0] = xph[0] + L[0];
    //     xph[1] = xph[1] + L[1];
    //     xph[2] = xph[2] + L[2];
    //     xph[3] = xph[3] + L[3];
    // // } 
    // int calc_xh(int xh[4], int xp[4], int dt){
    //     xh[0] = xh[0] + xph[0]*dt;
    //     xh[1] = xh[1] + xph[1]*dt;
    //     xh[2] = xh[2] + xph[2]*dt;
    //     xh[3] = xh[3] + xph[3]*dt;
    // }

};
