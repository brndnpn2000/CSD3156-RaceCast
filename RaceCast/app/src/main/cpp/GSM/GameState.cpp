#include "GameState.h"

void convertSecondsToMinutes(const int &in, int &min, int &seconds)
{
    min = in / 60;
    seconds = in % 60;
}
