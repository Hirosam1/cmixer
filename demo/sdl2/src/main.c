#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "cmixer.h"


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

  //cm_play(src);
  cm_play(src2);
  cm_play(src3);
  /* Choose option*/
  printf("Choose option:\n1-Exit\n2-Play Sound\n");
  int op = 2;
  char input;
  while(op > 1){
    int n = scanf(" %c", &input);
    //Consumes newline
    getchar();
    printf(">>%c: ",input);
    op = input - '0';
    switch (op)
    {  
    case 1:
      printf("Exiting!\n");
      break;
    case 2:
      printf("Play sound\n");
      cm_stop(src);
      cm_play(src);
      break;
    default:
      printf("Bad parameter!\n");
      break;
    }
  }
  /* Clean up */
  printf("Cleaning up...\n");
  cm_destroy_source(src);
  cm_destroy_source(src2);
  printf("Closing SDL...\n");
  SDL_CloseAudioDevice(dev);
  SDL_Quit();

  return EXIT_SUCCESS;
}
