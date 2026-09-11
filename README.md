# mountain_pathfinding_algorithm
College project for C programming module
This repo includes my solution file to the project and 2 other files written by Dr. Victor Cionca used for generating the landscape, providing debugging utilities and evaluating the solution.

“The landscape is implemented as a 2D matrix on top of which a mountain of a given height and with some irregularities (plateaus) has been placed. The mountain has a single peak. Each element of the 2D matrix contains a value that represents the altitude (height) of that cell, with the mountain peak representing the highest point in the lanscape, or the maximum matrix value. You will not have access to the 2D matrix, or the matrix’s dimensions (height and width). The only thing you can do is make calls to the function
void generate_view(float view[VIEW_SIZE][VIEW_SIZE], int center_y, int center_x)
that will populate a 2D view array from the landscape matrix centered on the center_y and center_x coordinates (resp row and column). Every time you call this function the program will increment your call count with the goal being to implement a search solution that makes the least calls to generate_view. When you believe that you have found the peak at a position x,y you must call the function int declare_peak(x,y). The function will return 1 (true) if the peak is correctly identified or 0 (false) otherwise. You can call this function any number of times, however every time it is called it will increment your call count and this will add up to your total” - excerpt from assignment specification

- Algorithm at its base runs in loop of: view, analyze, update, check for peak?, move
- direction decided by splitting view into quadrants and calculating average of each to find the corner with the highest rise to move in
- effective plateau detection implemented
- many boundary cases implemented after debugging
- large jumps by multiples increasing and decreasing based on highest altitude in view, sub-view jumps near peak
This project helped me consider problems algorithmically and gave me valuable debugging experience while trying to optimize my solution. After submitting a solution I was relatively happy with I had placed top 5 out of the module’s submissions earning a 9.10 on the performance evaluation mode.
