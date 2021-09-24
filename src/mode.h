#pragma once

/**
 * @brief Runs if we're in programMode.
 * Handles buttons, cycling through shows, enabling/disabling.
 * 
 */
void program();

/**
 * @brief Runs if we're not in programMode.
 * Reads RC input and sets currentShow correctly.
 * 
 */
void normal();