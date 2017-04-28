//
// Created by pfm on 4/20/17.
//

#ifndef LOCSRV_LOCDET_H
#define LOCSRV_LOCDET_H



struct position{
    short x;
    short y;
};



class LocDet {
public:
    LocDet();
    ~LocDet();
    void get_position(position *pos);

private:
    int distance;
    int Dist_A;
    int Dist_B;
    int Dist_C;
    double Pos_x;
    double Pos_y;

    void calculate_angle(int Side_A, int Side_B, int Side_C, double* angle);
    void execute();
    void calc_xpos();
    void calc_ypos();


};


#endif //LOCSRV_LOCDET_H
