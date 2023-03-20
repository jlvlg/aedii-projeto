#include "index.h"
#include <stdio.h>
#include <stdlib.h>

void print_inner(Garbage garbage) {
    if (garbage != NULL) {
        printf("%d:%d ", garbage->start, garbage->end);
        print_inner(garbage->next);
    }
}

void print(Garbage garbage) {
    print_inner(garbage);
    printf("\n");
}

int main(int argc, char *argv[]) {
    Garbage garbage = NULL;
    int pos;
    for (int i = 1; i < argc;) {
        switch (atoi(argv[i])) {
            case 1:
                idx.dump(&garbage, atoi(argv[i+1]), atoi(argv[i+2]));
                i += 3;
                break;
            case 2:
                pos = idx.recycle(&garbage, atoi(argv[i+1]));
                printf("%d\n", pos);
                i += 2;
                break;
            case 3:
                print(garbage);
                i++;
        }
    }
    idx.destroy_garbage(garbage);
}