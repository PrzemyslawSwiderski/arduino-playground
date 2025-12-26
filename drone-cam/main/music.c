#include "bsp/esp-bsp.h"
#include "esp_random.h"

#define BUFFER_SIZE (512)
#define DEFAULT_VOLUME (70)

static const char *TAG = "MUSIC";
static esp_codec_dev_handle_t spk_codec_dev = NULL;

/* Available audio files */
static const char *music_files[] = {
    // BSP_SPIFFS_MOUNT_POINT "/alive.wav",
    BSP_SD_MOUNT_POINT "/alive.wav",
    // BSP_SPIFFS_MOUNT_POINT "/audio.wav",
    BSP_SD_MOUNT_POINT "/audio.wav",
};

#define MUSIC_FILE_COUNT (sizeof(music_files) / sizeof(music_files[0]))

/* WAV parsing structs */
typedef struct
{
  char id[4]; // "RIFF"
  uint32_t size;
  char format[4]; // "WAVE"
} riff_header_t;

typedef struct
{
  char id[4]; // "fmt " or "data"
  uint32_t size;
} chunk_header_t;

typedef struct
{
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
} fmt_chunk_t;

/* WAV playback task */
static void music_task(void *args)
{
  const char *filename = (const char *)args;
  FILE *file = NULL;
  int16_t *audio_buf = NULL;

  if (spk_codec_dev == NULL)
  {
    spk_codec_dev = bsp_audio_codec_speaker_init();
    assert(spk_codec_dev);
    esp_codec_dev_set_out_vol(spk_codec_dev, DEFAULT_VOLUME);
  }

  ESP_LOGI(TAG, "Playing file %s", filename);

  file = fopen(filename, "rb");
  if (!file)
  {
    ESP_LOGW(TAG, "%s file does not exist!", filename);
    vTaskDelete(NULL);
    return;
  }

  riff_header_t riff;
  fread(&riff, sizeof(riff), 1, file);
  if (memcmp(riff.id, "RIFF", 4) != 0 || memcmp(riff.format, "WAVE", 4) != 0)
  {
    ESP_LOGE(TAG, "Not a valid WAV file");
    fclose(file);
    vTaskDelete(NULL);
    return;
  }

  fmt_chunk_t fmt = {0};
  uint32_t data_size = 0;
  long data_offset = 0;
  chunk_header_t chunk;

  /* Scan WAV chunks */
  while (fread(&chunk, sizeof(chunk), 1, file) == 1)
  {
    if (memcmp(chunk.id, "fmt ", 4) == 0)
    {
      fread(&fmt, sizeof(fmt), 1, file);
      fseek(file, chunk.size - sizeof(fmt), SEEK_CUR);
    }
    else if (memcmp(chunk.id, "data", 4) == 0)
    {
      data_size = chunk.size;
      data_offset = ftell(file);
      break;
    }
    else
    {
      fseek(file, chunk.size, SEEK_CUR);
    }
  }

  if (data_size == 0)
  {
    ESP_LOGE(TAG, "No data chunk found in WAV");
    fclose(file);
    vTaskDelete(NULL);
    return;
  }

  ESP_LOGI(TAG, "Number of channels: %u", fmt.num_channels);
  ESP_LOGI(TAG, "Bits per sample: %u", fmt.bits_per_sample);
  ESP_LOGI(TAG, "Sample rate: %lu", fmt.sample_rate);
  ESP_LOGI(TAG, "Data size: %lu", data_size);

  esp_codec_dev_sample_info_t fs = {
      .sample_rate = fmt.sample_rate,
      .channel = fmt.num_channels, // mono or stereo as-is
      .bits_per_sample = fmt.bits_per_sample,
  };
  esp_codec_dev_open(spk_codec_dev, &fs);

  fseek(file, data_offset, SEEK_SET);

  audio_buf = malloc(BUFFER_SIZE);
  assert(audio_buf);

  uint32_t bytes_sent = 0;
  while (bytes_sent < data_size)
  {
    size_t to_read = BUFFER_SIZE;
    if (bytes_sent + to_read > data_size)
    {
      to_read = data_size - bytes_sent;
    }

    size_t bytes_read = fread(audio_buf, 1, to_read, file);
    if (bytes_read == 0)
      break;

    /* Send directly to codec without duplicating */
    esp_codec_dev_write(spk_codec_dev, audio_buf, bytes_read);
    bytes_sent += bytes_read;
  }

  ESP_LOGI(TAG, "Playback finished");

  free(audio_buf);
  fclose(file);
  esp_codec_dev_close(spk_codec_dev);

  vTaskDelete(NULL);
}

/* Public API to start playback */
void play_music()
{
  uint32_t idx = esp_random() % MUSIC_FILE_COUNT;
  const char *selected = music_files[idx];

  ESP_LOGI(TAG, "Selected audio: %s", selected);

  xTaskCreate(
      music_task,
      "music",
      4096,
      (void *)selected,
      5,
      NULL);
}
