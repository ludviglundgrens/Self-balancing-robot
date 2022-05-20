class PID {       
  public:             
    float Kp;        
    float Ki;
    float Kd;
    
    int reference = 0;
    float curr_error;
    float last_error;
    float sum_error = 0;
    float diff_error = 0;
    float diff_error_last = 0;
    float diff_error_last_last = 0;
    
    int last_time;
    int new_time;
    int elapsed_time;
    
    PID(float kp, float ki, float kd) {
        Kp = kp;
        Ki = ki;
        Kd = kd;
        last_time = micros();
    }
        
    double update(float newval) {
        new_time = micros();                
        elapsed_time = new_time - last_time;        
        
        curr_error = reference - newval;         
        
        sum_error += curr_error * elapsed_time;                
        sum_error = constrain(sum_error, -200, 200);  
        diff_error = (curr_error - last_error)/elapsed_time;  
        
        float out = Kp*curr_error + Ki*sum_error/(1000000) + Kd*(1000000)*(diff_error + diff_error_last)/2;                                          
        
        last_error = curr_error; 
                   
        diff_error_last = diff_error;       
        last_time = new_time;                        

        return out;                 
    }
};
