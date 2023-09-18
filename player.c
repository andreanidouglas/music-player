#include "id3v2lib.h"

#define MINIAUDIO_IMPLEMENTATION
#include <assert.h>
#include <stdio.h>

#include "miniaudio.h"

typedef struct player {
  ma_decoder decoder;
  ma_device device;
} player;

void data_callback(ma_device *device, void *output, const void *input,
                   ma_uint32 frameCount) {
  ma_decoder *decoder = (ma_decoder *)device->pUserData;
  assert(decoder != NULL && "could not find decoder");

  ma_decoder_read_pcm_frames(decoder, output, frameCount, NULL);
  (void)input;
}

void init_player(player *player) {
  // ma_decoder_init_file(NULL, NULL, player->decoder);
  // assert(player->decoder != NULL && "COULD NOT INITIALIZE DECODER");

  ma_result result;
  ma_device_config config = ma_device_config_init(ma_device_type_playback);
  config.playback.format = ma_format_f32;
  config.playback.channels = 2;
  config.sampleRate = 44100;
  config.dataCallback = data_callback;
  config.pUserData = &player->decoder;

  result = ma_device_init(NULL, &config, &player->device);
  if (result != MA_SUCCESS) {
    fprintf(stderr, "could not intialize playback device");
    return;
  }
}

void destroy_player(player *player) {
  ma_device_uninit(&player->device);
  ma_decoder_uninit(&player->decoder);
}

int main(void) {
  char *file_name = "audio.mp3";
  ID3v2_Tag *tag = ID3v2_read_tag(file_name);

  if (tag == NULL) {
    fprintf(stderr, "could not read tag");
    return -1;
  }

  ID3v2_TextFrame *artist_frame = ID3v2_Tag_get_artist_frame(tag);
  ID3v2_TextFrame *title_frame = ID3v2_Tag_get_title_frame(tag);
  ID3v2_TextFrame *album_frame = ID3v2_Tag_get_album_frame(tag);
  printf("Artist: %s\nTitle: %s\nAlbum: %s\n", artist_frame->data->text,
         title_frame->data->text, album_frame->data->text);

  player player;
  init_player(&player);

  ma_result result = ma_decoder_init_file(file_name, NULL, &player.decoder);
  if (result != MA_SUCCESS) {
    fprintf(stderr, "could not initialize audio decoder");
    return -1;
  }

  result = ma_device_start(&player.device);
  if (result != MA_SUCCESS) {
    fprintf(stderr, "could not start player");
    return -2;
  }

  printf("Press any key to quit...");
  getchar();

  destroy_player(&player);
  return 0;
}
