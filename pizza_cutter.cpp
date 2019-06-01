
#include "pizza_cutter.h"

#include <iostream>



std::ostream& operator<<(std::ostream& os, pizza_slice ps) noexcept
{
    os << ps.row1 << ' ' << ps.col1 << ' ' << ps.row2 << ' ' << ps.col2;
    return os;
}

std::vector<pizza_slice> pizza_cutter::solve() noexcept
{
    init_shapes();
    std::cout << "done shapes creation: " << possible_shapes.size() <<  " shapes created\n";

    // for any possible group create new group and save a pointer to it in pc.groups
    init_all_groups();
    std::cout << "done groups creation: " << groups.size() <<  " groups created\n";

    // rank a group according to the number of groups it disturbing. the lower in score is the better
    rank_all_groups();
    std::cout << "done first ranking\n";

    set_additional_rank();
    std::cout << "done second ranking\n";

    init_main_set();
    std::cout << "done set creating\n";

    choose_slices();

    // in this part we try to extend all the groups. the slimmer the group is, the more likely it
    // would be to extend
    for (pizza_slice * ps : answer) slices_final.push_back(* ps);
    try_to_extend_all_slices();

    return slices_final;
}


std::optional<pizza_slice> pizza_cutter::extend_slice(pizza_slice ps, direction dir) const noexcept
{
    switch(dir) {
        case direction::right:
            if (is_new_part_valid(pizza_slice{true, ps.row1, ps.col2 + 1, ps.row2, ps.col2 + 1, 0}))
                return pizza_slice{true, ps.row1, ps.col1, ps.row2, ps.col2 + 1, 0};
            break;
        case direction::down:
            if (is_new_part_valid(pizza_slice{true, ps.row2 + 1, ps.col1, ps.row2 + 1, ps.col2, 0}))
                return pizza_slice{true, ps.row1, ps.col1, ps.row2 + 1, ps.col2, 0};
            break;
        case direction::left:
            if (is_new_part_valid(pizza_slice{true, ps.row1, ps.col1 - 1, ps.row2, ps.col1 - 1, 0}))
                return pizza_slice{true, ps.row1, ps.col1 - 1, ps.row2, ps.col2, 0};
            break;
        case direction::up:
            if (is_new_part_valid(pizza_slice{true, ps.row1 - 1, ps.col1, ps.row1 - 1, ps.col2, 0}))
                return pizza_slice{true, ps.row1 - 1, ps.col1, ps.row2, ps.col2, 0};
            break;
    }
    return std::nullopt;
}

bool pizza_cutter::is_new_part_valid(pizza_slice new_part) const noexcept
{
    if (new_part.col1 < 0 || new_part.col2 >= pizza_[0].size() ||
        new_part.row1 < 0 || new_part.row2 >= pizza_.size()) {
        return false;
    }

    for (int row = new_part.row1; row <= new_part.row2; ++row) {
        for (int col = new_part.col1; col <= new_part.col2; ++col) {
            if (pizza_[row][col].has_owner)
                return false;
        }
    }
    return true;
}


void pizza_cutter::mark_slice(pizza_slice ps, int slice_id) noexcept
{
    for (int row = ps.row1; row <= ps.row2; ++row) {
        for (int col = ps.col1; col <= ps.col2; ++col) {
            pizza_[row][col].has_owner = true;
        }
    }
}


std::tuple<int,int> pizza_cutter::count_ingredients(pizza_slice ps) const noexcept
{
    int mushrooms = 0;
    int tomatoes = 0;

    for (int row = ps.row1; row <= ps.row2; ++row) {
        for (int col = ps.col1; col <= ps.col2; ++col) {
            if (pizza_[row][col].ingr == 'M') {
                ++mushrooms;
            } else {
                ++tomatoes;
            }
        }
    }
    return std::tie(mushrooms, tomatoes);
}


void pizza_cutter::try_to_extend_all_slices() noexcept
{
    for (int i = 0; i < slices_final.size(); ++i) {
        for (auto d : directions) {
            while(true){

                auto new_slice = extend_slice(slices_final[i], d);
                if (!new_slice) break;

                auto [mushrooms, tomatoes] = count_ingredients(new_slice.value());

                if (is_too_large(mushrooms, tomatoes))
                    break;


                mark_slice(new_slice.value(), i);
                // we have to correct the current slice instead
                pizza_slice * cur_slice = & slices_final[i];

                cur_slice->row1 = new_slice.value().row1;
                cur_slice->row2 = new_slice.value().row2;
                cur_slice->col1 = new_slice.value().col1;
                cur_slice->col2 = new_slice.value().col2;

                cur_slice->cells_inside.clear();

                for (int row = cur_slice->row1; row <= cur_slice->row2; row ++){
                    for (int column = cur_slice->col1; column <= cur_slice->col2; column ++){
                        cur_slice->cells_inside.push_back(& pizza_[row][column]);
                        pizza_[row][column].inGroups.push_back(cur_slice);
                    }
                }
            }
        }
    }
}
