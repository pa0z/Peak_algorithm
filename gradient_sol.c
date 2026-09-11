#include "gradient.h"

/**
 * patryk zbos R00******
 * general thoughts and plans
 * generate_view creates a matrix of size 100-199 width and height
 * it then plots the peak of height 400 somewhere
 * then calculates the furtherst corner with some fancy maths and uses it in gradient
 * then begins plotting values in the matrix, calculates distance of current point to center
 * then gives it an altitude of peak height - (distance*gradient)
 * that should create a cone and all cells should gradually increase in height as they get closer
 * to the peak quite evenly
 * given view shows 11x11 I should be able to find out which direction is rising most and move that way
 * could try compare values from view center to each corner, but sample size too low on its own, would be inconsistent
 * since starting from a corner i should move diagonally, use corners instead of walls
 * could try get the average of each quartile in the view and move in the way of the highest average quartile
 * have to deal with noise aswell, a corner with noise would mess up the results and could make my algorithm
 * move in a direction it shouldn't
 * noise is added 20 times with a max radius of 1-9 and 50 50 chance of extruding or protruding
 * finds the highest/lowest value within the spot noise selected and turns all cell heights in that spot to that value
 * i need to find a way to detect these areas, knowing its min-max radius is 1-9 could be helpful
 * when i detect them i should jump over them instead of moving back or around since given the cone shape of
 * the mountain the peak should definetely be in the direction im moving, i just have an abyss in the way to cross
 *
 * for fast movement i remember that search algorithm that goes halfway through a list
 * and checks if val is higher or lower, then jumps half of original jump back or forward
 * binary search, i think it was the most time efficient algorithm i studied
 * since i don't know the actual matrix height, width i can't implement it fully
 * but knowing the min height and width is atleast 100 i can make jumps at half that size
 * and hopefully still have a good search time
 * calculate highest quartile average > jump 50 diagonally > calculate highest quartile  > jump 25
 * may have to jump back at some points since I might overshoot, will have to detect overshoots and distinguish from noise
 *
 *
 */

path_point find_highest_point() {
    // your solution here
    float view[VIEW_SIZE][VIEW_SIZE];
    // viewing at 0,0 would have half the view out of bounds, insetting by the view radius should give me a full first view
    int current_x = VIEW_RADIUS;
    int current_y = VIEW_RADIUS;
    int x_direction = 1;
    int y_direction = 1;
    int last_x_direction = 1;
    int last_y_direction = 1;
    // highest and lowest cell i've encountered throughtout entire explored matrix
    float genuinely_highest_cell = -3;
    int genuinely_highest_cell_x;
    int genuinely_highest_cell_y;

    float genuinely_lowest_cell = 401;
    int genuinely_lowest_cell_x;
    int genuinely_lowest_cell_y;
    // used to detect if my algorithm is stuck or at top, lifeline is for severe cases
    int no_change_counter = 0;
    int lifeline = 0;

    // the view, analyse, end and declare or move loop
    for (int a = 0; a < 999; a++) { // cant be over 1000 because of user_path[1000], would cause overflow and problems
        if (no_change_counter >= 5 && genuinely_highest_cell > 350 && lifeline <1) {
        current_x = genuinely_highest_cell_x;
        current_y = genuinely_highest_cell_y;
        lifeline++;
        }
        // viewing from my current position
        generate_view(view, current_y, current_x);
        // analysing my current view, getting quadrant averages, ignoring out of bounds cells, finding higest and lowest cells, 1 of my boundary solutions
        float top_left_sum = 0;
        float top_left_cnt = 0;
        float top_left_avg = 0;

        float top_right_sum = 0;
        float top_right_cnt = 0;
        float top_right_avg = 0;

        float bot_left_sum = 0;
        float bot_left_cnt = 0;
        float bot_left_avg = 0;

        float bot_right_sum = 0;
        float bot_right_cnt = 0;
        float bot_right_avg = 0;

        float highest_cell = -2;
        int highest_cell_x;
        int highest_cell_y;

        float lowest_cell = 401;
        int lowest_cell_x;
        int lowest_cell_y;

        for (int i = 0; i < VIEW_SIZE; i++) {
            for (int j = 0; j < VIEW_SIZE; j++) {
                float cell_altitude = view[i][j];
                if (cell_altitude == -1) continue;

                if (cell_altitude > highest_cell) {
                    highest_cell = cell_altitude;
                    highest_cell_y = i;
                    highest_cell_x = j;
                }

                if (cell_altitude < lowest_cell) {
                    lowest_cell = cell_altitude;
                    lowest_cell_y = i;
                    lowest_cell_x = j;
                }

                if (i < VIEW_RADIUS && j < VIEW_RADIUS) {
                    top_left_sum += cell_altitude;
                    top_left_cnt += 1;
                }

                else if (i < VIEW_RADIUS && j > VIEW_RADIUS) {
                    top_right_sum += cell_altitude;
                    top_right_cnt += 1;
                }

                else if (i > VIEW_RADIUS && j < VIEW_RADIUS) {
                    bot_left_sum += cell_altitude;
                    bot_left_cnt += 1;
                }

                else if (i > VIEW_RADIUS && j > VIEW_RADIUS) {
                    bot_right_sum += cell_altitude;
                    bot_right_cnt += 1;
                }



            }

        }
        /**
         * boundary solution
         * adding the sum of valid cells in the view which was gotten for averages in each quadrant
         * each quadrant can have a total of 25 valid cells when excluding the center cross, 4 quads, 100 cells
         * therefore if theres 50 or less valid cells then up to half the view could be in the boundary, ignoring center cross
         * jump back 1 view size to get out of boundary but not a completely different loacation and reanalyse view
         * but this would cost another generate view call which wastes a turn
         * if half the view on the frontside of the jump is past boundary, it is basically excluded
         * from the highest quadrant calculations and won't jump in that direction again
         * however if its at the border then thats obviously not where it should jump so its not an issue
         * this should only trigger jump back and reanalyse if the majority of the view is in the boundary, messing up direction decision greatly
         * then jump back 1 view size, no need for greater jumps since it's unlikely to be many view sizes beyond the border
         *
         * added if (current_x/y < VIEW_RADIUS) current_x/y = 0; to set view back into bounds if it is still out of bounds after first jump back
         *
         */
        int valid_count = top_left_cnt + top_right_cnt + bot_left_cnt + bot_right_cnt;
        if (valid_count < 15) {
            last_x_direction = -last_x_direction;
            last_y_direction = -last_y_direction;
            current_x += last_x_direction * VIEW_SIZE;
            current_y += last_y_direction * VIEW_SIZE;
            if (current_x < VIEW_RADIUS) current_x = 0;
            if (current_y < VIEW_RADIUS) current_y = 0;
            no_change_counter = 0;
            continue;
        }

        // making sure the count of each quarter is greater than zero before division for average to prevent a possible div by 0
        if (top_left_cnt > 0) top_left_avg = top_left_sum / top_left_cnt;
        if (top_right_cnt > 0) top_right_avg = top_right_sum / top_right_cnt;
        if (bot_left_cnt > 0) bot_left_avg = bot_left_sum / bot_left_cnt;
        if (bot_right_cnt > 0) bot_right_avg = bot_right_sum / bot_right_cnt;

        // updating my global view values if better ones were found in the view
        if (highest_cell <= genuinely_highest_cell) no_change_counter++;
        else if (highest_cell > 0 && highest_cell > genuinely_highest_cell) {
            genuinely_highest_cell = highest_cell;
            genuinely_highest_cell_y = current_y + (highest_cell_y - VIEW_RADIUS);
            genuinely_highest_cell_x = current_x + (highest_cell_x - VIEW_RADIUS);
            no_change_counter = 0;
        }

        if (lowest_cell < genuinely_lowest_cell) {
            genuinely_lowest_cell = lowest_cell;
            genuinely_lowest_cell_y = current_y + (lowest_cell_y - VIEW_RADIUS);
            genuinely_lowest_cell_x = current_x + (lowest_cell_x - VIEW_RADIUS);
        }



        // checking if peak is in the view and ending loop early to declare


        if (no_change_counter >= 15 || genuinely_highest_cell == 400) break;
        /* old solution failed on run 54 but ideal for most cases
        i should just center first and then move in such small increments
        if (no_change_counter >= 5 && genuinely_highest_cell > 350) {
            current_x = genuinely_highest_cell_x + x_direction * 2;
            current_y = genuinely_highest_cell_y + y_direction * 2;
            no_change_counter++;
            continue;
        }
        */
        // plateu detection, also altitude cases
        /**
         * since a plateu has all the same values, the lowest cell and highest cell will also be the same in that quadrant
         * so taking highest cell - lowest cell should = 0 but i could only check if a plateu is fully in a quad this way
         * if a plateu is fully in a quad, the average should be identical and at least a quarter of a plateu is in the view quadrant
         * since a plateu radius is from 1-9 so a diameter of 2-18 and radius from view center to a corner is 5
         * if it's partially in a quadrant the average should be close to either the lowest or heighest cell
         * if even a few same values are in a quadrant the average should still be quite close to highest or lowest cell value
         * this would likely mean that i'm just touching the edge of a plateu, if im moving in that direction I would definetely want to jump over it fully
         * but i need to find a sweet spot since the cells arent random and gradually rise in altitude so the average of a regular quad
         * may not be too different from the value of the highest or lowest cell in a quad either
         * should be safe and make the detection very close to lowest or highest since I will cleanly jump over all cases anyways
         *
         */
        int move_multiple = 5;
        float average_difference = 1;
        /**
         * thought merge sort would be the move but this isnt a line with 2 sides nor does it have a known end
         * i can jump in wrong directions, overshoot or land at boundaries or plateus
         * decreasing my jump size by half after every jump might not work out
         * going to decrease my jump height based on altitude instead since the higher altitude the closer i should be, the less a jump i need
         * was going to do similar with average difference since the difference at the peak will be lower than the difference at the bot cause of spread or whatever
         * but this plateu detection method just failed me mostly so tried a different method.
         */
        if (highest_cell >= 390) {
            move_multiple = 1;
            average_difference = 0.1;
        }
        else if (highest_cell >= 350) {
            move_multiple = 1;
            average_difference = 0.2;
        }
        else if (highest_cell >= 300) {
            move_multiple = 2;
            average_difference = 0.4;
        }
        else if (highest_cell >= 200) {
            move_multiple = 3;
            average_difference = 0.6;
        }
        else if (highest_cell >= 100) {
            move_multiple = 4;
            average_difference = 0.8;
        }
        else {
            move_multiple = 5;
            average_difference = 1.0;
        }

        // if its jumping back and forth not near the peak, thisll reduce the move multiple hopefully getting it out of some dodgy back and forth loop
        if (no_change_counter >= 5 && genuinely_highest_cell < 350) {
            if (move_multiple > 1) move_multiple = move_multiple - 1;
        }

        /* this doesn't seem to work had to try a different method
        if (((highest_cell - top_left_avg) > 0 && (highest_cell - top_left_avg) < average_difference) || ((lowest_cell - top_left_avg) > 0 && (lowest_cell - top_left_avg) < average_difference)
        || (((highest_cell - top_right_avg) > 0 && (highest_cell - top_right_avg) < average_difference) || ((lowest_cell - top_right_avg) > 0 && (lowest_cell - top_right_avg) < average_difference))
        || (((highest_cell - bot_left_avg) > 0 && (highest_cell - bot_left_avg) < average_difference) || ((lowest_cell - bot_left_avg) > 0 && (lowest_cell - bot_left_avg) < average_difference))
        || (((highest_cell - bot_right_avg) > 0 && (highest_cell - bot_right_avg) < average_difference) || ((lowest_cell - bot_right_avg) > 0 && (lowest_cell - bot_right_avg) < average_difference))) {
            current_x += last_x_direction * VIEW_SIZE * move_multiple;
            current_y += last_y_direction * VIEW_SIZE * move_multiple;
            continue;
        }
        */
       // this is just a much more locked in, decisive plateu detection, either all cells are the same meaning plateu, or not
       if (highest_cell - lowest_cell == 0) {
            current_x += last_x_direction * VIEW_SIZE * move_multiple;
            current_y += last_y_direction * VIEW_SIZE * move_multiple;
            no_change_counter = 0;
            continue;
        }
        // moving and dealing with boundary edges
        // finding the quadrant with the highest avg altitude and setting to move in that direction
        float highest_avg_altitude = top_left_avg;
        x_direction = -1;
        y_direction = -1;

        if (top_right_avg > highest_avg_altitude) {
            highest_avg_altitude = top_right_avg;
            x_direction = 1;
            y_direction = -1;
        }
        if (bot_left_avg > highest_avg_altitude) {
            highest_avg_altitude = bot_left_avg;
            x_direction = -1;
            y_direction = 1;
        }
        if (bot_right_avg > highest_avg_altitude) {
            highest_avg_altitude = bot_right_avg;
            x_direction = 1;
            y_direction = 1;
        }
        // used if in a plateu with no new good calculation for correct direction, moves in the last correct direction, jumping over the plateu
        last_x_direction = x_direction;
        last_y_direction = y_direction;

        // if its going back and forth near the peak, reduces the jumps to sub view size
        if (no_change_counter >= 5 && genuinely_highest_cell > 350) {
            current_x = current_x + x_direction * 6;
            current_y = current_y + y_direction * 6;
        }
        else {
            current_x += x_direction * VIEW_SIZE * move_multiple;
            current_y += y_direction * VIEW_SIZE * move_multiple;
        }
        // boundary inset, if x/y goes below 0 ie out of bounds, its x/y is set to 0 moving it back into bounds
        if (current_x < 0) current_x = 0; // was view radius and view radius instead of 0 before
        if (current_y < 0) current_y = 0;

        //if (current_x < VIEW_RADIUS && highest_cell >= 350) current_x = 0;
        //if (current_y < VIEW_RADIUS && highest_cell >= 350) current_y = 0;
        // actually this is overkill, would need a temp for x and y since the first if's set it to view radius already
        // and if the top half is partially in the border it doesn't matter since it shoudlnt go further upwards so it shouldnt be counted
        // can just move diagonally bot right, then top right, instead of top right, set to whatever,5 and then top right again

    }

    declare_peak(genuinely_highest_cell_x, genuinely_highest_cell_y);

    path_point ret;
    ret.x = genuinely_highest_cell_x;
    ret.y = genuinely_highest_cell_y;
    return ret;

    /**
     * so far running this gives fast climbs, sub 13 jumps, but around a 50% failure rate
     * issues:
     * didn't add a way to prevent view from going out of bounds
     * either jumps too large near peak or not enough a no_change_counter limit to allow the alg to circle around and correct itself
     * to do:
     * add boundary fix, could check when x or y go below 5 and adjust them to 5 to prevent going over the start edges, 5 since view radius will view it all
     * don't know specific ends of matrix but know it's at most 199x199 and atleast 100x100 so could do a similar thing for ends
     * when x or y go past 199, set them back to 100 - view radius, this would solve some boundary cases but it's only specific cases and not a good solution
     * could detect when a quadrant is in -1's, count them and adjust the corresponding x y coordinates and direction
     *
     * failing attempts 40, 41, 49, 54, 55, 70, 75, 78, 83, 86, 94
     * attempt 40: top right quad won on first view, jumped top right, maybe my diagonal approach was not ideal
     * solution: prevent going beyond less than view radius x and y protecting the top and left wall
     * I noted this in the earlier border checks kindof and should've implemented it immediately alongside my other boundary solution
     *
     * now only failing 54, 75, 83 with all passes sub 30
     * attempt 75: peak is at x 4 which my view cannot center on because I set my border edge cases to view radius which = 5
     * solution: I should remove the view radius inset on the border edge cases
     * would allow border cells to in the view which could mess some things up but it is what it is
     * can only apply this change when detecting it's near a peak ie high altitude
     *
     * failing 83 and 54
     * attempt 83: jump enters far into boundary and doesn't make it back in 1 jump
     * causing my boundary solution to flip direction again and jump back further into the boundary again, repeating infinitely
     * solution: my edge cases are after my boundary sol so they dont trigger, I can either move my boundary sol,
     * add the edgecases inside the boundary sol jump
     * or have boundary sol detect if its still in boundary after last jump and use last jump directions
     * adding the edge cases is easiest and doesnt add extra turns
     * looking back, insetting first view by view radius was kind of pointless since jumping top left or top right would be useless and wasteful anyways
     *
     * attempt 54: unsure of the problem fully all I know is it went back and fourth
     * til it hit the no_change counter limit with an altitude over 350, then stayed around the same area
     * and failed to find the peak
     * if i change that trigger to center my view at the highest the view has seen
     * and then reanalyse the view from there it should point in the right direction hopefully
     * at the cost of more turns and possibly other run fails
     *
     * after many failures somehow fixed attempt 54 by increasing the jump size slightly for its case
     * issue with smaller increment like by 2 cells, is proabaly that each new view call change was insignificant
     * if around a plateu or edge a single cell from those could mess up the quadrant averages and move me in the wrong direction
     *
     * however my runs take more turns, and attempt 19 only completes after the loop runs out
     * attempt 19: keeps jumping between 2 areas til end of loop
     * if it keeps going then that means the cells its landing on are the same or close so likely a plateu
     * i guess my plateu detection is meh
     * couldnt think of a fix so just added a condition to end the loop if the highest cell found is over 399
     * since that would mean it's really close to, and likely is the peak
     * this fixed this run and made all runs much more efficient reaching performance eval 9.10. I am dumbfounded by my stupidity why didn't I consider adding this simple fix earlier
     * that is all it took to detect a peak and exit the loop early and i fiddled around with no_change counts and strange cases instead
     *
     */


}
