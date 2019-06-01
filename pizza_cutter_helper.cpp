//
// Created by Yacov Averbuch on 26/02/2019.
//


#include "pizza_cutter.h"



void pizza_cutter::init_shapes()
{
    for (int height = 1 ;height <= max_total_; height ++){
        for (int width = 1; (width * height) <= max_total_ ; width ++){
            slice_shape cur_shape = slice_shape{0, 0, height - 1, width - 1};
            if (cur_shape.size() >= (min_each_ * 2) && cur_shape.size() <= max_total_){
                possible_shapes.push_back(cur_shape);
            }
        }
    }
}


bool pizza_cutter::has_ingredients(const pizza_slice & ps) const noexcept
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
    return (mushrooms >= min_each_) && (tomatoes >= min_each_);
}


void pizza_cutter::init_all_groups(){
    int row2, column2;
    for (pizza_row_t cur_row : pizza_){
        for(pizza_cell cur_sell : cur_row ){
            for(slice_shape cur_shape : possible_shapes){
                row2 = cur_sell.row + cur_shape.row2;
                column2 = cur_sell.column + cur_shape.col2;

                if ((column2 >= pizza_[0].size()) || (row2 >= pizza_.size()))  continue;

                pizza_slice * cur_slice = new pizza_slice{true, cur_sell.row, cur_sell.column, row2, column2, 0};

                if (! has_ingredients(* cur_slice)){
                    delete cur_slice;
                    continue;
                }

                for (int i = cur_slice->row1; i <= cur_slice->row2; i ++){
                    for (int j = cur_slice->col1; j <= cur_slice->col2; j ++){
                        cur_slice->cells_inside.push_back(& pizza_[i][j]);
                        pizza_[i][j].inGroups.push_back(cur_slice);
                    }
                }
                groups.push_back(cur_slice);
            }
        }
    }
}


void* rank_groups(void *arg){
    pizza_cutter * pizzaCutter = (pizza_cutter *)arg;
    std::set<pizza_slice *> shared_groups;


    while(true){
        uint group_key = (uint)pizzaCutter->atomic_group_to_score ++;
        if (group_key >= pizzaCutter->groups.size()) break;

        shared_groups.clear();
        for (pizza_cell * cell : pizzaCutter->groups[group_key]->cells_inside){
            for (pizza_slice * ps : cell->inGroups){
                shared_groups.insert(ps);
            }
        }
        pizzaCutter->groups[group_key]->num_of_shared_groups = (uint)shared_groups.size() - 1;

        if (group_key > 0 && group_key % 10000 == 0){
            std::cout << group_key << " groups ranked\n";
        }
    }
    return nullptr;
}


void pizza_cutter::rank_all_groups()
{
    pthread_t pthread_identifiers[4];

    for (int i = 0; i < 4; ++i) {
        pthread_create(& pthread_identifiers[i], NULL, rank_groups, this);
    }
    for (int i = 0; i < 4; ++i){
        pthread_join( pthread_identifiers[i], nullptr);
    }
}


// to correct the rank adding later we want to know ahead what is the characteristic of the groups
// the characteristics we are interest in is (the number of groups with every thickness), and the average
// score each of the group took so we can correct the rank without distruct the initial ranking
void pizza_cutter::analyse_legal_groups()
{
    std::vector<std::pair<uint, uint>> analysis((unsigned long)max_total_);

    uint sample_num = (groups.size() < 1000) ? (uint)groups.size() : 1000;
    for(uint sample = 0; sample < sample_num; sample ++){
        pizza_slice * cur_slice = groups.at( rand() % groups.size() );
        uint width = (cur_slice->col2 - cur_slice->col1 + 1);
        uint height = (cur_slice->row2 - cur_slice->row1 + 1);

        uint thick = (width < height) ? width : height;

        analysis[thick].first ++;
        analysis[thick].second += cur_slice->num_of_shared_groups;
    }
    for (int i = 0; i < analysis.size(); i ++){
        if (analysis[i].first == 0) continue;
         std::cout << "Thick: " << i << " num of groups: " << analysis[i].first <<
                " average shared groups: " << analysis[i].second / analysis[i].first << std::endl;
    }
}


// add rank to groups that it is likely to be extended after we chioce all the groups on the board
// the rank adding should be in a proportional way to the initial ranking
void pizza_cutter::set_additional_rank()
{
    analyse_legal_groups();

    int add_rank_table[5];
    std::cout << "Add rank to group which can be extend at the extend process. (slim groups is more likely to be extend)\n";
    std::cout << "Enter rank to add to groups with 1-thick: ";
    std::cin >> add_rank_table[1];
    std::cout << "Enter rank to add to groups with 2-thick: ";
    std::cin >> add_rank_table[2];
    std::cout << "Enter rank to add to groups with 3-thick: ";
    std::cin >> add_rank_table[3];
    std::cout << "Enter rank to add to groups with 4-thick: ";
    std::cin >> add_rank_table[4];

    for(pizza_slice * cur_slice : groups){
        int width = (cur_slice->col2 - cur_slice->col1 + 1);
        int height = (cur_slice->row2 - cur_slice->row1 + 1);
        int thick = (width < height) ? width : height;

        assert(0 < thick && thick < 5);
		if (cur_slice->size() <= max_total_ - thick){
			cur_slice->num_of_shared_groups -= add_rank_table[thick];
		}
        
    }
}


void pizza_cutter::init_main_set(){
    for (auto it = groups.begin(); it != groups.end(); it ++ ){
        // we insert to the multiset a pointers to the elements in the group
        uint rank =  (* it)->num_of_shared_groups;
        if (legal_groups_set.find(rank) == legal_groups_set.end()){
            legal_groups_set[rank] = new std::set<pizza_slice *>();
        }
        legal_groups_set[rank]->insert(* it);
    }
}


void pizza_cutter::correct_score(pizza_slice * group_to_cancel, std::set<pizza_slice *> & groups_to_correct_score)
{
    for (pizza_cell * cur_cell : group_to_cancel->cells_inside){
        for(pizza_slice * slice_to_correct : cur_cell->inGroups){
            // (we can get non active group_to_correct in the previous iteration, so: )
            if (! slice_to_correct->is_on) continue;

            groups_to_correct_score.insert(slice_to_correct);
            slice_to_correct->temp_slices_not_disturb.insert(group_to_cancel);
        }
    }
}


void pizza_cutter::erase_from_set(pizza_slice * group_to_delete){
    auto group_set = legal_groups_set[group_to_delete->num_of_shared_groups];
    group_set->erase(group_to_delete);

    if (group_set->empty()){
        delete group_set;
        legal_groups_set[group_to_delete->num_of_shared_groups] = nullptr;
        legal_groups_set.erase(group_to_delete->num_of_shared_groups);
    }
}


void pizza_cutter::add_to_set(pizza_slice * group_to_add){
    uint rank =  group_to_add->num_of_shared_groups;

    if (legal_groups_set.find(rank) == legal_groups_set.end()){
        legal_groups_set[rank] = new std::set<pizza_slice *>();
    }
    legal_groups_set[rank]->insert(group_to_add);
}


void pizza_cutter::choose_slices()
{
    while (! legal_groups_set.empty()){
        auto best_score_key_value = * legal_groups_set.begin();
        pizza_slice * cur_slice = * best_score_key_value.second->begin();

        answer.push_back(cur_slice);
        erase_from_set(cur_slice);

        if (answer.size() % 1000 == 0){
            std::cout << answer.size() <<  " groups had chosen" << "\n";
        }

        cur_slice->is_on = false;
        // we remove all the groups that change their value in this iteration and add them afterward
        std::set<pizza_slice *> groups_to_correct_score;

        for (pizza_cell * cell : cur_slice->cells_inside){
            cell->has_owner = true;
            for (pizza_slice * group_to_cancel : cell->inGroups){
                if (! group_to_cancel->is_on) continue;
                group_to_cancel->is_on = false;  // this field is for ignoring it at correct score

                erase_from_set(group_to_cancel);
                correct_score(group_to_cancel, groups_to_correct_score);
            }
        }

        for(pizza_slice * group_to_score : groups_to_correct_score){
            if (! group_to_score->is_on) continue;

            erase_from_set(group_to_score);
            group_to_score->num_of_shared_groups -= group_to_score->temp_slices_not_disturb.size();
            group_to_score->temp_slices_not_disturb.clear();
            add_to_set(group_to_score);
        }
    }
}

