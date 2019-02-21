#ifndef MATRIX_H
#define MATRIX_H



using namespace std;

class Matrix {

    private:
        double* mat;
        double rows;
        double cols;       

    public:

        int size;

        Matrix(double _rows, double _cols){
            rows = _rows;
            cols = _cols;

            size = _rows * _cols;

            mat = new double[size];

            for (int i = 0; i < _rows * _cols; i++){
               mat[i] = 0;
            }
        };
        Matrix(double _rows, double _cols,double* matrix){
            rows = _rows;
            cols = _cols;
            mat = matrix;

            size = _rows * _cols;

        };

        // Scalar Math
        // Matrix operator*(double rhs){
        //     Matrix result(rows, cols);

        //     for (double i=0; i<rows; i++) {
        //         for (double j=0; j<cols; j++) {
        //             result.mat[i][j] = mat[i][j] * rhs;
        //         }
        //     }

        //     return result;
        // };
        // Matrix operator/(double rhs){
        //     Matrix result(rows, cols);

        //     for (double i=0; i<rows; i++) {
        //         for (double j=0; j<cols; j++) {
        //             result.mat[i][j] = mat[i][j] / rhs;
        //         }
        //     }

        //     return result;
        // };

        // Matrix Math
        Matrix operator*(Matrix rhs){
            double rows = rhs.get_rows();
            double cols = rhs.get_cols();
            Matrix result(rows, cols);

            for (double i=0; i<rows; i++) {
                for (double j=0; j<cols; j++) {
                    for (double k=0; k<rows; k++) {
                        double first = this->getElement(i,k);
                        double second = rhs.getElement(k,j);
                        result.setElement(i,j,result.getElement(i,j) + (first * second));
                    }
                }
            }

            return result;
        };
        // Matrix operator*=(Matrix& rhs){
        //     Matrix result = (*this) * rhs;
        //     (*this) = result;
        //     return *this;
        // };

        // Access the individual elements                                                                                                                                                                                               
        double getElement(int row, int col){
            int index = (row * cols) + col;

            return mat[index];
        };

        void setElement(int row, int col, double val){
            int index = (row * cols) + col;
            mat[index] = val;
        };

        // Access the row and column sizes                                                                                                                                                                                              
        double get_rows() {
            return rows;
        };
        double get_cols() {
            return cols;
        };
};

#endif