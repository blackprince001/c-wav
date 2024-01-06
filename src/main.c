#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define byte char

typedef struct Wav_header {
  // Format should be chronologically the same as specified from documentation.
  // https://docs.fileformat.com/audio/wav/#wav-file-header for more info.
  byte riff[4];
  int32_t file_size;
  byte wave[4];
  byte fmt[4];
  int32_t chunk_size;
  int16_t format_tag;
  int16_t num_chans;
  int32_t sample_rate;
  int32_t bytes_per_second;
  int16_t bytes_per_sample;
  int16_t bits_per_sample;
  byte data[4];
  int32_t data_len;
} Wav_t;

#define SAMPLE_RATE 8000
#define MIDDLE_C 256.00

// You can change the constant value here but
// since you can't really do Constants with const in C
// You would to multiply value to Sample_rate for your Buffer_Size
#define DURATION_SECONDS 30

// BUFFER_SIZE = SAMPLE_RATE * DURATION_SECONDS
#define BUFFER_SIZE 240000

// Increase the amplitude of the sound generated to this height
// Always make sure this is within the size limit of short int
// which -32,767 to +32,767.
#define AMPLITUDE 32000

void initial_wave_headers(Wav_t *wav_header);
void generate_wav_file(const char *filename, Wav_t wav_header,
                       short int *buffer);

int main() {
  Wav_t wav_header;

  strncpy(wav_header.riff, "RIFF", 4);
  strncpy(wav_header.wave, "WAVE", 4);
  strncpy(wav_header.fmt, "fmt ", 4);
  strncpy(wav_header.data, "data", 4);

  initial_wave_headers(&wav_header);

  // Generating the wave progression for the wav file.
  short int buffer[BUFFER_SIZE] = {};

  // Fill the buffer with periodical crests and trough
  for (int i = 0; i < BUFFER_SIZE; ++i) {
    buffer[i] =
        (short int)(cos((2 * M_PI * MIDDLE_C * i) / SAMPLE_RATE) * AMPLITUDE);
  }

  generate_wav_file("test.wav", wav_header, buffer);

  return EXIT_SUCCESS;
}

void initial_wave_headers(Wav_t *wav_header) {
  wav_header->chunk_size = 16;
  wav_header->format_tag = 1;
  wav_header->num_chans = 1;

  wav_header->sample_rate = SAMPLE_RATE;

  wav_header->bits_per_sample = 16;
  wav_header->bytes_per_sample =
      (wav_header->bits_per_sample / 8) * wav_header->num_chans;

  wav_header->bytes_per_second =
      wav_header->sample_rate * wav_header->bytes_per_sample;

  wav_header->data_len = BUFFER_SIZE * wav_header->bytes_per_sample;
  wav_header->file_size = wav_header->data_len + 44;
}

void generate_wav_file(const char *filename, Wav_t wav_header,
                       short int *buffer) {
  FILE *fptr = fopen(filename, "w");

  fwrite(&wav_header, 1, sizeof(wav_header), fptr);
  fwrite(buffer, 2, BUFFER_SIZE, fptr);

  printf("Complete Writing to Wav file.");
}
