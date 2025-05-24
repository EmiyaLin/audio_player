#include "wav.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

WAVHeader *load_wav_header(const char *filename, FILE **file_ptr)
{
    WAVHeader *header = NULL;
    FILE *file = NULL;

    if (file_ptr == NULL)
    {
        fprintf(stderr, "Output file pointer (file_ptr) is NULL itself.\n");
        return NULL;
    }
    *file_ptr = NULL;

    file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file %s\n", filename);
        return NULL;
    }
    header = (WAVHeader *)malloc(sizeof(WAVHeader));
    if (!header)
    {
        fprintf(stderr, "Failed to allocate memory for WAV header\n");
        fclose(file);
        return NULL;
    }
    size_t items_read = fread(header, sizeof(WAVHeader), 1, file);
    if (items_read != 1)
    {
        fprintf(stderr, "Failed to read complete WAV header\n");
        free_wav_header(header);
        fclose(file);
        return NULL;
    }
    if (strncmp(header->riff_id, "RIFF", 4) != 0 || strncmp(header->wave_id, "WAVE", 4) != 0 || strncmp(header->fmt_id, "fmt ", 4) != 0 || strncmp(header->data_id, "data", 4) != 0)
    {
        fprintf(stderr, "Invalid WAV file: %s\n", filename);
        free_wav_header(header);
        fclose(file);
        return NULL;
    }

    *file_ptr = file;
    return header;
}

void free_wav_header(WAVHeader *header)
{
    if (header)
        free(header);
}

void free_wav_data(int16_t *data)
{
    if (data)
        free(data);
}

int16_t *load_wav_data(FILE *file, const WAVHeader *header, size_t *num_frames_ptr)
{
    if (num_frames_ptr == NULL)
    {
        fprintf(stderr, "Output parameter num_frames_ptr is NULL.\n");
        return NULL;
    }
    *num_frames_ptr = 0;
    if (file == NULL || header == NULL)
    {
        fprintf(stderr, "Invalid arguments: file or header is NULL.\n");
        return NULL;
    }
    if (header->audio_format != 1)
    {
        fprintf(stderr, "暂不支持非PCM数据");
        return NULL;
    }
    if (header->bits_per_sample != 16)
    {
        fprintf(stderr, "暂不支持非16bit的音频");
        return NULL;
    }
    if (header->data_size == 0)
    {
        return NULL;
    }
    if (header->block_align == 0)
    {
        fprintf(stderr, "block_align can not be 0");
        return NULL;
    }
    *num_frames_ptr = header->data_size / header->block_align;
    int16_t *audio_data = (int16_t *)malloc(header->data_size);
    if (audio_data == NULL) {
        fprintf(stderr, "给音频数据缓冲去分配内存失败");
        *num_frames_ptr = 0;
        return NULL;
    }
    if (fread(audio_data, header->data_size, 1, file) != 1) {
        fprintf(stderr, "读取音频数据时发生了错误");
        free_wav_data(audio_data);
        *num_frames_ptr = 0;
        return NULL;
    }
    return audio_data;
}