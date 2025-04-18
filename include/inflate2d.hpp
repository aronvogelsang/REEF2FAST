#pragma once

#include "structs.hpp"

/**
 * Inflates a 2D wavefield into a 3D wavefield by duplicating it along the y-axis.
 *
 * The duplicated slices are centered between -Y_total/2 and Y_total/2 and spaced equally.
 * This produces NY-1 slices in total, as expected by OpenFAST's SeaState format.
 *
 * @param wf       The 2D wavefield to be expanded in-place
 * @param Y_total  Full span in y-direction
 * @param NY       Total number of y-grid points
 */
void inflate_wavefield_y(Wavefield& wf, double Y_total, int NY);