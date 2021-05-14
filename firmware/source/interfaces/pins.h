#define BOARD_FEATHER

#if defined (BOARD_QTPY)
//---------------------------------------------------------
// QT Py
//---------------------------------------------------------
  // I2C0
  #define U2IF_I2C0_SDA 24
  #define U2IF_I2C0_SCL 25
  // I2C1
  #define U2IF_I2C1_SDA 22
  #define U2IF_I2C1_SCL 23
  // SPI0
  #define U2IF_SPI0_CK 6
  #define U2IF_SPI0_MOSI 3
  #define U2IF_SPI0_MISO 4
  // SPI1
  #define U2IF_SPI1_CK 10
  #define U2IF_SPI1_MOSI 11
  #define U2IF_SPI1_MISO 12
  // UART0
  #define U2IF_UART0_TX 20
  #define U2IF_UART0_RX 5

#elif defined (BOARD_ITSYBITSY)
//---------------------------------------------------------
// Itsy Bitsy
//---------------------------------------------------------
  // I2C0
  #define U2IF_I2C0_SDA 4
  #define U2IF_I2C0_SCL 5
  // I2C1
  #define U2IF_I2C1_SDA 2
  #define U2IF_I2C1_SCL 3
  // SPI0
  #define U2IF_SPI0_CK 18
  #define U2IF_SPI0_MOSI 19
  #define U2IF_SPI0_MISO 20
  // SPI1
  #define U2IF_SPI1_CK 10
  #define U2IF_SPI1_MOSI 11
  #define U2IF_SPI1_MISO 12
  // UART0
  #define U2IF_UART0_TX 0
  #define U2IF_UART0_RX 1

#elif defined (BOARD_FEATHER)
//---------------------------------------------------------
// Feather 
//---------------------------------------------------------
  // I2C0
  #define U2IF_I2C0_SDA 4
  #define U2IF_I2C0_SCL 5
  // I2C1
  #define U2IF_I2C1_SDA 2
  #define U2IF_I2C1_SCL 3
  // SPI0
  #define U2IF_SPI0_CK 18
  #define U2IF_SPI0_MOSI 19
  #define U2IF_SPI0_MISO 20
  // SPI1
  #define U2IF_SPI1_CK 10
  #define U2IF_SPI1_MOSI 11
  #define U2IF_SPI1_MISO 12
  // UART0
  #define U2IF_UART0_TX 0
  #define U2IF_UART0_RX 1

#elif defined (BOARD_PICO)
//---------------------------------------------------------
// Pico (default)
//---------------------------------------------------------
  // I2C0
  #define U2IF_I2C0_SDA 4
  #define U2IF_I2C0_SCL 5
  // I2C1
  #define U2IF_I2C1_SDA 14
  #define U2IF_I2C1_SCL 15
  // SPI0
  #define U2IF_SPI0_CK 18
  #define U2IF_SPI0_MOSI 19
  #define U2IF_SPI0_MISO 16
  // SPI1
  #define U2IF_SPI1_CK 10
  #define U2IF_SPI1_MOSI 11
  #define U2IF_SPI1_MISO 12
  // UART0
  #define U2IF_UART0_TX 0
  #define U2IF_UART0_RX 1

#endif