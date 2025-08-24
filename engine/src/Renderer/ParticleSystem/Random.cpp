#include "Random.hpp"

static std::mt19937 random;
static std::uniform_int_distribution<std::mt19937::result_type> distribution;

void Random_Initialize() {
    random.seed(std::random_device()());
}

// returns a random number between 0 and 1 
float Random_Float() {
    return (float)distribution(random) / (float)std::numeric_limits<uint32_t>::max();
}