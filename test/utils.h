#define PRINT_ALL_SAMPLES false
#define NO_RATE -1.0
#define LOOP_BLOCKS 42 // number of blocks to generate when looping

#define REFERENCE_PATH "../../test/resources/reference/"
#define INPUT_PATH "../../test/resources/input"

extern void printTest(uint32_t expectedDataSize = 0, 
                      double rate = NO_RATE);