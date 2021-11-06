#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#define arraysize 35
#define BASE 1000000000

uint1024_t from_uint(unsigned int num){
    uint1024_t from_u;
    from_u.array[arraysize - 1] = num % BASE;
    from_u.array[arraysize - 2] = num / BASE;
    return from_u;
}

unsigned int charInInt(unsigned int array [9])
{
    unsigned int outValue = 0, t = 1;
    for (int i = 8; i >= 0; i--)
    {
        outValue += array[i] * t;
        t *= 10;
    }
    return outValue;
}
void printf_value(uint1024_t num){
    int count_null = 0;
    while (num.array[count_null] == 0){
        count_null++;
    }
    if (count_null == arraysize){
        printf("%d", 0);
    }
    else{
        printf("%u", num.array[count_null++]);
        for (int i = count_null; i < arraysize; i++){
            printf("%0.9u", num.array[i]);
        }
    }
}
typedef struct uint1024_t {
    uint32_t array[arraysize];
}uint1024_t;

void scan_value(uint1024_t *num){
    for (int i = 0; i < arraysize; i++){
        num->array[i] = 0;
    }
    char string [315];
    for (int i = 0; i < 315; i++){
        string[i] = 0;
    }
    int symbol = 0;
    int i = 0;
    int num_of_symbols = 0;
    while ((symbol = getchar()) != EOF){
        if (symbol == '\n'){
            break;
        }
        if (i > 315){
            printf("переполнение");
            exit(1);
        }
        string[i++] = symbol;
        num_of_symbols++;
    }
    unsigned int array [9];
    for (int j = 0; j < 9; j++){
        array[j] = 0;
    }
    int count_array = 8; //т.к. записываем число с конца
    int count_uint1024 = arraysize - 1;
    for (int i = (num_of_symbols - 1); i >= 0; i--){
        array[count_array--] = string[i]- '0';
        if (count_array == -1){
            count_array = 8;
            num->array[count_uint1024--] = charInInt(array);
            for (int j = 0; j < 9; j++){
                array[j] = 0;
            }
        }
    }
    if (count_array != 8){
        num->array[count_uint1024] = charInInt(array);
    }
}



uint1024_t add_op(uint1024_t num, uint1024_t y){
    uint1024_t sum;
    for (int i = 0; i < arraysize; i++){
        sum.array[i] = 0;
    }
    unsigned int next_rank = 0;
    for(int i = arraysize - 1; i != -1; i--){
        unsigned long current_sum = num.array[i] + y.array[i] + next_rank;
        sum.array[i] = current_sum % BASE;
        next_rank = current_sum / BASE;
    }
    return sum;
}

uint1024_t subtr_op(uint1024_t num, uint1024_t y){
    uint1024_t subtr;
    for (int i = 0; i < arraysize; i++){
        subtr.array[i] = 0;
    }
    unsigned int next_rank = 0;
    for(int i = arraysize -1; i != - 1; i--){
        unsigned  long current_subtr =(num.array[i] + BASE) - (y.array[i] + next_rank);
        subtr.array[i]= current_subtr % BASE;
        if (current_subtr >= BASE){
            next_rank = 0;
        }
        else{
            next_rank = 1;
        }
    }
    return subtr;
}

uint1024_t mult_op(uint1024_t num, uint1024_t y){
    uint1024_t mult;
    for (int i = 0; i < arraysize; i++){
        mult.array[i] = 0;
    }
    for(int i = arraysize - 1; i >= 0; i--){
        unsigned long next_rank = 0;
        for (int j = arraysize - 1; j >= 0; j--){
            if ((i + j - arraysize + 1) < 0 || (i + j - arraysize + 1) > 34){
                break;
            }
            unsigned long current_mult = mult.array[i + j - arraysize + 1] + num.array[i] * y.array[j] + next_rank;
            mult.array[i + j - arraysize + 1]= current_mult % BASE;
            next_rank = current_mult / BASE;
        }
    }
    return mult;
}

int main() {
    uint1024_t num, y, sum, subtr, mult;
    scan_value(&num);
    scan_value(&y);

    printf_value(num);
    printf("\n");
    printf_value(y);
    sum = add_op(num, y);
    subtr = subtr_op(num, y);
    mult = mult_op(num, y);
    printf("\n");
    printf_value(sum);
    printf("\n");
    printf_value(subtr);
    printf("\n");
    printf_value(mult);
    return 0;
}