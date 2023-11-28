/**
 * @file game_business.h
 */

#ifndef GAME_BUSINESS_H
#define GAME_BUSINESS_H

#include <stdint.h>

struct point
{
   uint8_t x;
   uint8_t y;
};

void game_process();
struct point game_get_point();

#endif // GAME_BUSINESS_H