#ifndef WAV_H
#define WAV_H

#include <stdint.h>
#include <stdio.h>

#pragma pack(push, 1)

// WAV file header
typedef struct
{
    char riff_id[4];    // "RIFF"
    uint32_t riff_size; // Size of the overall file
    char wave_id[4];    // "WAVE"

    char fmt_id[4];           // "fmt "
    uint32_t fmt_size;        // format data length
    uint16_t audio_format;    // type of format
    uint16_t num_channels;    // number of channels
    uint32_t sample_rate;     // sample rate
    uint32_t byte_rate;       // sample rate * byte per sample * channels
    uint16_t block_align; // byte per frame. 1 for 8 bit mono, 2 for 8 bit stereo or 16 bit mono, 4 for 16 bit stereo
    uint16_t bits_per_sample; // bits per sample
    char data_id[4];          // "data"
    uint32_t data_size;       // data size
} WAVHeader;

#pragma pack(pop)

/**
 * @brief 加载WAV文件头部信息。
 *
 * 打开指定的WAV文件，读取并验证其头部。如果成功，则返回一个指向
 * 动态分配的WAVHeader结构体的指针，并将文件指针*file_ptr更新为
 * 指向数据块开始之前的位置（或者就是打开的文件指针，具体逻辑待定）。
 * 如果失败（文件不存在、不是WAV文件、格式不支持等），返回NULL。
 * 调用者负责在不再需要时调用 free_wav_header() 释放返回的结构体，
 * 并负责关闭*file_ptr指向的文件。
 *
 * @param filename 要加载的WAV文件名。
 * @param file_ptr 输出参数，用于返回打开的文件指针。文件将打开为二进制读取模式。
 * @return WAVHeader* 指向加载的头部信息的指针，或NULL表示失败。
 */
WAVHeader *load_wav_header(const char *filename, FILE **file_ptr);

/**
 * @brief 从已打开的WAV文件中加载音频数据。
 *
 * 假设文件指针 file 当前位于WAV数据块的起始位置。
 * 根据header中的信息 (特别是data_size和bits_per_sample)，读取音频数据。
 * 目前主要支持16位PCM数据。
 *
 * @param file 已打开并定位到数据块的WAV文件指针。
 * @param header 指向已加载的WAVHeader的指针。
 * @param num_frames_ptr 输出参数，返回加载的音频帧总数 (总帧数 = header->data_size / header->block_align)。
 * @return int16_t* 指向动态分配的音频数据缓冲区的指针 (16位PCM数据)，或NULL表示失败。
 *         调用者负责在不再需要时调用 free_wav_data() 释放此缓冲区。
 */
int16_t *load_wav_data(FILE *file, const WAVHeader *header, size_t *num_frames_ptr);

/**
 * @brief 释放由 load_wav_header 分配的 WAVHeader 结构体内存。
 * @param header 指向要释放的 WAVHeader 结构体的指针。
 */
void free_wav_header(WAVHeader *header);

/**
 * @brief 释放由 load_wav_data 分配的音频数据缓冲区内存。
 * @param data 指向要释放的音频数据缓冲区的指针。
 */
void free_wav_data(int16_t *data);
#endif