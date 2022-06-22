#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "cmixer.h"

#define CLAMP(val,max,min) (val > max ? max : val < min ? min : val)
#define VOLUME_STEP 0.05

static double master_gain =0.5;
static SDL_mutex* audio_mutex;

static void lock_handler(cm_Event *e) {
  if (e->type == CM_EVENT_LOCK) {
    SDL_LockMutex(audio_mutex);
  }
  if (e->type == CM_EVENT_UNLOCK) {
    SDL_UnlockMutex(audio_mutex);
  }
}


static void audio_callback(void *udata, Uint8 *stream, int size) {
  cm_process((void*) stream, size / 2);
}

void PrintOptions(){
    printf("\n===============\nChoose option:\n0-Exit\n1-Play coin sound\n2-Increase volume\n3-Decrease volume\n4-Pause/Unpause music\n");
}

int main(int argc, char **argv) {
  SDL_AudioDeviceID dev;
  SDL_AudioSpec fmt, got;
  cm_Source *src, *src2,*src3;

  /* Init SDL */
  SDL_Init(SDL_INIT_AUDIO);
  audio_mutex = SDL_CreateMutex();

  /* Init SDL audio */
  memset(&fmt, 0, sizeof(fmt));
  fmt.freq      = 44100;
  fmt.format    = AUDIO_S16;
  fmt.channels  = 2;
  fmt.samples   = 1024;
  fmt.callback  = audio_callback;

  dev = SDL_OpenAudioDevice(NULL, 0, &fmt, &got, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
  if (dev == 0) {
    fprintf(stderr, "Error: failed to open audio device '%s'\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  /* Init library */
  cm_init(got.freq);
  cm_set_lock(lock_handler);
  cm_set_master_gain(0.5);

  /* Start audio */
  SDL_PauseAudioDevice(dev, 0);

  /* Create source and play */
  src = cm_new_source_from_file("../sounds/coin.wav");
  src2 = cm_new_source_from_file("../sounds/drum_loop.wav");
  src3 = cm_new_source_from_file("../sounds/loop.wav");

  if (!src || !src2 || !src3) {
    fprintf(stderr, "Error: failed to create source: '%s'\n", cm_get_error());
    exit(EXIT_FAILURE);
  }
  cm_set_loop(src2, 1);
  cm_set_loop(src3, 1);
  cm_set_gain(src2,0.4);
  cm_set_pitch(src2,1.0587);
  cm_set_gain(src,0.8);

  cm_set_master_gain(master_gain);
  cm_play(src2);
  cm_play(src3);

  /* Choose option*/
  PrintOptions();
  int op = 2;
  char input;
  while(op > 0){
      int n = scanf(" %c", &input);
      //Consumes newline
      getchar();
      printf(">>%c: ",input);
      op = input - '0';
      switch (op)
      {  
      case 0:
          printf("Exiting!\n");
          break;
      case 1:
          printf("Play coin sound\n");
          cm_stop(src);
          cm_play(src);
          PrintOptions();
      break;
      case 2:
          master_gain-=VOLUME_STEP;
          cm_set_master_gain(master_gain = CLAMP(master_gain,1.0,0.0));
          printf("Decrease volume->%f\n",(float)master_gain);
          PrintOptions();
          break;
      case 3:
          master_gain+=VOLUME_STEP;
          cm_set_master_gain(master_gain = CLAMP(master_gain,1.0,0.0));
          printf("Increasing volume->%f\n",(float)master_gain);
          PrintOptions();
          break;
      case 4:
          if (cm_get_state(src2) == CM_STATE_PAUSED){
              cm_play(src2);
              cm_play(src3);
              printf("Unpause audio\n");
          }else{
              cm_pause(src2);
              cm_pause(src3);
              printf("Pause audio\n");
          }
          
          PrintOptions();
          break;
      default:
          printf("Bad parameter!\n");
          PrintOptions();
          break;
      }
  }
  /* Clean up */
  printf("Cleaning up...\n");
  cm_destroy_source(src);
  cm_destroy_source(src2);
  cm_destroy_source(src3);
  printf("Closing SDL...\n");
  SDL_CloseAudioDevice(dev);
  SDL_Quit();

  return EXIT_SUCCESS;
}
