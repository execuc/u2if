/**
 * Pin Mappings
 * 
 * For now, pins are assigned for all interfaces, even if not routed. This
 * doesn't seem to cause any issues, and unused interfaces are not exposed
 * in Blinka.
 *
*/

//---------------------------------------------------------
// Feather 
//---------------------------------------------------------
#if defined (BOARD_FEATHER)
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

//---------------------------------------------------------
// QT Py
//---------------------------------------------------------
#elif defined (BOARD_QTPY)

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

//---------------------------------------------------------
// Itsy Bitsy
//---------------------------------------------------------
#elif defined (BOARD_ITSYBITSY)

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

//---------------------------------------------------------
// QT2040 Trinkey
//---------------------------------------------------------
#elif defined (BOARD_QT2040_TRINKEY)

  // I2C0
  #define U2IF_I2C0_SDA 16
  #define U2IF_I2C0_SCL 17
  // I2C1
  #define U2IF_I2C1_SDA 2
  #define U2IF_I2C1_SCL 3
  // SPI0
  #define U2IF_SPI0_CK 6
  #define U2IF_SPI0_MOSI 7
  #define U2IF_SPI0_MISO 8
  // SPI1
  #define U2IF_SPI1_CK 10
  #define U2IF_SPI1_MOSI 11
  #define U2IF_SPI1_MISO 12
  // UART0
  #define U2IF_UART0_TX 1
  #define U2IF_UART0_RX 2

//---------------------------------------------------------
// Pico (default)
//---------------------------------------------------------
#else

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