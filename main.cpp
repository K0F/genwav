#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

namespace little_endian_io
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;
using namespace std;


std::vector<float> loadFromTxt(const char *filename){

  std::ifstream instream(filename);
    float Y;
    std::string line;
    std::vector<float> tmp;
    int counter = 0;
    int frms = 0;
    while(std::getline(instream, line)){
      if(counter%3==0){
          Y = float(strtof(line.c_str(),NULL));
          tmp.push_back(Y);
        frms++;
      }
      counter++;
    }
    instream.close();


    return tmp;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}



int main(int argc, char** argv)
{


  std::vector<float> data;

  char * filename = getCmdOption(argv, argv + argc, "-i"); 
  char * filenameOut = getCmdOption(argv, argv + argc, "-o");

  data = loadFromTxt(filename);
  
  ofstream f( filenameOut, ios::binary );


  // Write the file headers
  f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
  write_word( f,     16, 4 );  // no extension data
  write_word( f,      1, 2 );  // PCM - integer samples
  write_word( f,      1, 2 );  // two channels (stereo file)
  write_word( f,  48000, 4 );  // samples per second (Hz)
  write_word( f,2304000, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
  write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  size_t data_chunk_pos = f.tellp();
  f << "data----";  // (chunk size to be filled in later)

  // Write the audio samples
  // (We'll generate a single C4 note with a sine wave, fading from left to right)
  constexpr double two_pi = 6.283185307179586476925286766559;
  constexpr double max_amplitude = 32760;  // "volume"

  double hz        = 48000;    // samples per second
  double frequency = 261.626;  // middle C
  double seconds   = 60;      // time

  int N = data.size();//hz * seconds;  // total number of samples
  for (int n = 0; n < N; n++)
  {
    double amplitude = (double)n / N * max_amplitude;
    //double value     = sin( (two_pi * n * (frequency+sin(n/hz*frequency/pow(log2(n+1),0.1)*two_pi)*frequency/16.01) ) / hz );
    double value = data[n]/1024.0-0.5;
    write_word( f, (int)(                 amplitude  * value), 2 );
    //write_word( f, (int)((max_amplitude - amplitude) * value), 2 );
  }

  // (We'll need the final file size to fix the chunk sizes above)
  size_t file_length = f.tellp();

  // Fix the data chunk header to contain the data size
  f.seekp( data_chunk_pos + 4 );
  write_word( f, file_length - data_chunk_pos + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  f.seekp( 0 + 4 );
  write_word( f, file_length - 8, 4 ); 
}
