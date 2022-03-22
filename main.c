#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <memory.h>
#include <dir.h>

#define TIME_TEST(testCode, time){ \
    clock_t start_time = clock(); \
    testCode; \
    clock_t end_time = clock();\
    clock_t sort_time = end_time - start_time; \
time = (double) sort_time / CLOCKS_PER_SEC; \
};

#define ARRAY_SIZE(a)  sizeof (a)/sizeof (a[0])

bool isOrdered(const int *a, size_t size) {
    for (size_t i = 1; i < size - 1; i++) {
        if (a[i - 1] > a[i])
            return false;
    }
    return true;
}

typedef struct SortFunc {
    void (*sort )(int *a, size_t n);

    char name[64];
} SortFunc;

// функция генерации
typedef struct GeneratingFunc {
    void (*generate )(int *a, size_t n);

    char name[64];
} GeneratingFunc;

//double getTime() {
//    clock_t start_time = clock();
//    // фрагмент кода
//    // время которого измеряется
//    clock_t end_time = clock();\
// clock_t sort_time = end_time - start_time;
//    return (double) sort_time / CLOCKS_PER_SEC;
//}


void outputArray(int *a, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", a[i]);
    }
}

void inputArray(int *a, size_t size) {
    for (size_t i = 0; i < size; i++) {
        scanf("%d", &a[i]);
    }
}

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void createFile(char* file_name,
                const SortFunc sort_func,
                const char* experiment_name,
                const char* suffix) {
    char file_dir[256];
    mkdir("../../data");
    sprintf(file_dir, "%s/%s/", "../../data", sort_func.name);
    mkdir(file_dir);
    sprintf(file_name, "%s/%s_%s.csv", file_dir, experiment_name, suffix);
}

void checkTime(SortFunc sortFuncn,
               void (*generateFunc )(int *, size_t),
               size_t size, char *experimentName) {
    static size_t runCounter = 1;
    // генерация последовательности
    static int innerBuffer[100000];
    generateFunc(innerBuffer, size);
    printf("Run #%zu| ", runCounter++);
    printf(" Name : %s\n", experimentName);

// замер времени
    double time;
    TIME_TEST((sortFuncn).sort(innerBuffer, size), time);

    // результаты замера
    printf(" Status : ");
    if (isOrdered(innerBuffer, size)) {
        printf("OK! Time : %.3f s.\n", time);

        // запись в файл
        char filename[256];
        createFile(filename, sortFuncn, experimentName, "time");
//        sprintf(filename, "./data/%s.csv", experimentName);
        FILE *f = fopen(filename, "a");
        if (f == NULL) {
            printf(" FileOpenError %s", filename);
            exit(1);
        }

        fprintf(f, "%zu; %.3f\n", size, time);
        fclose(f);
    } else {
        printf(" Wrong !\n");

        // вывод массива, который не смог быть отсортирован
        outputArray(innerBuffer, size);

        exit(1);
    }
}


// пузырьковая сортировка. Бессмертная классика.
void bubbleSort(int *a, size_t size) {
    for (size_t i = 0; i < size - 1; i++)
        for (size_t j = size - 1; j > i; j--)
            if (a[j - 1] > a[j])
                swap(&a[j - 1], &a[j]);
}

// Коктельная сортировка.
void shakerSort(int *a, size_t size) {
    int left = 0;
    int right = size > 0 ? size - 1 : left;
    int isDisplacement = 1;

    while ((left < right) && isDisplacement) {
        isDisplacement = 0;
        for (int i = left; i < right; i++) {
            if (a[i] > a[i + 1])
                swap(&a[i], &a[i + 1]);
            isDisplacement = 1;
        }


        right--;
        for (int i = right; i > left; i--) {
            if (a[i - 1] > a[i]) {
                swap(&a[i], &a[i - 1]);
                isDisplacement = 1;
            }
        }

        left++;
    }
}

// чет - нечет.
void oddEvenSort(int *a, size_t size) {
    for (size_t i = 0; i < size; i++)
        for (size_t j = !(i % 2); j + 1 < size; j += 2)
            if (a[j] > a[j + 1])
                swap(&a[j], &a[j + 1]);
}

// Сортировка выбором. Самое простое.
void selectionSort(int *a, size_t size) {
    for (int i = 0; i < size - 1; i++) {
        int minPos = i;
        for (int j = i + 1; j < size; j++)
            if (a[j] < a[minPos])
                minPos = j;
        swap(&a[i], &a[minPos]);
    }
}


// Сортировка вставками. Самое простое.
void insertionSort(int *a, size_t size) {
    for (size_t i = 1; i < size; i++) {
        int t = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > t) {
            a[j] = a[j - 1];
            j--;
        }
        a[j] = t;
    }
}


// Сортировка рассчесткой
void combSort(int *a, const size_t size) {
    size_t step = size;
    int isDisplacement = 1;
    while (step > 1 || isDisplacement) {
        if (step > 1)
            step /= 1.24733;
        isDisplacement = 0;
        for (size_t i = 0, j = i + step; j < size; ++i, ++j)
            if (a[i] > a[j]) {
                swap(&a[i], &a[j]);
                isDisplacement = 1;
            }
    }
}

// Сортировка Шелла
void shellSort(int *a, size_t size) {
    int i, j, step;
    int tmp;
    for (step = size / 2; step > 0; step /= 2)
        for (i = step; i < size; i++) {
            tmp = a[i];
            for (j = i; j >= step; j -= step) {
                if (tmp < a[j - step])
                    a[j] = a[j - step];
                else
                    break;
            }
            a[j] = tmp;
        }
}

void radixSort(int* a, size_t size) {
    const unsigned char mask = UCHAR_MAX;
    const size_t step = sizeof(char) * 8;
    int* buf = (int*) malloc(size * sizeof(int));

    for (size_t byteI = 0; byteI < sizeof(int); byteI++) {
        size_t positions[UCHAR_MAX + 1] = {0};
        for (int i = 0; i < size; i++) {
            unsigned char curByte = ((a[i] >> (byteI * step)) ^
                                     ((byteI + 1 == sizeof(int)) ?
                                       (1 << (step - 1)) : 0)) & mask;
            positions[curByte + 1]++;
        }

        for (size_t i = 1; i < ARRAY_SIZE(positions); i++) {
            positions[i] += positions[i - 1];
        }

        for (size_t i = 0; i < size; i++) {
            unsigned char curByte = ((a[i] >> (byteI * step)) ^
                                     ((byteI + 1 == sizeof(int)) ?
                                       (1 << (step - 1)) : 0)) & mask;
            buf[positions[curByte]++] = a[i];
        }

        memcpy(a, buf, size * sizeof(int));
    }

    free(buf);
}

int cmp(const void *a, const void *b) {
    return *(const int *) a - *(const int *) b;
}

int cmpReverse(const void *a, const void *b) {
    return *(const int *) b - *(const int *) a;
}

void generateOrderedArray(int *a, size_t size) {
    srand(time(0));
    for (size_t i = 0; i < size; i++) {
        a[i] = 100000 - rand() % 100000;
    }
    qsort(a, size, sizeof(int), cmp);
}

void generateOrderedBackwards(int *a, size_t size) {
    srand(time(0));
    for (size_t i = 0; i < size; i++) {
        a[i] = 100000 - rand() % 100000;
    }
    qsort(a, size, sizeof(int), cmpReverse);
}

void generateRandomArray(int *a, size_t size) {
    srand(time(0));
    for (size_t i = 0; i < size; i++) {
        a[i] = 100000 - rand() % 100000;
    }
}


void timeExperiment() {
    // описание функций сортировки
    SortFunc sorts[] = {
            {bubbleSort, " bubbleSort "},
            {oddEvenSort, " oddEvenSort "},
            {shakerSort, " shakerSort "},
            {selectionSort, " selectionSort "},
            {insertionSort, " insertionSort "},
            {radixSort, " radixSort "},
            {shellSort, " shellSort "},
            {combSort, " combSort "},
    };
    const unsigned FUNCS_N = ARRAY_SIZE (sorts);

    // описание функций генерации
    GeneratingFunc generatingFuncs[] = {
            // генерируется случайный массив
            {generateRandomArray,      " random "},
            // генерируется массив 0, 1, 2, ..., n - 1
            {generateOrderedArray,     " ordered "},
            // генерируется массив n - 1, n - 2, ..., 0
            {generateOrderedBackwards, " orderedBackwards "}
    };

    const unsigned CASES_N = ARRAY_SIZE(generatingFuncs);

    // запись статистики в файл
    for (size_t size = 10000; size <= 100000; size += 10000) {
        printf(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
        printf(" Size : %d\n", size);
        for (int i = 0; i < FUNCS_N; i++) {
            for (int j = 0; j < CASES_N; j++) {
                // генерация имени файла
                static char filename[128];
                sprintf(filename, "%s_% s_time ",
                        sorts[i].name, generatingFuncs[j].name);
                checkTime(sorts[i], generatingFuncs[j].generate, size, filename);
            }
        }
        printf("\n");
    }
}

int main() {
    timeExperiment();

    return 0;
}
