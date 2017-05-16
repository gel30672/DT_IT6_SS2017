//
// Created by pfm on 4/20/17.
//

#ifndef LOCSRV_LOCDET_H
#define LOCSRV_LOCDET_H
#define quad(x) ((x)*(x))

struct Position{
    short x;
    short y;
};

class LocDet {
public:
    LocDet();
    ~LocDet();
    int get_position(Position *pos);
    void collectTestData();

private:
    Position Anchor_B;
    Position Anchor_C;
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