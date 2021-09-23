#pragma once

// Function: program
// -------------------
//   runs if we're in programMode
//   handles buttons, cycling through shows, enabling/disabling
void program();

// Function: normal
// -------------------
//   runs if we're not in programMode
//   reads rc input and sets currentShow correctly
void normal();