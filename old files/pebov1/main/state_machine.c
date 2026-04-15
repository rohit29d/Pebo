#include "state_machine.h"

static pebo_mode_t current_mode = MODE_1;

void pebo_set_mode(pebo_mode_t mode)
{
    current_mode = mode;
}

pebo_mode_t pebo_get_mode(void)
{
    return current_mode;
}
