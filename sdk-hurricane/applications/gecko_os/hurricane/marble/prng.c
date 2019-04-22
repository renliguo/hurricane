/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2019, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"

#include "prng.h"

/*

This is a simple LFSR based PRNG, based on Maxim's App Note 4400.
https://www.maximintegrated.com/en/app-notes/index.mvp/id/4400

*/

#define LFSR_POLY_32 0xB4BCD35C
#define LFSR_POLY_31 0x7A5BC2E3

static uint32_t lfrs(uint32_t *value, uint32_t poly)
{
	uint32_t feedback = *value & 1;

	*value >>= 1;
	if(feedback)
	{
		*value ^= poly;
	}

	return *value;
}


uint32_t pseudo_rand(uint32_t *seed)
{
	// ensure seed is not 0, the LFSR will not work with all 0s.
	// if it is 0, assume we want to initialize with data
	// from the system's RNG
	while(*seed == 0)
	{
		*seed = rand();
	}

	uint32_t value = (lfrs(seed, LFSR_POLY_32) & 0xffff) << 16;
	value |= lfrs(seed, LFSR_POLY_32) & 0xffff;
	
	return value;
}






