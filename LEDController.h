#pragma once

/*
 * Features can be adjusted or enabled in this file
 */

/* This is double the actual altitude limit.
   The wings/fuse fill up to half this value, then "overflow" a red/orange color. */
#define MAX_ALTIMETER 800

/* Uncomment to override RC signal and force a specific show number for testing */
// #define TESTMODE 1

/* Uncomment to override brightness for testing */
// #define TMP_BRIGHTNESS 40

/* Uncomment to include visualizer mode (WIP) */
// #define VISUALIZER