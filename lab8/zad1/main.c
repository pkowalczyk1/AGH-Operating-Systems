#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int method;   /* 0 - numbers   1 - blocks */
    int thread_index;
    int total_threads;
    int width;
    int height;
    int **image;
    int **output_img;
    double time;
} Thread_job;

int** read_image(FILE *image, int *width, int *height) {
    char header[3];
    fread(header, sizeof(char), 3, image);
    header[2] = '\0';
    if (strcmp(header, "P2") != 0) {
        fprintf(stderr, "Wrong file header\n");
        exit(-1);
    }

    char c;
    while (fgetc(image) == '#') {
        while (fgetc(image) != '\n');
    }

    fseek(image, -1, SEEK_CUR);
    char number_buf[50];
    int ind = 0;
    while ((c = fgetc(image)) != ' ') {
        number_buf[ind] = c;
        ind++;
    }

    number_buf[ind] = '\0';
    *width = atoi(number_buf);
    ind = 0;
    while ((c = fgetc(image)) != '\n') {
        number_buf[ind] = c;
        ind++;
    }

    number_buf[ind] = '\0';
    *height = atoi(number_buf);
    int **image_arr = calloc(*height, sizeof(int*));
    int number;
    fscanf(image, "%d", &number);
    for (int i = 0; i < *height; i++) {
        image_arr[i] = calloc(*width, sizeof(int));
        for (int j = 0; j < *width; j++) {
            fscanf(image, "%d", &image_arr[i][j]);
        }
    }

    return image_arr;
}

void invert(void *job) {
    Thread_job* job_info = (Thread_job*)job;
    struct timespec begin_time, end_time;
    clock_gettime(CLOCK_REALTIME, &begin_time);

    if (job_info->method == 0) {
        int left_bound = 255 * (job_info->thread_index / job_info->total_threads);
        int right_bound;
        if (job_info->thread_index == job_info->total_threads - 1) {
            right_bound = 255;
        }
        else {
            right_bound = 255 * ((job_info->thread_index + 1) / job_info->total_threads) - 1;
        }

        for (int i = 0; i < job_info->height; i++) {
            for (int j = 0; j < job_info->width; j++) {
                if (job_info->image[i][j] <= right_bound && job_info->image[i][j] >= left_bound) {
                    job_info->output_img[i][j] = 255 - job_info->image[i][j];
                }
            }
        }
    }
    else {
        int start_col = (job_info->thread_index) * ceil(job_info->width / job_info->total_threads);
        int end_col = (job_info->thread_index + 1) * ceil(job_info->width / job_info->total_threads) - 1;
        for (int i = 0; i < job_info->height; i++) {
            for (int j = start_col; j <= end_col; j++) {
                job_info->output_img[i][j] = 255 - job_info->image[i][j];
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    job_info->time = (double)end_time.tv_sec * 1000000 + (double)(end_time.tv_nsec) / 1000 - ((double)begin_time.tv_sec * 1000000 + (double)(begin_time.tv_nsec) / 1000);

    pthread_exit(&(job_info->time));
}

void save_image(FILE *file, int **image_arr, int width, int height) {
    fprintf(file, "P2\n%d %d\n255\n", width, height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file, "%d ", image_arr[i][j]);
        }
        fprintf(file, "\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Too few arguments\n");
        return -1;
    }
    int threads_count = atoi(argv[1]);
    int method;
    if (strcmp(argv[2], "numbers") == 0) {
        method = 0;
    }
    if (strcmp(argv[2], "blocks") == 0) {
        method = 1;
    }
    FILE *times_file = fopen("Times.txt", "a");
    FILE *image = fopen(argv[3], "r");
    FILE *output_file = fopen(argv[4], "w");
    int width, height;
    int **image_arr = read_image(image, &width, &height);
    int **output_img = calloc(height, sizeof(int*));
    for (int i = 0; i < height; i++) {
        output_img[i] = calloc(width, sizeof(int));
    }

    struct timespec begin_time, end_time;
    clock_gettime(CLOCK_REALTIME, &begin_time);
    pthread_t *threads = calloc(threads_count, sizeof(pthread_t));
    Thread_job *thread_jobs = calloc(threads_count, sizeof(Thread_job));
    for (int i = 0; i < threads_count; i++) {
        thread_jobs[i].output_img = output_img;
        thread_jobs[i].image = image_arr;
        thread_jobs[i].height = height;
        thread_jobs[i].width = width;
        thread_jobs[i].method = method;
        thread_jobs[i].thread_index = i;
        thread_jobs[i].total_threads = threads_count;
        pthread_create(threads + i, NULL, (void *)invert, (void *)(thread_jobs + i));
    }

    for (int i = 0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    fprintf(times_file, "Total time: %lf\n", (double)end_time.tv_sec * 1000000 + (double)(end_time.tv_nsec) / 1000 - ((double)begin_time.tv_sec * 1000000 + (double)(begin_time.tv_nsec) / 1000));

    for (int i = 0; i < threads_count; i++) {
        fprintf(times_file, "Thread time %d: %lf\n", i, thread_jobs[i].time);
    }

    save_image(output_file, output_img, width, height);
    for (int i = 0; i < height; i++) {
        free(output_img[i]);
        free(image_arr[i]);
    }

    free(output_img);
    free(image_arr);
    fclose(image);
    fclose(output_file);
    fclose(times_file);

    return 0;
}
