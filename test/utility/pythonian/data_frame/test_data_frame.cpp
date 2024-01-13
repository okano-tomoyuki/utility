#include <utility>
#include "utility/pythonian/data_frame.hpp"

int main()
{
    // header true, separator charactor ';'
    auto df = DataFrame::read_csv("./winequality-red.csv", true, ';');
    df.describe();
    df.show();

    // filtering columns by name
    df = df[{"\"volatile acidity\"", "\"residual sugar\""}];
    df.describe();
    df.show();

    // filtering rows by index
    df = df[std::make_pair(100, 150)];
    df.describe();
    df.show();

    // rename columns
    df.rename({"acidity", "sugar"});
    df.show();

    // save csv
    df.to_csv("./output.csv");
}