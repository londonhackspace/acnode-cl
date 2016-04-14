/*
 
 SD - a slightly more friendly wrapper for sdfatlib
 
 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.
 
 License: GNU General Public License V3
 (Because sdfatlib is licensed with this.)
 
 (C) Copyright 2010 SparkFun Electronics
 
 Adaptation for LaunchPad LM4F / TM4C
 Rei Vilo - May 20, 2014
 
 */

#ifndef __SD_H__
#define __SD_H__

// Core library - IDE-based
#if defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#elif defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(ARDUINO) // Arduino 1.0x and 1.5x specific
#include "Arduino.h"
#endif // end IDE

#include <utility/SdFat.h>
#include <utility/SdFatUtil.h>

#define FILE_READ O_READ
#define FILE_WRITEREAD (O_READ | O_WRITE | O_CREAT)
#define FILE_WRITE (O_WRITE | O_CREAT)
#define FILE_APPEND (O_WRITE | O_CREAT | O_APPEND)

class File : public Stream {
private:
    char _name[13];                                                             // our name
    SdFile *_file;                                                              // underlying file pointer
    
public:
    File(SdFile f, const char *name);                                           // wraps an underlying SdFile
    File(void);                                                                 // 'empty' constructor
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int read();
    virtual int peek();
    virtual int available();
    virtual void flush();
    int read(void *buf, uint16_t nbyte);
    boolean seek(uint32_t pos);
    uint32_t position();
    uint32_t size();
    void close();
    operator bool();
    char * name();
    
    boolean isDirectory(void);
    File openNextFile(uint8_t mode = O_RDONLY);
    void rewindDirectory(void);
    
    using Print::write;
};

class SDClass {
    
private:
    // These are required for initialisation and use of sdfatlib
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    
    // my quick&dirty iterator, should be replaced
    SdFile getParentDir(const char *filepath, int *indx);
public:
    // This needs to be called to set up the connection to the SD card
    // before other methods are used.
    //    boolean begin(uint8_t csPin = SD_CHIP_SELECT_PIN);
    
    ///
    /// @brief      Initialise the SD-card
    /// @param      chipSelectPin pin number for SD-card chip select
    /// @param      sckRateID SPI speed, optional, default=SPI_HALF_SPEED
    /// @param      SPI_Port SPI port number, optional, default=-1=none
    /// @param      cardDetectionPin card detection pin number, optional, default=-1=none
    /// @param		level expected level when SD-card available, default=LOW
    /// @return     true is success, false otherwise
    ///
    boolean begin(uint8_t chipSelectPin,
                  uint8_t sckRateID = SPI_HALF_SPEED,
                  int8_t SPI_Port = -1,
                  int8_t cardDetectionPin = -1,
                  int8_t level = LOW);
    
    ///
    /// @brief      SD-card hardware detection
    /// @return     true is SD-card inserted, false otherwise
    /// @note       Uses previously defined cardDetectionPin and level
    ///
    uint8_t detectSD();

    // Open the specified file/directory with the supplied mode (e.g. read or
    // write, etc). Returns a File object for interacting with the file.
    // Note that currently only one file can be open at a time.
    File open(const char *filename, uint8_t mode = FILE_READ);
    
    // Methods to determine if the requested file path exists.
    boolean exists(char *filepath);
    
    // Create the requested directory heirarchy--if intermediate directories
    // do not exist they will be created.
    boolean mkdir(char *filepath);
    
    // Delete the file.
    boolean remove(char *filepath);
    
    boolean rmdir(char *filepath);
    
    uint8_t getErrorCode() { return card.errorCode(); };
    
private:
    
    // This is used to determine the mode used to open a file
    // it's here because it's the easiest place to pass the
    // information through the directory walking function. But
    // it's probably not the best place for it.
    // It shouldn't be set directly--it is set via the parameters to `open`.
    int fileOpenMode;
    
    friend class File;
    friend boolean callback_openPath(SdFile&, char *, boolean, void *);
};

extern SDClass SD;

#endif
