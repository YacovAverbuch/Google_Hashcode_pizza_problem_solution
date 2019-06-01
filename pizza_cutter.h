#ifndef GOOGLE_PIZZA_PIZZA_CUTTER_H
#define GOOGLE_PIZZA_PIZZA_CUTTER_H

#include <iostream>

#include <array>
#include <optional>
#include <tuple>
#include <vector>

#include <set>
#include <map>

#include <pthread.h>
#include <atomic>
#include <cstdio>

#include <cassert>

// available shapes of group
struct slice_shape
{
    int row1, col1, row2, col2;
    int size() { return (col2 - col1 + 1) * (row2 - row1 + 1); }
    void print(){
        std::cout << "possible shape: (" << row1 << ',' << col1 << ") (" << row2 << ',' << col2 << ')'
                  << "size: " << size() << std::endl; }
};


struct pizza_slice;

struct pizza_cell
{
    int row, column;
    char ingr;

    void print(){
        std::cout << "cur cell: row: " << row << "column: " << column << std::endl;
    }
    bool has_owner = false;
    std::vector<pizza_slice *> inGroups;
};


using pizza_row_t = std::vector<pizza_cell>;
using pizza_t = std::vector<pizza_row_t>;

struct pizza_slice
{
    bool is_on;
    int row1, col1, row2, col2;

    // rank to this group. if it is low, it more likely to be chosen first
    uint num_of_shared_groups;
    std::set<pizza_slice *> temp_slices_not_disturb;

    std::vector<pizza_cell *> cells_inside;

    int size() { return (col2 - col1 + 1) * (row2 - row1 + 1); }

    void print(){
        std::cout << "cur slice: (" << row1 << ',' << col1 << ") (" << row2 << ',' <<
            col2 << ')' << "size: " << size() << " shared groups: " << num_of_shared_groups << std::endl; }
};


std::ostream& operator<<(std::ostream& os, pizza_slice ps) noexcept;

class pizza_cutter
{
public:

    pizza_cutter(int min_each, int max_total, pizza_t&& pizza) noexcept
            :
            pizza_{std::move(pizza)}, min_each_{min_each}, max_total_{max_total}, slices_final{}
            {}

    // for rank the groups in multi treading scheme
    std::atomic<uint> atomic_group_to_score{0};
    // hold all the pizza slice pointers
    // there is a set to hold the pointers to the current valid slices
    std::vector<pizza_slice *> groups;

    std::vector<pizza_slice> solve() noexcept;

    uint get_row_num() const { return (uint)pizza_.size(); }
    uint get_column_num() const { return (uint)pizza_.at(0).size(); }
    const std::vector<pizza_slice> & get_final_slices() const { return slices_final; }
    const pizza_t & get_pizza() const { return pizza_; }

    ~pizza_cutter(){
        for(pizza_slice * slice : groups) delete slice;
    }

private:

    std::vector<slice_shape> possible_shapes;
    std::map<uint, std::set<pizza_slice *> *> legal_groups_set;
    std::vector<pizza_slice *> answer;
    std::vector<pizza_slice> slices_final;

    void init_shapes();

    bool has_ingredients(const pizza_slice & ps) const noexcept;

    void init_all_groups();

    // for rank the groups in multitreading scheme

    void rank_all_groups();
    void analyse_legal_groups();
    void set_additional_rank();

    void init_main_set();

    void erase_from_set(pizza_slice * group_to_delete);
    void add_to_set(pizza_slice * group_to_add);
    void choose_slices();

    void correct_score(pizza_slice * group_to_cancel, std::set<pizza_slice *> & groups_to_correct_score);

    enum class direction {right, down, left, up};
    constexpr static std::array<direction, 4> directions
            {direction::right, direction::down, direction::left, direction::up};
    int max_total_;
    int min_each_;

    pizza_t pizza_;

    void try_to_extend_all_slices() noexcept;

    void mark_slice(pizza_slice ps, int slice_id) noexcept;
    std::optional<pizza_slice> extend_slice(pizza_slice ps, direction dir) const noexcept;
    bool is_new_part_valid(pizza_slice new_part) const noexcept;
    std::tuple<int,int> count_ingredients(pizza_slice ps) const noexcept;


    bool is_too_large(int i1, int i2) const noexcept
    {
        return i1 + i2 > max_total_;
    }
};

#endif //GOOGLE_PIZZA_PIZZA_CUTTER_H
