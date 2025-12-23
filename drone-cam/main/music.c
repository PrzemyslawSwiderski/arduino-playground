#include "bsp/esp-bsp.h"

#define BUFFER_SIZE (512)
#define DEFAULT_VOLUME (70)

static const char *TAG = "MUSIC";
static esp_codec_dev_handle_t spk_codec_dev = NULL;

// Very simple WAV header, ignores most fields
typedef struct __attribute__((packed))
{
  uint8_t ignore_0[22];
  uint16_t num_channels;
  uint32_t sample_rate;
  uint8_t ignore_1[6];
  uint16_t bits_per_sample;
  uint8_t ignore_2[4];
  uint32_t data_size;
  uint8_t data[];
} dumb_wav_header_t;

static void music_task(void *args)
{
  /* Initialize codec only once */
  if (spk_codec_dev == NULL)
  {
    spk_codec_dev = bsp_audio_codec_speaker_init();
    assert(spk_codec_dev);
    esp_codec_dev_set_out_vol(spk_codec_dev, DEFAULT_VOLUME);
  }

  /* Pointer to a file that is going to be played */
  // const char music_filename[] = BSP_SPIFFS_MOUNT_POINT "/16bit_mono_22_05khz.wav";
  const char music_filename[] = BSP_SPIFFS_MOUNT_POINT "/alive.wav";
  const char *play_filename = music_filename;
  int16_t *wav_bytes = malloc(BUFFER_SIZE);
  assert(wav_bytes != NULL);

  /* Open WAV file */
  ESP_LOGI(TAG, "Playing file %s", play_filename);
  FILE *play_file = fopen(play_filename, "rb");
  if (play_file == NULL)
  {
    ESP_LOGW(TAG, "%s file does not exist!", play_filename);
    free(wav_bytes);
    vTaskDelete(NULL);
    return;
  }

  /* Read WAV header file */
  dumb_wav_header_t wav_header;
  if (fread((void *)&wav_header, 1, sizeof(wav_header), play_file) != sizeof(wav_header))
  {
    ESP_LOGW(TAG, "Error in reading file");
    fclose(play_file);
    free(wav_bytes);
    vTaskDelete(NULL);
    return;
  }
  ESP_LOGI(TAG, "Number of channels: %" PRIu16 "", wav_header.num_channels);
  ESP_LOGI(TAG, "Bits per sample: %" PRIu16 "", wav_header.bits_per_sample);
  ESP_LOGI(TAG, "Sample rate: %" PRIu32 "", wav_header.sample_rate);
  ESP_LOGI(TAG, "Data size: %" PRIu32 "", wav_header.data_size);

  esp_codec_dev_sample_info_t fs = {
      .sample_rate = wav_header.sample_rate,
      .channel = wav_header.num_channels,
      .bits_per_sample = wav_header.bits_per_sample,
  };
  esp_codec_dev_open(spk_codec_dev, &fs);

  uint32_t bytes_send_to_i2s = 0;
  while (bytes_send_to_i2s < wav_header.data_size)
  {
    /* Get data from SPIFFS and send it to codec */
    size_t bytes_read_from_spiffs = fread(wav_bytes, 1, BUFFER_SIZE, play_file);
    if (bytes_read_from_spiffs > 0)
    {
      esp_codec_dev_write(spk_codec_dev, wav_bytes, bytes_read_from_spiffs);
      bytes_send_to_i2s += bytes_read_from_spiffs;
    }
    else
    {
      break; // End of file or read error
    }
  }

  ESP_LOGI(TAG, "Playback finished");

  /* Clean up resources */
  fclose(play_file);
  free(wav_bytes);

  /* Close codec but don't delete it - it can be reused */
  esp_codec_dev_close(spk_codec_dev);

  /* Delete task - MUST be last thing in function */
  vTaskDelete(NULL);
}

void play_music()
{
  xTaskCreate(music_task, "music", 4096, NULL, 5, NULL);
}
