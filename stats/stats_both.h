#include <unistd.h>

char *file[] = {"../testfiles/pinguim.gif"};
size_t file_size = sizeof(file)/sizeof(char*);

size_t dtau[] = {0};
size_t dtau_size = sizeof(dtau)/sizeof(size_t);

float prob_data[] = {0.0};
float prob_head[] = {0.0};
size_t prob_size = sizeof(prob_data)/sizeof(float);

size_t capacity[] = {
//        50,
//        75,
//       110,
//       134,
//       150,
//       200,
//       300,
//       600,
//      1200,
//      1800,
//      2400,
//      4800,
//      9600,
//     19200,
     38400,
     57600,
    115200
};
size_t capacity_size = sizeof(capacity)/sizeof(size_t);
