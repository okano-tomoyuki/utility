#ifndef _TEST_UTILITY_SHARED_MEMORY_SAMPLE_DATA_HPP_
#define _TEST_UTILITY_SHARED_MEMORY_SAMPLE_DATA_HPP_

struct Sample
{
    char update_time[32];
    double d_data;
};

static constexpr char SM_DATA_PATH[32] = "SAMPLE";

#endif