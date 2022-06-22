#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmixer.h"
#include "portaudio.h"


#define GLOBAL_SAMPLERATE 44100
#define FRAMES_PER_BUFFER 512
#define VOLUME_STEP 0.05

#define CLAMP(val,max,min) (val > max ? max : val < min ? min : val)

static double master_gain =0.5;

static int PlaySoundCallback(const void* input_buffer, void* output_buffer,
                            unsigned long frames_per_buffer,
                            const PaStreamCallbackTimeInfo* time_info,
                            PaStreamCallbackFlags status_flags,
                            void* usr_data){
    cm_Int16* out = output_buffer;
    cm_process(out,FRAMES_PER_BUFFER*2);
    return 0;
}

void PrintOptions(){
    printf("\n===============\nChoose option:\n0-Exit\n1-Play coin sound\n2-Increase volume\n3-Decrease volume\n4-Pause/Unpause music\n");
}

int main(int argc, char** argv){
    printf("Starting program...\n");
    printf("Initializing cmixer and sounds...\n");
    cm_Source *src,*src2,*src3;
    cm_init(GLOBAL_SAMPLERATE);
    cm_set_master_gain(0.5);
    /* Create source and play */
    src = cm_new_source_from_file("../sounds/coin.wav");
    src2 = cm_new_source_from_file("../sounds/drum_loop.wav");
    src3 = cm_new_source_from_file("../sounds/loop.wav");
    
    cm_set_loop(src2, 1);
    cm_set_loop(src3, 1);
    cm_set_gain(src2,0.6);
    cm_set_pitch(src2,1.05865);

    if (!src || !src2 || !src3) {
    fprintf(stderr, "Error: failed to create source: '%s'\n", cm_get_error());
    exit(EXIT_FAILURE);
    }

    printf("Initializing portaudio...\n");
    PaStream* stream;
    PaError err = Pa_Initialize();
    if (err != paNoError) printf("Something Went wrong!");

    err = Pa_OpenDefaultStream(&stream,0,2,paInt16,GLOBAL_SAMPLERATE,FRAMES_PER_BUFFER,PlaySoundCallback,NULL);
    if (err != paNoError) printf("Something Went wrong!");
    err = Pa_StartStream(stream);
    if (err != paNoError) printf("Something Went wrong!");

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
    printf("Closing Portaudio...\n");
    err = Pa_StopStream(stream);
    err = Pa_CloseStream(stream);
    err = Pa_Terminate();
    if (err != paNoError) printf("Something Went wrong!");
    return 0;

}