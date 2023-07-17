#pragma once


extern bool programMode;
extern bool visualizeMode;
extern bool haveSignal;


void currentMode();

void program();

void normal();

void enterProgramMode();

void exitProgramMode();

void goNextShow();

void goPrevShow();

void toggleShow();

void updateStep();

void updateActiveShows();