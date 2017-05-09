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
    position Anchor_B;
    position Anchor_C;
    int Dist_A;
    int Dist_B;
    int Dist_C;
    double Pos_x;
    double Pos_y;

    int execute();
    void executewithVector();
    void calc_xpos();
    void calc_ypos(int x);


};


#endif //LOCSRV_LOCDET_H
