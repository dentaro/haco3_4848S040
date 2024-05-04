Panel info

https://homeding.github.io/boards/esp32s3/panel-4848S040.htm

lovyanGFXの設定がそのままだと色反転したりしますので、

Panel_RGB.cppの
Panel_ST7701::getInitCommands
の設定を

0x20,  0,  // ノーマルにしてください。0x21 IPS用だと反転してしまいます。
0x3A,  1, 0x70, // こちらも、0x70 RGB888で24ビットにしてさい。0x60 RGB666, 0x50 RGB565だと色がおかしくなります。

