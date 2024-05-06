Panel info

https://homeding.github.io/boards/esp32s3/panel-4848S040.htm

lovyanGFXの設定がそのままだと色深度が合わず、色反転もしますので、

Panel_RGB.cppの
Panel_ST7701::getInitCommands
の設定243,244行目

0x20,  0,  // ノーマルにしてください。0x21 IPS用だと色反転してしまいます。
0x3A,  1, 0x70, // こちらも、0x70 RGB888で24ビットにしてさい。0x60 RGB666, 0x50 RGB565だと色がおかしくなります。


ST7701のデータシートはこちらです。

https://www.crystalfontz.com/controllers/datasheet-viewer.php?id=495
