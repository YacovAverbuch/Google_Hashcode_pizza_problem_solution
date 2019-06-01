//
// Created by Yacov Averbuch on 26/02/2019.
//


#include "pizza_cutter.h"

#include <exception>

#include <fstream>

#include <string>
#include <vector>
#include <unistd.h>


pizza_cutter parse_input_file(const std::string& file_name);
void print_results_to_file(const std::vector<pizza_slice>& res, const std::string& file_name);
void show_results(const pizza_cutter & cutter, std::vector<pizza_slice> & results);
uint verify_solution(const pizza_cutter &cutter, std::vector<pizza_slice> & results);

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: google_pizza <input_file_name> <output_file_name>\n";
        return 1;
    }
    try {
        pizza_cutter pc = parse_input_file(argv[1]);
        auto results = pc.solve();

        uint ans_size = verify_solution(pc, results);
        std::cout << "\ndone groups select. answer size " << ans_size << " cells, from "
                << pc.get_row_num() * pc.get_column_num() << " pizza size\n";

        std::ofstream ofs;
        ofs.open((std::string)argv[2] + ".size", std::ifstream::trunc);
        ofs.clear();
        ofs << "ans size: " << ans_size << " cells, out of " << pc.get_row_num() * pc.get_column_num() << std::endl;


        show_results(pc, results);
        print_results_to_file(results, argv[2]);

    } catch(const std::ios_base::failure& ios_exc) {
        std::cout << ios_exc.what() << std::endl;
        return 1;
    }
    return 0;
}


pizza_cutter parse_input_file(const std::string& file_name) {
    std::ifstream ifs(file_name);

    if (!ifs.is_open()) {

        throw std::ios_base::failure("Cannot open the input file.");
    }

    int rows, cols, min_each, max_total;
    ifs >> rows >> cols >> min_each >> max_total;
    ifs.get();  // eat new line

    pizza_t pizza;
    for (int i = 0; i < rows; ++i) {
        pizza_row_t row;
        row.reserve(cols);  // build a vector with the needed size

        for (int j = 0; j < cols; ++j) {
            char val = ifs.get();  // 'M' or 'T'
            row.push_back(pizza_cell{i, j , val});
        }

        //  use std::move so it will not need to copy all the data
        // this is allow only because std::vector has a move
        pizza.push_back(std::move(row));
        ifs.get();
    }
    // again, use move on pizza
    return pizza_cutter(min_each, max_total, std::move(pizza));
}

void print_results_to_file(const std::vector<pizza_slice>& res, const std::string& file_name) {
    std::ofstream ofs;

    ofs.open(file_name, std::ifstream::trunc);

    if (!ofs.is_open()) {
        throw std::ios_base::failure("Cannot open the output file.");
    }

    ofs.clear();
    ofs << res.size() << std::endl;

    for (auto& slice : res) {
        ofs << slice << std::endl;
    }
}


uint verify_solution(const pizza_cutter &cutter, std::vector<pizza_slice> & results) {
    uint pixels_score = 0;
    std::vector<std::vector<bool>> grid_test (cutter.get_row_num(), std::vector<bool>(cutter.get_column_num(), false));

    for (pizza_slice group : cutter.get_final_slices()){
        for (pizza_cell * cell : group.cells_inside){
            pixels_score ++;
            if (grid_test[cell->row][cell->column] == true) std::cerr << "ERR\n";
            // raise assertion if the statement is incorrect
            assert(! grid_test[cell->row][cell->column]);
            grid_test[cell->row][cell->column] = true;
        }
    }
    return pixels_score;
}


void show_results(const pizza_cutter & cutter, std::vector<pizza_slice> & results){
    std::vector<std::vector<char>> picture (cutter.get_row_num(), std::vector<char>(cutter.get_column_num(), ' '));
    const pizza_t & pizza = cutter.get_pizza();

    for (uint row = 0; row < picture.size(); row ++){
        for (uint column = 0; column < picture.at(0).size(); column ++){
            picture[row][column] = pizza[row][column].ingr;
        }
    }

    for (pizza_slice group : results){
        for (pizza_cell * cell : group.cells_inside){
            picture[cell->row][cell->column] = '*';
        }
    }

    for (uint row = 0; row < picture.size(); row ++){
        for (uint column = 0; column < picture.at(0).size(); column ++){
            std::cout << picture.at(row).at(column);
        }
        std::cout << std::endl;
    }
}



