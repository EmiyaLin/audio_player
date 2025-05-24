#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include "wav.h"
#include "audio.h"

const size_t FRAMES_PER_BUFFER = 1024;
volatile __sig_atomic_t keep_playing = 1;

void handle_sigint(int sig) {
    (void) sig;
    printf("\nStopping playback...\n");
    keep_playing = 0;
}

int main(int argc, char *argv[])
{
    int exit_status = 0;

    WAVHeader *header = NULL;
    FILE *wav_file = NULL;
    int16_t *audio_data = NULL;
    size_t num_frames = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename.wav>\n", argv[0]);
        exit_status = 1;
        goto cleanup;
    }
    const char *filename = argv[1];
    header = load_wav_header(filename, &wav_file);
    if (header == NULL)
    {
        fprintf(stderr, "Error: Could not load WAV header from %s\n", filename);
        exit_status = 1;
        goto cleanup;
    }

    audio_data = load_wav_data(wav_file, header, &num_frames);
    fclose(wav_file);
    wav_file = NULL;
    if (audio_data == NULL)
    {
        fprintf(stderr, "Error: Could not load WAV data from %s\n", filename);
        exit_status = 1;
        goto cleanup;
    }
    AudioResult audio_init_status = init_audio_playback(header);

    if (audio_init_status != AUDIO_OK)
    {
        if (audio_init_status == AUDIO_ERR_WAV_HEADER_NULL) {
            fprintf(stderr, "Error initializing audio playback: WAV header was NULL (code: %d)\n", audio_init_status);
        } else {
            fprintf(stderr, "Error initializing audio playback: %s (code: %d)\n", snd_strerror(audio_init_status), audio_init_status);
        }
        exit_status = 1;
        goto cleanup;
    }
    size_t current_pos_frames = 0;
    while (current_pos_frames < num_frames)
    {
        size_t frames_this_iteration = FRAMES_PER_BUFFER;
        if (current_pos_frames + FRAMES_PER_BUFFER > num_frames)
        {
            frames_this_iteration = num_frames - current_pos_frames;
        }
        int16_t *current_buf_ptr = audio_data + (current_pos_frames * header->num_channels);
        snd_pcm_sframes_t frames_written = play_audio_frames(current_buf_ptr, frames_this_iteration);
        if (frames_written < 0)
        {
            fprintf(stderr, "Error playing audio frames: %s (code: %ld)\n", snd_strerror(frames_written), frames_written);
            exit_status = 1;
            break;
        }
        current_pos_frames += frames_written;
    }

    AudioResult drain_status = drain_audio_playback();
    if (drain_status != AUDIO_OK)
    {
        if (drain_status == AUDIO_ERR_DEVICE_NOT_INITIALIZED)
        {
            fprintf(stderr, "Error draining audio playback: Device not initialized (code: %d)\n", drain_status);
        }
        else
        { // 假设其他负值是 ALSA 错误
            fprintf(stderr, "Error draining audio playback: %s (code: %d)\n", snd_strerror(drain_status), drain_status);
        }
        exit_status = 1;
        goto cleanup;
    }

cleanup:
    close_audio_playback();
    if (wav_file != NULL)
    {
        fclose(wav_file);
        wav_file = NULL;
    }
    if (audio_data != NULL)
    {
        free_wav_data(audio_data);
        audio_data = NULL;
    }
    if (header != NULL)
    {
        free_wav_header(header);
        header = NULL;
    }

    return exit_status;
}