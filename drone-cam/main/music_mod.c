#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_gmf_pool.h"
#include "esp_gmf_element.h"
#include "esp_gmf_pipeline.h"
#include "esp_gmf_audio_helper.h"
#include "esp_gmf_audio_dec.h"
#include "esp_gmf_io_codec_dev.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "esp_gmf_app_setup_peripheral.h"
#include "gmf_loader_setup_defaults.h"

#define PIPELINE_BLOCK_BIT BIT(0)

#define DEFAULT_PLAY_URL "/sdcard/test.mp3"

static const char *TAG = "MUSIC";

esp_gmf_err_t _pipeline_event(esp_gmf_event_pkt_t *event, void *ctx)
{
  ESP_LOGI(TAG, "CB: RECV Pipeline EVT: el: %s-%p, type: %x, sub: %s, payload: %p, size: %d",
           "OBJ_GET_TAG(event->from)", event->from, event->type,
           esp_gmf_event_get_state_str(event->sub),
           event->payload, event->payload_size);

  if ((event->sub == ESP_GMF_EVENT_STATE_STOPPED) ||
      (event->sub == ESP_GMF_EVENT_STATE_FINISHED) ||
      (event->sub == ESP_GMF_EVENT_STATE_ERROR))
  {
    if (event->sub == ESP_GMF_EVENT_STATE_ERROR)
    {
      ESP_LOGE(TAG, "Pipeline error occurred!");
    }
    else if (event->sub == ESP_GMF_EVENT_STATE_FINISHED)
    {
      ESP_LOGI(TAG, "Playback finished successfully");
    }
    xEventGroupSetBits((EventGroupHandle_t)ctx, PIPELINE_BLOCK_BIT);
  }
  return ESP_GMF_ERR_OK;
}

void play_music()
{
  int ret;

  ESP_LOGI(TAG, "[ 1 ] Setup codec device");
  // Configuration of codec to be aligned with audio pipeline output
  esp_gmf_app_codec_info_t codec_info = ESP_GMF_APP_CODEC_INFO_DEFAULT();
  codec_info.play_info.sample_rate = CONFIG_GMF_AUDIO_EFFECT_RATE_CVT_DEST_RATE;
  codec_info.play_info.channel = CONFIG_GMF_AUDIO_EFFECT_CH_CVT_DEST_CH;
  codec_info.play_info.bits_per_sample = CONFIG_GMF_AUDIO_EFFECT_BIT_CVT_DEST_BITS;
  codec_info.record_info = codec_info.play_info;

  ESP_LOGI(TAG, "Codec config - Rate: %d, Channels: %d, Bits: %d",
           codec_info.play_info.sample_rate,
           codec_info.play_info.channel,
           codec_info.play_info.bits_per_sample);

  esp_gmf_app_setup_codec_dev(&codec_info);

  // Give codec time to initialize
  vTaskDelay(pdMS_TO_TICKS(200));

  // Get codec handle
  esp_codec_dev_handle_t codec_handle =
      (esp_codec_dev_handle_t)esp_gmf_app_get_playback_handle();
  if (!codec_handle)
  {
    ESP_LOGE(TAG, "Failed to get codec handle!");
    return;
  }

  ESP_LOGI(TAG, "[ 1 ] Configure ES8311 codec for playback");

  // Unmute the codec DAC
  ret = esp_codec_dev_set_out_mute(codec_handle, false);
  if (ret == ESP_OK)
  {
    ESP_LOGI(TAG, "DAC unmuted successfully");
  }
  else
  {
    ESP_LOGW(TAG, "Failed to unmute DAC: %d", ret);
  }

  // Set volume to maximum for testing
  ret = esp_codec_dev_set_out_vol(codec_handle, 50);
  if (ret == ESP_OK)
  {
    ESP_LOGI(TAG, "Volume set to 50%%");
  }
  else
  {
    ESP_LOGW(TAG, "Failed to set volume: %d", ret);
  }

  ESP_LOGI(TAG, "[ 3 ] Register all elements");
  esp_gmf_pool_handle_t pool = NULL;
  ret = esp_gmf_pool_init(&pool);
  if (ret != ESP_OK || !pool)
  {
    ESP_LOGE(TAG, "Failed to init pool: %d", ret);
    return;
  }

  gmf_loader_setup_io_default(pool);
  gmf_loader_setup_audio_codec_default(pool);
  gmf_loader_setup_audio_effects_default(pool);
  ESP_GMF_POOL_SHOW_ITEMS(pool);

  ESP_LOGI(TAG, "[ 4 ] Create audio pipeline");
  esp_gmf_pipeline_handle_t pipe = NULL;
  const char *name[] = {"aud_dec", "aud_ch_cvt", "aud_bit_cvt", "aud_rate_cvt"};
  ret = esp_gmf_pool_new_pipeline(pool, "io_file", name,
                                  sizeof(name) / sizeof(char *),
                                  "io_codec_dev", &pipe);
  if (ret != ESP_OK || !pipe)
  {
    ESP_LOGE(TAG, "Failed to create pipeline: %d", ret);
    goto cleanup;
  }

  // Set codec device to pipeline output
  ret = esp_gmf_io_codec_dev_set_dev(
      ESP_GMF_PIPELINE_GET_OUT_INSTANCE(pipe), codec_handle);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to set codec device: %d", ret);
    goto cleanup;
  }

  // Configure decoder
  esp_gmf_element_handle_t dec_el = NULL;
  ret = esp_gmf_pipeline_get_el_by_name(pipe, "aud_dec", &dec_el);
  if (ret != ESP_OK || !dec_el)
  {
    ESP_LOGE(TAG, "Failed to get decoder element: %d", ret);
    goto cleanup;
  }

  esp_gmf_info_sound_t info = {};
  ret = esp_gmf_audio_helper_get_audio_type_by_uri(DEFAULT_PLAY_URL, &info.format_id);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to detect audio format: %d", ret);
    goto cleanup;
  }
  ESP_LOGI(TAG, "Audio format detected: 0x%x", info.format_id);

  ret = esp_gmf_audio_dec_reconfig_by_sound_info(dec_el, &info);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to configure decoder: %d", ret);
    goto cleanup;
  }

  ESP_LOGI(TAG, "[ 5 ] Set audio URL");
  ret = esp_gmf_pipeline_set_in_uri(pipe, DEFAULT_PLAY_URL);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to set input URI: %d", ret);
    goto cleanup;
  }

  ESP_LOGI(TAG, "[ 6 ] Create task and bind to pipeline");
  esp_gmf_task_cfg_t cfg = DEFAULT_ESP_GMF_TASK_CONFIG();
  cfg.thread.stack = 4 * 1024;

  esp_gmf_task_handle_t work_task = NULL;
  ret = esp_gmf_task_init(&cfg, &work_task);
  if (ret != ESP_OK || !work_task)
  {
    ESP_LOGE(TAG, "Failed to create task: %d", ret);
    goto cleanup;
  }

  esp_gmf_pipeline_bind_task(pipe, work_task);
  ret = esp_gmf_pipeline_loading_jobs(pipe);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to load jobs: %d", ret);
    esp_gmf_task_deinit(work_task);
    goto cleanup;
  }

  ESP_LOGI(TAG, "[ 7 ] Setup event handler");
  EventGroupHandle_t pipe_sync_evt = xEventGroupCreate();
  if (!pipe_sync_evt)
  {
    ESP_LOGE(TAG, "Failed to create event group");
    esp_gmf_task_deinit(work_task);
    goto cleanup;
  }
  esp_gmf_pipeline_set_event(pipe, _pipeline_event, pipe_sync_evt);

  ESP_LOGI(TAG, "[ 8 ] Start playback");
  ESP_LOGI(TAG, "========================================");
  ESP_LOGI(TAG, "Playing: %s", DEFAULT_PLAY_URL);
  ESP_LOGI(TAG, "========================================");

  ret = esp_gmf_pipeline_run(pipe);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to start pipeline: %d", ret);
    vEventGroupDelete(pipe_sync_evt);
    esp_gmf_task_deinit(work_task);
    goto cleanup;
  }

  // Ensure codec is actively playing
  ESP_LOGI(TAG, "Verifying codec output is active...");
  vTaskDelay(pdMS_TO_TICKS(100));

  // Wait for completion
  ESP_LOGI(TAG, "[ 9 ] Wait for playback completion");
  EventBits_t bits = xEventGroupWaitBits(pipe_sync_evt, PIPELINE_BLOCK_BIT,
                                         pdTRUE, pdFALSE, portMAX_DELAY);

  if (bits & PIPELINE_BLOCK_BIT)
  {
    ESP_LOGI(TAG, "Playback stopped");
  }

  // Small delay before stopping to ensure last audio buffer plays
  vTaskDelay(pdMS_TO_TICKS(100));

  esp_gmf_pipeline_stop(pipe);

  ESP_LOGI(TAG, "[ 10 ] Cleanup");
  vEventGroupDelete(pipe_sync_evt);
  esp_gmf_task_deinit(work_task);

cleanup:
  if (pipe)
  {
    esp_gmf_pipeline_destroy(pipe);
  }
  gmf_loader_teardown_audio_effects_default(pool);
  gmf_loader_teardown_audio_codec_default(pool);
  gmf_loader_teardown_io_default(pool);
  esp_gmf_pool_deinit(pool);

  ESP_LOGI(TAG, "Playback complete");
}
