#include <stdio.h>
#include <alsa/asoundlib.h>
#include "audio.h"
#include "wav.h"

static snd_pcm_t *pcm_handle = NULL;

AudioResult init_audio_playback(const WAVHeader *header)
{
    if (header == NULL)
    {
        fprintf(stderr, "WAV Header is NULL");
        return AUDIO_ERR_WAV_HEADER_NULL;
    }
    int err;

    // 打开PCM音频设备
    if ((err = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        fprintf(stderr, "Cannot open audio device default: %s\n", snd_strerror(err));
        return err;
    }

    // 配置硬件参数
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    if ((err = snd_pcm_hw_params_any(pcm_handle, hw_params)) < 0)
    {
        fprintf(stderr, "Cannot initialize hardware parameter structure: %s\n", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
        return err;
    }
    if ((err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf(stderr, "Cannot set access type:%s\n", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
        return err;
    }
    if ((err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        fprintf(stderr, "Cannot set format type:%s\n", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
        return err;
    }
    unsigned int sample_rate = header->sample_rate;
    if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &sample_rate, 0)) < 0)
    {
        fprintf(stderr, "Cannot set sample rate:%s\n", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
        return err;
    }
    if (sample_rate != header->sample_rate)
    {
        fprintf(stderr, "Warning: Sample rate %u not available, using %u instead.\n", header->sample_rate, sample_rate);
    }
    if ((err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, header->num_channels)) < 0)
    {
        fprintf(stderr, "Cannot set channel:%s\n", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
        return err;
    }
    if ((err = snd_pcm_hw_params(pcm_handle, hw_params)) < 0)
    {
        fprintf(stderr, "Cannot set hardware params:%s\n", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
        return err;
    }

    // 准备PCM设备以供使用
    if ((err = snd_pcm_prepare(pcm_handle)) < 0)
    {
        fprintf(stderr, "Cannot start pcm device:%s\n", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
        return err;
    }
    return AUDIO_OK;
}

snd_pcm_sframes_t play_audio_frames(const int16_t *pcm_data, size_t num_frames)
{
    if (pcm_handle == NULL)
    {
        fprintf(stderr, "Audio device not initialized");
        return AUDIO_ERR_DEVICE_NOT_INITIALIZED;
    }
    if (pcm_data == NULL)
    {
        fprintf(stderr, "PCM data is NULL");
        return AUDIO_ERR_PCM_DATA_NULL;
    }
    if (num_frames == 0)
    {
        fprintf(stderr, "Number of frames is 0");
        return 0;
    }
    snd_pcm_sframes_t frames_written = snd_pcm_writei(pcm_handle, pcm_data, num_frames);
    if (frames_written < 0)
    {
        // try to recover
        int recover_code = snd_pcm_recover(pcm_handle, frames_written, 0);
        if (recover_code < 0)
        {
            fprintf(stderr, "recover failed:%s\n", snd_strerror(recover_code));
            return recover_code;
        }
        return frames_written;
    }
    if (frames_written >= 0 && (size_t)frames_written != num_frames)
    {
        fprintf(stderr, "Warning: Only %ld frames written, expected %zu\n", frames_written, num_frames);
    }
    return frames_written;
}

AudioResult drain_audio_playback()
{
    if (pcm_handle == NULL)
    {
        fprintf(stderr, "Audio device not initialized");
        return AUDIO_ERR_DEVICE_NOT_INITIALIZED;
    }
    int err = snd_pcm_drain(pcm_handle);
    if (err < 0)
    {
        fprintf(stderr, "Failed to drain PCM: %s\n", snd_strerror(err));
        return err;
    }
    return AUDIO_OK;
}

void close_audio_playback()
{
    if (pcm_handle == NULL)
    {
        return;
    }
    int err = snd_pcm_close(pcm_handle);
    if (err < 0) {
        fprintf(stderr, "Failed to close PCM device: %s\n", snd_strerror(err));
    }
    pcm_handle = NULL;
}