/*
    Author: Robert Cofield, for GAVLab
*/
#ifndef NVS_H
#define NVS_H

#include <fstream>
#include <iostream>
#include <queue>

#include "nvs_structures.h"
#include <serial/serial.h> 

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/pthread/condition_variable_fwd.hpp>
#include <boost/algorithm/string.hpp>

// #include <boost/thread/thread.hpp>
// #include <boost/thread/locks.hpp>

#define PI 3.14159265

/* 
    Utility Functions
*/
double default_get_time();  // TODO reevaluate
void sleep_msecs(unsigned int);  // Sleep for specified milliseconds


/*
    Primary Class For interfacing with the receiver
*/
class NVS {

/*
    Primary Stuff
*/
public:
    NVS();
    ~NVS();

    bool Connect(std::string, int baudrate=115200);
    void Disconnect();

    bool IsConnected() {return is_connected_;}
    bool Ping(int num_attempts=5);

    void WaitForCommand();

/*
    Data-related stuff
*/
public:
    /*
        Data to send out to apps from receiver
    */
    int updRate;       // (Hz) rate at which receiver updates
    float rmsError;     // PORZD - planar
    bool dataIsValid;   
    uint satCountUsing; // Number of satellites used in current solution
    uint satCountView;  //Number of satellite in view
    float lat;
    float lon;
    float alt;          // mean-sea-level (geoid), meters [as per GGA]
    float sog;          // Speed over Ground (knots)
    float cog;          // Course over Ground (degrees True)
    float latErr;       // Latitude Error
    float lonErr;       // Longitude Error
    float altErr;       // Altitude Error
    float gpsTime;      // Time of day (24hr) associated with the gps fix measurement
    float gpsDate;      // day month year
    float pdop;         // position dilution of precision
    float hdop;         // horizontal dilution of precision
    float vdop;         // verticcal dilution of precsion


    /*
        background info pertaining to data from receiver
    */
    double read_timestamp_;         // time stamp when last serial port read completed
    std::string manufacturer_;
    std::string device_id_;
    std::string firmware_version_;
    int msg_rate_;                  // rate (Hz) at which data is sent out

    /*
        user attributes & output messaging
    */
    // void log_data(std::string);  // output takes in strings that represent data from the receiver
    bool display_log_data_;  // Whether to print data to terminal as it comes in


/*
    Private Functions
*/
private:
    void StartReading();
    void StopReading();    
    void ReadSerialPort();
    void BufferIncomingData(std::vector<std::string>);
    void DelegateParsing();

    bool wait_for_command_;             // Whether to continuously wait for user terminal input
    void ParseCommand(std::string);     // decide how to act on user terminal input

    /*
        send data to the receiver
    */
    bool SendMessage(std::string);
    bool SendMessage(const uint8_t*);
    bool SendMessage(const std::vector< uint8_t > &);
    void RequestNMEAMsgs();

    /*
        Parse Specific NMEA Messages
    */
    /* NMEA Standard Messages */
    void ParseGBS(std::string, std::string);
    void ParseGGA(std::string, std::string);
    void ParseGSA(std::string, std::string);
    void ParseGSV(std::string, std::string);
    void ParseRMC(std::string, std::string);
    /* NMEA Proprietary Messages */
    void ParseALVER(std::string);
    void ParsePORZD(std::string);



/*
 * Private Attributes
 */
private:
    boost::function<double()> time_handler_;

    /*
     *  Serial Port
     */
    // Number of milliseconds between bytes received to timeout on.
    const static uint32_t serial_inter_byte_timeout_ = 100;
    // A constant number of milliseconds to wait after calling read.
    const static uint32_t serial_read_timeout_constant_ = 1500;
    // A multiplier against the number of requested bytes to wait after calling read.
    const static uint32_t serial_read_timeout_multiplier_ = 0;
    // A constant number of milliseconds to wait after calling write.
    const static uint32_t serial_write_timeout_constant_ = 1000;
    // A multiplier against the number of requested bytes to wait after calling write.
    const static uint32_t serial_write_timeout_multiplier_ = 0;
    serial::Serial* serial_port_;
    bool is_connected_;

    boost::shared_ptr<boost::thread> read_thread_;

    const static uint max_buffer_size_ = 5000;  

    /* 
     *  Incoming data buffers
     */
    bool reading_status_;   // Set whether to iteratively read data from the receiver
    std::queue<std::string> data_buffer_;  // valid messages are stored here for parsing


};


#endif 