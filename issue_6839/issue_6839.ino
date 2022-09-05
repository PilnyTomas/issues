#include "Arduino.h"

#include <stddef.h>
#include <stdint.h>

#include <WiFi.h>
#include "driver/i2s.h"

/*
 * CONSTANTS AND MACROS
 */

#define HOME_SSID                   "foo"
#define PASSWORD                    "bar"
#define SERVER_ADDRESS              "1.1.1.1"

#define SERVER_PORT                 8115U

#define I2S_DOUT                25
#define I2S_BCLK                27
#define I2S_LRC                 26
#define I2S_FREQ_HZ             44100U
#define I2S_GAIN_ADC            34U
#define I2S_GAIN_ADC_SAMPLES    20U
#if (USE_INTERNAL_DAC)
#define I2S_VOLUME_SHIFT        6U
#else
#define I2S_VOLUME_SHIFT        10U
#endif

/* Can be changed, if the internal DAC should be used in case of I2S */
#define USE_INTERNAL_DAC        0U

/*
 * TYPE DEFINITIONS
 */

/*
 * LOCAL DATA
 */

static WiFiClass WiFiConnection;
static WiFiClient tcpConnection;

/*
 * FUNCTION DECLARATIONS
 */

static int connectToServer( void );
static int getMp3( uint8_t *pBuffer, size_t sizeOfBuffer );
static int runWiFiServer( uint8_t *pBuffer, size_t sizeOfBuffer );
static void initI2s( void );
static void initWifi( void );

/*
 * CODE
 */

static int connectToServer( void )
{
    int ret = 0;
    IPAddress serverAddress;

    serverAddress.fromString( SERVER_ADDRESS );
    Serial.printf( "Try to connect to %s\n", serverAddress.toString( ).c_str( ) );
    ret = tcpConnection.connect( serverAddress, SERVER_PORT );
    if ( 1 != ret ) {
        Serial.printf( "Connect() failed with status: %d\n", ret );
    }
    return ret;
}

static int getMp3( uint8_t *pBuffer, size_t sizeOfBuffer )
{
    static uint32_t audioEntry = 0;
    uint8_t buffer[100] = { 0 };
    int ret = 0;
    uint32_t readSize = 0;
    static uint32_t sizeOfChunk = 0;
    uint32_t sizeLeft = 0;
    uint32_t sizeToRead = sizeOfBuffer;
    static uint32_t receivedSize = 0;

    if ( 0 == audioEntry ) {
        tcpConnection.write( (uint8_t *) &audioEntry, sizeof(audioEntry) );
        while ( 0 == tcpConnection.available( ) ) {
            delay( 0 );
        }
        ret = tcpConnection.read( buffer, sizeof(buffer) );
        Serial.printf( "Read %d bytes of header data\n", ret );
        audioEntry++;
        sizeOfChunk = (uint32_t) (buffer[3] << (3U * 8U)) | (uint32_t) (buffer[2] << (2U * 8U)) | (uint32_t) (buffer[1] << 8U) | buffer[0];
        Serial.printf( "SizeOfChunk: %d\n", sizeOfChunk );
        tcpConnection.write( (uint8_t *) &audioEntry, sizeof(audioEntry) );
        while ( 0 == tcpConnection.available( ) ) {
            delay( 0 );
        }
    }
    memset( pBuffer, 0, sizeOfBuffer );
    sizeLeft = sizeOfChunk - ((audioEntry - 1U) * sizeOfBuffer);
    if ( sizeLeft < sizeOfBuffer ) {
        sizeToRead = sizeLeft;
    }
    while ( sizeToRead != readSize ) {
        errno = 0;
        ret = tcpConnection.read( pBuffer + readSize, sizeOfBuffer - readSize );
        if ( 0 < ret ) {
            readSize += ret;
        }
        if ( 0 != errno ) {
            Serial.printf( "read() failed with errno %d\n", errno );
        }
        if ( !tcpConnection.connected( ) ) {
            Serial.printf( "Connection broke up\n" );
            break;
        }
    }

    receivedSize += readSize;
    audioEntry++;
    if ( receivedSize >= sizeOfChunk ) {
        audioEntry = 0;
        receivedSize = 0;
    }
    return readSize;
}

static int runWiFiServer( uint8_t *pBuffer, size_t sizeOfBuffer )
{
    int ret = -1;
    if ( !tcpConnection.connected( ) ) {
        while ( 0 == connectToServer( ) ) {
            delay(1000);
        }
    }
    ret = getMp3( pBuffer, sizeOfBuffer );
    return ret;
}

static void initI2s( void )
{
#if (!USE_INTERNAL_DAC)
    i2s_pin_config_t pins = {
            .bck_io_num = I2S_BCLK,
            .ws_io_num = I2S_LRC,                              //  wclk,
            .data_out_num = I2S_DOUT,
            .data_in_num = I2S_PIN_NO_CHANGE
    };
    i2s_config_t i2s_config = {
         .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#else
    i2s_config_t i2s_config = {
         .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_PDM),
#endif
         .sample_rate = I2S_FREQ_HZ,
         .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
         .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
         .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
         .dma_buf_count = 8,  // max buffers
         .dma_buf_len = 64, // max value
         .use_apll=0,
         .tx_desc_auto_clear= true,  // new in V1.0.1
         .fixed_mclk=-1
    };
    i2s_driver_install( (i2s_port_t) I2S_NUM_0, &i2s_config, 0, NULL );
#if (USE_INTERNAL_DAC)
    i2s_set_pin( (i2s_port_t) I2S_NUM_0, NULL );
#else
    i2s_set_pin( (i2s_port_t) I2S_NUM_0, &pins );
#endif
    i2s_zero_dma_buffer( I2S_NUM_0 );
}

static void initWifi( void )
{
    bool isConnected = false;

    while ( !isConnected ) {
        /* connect to the WiFi */
        if ( WiFiConnection.status( ) != WL_CONNECTED ) {
            WiFiConnection.disconnect( true );
            WiFiConnection.begin( HOME_SSID, PASSWORD );
        }

        for ( uint32_t i = 0; i < 5; i++ ) {
            delay( 100 );
            if ( WL_CONNECTED == WiFiConnection.status( ) ) {
                break;
            }
            Serial.println( ".\n" );
        }
        Serial.printf( "Connection result: %d\n", WiFi.waitForConnectResult( ) );

        if ( WiFiConnection.status( ) == WL_CONNECTED ) {
            delay( 3000 );
            WiFi.setAutoReconnect( true );
            isConnected = true;
        }
    }
}

//The setup function is called once at startup of the sketch
void setup()
{
    vTaskPrioritySet( NULL, 10 );
    Serial.begin( 115200 );
    initWifi( );
    initI2s( );

    uint32_t sizeOfBuffer = 5U * 1000U;
    uint16_t *pBuffer = (uint16_t *) malloc( sizeOfBuffer );
    size_t bytesWritten = 0;
    while(true)
    {
        runWiFiServer( (uint8_t *) pBuffer, sizeOfBuffer );
        for ( uint32_t i = 0; i < sizeOfBuffer/4; i++ ) {
            if ( ESP_OK != i2s_write( I2S_NUM_0, (const void*) &pBuffer[i*2], sizeof(uint16_t), &bytesWritten, 1000 ) ) {
                Serial.printf( "Error during i2s_write\n" );
            }
            if ( ESP_OK != i2s_write( I2S_NUM_0, (const void*) &pBuffer[i*2 + 1], sizeof(uint16_t), &bytesWritten, 1000 ) ) {
                Serial.printf( "Error during i2s_write\n" );
            }
        }
    }
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
}