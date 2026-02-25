## Forex Data
Data files which can be passed to the CandleSeries's `clean` function must be in the form of the files found in the following [google drive folder](https://drive.google.com/drive/folders/1zg6DB-i43fscfZ2cD6MMwTbwGCy1PeLa?usp=drive_link).

## Dependencies
- The `glaze` and `nlohmann` json libraries.
- The `pugixml` library for parsing XML.
- The machine learning package `mlpack` and its dependencies `armadillo` and `ensmallen`.
- The header file `gnuplot-iostream.h` for generating plots.
- The optimization library `alglib`.
- The `boost/regex` library which is used in conjunction with `pugixml` for parsing Edgar filings. 

## Compilation
- If using any of the report functions in `Backtest`, then `gnuplot-iostream.h` must be in a folder named gnuplot_iostream in the same directory as the folder containing 
the backtesting repo (alternatively, you can change the `#include` directive to be the path pointing to where you have gnuplot-iostream downloaded). gnuplot-iostream is used 
to generate png files of the equity curve for a backtest which is then included in the backtest report tex file. Note that using gnuplot-iostream requires you to 
link against `boost_filesystem` and `boost_iostreams`. 
- Due to the use of pugixml in the the namespace `utility::parse`, when compiling `utility.cpp` you must also link against `pugixml`. 
- This repository mainly uses the `glaze` package for reading and writing json which requires you to compile with the C++23 standard.
- If using the `OptimalPortfolio` class you must link against `alglib` because we find the minimum weighted variance portfolio weights
  by using a constrained gradient descent algorithm in which after each step against the gradient we may need to project back onto the constraints
  (Projection onto the constraints that the porfolio weights sum to $1$ and that the weighted expected return is above the target return is done by formulating
  the norm minimization problem as a Linear Program to be solved by `alglib`).

## Note on Model Templates
The genetic hyperparameter tuner in `Genetic.h`, modeling pipelines in `model.h`, and the backtesting functions in `Backtest.h` are templated on model type 
(among other things). The class used when instantiating functions templated on model type must behave like `mlpack`'s classification model classes, that is, 
they must have `.Train()` and `.Classify()` member functions. See mlpack's documentation for details. 
