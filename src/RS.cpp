#include "RS.hpp"

ReservationStation::ReservationStation() {
    for (int i = 0; i < STATION_SIZE; ++i) {
        info[i].busy = false;
    }
}
bool ReservationStation::available() {
    bool res = false;
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (!info[i].busy) {
            res = true;
            break;
        }
    }
    return res;
}
void ReservationStation::insert() {
    
}