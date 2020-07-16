//
// C++ Implementation by Ajay Bhaga
//

#ifndef EANN_SIMPLE_GRID_H
#define EANN_SIMPLE_GRID_H

#include <memory>

class Grid {
    size_t _rows;
    size_t _columns;
    std::unique_ptr<int[]> data;

public:
    Grid(size_t rows, size_t columns)
            : _rows{rows},
              _columns{columns},
              data{std::make_unique<int[]>(rows * columns)} {}

    size_t rows() const { return _rows; }

    size_t columns() const { return _columns; }

    int *operator[](size_t row) { return row * _columns + data.get(); }

    int &operator()(size_t row, size_t column) {
        return data[row * _columns + column];
    }
};

#endif //EANN_SIMPLE_GRID_H
