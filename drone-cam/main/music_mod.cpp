#include "music_mod.hpp"
#include "bsp/esp-bsp.h"
#include "task.hpp"
#include "logger.hpp"
#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include <random>

namespace music
{

  static constexpr size_t BUFFER_SIZE = 512;
  static constexpr int DEFAULT_VOLUME = 90;

  static espp::Logger logger({.tag = "MUSIC", .level = espp::Logger::Verbosity::INFO});
  static esp_codec_dev_handle_t spk_codec_dev = nullptr;
  static std::unique_ptr<espp::Task> music_task;

  /* Available audio files */
  static const std::vector<std::string> music_files = {
      BSP_SD_MOUNT_POINT "/alive.wav",
      BSP_SD_MOUNT_POINT "/audio.wav",
  };

  /* WAV parsing structs */
  struct RiffHeader
  {
    char id[4]; // "RIFF"
    uint32_t size;
    char format[4]; // "WAVE"
  };

  struct ChunkHeader
  {
    char id[4]; // "fmt " or "data"
    uint32_t size;
  };

  struct FmtChunk
  {
    uint16_t audio_format;
    uint8_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint8_t bits_per_sample;
  };

  /* WAV playback function */
  static bool play_wav_file(const std::string &filename)
  {
    FILE *file = nullptr;
    std::vector<int16_t> audio_buffer(BUFFER_SIZE / sizeof(int16_t));

    // Initialize codec if needed
    if (spk_codec_dev == nullptr)
    {
      spk_codec_dev = bsp_audio_codec_speaker_init();
      if (!spk_codec_dev)
      {
        logger.error("Failed to initialize speaker codec");
        return false;
      }
      esp_codec_dev_set_out_vol(spk_codec_dev, DEFAULT_VOLUME);
    }

    logger.info("Playing file: {}", filename);

    file = fopen(filename.c_str(), "rb");
    if (!file)
    {
      logger.warn("File does not exist: {}", filename);
      return false;
    }

    // Read RIFF header
    RiffHeader riff;
    if (fread(&riff, sizeof(riff), 1, file) != 1 ||
        memcmp(riff.id, "RIFF", 4) != 0 ||
        memcmp(riff.format, "WAVE", 4) != 0)
    {
      logger.error("Not a valid WAV file");
      fclose(file);
      return false;
    }

    // Parse WAV chunks
    FmtChunk fmt = {};
    uint32_t data_size = 0;
    long data_offset = 0;
    ChunkHeader chunk;

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
      logger.error("No data chunk found in WAV");
      fclose(file);
      return false;
    }

    logger.info("Channels: {}, Bits: {}, Sample rate: {}, Data size: {}",
                fmt.num_channels, fmt.bits_per_sample, fmt.sample_rate, data_size);

    // Configure codec
    esp_codec_dev_sample_info_t fs = {
        .bits_per_sample = fmt.bits_per_sample,
        .channel = fmt.num_channels,
        .sample_rate = fmt.sample_rate,
    };
    esp_codec_dev_open(spk_codec_dev, &fs);

    // Seek to data
    fseek(file, data_offset, SEEK_SET);

    // Playback loop
    uint32_t bytes_sent = 0;
    while (bytes_sent < data_size)
    {
      size_t to_read = BUFFER_SIZE;
      if (bytes_sent + to_read > data_size)
      {
        to_read = data_size - bytes_sent;
      }

      size_t bytes_read = fread(audio_buffer.data(), 1, to_read, file);
      if (bytes_read == 0)
      {
        break;
      }

      esp_codec_dev_write(spk_codec_dev, audio_buffer.data(), bytes_read);
      bytes_sent += bytes_read;
    }

    logger.info("Playback finished");

    fclose(file);
    esp_codec_dev_close(spk_codec_dev);

    return true;
  }

  void play()
  {
    // Stop any existing playback
    if (music_task)
    {
      music_task->stop();
      music_task.reset();
    }

    // Select random file
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, music_files.size() - 1);

    size_t idx = dis(gen);
    std::string selected = music_files[idx];

    logger.info("Selected audio: {}", selected);

    // Create playback task
    music_task = std::make_unique<espp::Task>(espp::Task::Config{
        .callback = [selected](auto &m, auto &cv) -> bool
        {
          play_wav_file(selected);
          return true; // Stop task after playback
        },
        .task_config = {
            .name = "music",
            .stack_size_bytes = 4096,
            .priority = 5}});

    music_task->start();
  }
}
