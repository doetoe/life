#include <valarray>
#include <memory>
#include <algorithm>

class Matrix
{
  private:
    uint32_t rows_;
    uint32_t cols_;
    std::valarray<uint8_t> entries_;

    // convert the coordinates r,c to a linear index
    uint32_t index(uint32_t r, uint32_t c) const { return r * cols_ + c; }

  public:
    // Return slice that indexes the specified range of rows (to_row excluded).
    std::slice row_slice(uint32_t from_row, uint32_t to_row)
    {
        return std::slice(from_row * cols_, (to_row - from_row) * cols_, 1);
    }

    // Return slice that indexes the specified range of columns.
    std::gslice col_slice(uint32_t from_col, uint32_t to_col)
    {
        return std::gslice(from_col, {rows_, to_col - from_col}, {cols_, 1});
    }

  public:
    // Matrix m(10,10);
    Matrix(uint32_t rows, uint32_t cols)
            : rows_(rows), cols_(cols), entries_(rows * cols) {}

    // Matrix m_copy(m);
    Matrix(const Matrix& other)
            : rows_(other.rows_), cols_(other.cols_),
              entries_(other.entries_) {}
    
    // Returns a reference to the valarray holding the data internally.
    // These can be directly operated on.
    std::valarray<uint8_t>& data() {return entries_;}
    const std::valarray<uint8_t>& data() const {return entries_;}

    // Assignment involves deep copy of the data.
    Matrix& operator=(const Matrix& rhs)
    {
        entries_ = rhs.entries_;
        return *this;
    }

    uint32_t getRows() const { return rows_; }
    uint32_t getCols() const { return cols_; }

    uint8_t get(uint32_t r, uint32_t c) const { return entries_[index(r,c)]; }
    void set(uint32_t r, uint32_t c, uint8_t x) { entries_[index(r,c)] = x; }

    void add_rowshift(const Matrix& other, int32_t row_shift)
    {
        uint32_t shift = ((row_shift % int32_t(rows_)) + rows_) % rows_;
        auto bot_slice_other = row_slice(shift, rows_);
        auto top_slice_other = row_slice(0, shift);
        auto top_slice_this = row_slice(0, rows_ - shift);
        auto bot_slice_this = row_slice(rows_ - shift, rows_);
        entries_[top_slice_this] += other.entries_[bot_slice_other];
        entries_[bot_slice_this] += other.entries_[top_slice_other];        
    }

    void add_colshift(const Matrix& other, int32_t col_shift)
    {
        uint32_t shift = ((col_shift % int32_t(cols_)) + cols_) % cols_;
        auto right_slice_other = col_slice(shift, cols_);
        auto left_slice_other = col_slice(0, shift);
        auto left_slice_this = col_slice(0, cols_ - shift);
        auto right_slice_this = col_slice(cols_ - shift, cols_);
        entries_[left_slice_this] += other.entries_[right_slice_other];
        entries_[right_slice_this] += other.entries_[left_slice_other];        
    }
    
    ~Matrix() {}
};

