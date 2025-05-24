#ifndef AUDIO_H
#define AUDIO_H
#include "wav.h"
#include <alsa/asoundlib.h> // For snd_pcm_sframes_t

typedef enum
{
    AUDIO_OK = 0, // 表示成功
    AUDIO_ERR_WAV_HEADER_NULL = -1,
    AUDIO_ERR_DEVICE_NOT_INITIALIZED = -2,
    AUDIO_ERR_PCM_DATA_NULL = -3,
    AUDIO_ERR_PCM_WRITE = -4,
    // 如果将来有其他特定于音频模块的错误，可以在这里添加
    // 注意：ALSA库函数本身返回的负数错误码将保持原样，
    // 这些是我们自定义的、在ALSA错误之外或之上的错误码。
} AudioResult;

/**
 * @brief 初始化ALSA音频播放系统。
 *
 * 设置PCM设备参数，如采样率、格式、声道数等。
 *
 * @param header 指向WAVHeader的指针，用于获取音频参数。
 * @return AUDIO_OK 表示成功, 其他 AudioResult 值表示错误。
 */
AudioResult init_audio_playback(const WAVHeader *header);

/**
 * @brief 播放一个音频数据块。
 *
 * 将指定大小的音频数据写入已初始化的ALSA PCM设备。
 * 这是一个阻塞调用，直到数据块被写入缓冲区。
 *
 * @param pcm_data 指向要播放的PCM数据 (int16_t类型)。
 * @param num_frames 要播放的帧数 (一帧包含所有声道的一个采样点)。
 * @return 成功写入的帧数 (非负)，或 ALSA 错误码 (负数)。
 */
snd_pcm_sframes_t play_audio_frames(const int16_t *pcm_data, size_t num_frames);

/**
 * @brief 等待所有挂起的音频帧播放完毕。
 * @return AUDIO_OK 表示成功, 其他 AudioResult 值表示错误。
 */
AudioResult drain_audio_playback();

/**
 * @brief 关闭并释放ALSA音频播放系统资源。
 */
void close_audio_playback();

#endif