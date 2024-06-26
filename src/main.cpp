using namespace std;
#include <Arduino.h>
#include <FS.h>
#include "SPIFFS.h"

// #include <esp_now.h>
// #include <WiFi.h>
#include <map>

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>
#include "baseGame.h"
#include <chrono>
#include <time.h>
#include <fstream>
#include "runLuaGame.h"
#include <sstream>
#include <map>

#include "baseGame.h"
#include "runLuaGame.h"
#include <LovyanGFX_DentaroUI.hpp>

#include <LovyanGUI.hpp>
#include "Editor.h"


bool isSoftLED[LED_NUM];


//キーボード関連
Editor editor;

static lgui::LovyanGUI gui;             // GUIライブラリのインスタンス
static lgui::LGUI_TextBox textbox;      // テキストボックス
// static lgui::LGUI_GridView grid;        // グリッドビュー
// static std::vector<std::string> gridstrs(26*30); // グリッドの文字列保持用変数

/// 8bit unsigned 44.1kHz mono (exclude wav header)
// extern const uint8_t wav_unsigned_8bit_click[46000];
// extern const uint8_t wav_unsigned_8bit_click[16];

/// wav data (include wav header)
// extern const uint8_t wav_with_header[230432];
// extern const uint8_t wav_with_header[16];
bool textMoveF = false;
bool shiftF = false;

uint8_t sprno;
uint8_t repeatnum;

static int menu_x = 2;
static int menu_y = 20;
static int menu_w = 120;
static int menu_h = 30;
static int menu_padding = 36;

#define MAPWH 16//マップのpixelサイズ

#define KEYBOARD_DATA 32
#define KEYBOARD_CLK  33

#define TFT_RUN_MODE 0
#define TFT_EDIT_MODE 1
#define TFT_WIFI_MODE 2

#define WIDE_MODE 2 //倍率になっています。

//UI設定用
#define GAME_UI 0
#define QWERTY_UI 1
uint8_t UImodeNo = QWERTY_UI;

//星のデータ用
std::vector<std::array<float, 2>> bsParamFloat;
std::vector<std::array<uint8_t, 1>> bsParamInt8t;
std::vector<std::array<uint16_t, 1>> bsParamInt16t;

uint64_t frame = 0;

// ワールドマップ情報を読み込むためのファイルクラス
File wfr;

int isEditMode;
bool firstBootF = true;
bool difffileF = false;//前と違うファイルを開こうとしたときに立つフラグ

bool sfxflag = false;
uint8_t sfxNo = 0;
uint8_t wavNo = 0;
uint8_t sfxChNo = 0;
uint8_t sfxVol = 0;
float sfxspeed = 100;
uint8_t patternNo = 0;//0~63

uint8_t bgmodeNo = 0;
uint8_t pngimgW = 0;
uint8_t pngimgH = 0;
uint8_t pngimgX = 0;
uint8_t pngimgY = 0;
uint8_t pngimgTransCn = 0;
String pngimgPath = "";

uint8_t toolNo = 0;
int gameState = 0;

uint8_t sfxlistNo = 0;
uint8_t sfxnos[8] ={0,1,2,3,4,5,6,7};

uint8_t loopStart = 0;
uint8_t loopEnd = 63;
uint8_t looplen = (loopEnd - loopStart)+1;
float bpm = 120;

// // //音関連
uint8_t buffAreaNo = 0;
uint8_t gEfectNo = 0;
uint8_t effectVal = 0.0f;
uint8_t toneTickNo = 0;
uint8_t sfxTickNo = 0;
uint8_t instrument = 0;
uint8_t targetChannelNo = 0;//描画編集する効果音番号を設定（sfx(n)のnで効果音番号を指定することで作った効果音がなる）
uint8_t tickTime = 125;//125ms*8chはbpm60
uint8_t tickSpeed = 5;//連動してない

std::deque<int> buttonState;//ボタンの個数未定

enum struct FileType {
  LUA,
  JS,
  BMP,
  PNG,
  TXT,
  OTHER
};

//キーボード関連
// Editor editor;

char keychar;//キーボードから毎フレーム入ってくる文字

//esp-idfのライブラリを使う！
//https://qiita.com/norippy_i/items/0ed46e06427a1d574625
#include <driver/adc.h>//アナログボタンはこのヘッダファイルを忘れないように！！

using namespace std;

#define MAX_CHAR 512
#define FORMAT_SPIFFS_IF_FAILED true

// #define BUF_PNG_NUM 0

uint8_t mainVol = 180;

//outputmode最終描画の仕方
// int outputMode = FAST_MODE;//50FPS程度128*128 速いけど小さい画面　速度が必要なモード
int outputMode = WIDE_MODE;//20FPS程度240*240 遅いけれどタッチしやすい画面　パズルなど

uint8_t xpos, ypos = 0;
uint8_t colValR = 0;
uint8_t colValG = 0;
uint8_t colValB = 0;

uint8_t charSpritex = 0;
uint8_t charSpritey = 0;
int pressedBtnID = -1;//この値をタッチボタン、物理ボタンの両方から操作してbtnStateを間接的に操作している

// esp_now_peer_info_t slave;

int mapsprnos[16];

int HACO3_C0    = 0x0000;
int HACO3_C1    = 6474;//27,42,86 
int HACO3_C2    = 35018;
int HACO3_C3    = 1097;
int HACO3_C4    = 45669;
int HACO3_C5    = 25257;
int HACO3_C6    = 50712;
int HACO3_C7    = 65436;
int HACO3_C8    = 63496;//0xF802;
int HACO3_C9    = 64768;
int HACO3_C10   = 65376;
int HACO3_C11   = 1856;
int HACO3_C12   = 1407;
int HACO3_C13   = 33715;
int HACO3_C14   = 64341;
int HACO3_C15   = 65108;

uint8_t clist2[16][3] =
  {
  { 0,0,0},//0: 黒色
  { 27,42,86 },//1: 暗い青色
  { 137,24,84 },//2: 暗い紫色
  { 0,139,75 },//3: 暗い緑色
  { 183,76,45 },//4: 茶色
  { 97,87,78 },//5: 暗い灰色
  { 194,195,199 },//6: 明るい灰色
  { 255,241,231 },//7: 白色
  { 255,0,70 },//8: 赤色
  { 255,160,0 },//9: オレンジ
  { 255,238,0 },//10: 黄色
  { 0,234,0 },//11: 緑色
  { 0,173,255 },//12: 水色
  { 134,116,159 },//13: 藍色
  { 255,107,169 },//14: ピンク
  { 255,202,165}//15: 桃色
  };

//uiIDを変換する
int convUiId[15] = {
 5, 3, 6,
 1, 9, 2,
 7, 4, 8,
-1,-1, 0, 
10,11,12,
};

const uint8_t RGBValues[][3] PROGMEM = {//16bit用
  {0, 0, 0},     // 0: 黒色=なし
  {24, 40, 82},  // 1: 暗い青色
  {140, 24, 82}, // 2: 暗い紫色
  {0, 138, 74},  // 3: 暗い緑色
  {181, 77, 41}, // 4: 茶色 
  {99, 85, 74},  // 5: 暗い灰色
  {198, 195, 198}, // 6: 明るい灰色
  {255, 243, 231}, // 7: 白色
  {255, 0, 66},  // 8: 赤色
  {255, 162, 0}, // 9: オレンジ
  {255, 239, 0}, // 10: 黄色
  {0, 235, 0},   // 11: 緑色
  {0, 174, 255}, // 12: 水色
  {132, 117, 156}, // 13: 藍色
  {255, 105, 173}, // 14: ピンク
  {255, 203, 165}  // 15: 桃色
};

//2倍拡大表示用のパラメータ
float matrix_side[6] = {2.0,   // 横2倍
                     -0.0,  // 横傾き
                     258.0,   // X座標
                     0.0,   // 縦傾き
                     2.0,   // 縦2倍
                     0.0    // Y座標
                    };

LGFX screen;//LGFXを継承

LovyanGFX_DentaroUI ui(&screen);
// LGFX_Sprite tft2(&screen);//倍角表示用のスプライト
LGFX_Sprite tft(&screen);


LGFX_Sprite sprite88_roi = LGFX_Sprite(&tft);
LGFX_Sprite sprite11_roi = LGFX_Sprite(&tft);
LGFX_Sprite sprite64 = LGFX_Sprite();

std::vector<uint8_t> sprite64cnos_vector;
LGFX_Sprite buffSprite = LGFX_Sprite(&tft);
LGFX_Sprite sprite88_0 = LGFX_Sprite(&tft);
BaseGame* game;
String appfileName = "";//最初に実行されるアプリ名
String savedAppfileName = "";
uint8_t mapsx = 0;
uint8_t mapsy = 0;
int readmapno = 0;
int divnum = 1;
bool readMapF = false;
LGFX_Sprite spriteMap;//地図用スプライト

int gWx;
int gWy;
uint8_t mapArray[16][20];//配列はyを先にしている（配列をcsvで手書きしたときの方向を一致させて可読性をよくするため）

bool mapready = false;

int8_t sprbits[128];//fgetでアクセスするスプライト属性を格納するための配列

char buf[MAX_CHAR];
// char str[100];//情報表示用
int mode = 0;//記号モード //0はrun 1はexit
// int gameState = 0;
String appNameStr = "init";
int soundNo = -1;
float soundSpeed = 1.0;
int musicNo = -1;
bool musicflag = false;
// bool sfxflag = false;
bool toneflag = false;
bool firstLoopF = true;

float sliderval[2] = {0,0};
bool optionuiflag = false;

int addUiNum[4];
int allAddUiNum = 0;

int xtile = 0;
int ytile = 0;
float ztile = 0.0;

int xtileNo = 29100;
int ytileNo = 12909;

LGFX_Sprite sprref;
String oldKeys[BUF_PNG_NUM];

uint8_t masterVol = 64;

String mapFileName = "/init/map/0.png";
// getSign関数をMapDictionaryクラス外に移動
Vector2<int> getSign(int dirno) {
    if (dirno == -1) {
        // 方向を持たない場合、(0.0, 0.0, 0.0)を返す
        return {0, 0};
    } else {
        float dx = (dirno == 0 || dirno == 1 || dirno == 7) ? 1.0 : ((dirno == 3 || dirno == 4 || dirno == 5) ? -1.0 : 0.0);
        float dy = (dirno == 1 || dirno == 2 || dirno == 3) ? 1.0 : ((dirno == 5 || dirno == 6 || dirno == 7) ? -1.0 : 0.0);
        return {int(dx), int(dy)};
    }
}

int readMap()
{
  mapready = false;

  for(int n = 0; n<divnum; n++)
  {
    uint8_t* gIMGBuf2 = new uint8_t[4096];

    auto file = SPIFFS.open(mapFileName.c_str(), "r");
    file.read(gIMGBuf2, 32768/divnum);
    file.close();

    spriteMap.drawPng(gIMGBuf2, 32768/divnum, 0, (int32_t)(-MAPWH * n / divnum));

    delete[] gIMGBuf2;

  // auto file = SPIFFS.open(mapFileName.c_str(), "r");
  // file.read(gIMGBuf2, 32768);
  // file.close();

  // spriteMap.drawPng(gIMGBuf2,32768, 0, (int32_t)(-MAPWH*n/divnum));
    
    for(int32_t j = 0; j<MAPWH/divnum; j++){
      for(int32_t i = 0; i<MAPWH; i++){

        int k = j+(MAPWH/divnum)*(n);//マップ下部
        colValR = uint8_t(spriteMap.readPixelRGB(i,j).R8());
        colValG = uint8_t(spriteMap.readPixelRGB(i,j).G8());
        colValB = uint8_t(spriteMap.readPixelRGB(i,j).B8());

  //16ビットRGB（24ビットRGB）
        if(colValR==0&&colValG==0&&colValB==0){//0: 黒色=なし
          mapArray[i][k] = mapsprnos[0];//20;
        }else if(colValR==24&&colValG==40&&colValB==82){//{ 27,42,86 },//1: 暗い青色
          mapArray[i][k] = mapsprnos[1];//11;//5*8+5;
        }else if(colValR==140&&colValG==24&&colValB==82){//{ 137,24,84 },//2: 暗い紫色
          mapArray[i][k] = mapsprnos[2];//32;//5*8+5;
        }else if(colValR==0&&colValG==138&&colValB==74){//{ 0,139,75 },//3: 暗い緑色
          mapArray[i][k] = mapsprnos[3];//44;//5*8+5;
        }else if(colValR==181&&colValG==77&&colValB==41){//{ 183,76,45 },//4: 茶色 
          mapArray[i][k] = mapsprnos[4];//53;//5*8+5;
        }else if(colValR==99&&colValG==85&&colValB==74){//{ 97,87,78 },//5: 暗い灰色
          mapArray[i][k] = mapsprnos[5];//49;
        }else if(colValR==198&&colValG==195&&colValB==198){//{ 194,195,199 },//6: 明るい灰色
          mapArray[i][k] = mapsprnos[6];//54;//5*8+5;
        }else if(colValR==255&&colValG==243&&colValB==231){//{ 255,241,231 },//7: 白色
          mapArray[i][k] = mapsprnos[7];//32;
        }else if(colValR==255&&colValG==0&&colValB==66){//{ 255,0,70 },//8: 赤色
          mapArray[i][k] = mapsprnos[8];//52;
        }else if(colValR==255&&colValG==162&&colValB==0){//{ 255,160,0 },//9: オレンジ
          mapArray[i][k] = mapsprnos[9];//41;//5*8+5;
        }else if(colValR==255&&colValG==239&&colValB==0){//{ 255,238,0 },//10: 黄色
          mapArray[i][k] = mapsprnos[10];//46;
        }else if(colValR==0&&colValG==235&&colValB==0){//{ 0,234,0 },//11: 緑色
          mapArray[i][k] = mapsprnos[11];//42;
        }else if(colValR==0&&colValG==174&&colValB==255){//{ 0,173,255 },//12: 水色
          mapArray[i][k] = mapsprnos[12];//45;//5*8+5;
        }else if(colValR==132&&colValG==117&&colValB==156){//{ 134,116,159 },//13: 藍色
          mapArray[i][k] = mapsprnos[13];//50;
        }else if(colValR==255&&colValG==105&&colValB==173){//{ 255,107,169 },//14: ピンク
          mapArray[i][k] = mapsprnos[14];//43;//5*8+5;
        }else if(colValR==255&&colValG==203&&colValB==165){//{ 255,202,165}//15: 桃色
          mapArray[i][k] = mapsprnos[15];//38;//5*8+5;
        }
        if(i==MAPWH-1 && k==MAPWH-1){mapready = true;return 1;}//読み込み終わったら1をリターン
      }
    }
  }
  // spriteMap.deleteSprite();//メモリに格納したら解放する
  return 1;
}

// 送信コールバック
// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   char macStr[18];
//   snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
//            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//   tft.print("Last Packet Sent to: ");
//   tft.println(macStr);
//   tft.print("Last Packet Send Status: ");
//   tft.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

// 受信コールバック
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  char msg[1];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // tft.printf("Last Packet Recv from: %s\n", macStr);//MACアドレスを表示させる
  tft.printf("Last Packet Recv Data(%d): ", data_len);
  for ( int i = 0 ; i < data_len ; i++ ) {
    msg[1] = data[i];
    tft.print(msg[1]);
  }
  tft.println("");
}

Vector2<int> getKey2Sign(String _currentKey, String _targetKey) {
    int slashPos = _currentKey.indexOf('/'); // '/'の位置を取得
    if (slashPos != -1) { // '/'が見つかった場合
        String numA_str = _currentKey.substring(0, slashPos); // '/'より前の部分を取得
        String numB_str = _currentKey.substring(slashPos + 1); // '/'より後の部分を取得
        int numA_current = numA_str.toInt(); // 数字に変換
        int numB_current = numB_str.toInt(); // 数字に変換
        
        slashPos = _targetKey.indexOf('/'); // '/'の位置を取得
        if (slashPos != -1) { // '/'が見つかった場合
            numA_str = _targetKey.substring(0, slashPos); // '/'より前の部分を取得
            numB_str = _targetKey.substring(slashPos + 1); // '/'より後の部分を取得
            int numA_target = numA_str.toInt(); // 数字に変換
            int numB_target = numB_str.toInt(); // 数字に変換
            
            int dx = numA_target - numA_current;
            int dy = numB_target - numB_current;
            
            return {dx, dy};
        }
    }
    
    return {0, 0}; // デフォルトの値
}

Vector3<float> currentgpos = {0,0,0};;
Vector3<float> prePos= {0.0, 0.0, 0.0};
Vector3<float> currentPos = {0,0,0};
Vector3<float> diffPos = {0.0,0.0,0.0};

int dirNos[9];
int shouldNo = 0;
int downloadLimitNum = 0;
String targetKey = "";
float tileZoom = 15.0;
float bairitu = 1.0;

std::vector<String> temporaryKeys;
std::vector<String> previousKeys;
std::vector<String> writableKeys;
std::vector<String> downloadKeys;
std::vector<String> predownloadKeys;
std::vector<String> allKeys;
std::vector<String> preallKeys;

void printDownloadKeys() {
  Serial.println("Download Keys:");
  for (const auto& key : downloadKeys) {
      Serial.print(key);
  }
  Serial.println("");
}

void drawUI()
{
  // fillRoundRectの最適化
  uint16_t x1 = 0;
  uint16_t y1 = 80;
  uint16_t bw = 48;
  uint16_t bh = 38;
  uint16_t cornerRadius = 5;
  uint16_t color = TFT_DARKGRAY;

  screen.setTextColor(TFT_DARKGRAY, TFT_BLACK);
  screen.setTextSize(2);//サイズ
  // screen.setFont(&lgfxJapanGothicP_8);//日本語可
  // screen.setTextWrap(true);
  // // screen.setClipRect(160, 0, 60, 128);

  // if(UImodeNo == GAME_UI){
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 3; i++) {
          if(convUiId[j*3+i] != -1){
            screen.drawRoundRect(x1 + bw * i, y1 + bh * j, bw, bh, cornerRadius, color);
            screen.setCursor(x1 + bw * i +8, y1 + bh * j +8);
            screen.print(convUiId[j*3+i]);
          }
        }
    }
  // }
  // else if(UImodeNo == QWERTY_UI){
  //   for (int j = 0; j < 6; j++) {
  //       for (int i = 0; i < 10; i++) {
  //         if(convUiId1[j*10+i] != ' '){
  //           screen.drawRoundRect(x1 + bw * i, y1 + bh * j, bw, bh, cornerRadius, color);
  //           screen.setCursor(x1 + bw * i +8, y1 + bh * j +8);
  //           screen.print(convUiId1[j*10+i]);
  //         }
  //       }
  //   }
  // }

  // screen.fillRoundRect(x1, 124, x2 + 30, 38, cornerRadius, TFT_LIGHTGRAY);

  // screen.fillRoundRect(x1, 164, x2, 38, cornerRadius, TFT_LIGHTGRAY);
  // screen.fillRoundRect(x1 + 30, 164, x2, 38, cornerRadius, TFT_LIGHTGRAY);

  // screen.fillRoundRect(x1, 204, x2 + 30, 38, cornerRadius, TFT_LIGHTGRAY);

  // スプライトの解放
  // logoscreen.deleteSprite();

}

void reboot()
{
  ESP.restart();
}

FileType detectFileType(String *appfileName)
{
  if(appfileName->endsWith(".js")){
    return FileType::JS;
  }else if(appfileName->endsWith(".lua")){
    return FileType::LUA;
  }else if(appfileName->endsWith(".bmp")){
    return FileType::BMP;
  }else if(appfileName->endsWith(".png")){
    return FileType::PNG;
  }else if(appfileName->endsWith(".txt")){
    return FileType::TXT;
  }
  return FileType::OTHER;
}

#include "runLuaGame.h"

String *targetfileName;
BaseGame* nextGameObject(String* _appfileName, int _gameState, String _mn)
{

  switch(detectFileType(_appfileName)){
    case FileType::JS:  
      // game = new RunJsGame(); 
      break;
    case FileType::LUA: 
      game = new runLuaGame(_gameState, _mn);
      break;
    case FileType::TXT: 
      // game = new RunJsGame(); 
      // //ファイル名がもし/init/param/caldata.txtなら
      // if(*_appfileName == CALIBRATION_FILE)
      // {
      //   ui.calibrationRun(tft);//キャリブレーション実行してcaldata.txtファイルを更新して
      //   drawLogo();//サイドボタンを書き直して
      // }
      // appfileName = "/init/txt/main.js";//txtエディタで開く
      break; //txteditorを立ち上げてtxtを開く
    case FileType::BMP: // todo: error
      game = NULL;
      break;
    case FileType::PNG: // todo: error
      // game = new RunJsGame(); 
      // appfileName = "/init/png/main.js";//pngエディタで開く
      break;
    case FileType::OTHER: // todo: error
      game = NULL;
      break;
  }

  return game;

}

// char *A;

uint32_t preTime;
// void setFileName(String s){
//   appfileName = s;
// }

void runFileName(String s){
  
  ui.setConstantGetF(false);//初期化処理 タッチポイントの常時取得を切る
  
  appfileName = s;
  mode = 1;//exit to run

}

int getcno2tftc(uint8_t _cno){
  switch (_cno)
  {
  case 0:return HACO3_C0;break;
  case 1:return HACO3_C1;break;
  case 2:return HACO3_C2;break;
  case 3:return HACO3_C3;break;
  case 4:return HACO3_C4;break;
  case 5:return HACO3_C5;break;
  case 6:return HACO3_C6;break;
  case 7:return HACO3_C7;break;
  case 8:return HACO3_C8;break;
  case 9:return HACO3_C9;break;
  case 10:return HACO3_C10;break;
  case 11:return HACO3_C11;break;
  case 12:return HACO3_C12;break;
  case 13:return HACO3_C13;break;
  case 14:return HACO3_C14;break;
  case 15:return HACO3_C15;break;

  default:
  return HACO3_C0;
    break;
  }
}

// タイマー
hw_timer_t * timer = NULL;

void readFile(fs::FS &fs, const char * path) {
   File file = fs.open(path);
   while(file.available()) file.read();
  //  while(file.available()) Serial.print(file.read());
}

//ファイル書き込み
void writeFile(fs::FS &fs, const char * path, const char * message){
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        return;
    }
    file.print(message);
}

void deleteFile(fs::FS &fs, const char * path){
   Serial.print("Deleting file: ");
   Serial.println(path);
   if(fs.remove(path)) Serial.print("− file deleted\n\r");
   else { Serial.print("− delete failed\n\r"); }
}

void listDir(fs::FS &fs){
   File root = fs.open("/");
   File file = root.openNextFile();
   while(file){
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.print(file.size());
      file = root.openNextFile();
   }
}


uint16_t gethaco3Col(uint8_t haco3ColNo) {
    uint16_t result = ((static_cast<uint16_t>(clist2[haco3ColNo][0]) >> 3) << 11) |
                      ((static_cast<uint16_t>(clist2[haco3ColNo][1]) >> 2) << 5) |
                       (static_cast<uint16_t>(clist2[haco3ColNo][2]) >> 3);
    return result;
}

vector<string> split(string& input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    return result;
}

String rFirstAppName(String _wrfile){
  File fr = SPIFFS.open(_wrfile.c_str(), "r");// ⑩ファイルを読み込みモードで開く
  String _readStr = fr.readStringUntil('\n');// ⑪改行まで１行読み出し
  fr.close();	// ⑫	ファイルを閉じる
  return _readStr;
}

void decompressData(const char *filename, std::vector<std::vector<uint8_t>> &output) {

}


int readMap(String _mapFileName, int startCol, int startRow) {

  return 1;
}

void getOpenConfig()
{
  File fr;

  fr = SPIFFS.open(SPRBITS_FILE, "r");
  for (int i = 0; i < 128; i++) {
    String _readStr = fr.readStringUntil(','); // ,まで１つ読み出し
    std::string _readstr = _readStr.c_str();

    // 改行を取り除く処理
    const char CR = '\r';
    const char LF = '\n';
    std::string destStr;
    for (std::string::const_iterator it = _readstr.begin(); it != _readstr.end(); ++it) {
      if (*it != CR && *it != LF && *it != '\0') {
        destStr += *it;
      }
    }

    _readstr = destStr;

    uint8_t bdata = 0b00000000;
    uint8_t bitfilter = 0b10000000; // 書き換え対象ビット指定用

    for (int j = 0; j < _readstr.length(); ++j) {
        char ch = _readstr[j];
        // Serial.print(ch);
        if (ch == '1') {
            bdata |= bitfilter; // 状態を重ね合わせて合成
        }
        bitfilter = bitfilter >> 1; // 書き換え対象ビットを一つずらす
    }

    sprbits[i] = bdata;

  }
  fr.close();

  fr = SPIFFS.open("/init/param/openconfig.txt", "r");
  String line;
  while (fr.available()) {
    line = fr.readStringUntil('\n');
    if (!line.isEmpty()) {
      int commaIndex = line.indexOf(',');
        String val = line.substring(0, commaIndex);
        if(val != NULL){
          appfileName =  val;

          // Serial.print(appfileName.c_str());
          // Serial.println("<-");

        }else {
          appfileName = "/init/main.lua";//configファイルが壊れていても強制的に値を入れて立ち上げる
        }
          int nextCommaIndex = line.indexOf(',', commaIndex + 1);//一つ先のカンマ区切りの値に進める
          if (nextCommaIndex != -1) {
            val = line.substring(commaIndex + 1, nextCommaIndex);
            if(val.toInt() != NULL){
              isEditMode = val.toInt();
              Serial.print("editmode[");Serial.print(isEditMode);Serial.println("]");
            }else{
              isEditMode = 0;//configファイルが壊れていても強制的に値を入れて立ち上げる
            }
          }
    }
  }
  fr.close();

  Serial.print(appfileName.c_str());
  Serial.println("<---");

  string str1 = appfileName.c_str();
  int i=0;
  char delimiter = '/';

  std::vector<std::string> result = split(str1, delimiter);

    // 分割結果の表示
    for (const std::string& s : result) {
        if(i==1){
        appNameStr = s.c_str();

        // Serial.print("/" + appNameStr + "/mapinfo.txt");
        // Serial.println("<-------");

        fr = SPIFFS.open("/" + appNameStr + "/mapinfo.txt", "r");// ⑩ファイルを読み込みモードで開く
      }
      i++;
    }
      
  // アプリで使うマップ名を取得する
  String _readStr;
  while (fr.available()) {
      String line = fr.readStringUntil('\n');
      int j = 0; // 列のインデックス
      int startIndex = 0;

      if (!line.isEmpty()) {
          // Serial.print(line);
          // Serial.println("<--------");

          while (j < 16) {
              int commaIndex = line.indexOf(',', startIndex);

              // if (j < 16) { // 0から15番目まで
                  if (commaIndex != -1) {
                      String columnValue = line.substring(startIndex, commaIndex);
                      mapsprnos[j] = atoi(columnValue.c_str());
                  } else {
                      // 行の末尾まで達した場合
                      mapsprnos[j] = atoi(line.substring(startIndex).c_str());
                  }
              if (commaIndex == -1) {
                  // 行の末尾まで達した場合
                  break;
              }

              startIndex = commaIndex + 1;
              j++;
          }

          i++;
      }
  }
  fr.close();
  mapFileName = "/init/param/map/"+_readStr;
}

void setOpenConfig(String fileName, int _isEditMode) {
  char numStr[64];//64文字まで
  sprintf(numStr, "%s,%d,", 
    fileName.c_str(), _isEditMode
  );

  // Serial.println(fileName.c_str());
  // Serial.println(_isEditMode);

  String writeStr = numStr;  // 書き込み文字列を設定
  File fw = SPIFFS.open("/init/param/openconfig.txt", "w"); // ファイルを書き込みモードで開く
  fw.println(writeStr);  // ファイルに書き込み
  // savedAppfileName = fileName;
  delay(50);
  fw.close(); // ファイルを閉じる
}

void setTFTedit(int _iseditmode)
{
  tft.setPsram( false );//DMA利用のためPSRAMは切る
  tft.createSprite( TFT_WIDTH, TFT_HEIGHT );//PSRAMを使わないギリギリ
  tft.startWrite();//CSアサート開始

  // tft2.setPsram( false );//DMA利用のためPSRAMは切る
  // tft2.createSprite( TFT_WIDTH, TFT_HEIGHT );//PSRAMを使わないギリギリ
  // tft2.startWrite();//CSアサート開始

  // if(_iseditmode ==TFT_RUN_MODE){
  //   tft.setPsram( false );//DMA利用のためPSRAMは切る
  //   tft.createSprite( TFT_WIDTH, TFT_HEIGHT );//PSRAMを使わないギリギリ
  //   tft.startWrite();//CSアサート開始
  // }else if(_iseditmode == TFT_EDIT_MODE){
  //   tft.setPsram( false );//DMA利用のためPSRAMは切る
  //   tft.createSprite( TFT_WIDTH, TFT_HEIGHT );
  //   tft.startWrite();//CSアサート開始
  // }
  // else if(_iseditmode == TFT_WIFI_MODE){
  //   tft.setPsram( false );//DMA利用のためPSRAMは切る
  //   tft.createSprite( TFT_WIDTH, TFT_HEIGHT );
  //   tft.startWrite();//CSアサート開始
  // }
}

void createAbsUI(){
  //抽象UIを生成
  File fr = SPIFFS.open("/init/param/uiinfo.txt", "r");
  String line;

  while (fr.available()) {
    line = fr.readStringUntil('\n');
    if (!line.isEmpty()) {
      int commaIndex = line.indexOf(',');
      if (commaIndex != -1) {
        String val = line.substring(0, commaIndex);
        addUiNum[0] = val.toInt();

        if(addUiNum[0]!=-1){//-1の時は生成しない

          for (int i = 1; i < 6; i++) {
            int nextCommaIndex = line.indexOf(',', commaIndex + 1);
            if (nextCommaIndex != -1) {
              val = line.substring(commaIndex + 1, nextCommaIndex);
              addUiNum[i] = val.toInt();
              commaIndex = nextCommaIndex;
            }
          }
          ui.createPanel( addUiNum[0], addUiNum[1], addUiNum[2], addUiNum[3], addUiNum[4], addUiNum[5], TOUCH, ui.getTouchZoom());//ホームボタン
          allAddUiNum++;
        }
      }
    }
  }
  fr.close();
}


int btn(int btnno)
{
  return buttonState[btnno];//ボタンの個数未定
}

void reboot(String _fileName, int _isEditMode)
{
  setOpenConfig(_fileName, _isEditMode);
  editor.setCursorConfig();//カーソルの位置を保存
  delay(100);
  ESP.restart();
}

void restart(String _fileName, int _isEditMode)
{
  setOpenConfig(_fileName, _isEditMode);
  
  editor.setCursorConfig();//カーソルの位置を保存
  delay(100);

  firstBootF = false;
  setup();

  // tunes.pause();
  game->pause();
  free(game);
  firstLoopF = true;
  toneflag = false;
  sfxflag = false;
  musicflag = false;
  // txtName = _fileName;
  game = nextGameObject(&_fileName, gameState, mapFileName);//ファイルの種類を判別して適したゲームオブジェクトを生成
  game->init();//resume()（再開処理）を呼び出し、ゲームで利用する関数などを準備
  // tunes.resume();
}


void broadchat() {
  // if ("/init/chat/m.txt" == NULL) return;
  // File fp = SPIFFS.open("/init/chat/m.txt", FILE_READ); // SPIFFSからファイルを読み込み

  // if (!fp) {
  //   // editor.editorSetStatusMessage("Failed to open file");
  //   return;
  // }

  // std::vector<uint8_t> data;
  // while (fp.available()) {
  //   char c = fp.read();
  //   data.push_back(c);
  //   if (data.size() >= 150) {
  //     esp_err_t result = esp_now_send(slave.peer_addr, data.data(), data.size());
  //     data.clear(); // データを送信したらクリア
  //     if (result != ESP_OK) {
  //       // editor.editorSetStatusMessage("Failed to send message");
  //       fp.close();
  //       return;
  //     }
  //   }
  // }

  // // ファイルの残りのデータを送信
  // if (data.size() > 0) {
  //   esp_err_t result = esp_now_send(slave.peer_addr, data.data(), data.size());
  //   if (result != ESP_OK) {
  //     // editor.editorSetStatusMessage("Failed to send message");
  //     fp.close();
  //     return;
  //   }
  // }

  // fp.close();
  // // editor.editorSetStatusMessage("Message sent");
}


uint8_t readpixel(int i, int j)
{
        // int k = j+(MAPWH/divnum)*(n);//マップ下部
        colValR = sprite64.readPixelRGB(i,j).R8();
        colValG = sprite64.readPixelRGB(i,j).G8();
        colValB = sprite64.readPixelRGB(i,j).B8();

  //16ビットRGB（24ビットRGB）
        if(colValR==0&&colValG==0&&colValB==0){//0: 黒色=なし
          return 0;//20;
        }else if(colValR==24&&colValG==40&&colValB==82){//{ 27,42,86 },//1: 暗い青色
          return 1;//11;//5*8+5;
        }else if(colValR==140&&colValG==24&&colValB==82){//{ 137,24,84 },//2: 暗い紫色
          return 2;//32;//5*8+5;
        }else if(colValR==0&&colValG==138&&colValB==74){//{ 0,139,75 },//3: 暗い緑色
          return 3;//44;//5*8+5;
        }else if(colValR==181&&colValG==77&&colValB==41){//{ 183,76,45 },//4: 茶色 
          return 4;//53;//5*8+5;
        }else if(colValR==99&&colValG==85&&colValB==74){//{ 97,87,78 },//5: 暗い灰色
          return 5;//49;
        }else if(colValR==198&&colValG==195&&colValB==198){//{ 194,195,199 },//6: 明るい灰色
          return 6;//54;//5*8+5;
        }else if(colValR==255&&colValG==243&&colValB==231){//{ 255,241,231 },//7: 白色
          return 7;//32;
        }else if(colValR==255&&colValG==0&&colValB==66){//{ 255,0,70 },//8: 赤色
          return 8;//52;
        }else if(colValR==255&&colValG==162&&colValB==0){//{ 255,160,0 },//9: オレンジ
          return 9;//41;//5*8+5;
        }else if(colValR==255&&colValG==239&&colValB==0){//{ 255,238,0 },//10: 黄色
          return 10;//46;
        }else if(colValR==0&&colValG==235&&colValB==0){//{ 0,234,0 },//11: 緑色
          return 11;//42;
        }else if(colValR==0&&colValG==174&&colValB==255){//{ 0,173,255 },//12: 水色
          return 12;//45;//5*8+5;
        }else if(colValR==132&&colValG==117&&colValB==156){//{ 134,116,159 },//13: 藍色
          return 13;//50;
        }else if(colValR==255&&colValG==105&&colValB==173){//{ 255,107,169 },//14: ピンク
          return 14;//43;//5*8+5;
        }else if(colValR==255&&colValG==203&&colValB==165){//{ 255,202,165}//15: 桃色
          return 15;//38;//5*8+5;
        }
}


// static void tone_up(bool holding)
// {
//   static int tone_hz;
//   if (!holding) { tone_hz = 100; }
//   speaker.tone(++tone_hz, 1000, 1);
// }

// static void bgm_play_stop(bool holding = false)
// {
  // if (holding) { return; }
  // if (speaker.isPlaying(0))
  // {
  //   speaker.stop(0);
  // }
  // else
  // {
  //   speaker.playWav(wav_with_header, sizeof(wav_with_header), ~0u, 0, true);
  // }
// }

// static void m_volume_up(bool)
// {
//   int v = speaker.getVolume() + 1;
//   if (v < 256) { speaker.setVolume(v); }
// }

// static void m_volume_down(bool)
// {
//   int v = speaker.getVolume() - 1;
//   if (v >= 0) { speaker.setVolume(v); }
// }

// static void c_volume_up(bool)
// {
//   int v = speaker.getChannelVolume(0) + 1;
//   if (v < 256) { speaker.setChannelVolume(0, v); }
// }

// static void c_volume_down(bool)
// {
//   int v = speaker.getChannelVolume(0) - 1;
//   if (v >= 0) { speaker.setChannelVolume(0, v); }
// }

// struct menu_item_t
// {
//   const char* title;
//   void (*func)(bool);
// };

// static const menu_item_t menus[] =
// {
//   { "tone"      , tone_up       },
//   { "play/stop" , bgm_play_stop },
//   { "ms vol u"  , m_volume_up   },
//   { "ms vol d"  , m_volume_down },
//   { "ch vol u"  , c_volume_up   },
//   { "ch vol d"  , c_volume_down },
// };
// static constexpr const size_t menu_count = sizeof(menus) / sizeof(menus[0]);

size_t cursor_index = 0;


void safeReboot(){
  editor.setCursorConfig(0,0,0);//カーソルの位置を強制リセット保存
      delay(50);

      ui.setConstantGetF(false);//初期化処理 タッチポイントの常時取得を切る
      appfileName = "/init/main.lua";
      
      firstLoopF = true;
      toneflag = false;
      sfxflag = false;
      musicflag = false;

      editor.editorSave(SPIFFS);//SPIFFSに保存
      delay(100);//ちょっと待つ
      reboot(appfileName, TFT_RUN_MODE);//現状rebootしないと初期化が完全にできない
}
void lguisetup(){

  gui.colorDepth = 8;
  gui.setup(&screen);

/*
  gui.colorMask = 0xFF00FFU;
  gui.colorFrameInactive    = 0x999999U;
  gui.colorFrameActive      = 0x1F7FFFU;
  gui.colorCursorInactive   = 0xCCCCCCU;
  gui.colorCursorActive     = 0x7F7FFFU;
  gui.colorSubCursorInactive= 0xEEEEEEU;
  gui.colorSubCursorActive  = 0xBBBBFFU;
  gui.colorBorder           = 0x000000U;
  gui.colorFixed            = 0xBFBFBFU;
  gui.colorBack             = 0xFFFFFFU;
*/
/*/
  canvas.setColorDepth(4);
  canvas.createPalette();

  gui.colorMask = 0;
  gui.colorFrameInactive    = 1;   canvas.setPaletteColor(1, 0x999999U);
  gui.colorFrameActive      = 2;   canvas.setPaletteColor(2, 0x1F7FFFU);
  gui.colorCursorInactive   = 3;   canvas.setPaletteColor(3, 0x888888U);
  gui.colorCursorActive     = 4;   canvas.setPaletteColor(4, 0x005FFFU);
  gui.colorSubCursorInactive= 5;   canvas.setPaletteColor(5, 0x777777U);
  gui.colorSubCursorActive  = 6;   canvas.setPaletteColor(6, 0x6666AAU);
  gui.colorBorder = 12;   canvas.setPaletteColor(12, 0x000000U);
  gui.colorFixed  = 13;   canvas.setPaletteColor(13, 0x555555U);
  gui.colorBack   = 14;   canvas.setPaletteColor(14, 0x333333U);
  gui.smoothMove = 32;           // スムーズ移動係数 (0でスムーズ移動無効)

//*/
  gui.smoothMove = 0;

  // スクリーンキーボードの設定
  auto osk = gui.getKeyboard();
  
  osk->frameWidth = 0;
  osk->setFont(&fonts::Font0);
  auto osk_hight = osk->getDestRect().height();

  textbox.setDestRect(0, screen.height()-osk_hight-16, screen.width(), 16);
  textbox.setHideRect(0, screen.height()-osk_hight-16, 0, 16);
  textbox.hide();

//osk.setup(0, screen.height()/2-64, screen.width(), 64);
//osk.input(lgui::input_none);

  gui.addControl(&textbox);
  // gui.addControl(&grid);
}

void ledSetup()
{
for(int n=0; n<LED_NUM; n++)
  {
    isSoftLED[n] = false;
  }
}
void ledUpdate(){
  for(int n=0; n<LED_NUM; n++)
  {
    if(isSoftLED[n]==true){
      screen.fillCircle(8+19*n,16,8,TFT_RED);
      screen.fillCircle(4+19*n,12,2,TFT_WHITE);
    }else{
      screen.fillCircle(8+19*n,16,8,TFT_DARKGRAY);
    }
    screen.drawCircle(8+19*n,16,8,TFT_LIGHTGRAY);
  }
}

void setup()
{
  
  ledSetup();
  // pinMode(OUTPIN_0, OUTPUT);
  // pinMode(INPIN_0, INPUT);
  Serial.begin(115200);

  if(firstBootF == true){
    difffileF = false;

    #if !defined(__MIPSEL__)
      while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
      #endif
      Serial.println("Keyboard Start");

    if (!SPIFFS.begin(true))
    {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  }

  editor.getCursorConfig("/init/param/editor.txt");//エディタカーソルの位置をよみこむ
  delay(50);
  
  getOpenConfig();//最初に立ち上げるゲームのパスとモードをSPIFFSのファイルopenconfig.txtから読み込む
  //この時点でappfileNameも更新される

  if(isEditMode == TFT_RUN_MODE){

    if(firstBootF == false){
      tft.deleteSprite();
      delay(100);
    }
    setTFTedit(TFT_RUN_MODE);
    ui.begin( screen, 16, 0, 0);//タッチキャリブレーションをしない

    screen.setBrightness(255);
    screen.fillScreen(TFT_BLACK);
    // drawUI();//UIを表示
    sprite88_0.setPsram(false );
    sprite88_0.setColorDepth(16);//子スプライトの色深度
    sprite88_0.createSprite(8, 8);//ゲーム画面用スプライトメモリ確保

    sprite64.setPsram(false );
    sprite64.setColorDepth(16);//子スプライトの色深度
    sprite64.createSprite(PNG_SPRITE_WIDTH, PNG_SPRITE_HEIGHT);//ゲーム画面用スプライトメモリ確保//wroomだと64*128だとメモリオーバーしちゃう問題を色番号配列にして回避した

    sprite64.drawPngFile(SPIFFS, "/init/initspr.png", 0, 0);//一時展開する

    sprite64cnos_vector.clear();//初期化処理

    //色番号配列化
    for(int y = 0; y < PNG_SPRITE_HEIGHT; y++) {
        for(int x = 0; x < PNG_SPRITE_WIDTH; x++) {
          if(x%2 == 0){
            uint8_t pixel_data = (readpixel(x, y) << 4) | (readpixel(x + 1, y) & 0b00001111);//ニコイチにして格納
            sprite64cnos_vector.push_back(pixel_data);
          }
        }
    }

    //破棄
    sprite64.deleteSprite();

    //psram使えない-------------------------------------------
    // buffscreen.setPsram( true );
    // buffscreen.setColorDepth(16);//子スプライトの色深度
    // buffscreen.createSprite(256, 256);//ゲーム画面用スプライトメモリ確保

    // for( int i = 0; i < BUF_PNG_NUM; i++ ){
    //   mapTileSprites[i].setPsram(true);
    //   mapTileSprites[i].setColorDepth(16);
    //   mapTileSprites[i].createSprite(256,256);
    //   // MapTile クラスをインスタンス化し、スプライトに描画して返す
    //   dict.copy2buff(buffSprite, &mapTileSprites[i], i);
    // }

    // //キーと紐づけ、初期設定のキー0~9と値のペアを適当に登録しておく
    // for(int j = 0; j<3; j++){
    //   for(int i = 0; i<3; i++){
    //     dict.setSprptr(i*3+j, &mapTileSprites[i]);
    //     dict.setNewKey(i*3+j, String(xtileNo+i) + "/" + String(ytileNo+j));
    //     dict.showKeyInfo(String(xtileNo+i) + "/" + String(ytileNo+j));
    //   }
    // }
    //psram使えない-------------------------------------------

    sprite88_roi.setPsram(false );
    sprite88_roi.setColorDepth(16);//子スプライトの色深度
    sprite88_roi.createSprite(8, 8);//ゲーム画面用スプライトメモリ確保

    sprite11_roi.setPsram(false );
    sprite11_roi.setColorDepth(16);//子スプライトの色深度
    sprite11_roi.createSprite(1, 1);//ゲーム画面用スプライトメモリ確保

    spriteMap.setPsram(false );
    spriteMap.setColorDepth(16);//子スプライトの色深度
    spriteMap.createSprite(MAPWH, MAPWH/divnum);//マップ展開用スプライトメモリ確保
    
    if(firstBootF == true)
    {
      createAbsUI();//外部ファイルから、タッチボタンを作る処理
  
    // ui.createSliders( 0, 160, 240, 160, 2, 1, XY_VAL, MULTI_EVENT );
    // ui.setBtnName( ui.getUiID("SLIDER_0"), "2DSlider0" );
    // ui.setBtnName( ui.getUiID("SLIDER_0")+1, "2DSlider1" );


//  File fr;
 

//       //アプリのパスからアプリ名を取得
//   string str1 = appfileName.c_str();
//   int i=0;

//   for (string s : split(str1,'/')) {
//     if(i==1){
//       appNameStr = s.c_str();
//       fr = SPIFFS.open("/" + appNameStr + "/mapinfo.txt", "r");// ⑩ファイルを読み込みモードで開く
//     }
//      i++;
//   }

//   for(int i= 0;i<16;i++){//マップを描くときに使うスプライト番号リストを読み込む
//     String _readStr = fr.readStringUntil(',');// ⑪,まで１つ読み出し
//     mapsprnos[i] = atoi(_readStr.c_str());
//   }

//   String _readStr = fr.readStringUntil(',');// 最後はマップのパス
//   mapFileName = "/init/param/map/"+_readStr;
//   fr.close();	// ⑫	ファイルを閉じる


      // mapFileName = "/init/param/map/0.csv";
      // readMap(mapFileName);
      // delay(50);

      game = nextGameObject(&appfileName, gameState, mapFileName);//ホームゲームを立ち上げる（オブジェクト生成している）
      game->init();//（オブジェクト生成している）
      // tunes.init();//（オブジェクト生成している）
    }

    frame=0;
    }
    else if(isEditMode == TFT_EDIT_MODE)//エディットモードの時
    {
      if(firstBootF == false){
        tft.deleteSprite();
        delay(10);
      }
      setTFTedit(TFT_EDIT_MODE);
      
      ui.begin( screen, 16, 0, 0);

      if(firstBootF == true)
      {

        if (SPIFFS.exists(appfileName)) {
          File file = SPIFFS.open(appfileName, FILE_READ);
          if (!file) {
            Serial.println("ファイルを開けませんでした");
            return;
          }
          // ファイルからデータを読み込み、シリアルモニターに出力
          while (file.available()) {
            Serial.write(file.read());
          }
          // ファイルを閉じる
          file.close();
        }

      createAbsUI();
      game = nextGameObject(&appfileName, gameState, mapFileName);//ホームゲームを立ち上げる（オブジェクト生成している）
      game->init();//（オブジェクト生成している）
      // tunes.init();//（オブジェクト生成している）

      frame=0;

      editor.initEditor(tft);
      editor.readFile(SPIFFS, appfileName.c_str());
      editor.editorOpen(SPIFFS, appfileName.c_str());
      editor.editorSetStatusMessage("Press ESCAPE to save file");
    }
  }
  else if(isEditMode == TFT_WIFI_MODE)
  {
    if(firstBootF == false){
      tft.deleteSprite();
      delay(100);
    }
    setTFTedit(TFT_WIFI_MODE);

    ui.begin( screen, 16, 0, 1);

    if(firstBootF == true)
    {
      tft.setTextSize(1);//サイズ
      tft.setFont(&lgfxJapanGothicP_8);//日本語可
      tft.setCursor(0, 0);//位置
      tft.setTextWrap(true);
      tft.println("BOOT:WIFI_MODE");

      if (SPIFFS.exists(appfileName)) {
        File file = SPIFFS.open(appfileName, FILE_READ);
        if (!file) {
          Serial.println("ファイルを開けませんでした");
          return;
        }
        // ファイルからデータを読み込み、シリアルモニターに出力
        while (file.available()) {
          Serial.write(file.read());
        }
        // ファイルを閉じる
        file.close();
      }

      // createAbsUI();//外部ファイルから、タッチボタンを作る処理

      game = nextGameObject(&appfileName, gameState, mapFileName);//ホームゲームを立ち上げる（オブジェクト生成している）
      game->init();//（オブジェクト生成している）
      // tunes.init();//（オブジェクト生成している）

      frame=0;

      //editor.initEditor(tft, EDITOR_ROWS, EDITOR_COLS);
      // editor.initEditor(tft);
      // editor.readFile(SPIFFS, "/init/chat/m.txt");
      // editor.editorOpen(SPIFFS, "/init/chat/m.txt");
      // editor.editorSetStatusMessage("Press ESCAPE to save file");

      // ESP-NOW初期化
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();

      // if (esp_now_init() == ESP_OK) {
      //   tft.println("ESPNow Init Success");
      // } else {
      //   tft.println("ESPNow Init Failed");
      //   ESP.restart();
      // }

      // マルチキャスト用Slave登録
      // memset(&slave, 0, sizeof(slave));
      // for (int i = 0; i < 6; ++i) {
      //   slave.peer_addr[i] = (uint8_t)0xff;
      // }
      
      // esp_err_t addStatus = esp_now_add_peer(&slave);
      // if (addStatus == ESP_OK) {
      //   // Pair success
      //   tft.println("Pair success");
      // }
      // // ESP-NOWコールバック登録
      // esp_now_register_send_cb(OnDataSent);
      // esp_now_register_recv_cb(OnDataRecv);

    }

  }
  savedAppfileName = appfileName;//起動したゲームのパスを取得しておく
  firstBootF = false;

  // screen.setCursor(0,0);
  // screen.fillScreen(TFT_RED);
  // screen.setTextSize(1);//サイズ
  // screen.setFont(&lgfxJapanGothicP_8);//日本語可
  // // screen.setTextWrap(true);
  // // screen.setClipRect(160, 0, 60, 128);
  // screen.println("HELP");
  // screen.println("qで音量をあげる");
  // screen.println("aで音量をさげる");

  
  // begin(cfg);

  // { /// I2S Custom configurations are available if you desire.
  //   auto spk_cfg = speaker.config();

  //   if (spk_cfg.use_dac || spk_cfg.buzzer)
  //   {
  //   /// Increasing the sample_rate will improve the sound quality instead of increasing the CPU load.
  //     spk_cfg.sample_rate = 64000; // default:64000 (64kHz)  e.g. 48000 , 50000 , 80000 , 96000 , 100000 , 128000 , 144000 , 192000 , 200000
  //   }

  //   speaker.config(spk_cfg);
  // }

  // speaker.begin();

  // //  The setVolume function can be set the master volume in the range of 0-255. (default : 64)
  // speaker.setVolume(255);

  // /// The setAllChannelVolume function can be set the all virtual channel volume in the range of 0-255. (default : 255)
  // speaker.setAllChannelVolume(255);

  // /// The setChannelVolume function can be set the specified virtual channel volume in the range of 0-255. (default : 255)
  // speaker.setChannelVolume(0, 255);

  // /// play do Hz tone sound, 100 msec. 
  // speaker.tone(2000, 100,1);

  // delay(100);

  // /// play mi Hz tone sound, 100 msec. 
  // speaker.tone(1000, 100,2);

  // delay(100);

  
  // /// stop output sound.
  // speaker.stop();

  // delay(500);

  // speaker.playRaw( wav_unsigned_8bit_click, sizeof(wav_unsigned_8bit_click) / sizeof(wav_unsigned_8bit_click[0]), 44100, false);

  // while (speaker.isPlaying()) { delay(1); } // Wait for the output to finish.

  // delay(500);

  // // The 2nd argument of the tone function can be used to specify the output time (milliseconds).
  // speaker.tone(440, 1000);  // 440Hz sound  output for 1 seconds.

  // while (speaker.isPlaying()) { delay(1); } // Wait for the output to finish.

  // delay(500);

  // speaker.setVolume(0);
  // speaker.tone(880);  // tone 880Hz sound output. (Keeps output until it stops.)
  // for (int i = 0; i <= 151; i++)
  // {
  //   speaker.setVolume(i); // Volume can be changed during sound output.
  //   delay(25);
  // }
  // speaker.stop();  // stop sound output.

  // delay(500);

  // //---------------------------------------------

  // // The tone function can specify a virtual channel number as its 3rd argument.
  // // If the tone function is used on the same channel number, the previous tone will be stopped and a new tone will be played.
  // speaker.tone(261.626, 1000, 1);  // tone 261.626Hz  output for 1 seconds, use channel 1
  // delay(200);
  // speaker.tone(329.628, 1000, 1);  // tone 329.628Hz  output for 1 seconds, use channel 1
  // delay(200);
  // speaker.tone(391.995, 1000, 1);  // tone 391.995Hz  output for 1 seconds, use channel 1

  // while (speaker.isPlaying()) { delay(1); } // Wait for the output to finish.

  // delay(500);

  // // By specifying different channels, multiple sounds can be output simultaneously.
  // speaker.tone(261.626, 1000, 1);  // tone 261.626Hz  output for 1 seconds, use channel 1
  // delay(200);
  // speaker.tone(329.628, 1000, 2);  // tone 329.628Hz  output for 1 seconds, use channel 2
  // delay(200);
  // speaker.tone(391.995, 1000, 3);  // tone 391.995Hz  output for 1 seconds, use channel 3

  // while (speaker.isPlaying()) { delay(1); } // Wait for the output to finish.

  // delay(500);

  /// tone data (8bit unsigned wav)
  // const uint8_t wavdata[64] = { 132,138,143,154,151,139,138,140,144,147,147,147,151,159,184,194,203,222,228,227,210,202,197,181,172,169,177,178,172,151,141,131,107,96,87,77,73,66,42,28,17,10,15,25,55,68,76,82,80,74,61,66,79,107,109,103,81,73,86,94,99,112,121,129 };

  // /// Using a single wave of data, you can change the tone.
  // speaker.tone(261.626, 1000, 1, true, wavdata, sizeof(wavdata));
  // delay(200);
  // speaker.tone(329.628, 1000, 2, true, wavdata, sizeof(wavdata));
  // delay(200);
  // speaker.tone(391.995, 1000, 3, true, wavdata, sizeof(wavdata));
  // delay(200);

  // while (speaker.isPlaying()) { delay(1); } // Wait for the output to finish.

  // bgm_play_stop(true);


  //キー取得
  //luaプログラムがバグで起動不能になった場合、ESCを押しながらリセットをかけると、メニューに戻れるようにする
  //ESCボタンで強制終了

  // if (keyboard.available()) {
  //   keychar = keyboard.read();
  //   if (keychar == PS2_ESC) {
  //     safeReboot();
  //   }
  // }

  drawUI();//UIを表示

  // screen.setTextSize((std::max(screen.width(), screen.height()/2) + 255) >> 8);

  // if(ui.getCalF()){
  //   // タッチが使用可能な場合のキャリブレーションを行います。（省略可）
  //   if (screen.touch())
  //   {
  //     ui.touchCalibration(screen);//タッチキャリブレーションを実行
  //     // ui.showSavedCalData(screen);//タッチキャリブレーションの値を表示
  //   }
  // }  

  lguisetup();
  textbox.showKeyboard();
}

float rad = 0;
float delta = 0.05;

uint8_t r, g, b;

int dx = 0, dy = 0;

void drawSpriteParrot(int pdx, int pdy, uint8_t r, uint8_t g, uint8_t b) {
    screen.setColor(screen.color332(255, 255, 255));
    screen.drawBezier(225 + pdx, 70 + pdy, 200 + pdx, 80 + pdy, 225 + pdx, 150 + pdy);
    screen.drawBezier(225 + pdx, 70 + pdy, 250 + pdx, 80 + pdy, 225 + pdx, 150 + pdy);
    // 左下の点のみ動かさない
    screen.drawBezier(40, 240, 160 + pdx, 210 + pdy, 110 + pdx, 40 + pdy, 220 + pdx, 40 + pdy);
    screen.drawBezier(220 + pdx, 40 + pdy, 280 + pdx, 40 + pdy, 300 + pdx, 120 + pdy);
    // 右下の点のみ動かさない
    screen.drawBezier(300 + pdx, 120 + pdy, 310 + pdx, 180 + pdy, 270 + pdx, 200 + pdy, 290, 240);
    screen.fillEllipse(260 + pdx, 80 + pdy, 10, 15);
    screen.fillEllipse(190 + pdx, 80 + pdy, 10, 15);
}

uint32_t cnt = ~0;
int fps=60;//デフォルト
bool btnpF = false;
bool prebtnpF = false;
int btnptick = 0;
int prebtnptick = 0;
int btnpms = 0;

unsigned long startTime = millis();

char gkey = ' ';

void loop()
{

  gui.loop();

  auto osk = gui.getKeyboard();
  gkey = osk->getKeyCode();
  keychar = gkey;

  // //通常の文字
  // if(keychar != 0){
  //   editor.editorProcessKeypress(keychar, SPIFFS);
  // }
  
  //--------------------------
  
  // 現在の時間を取得する
  uint32_t currentTime = millis();
  // 前フレーム処理後からの経過時間を計算する
  uint32_t elapsedTime = currentTime - startTime;
  // 前フレームからの経過時間を計算する
  uint32_t remainTime = (currentTime - preTime);
  preTime = currentTime;

//--------------------------

  ui.update(screen);//タッチイベントを取るので、LGFXが基底クラスでないといけない
  
  if( ui.getEvent() != NO_EVENT ){//何かイベントがあれば

  drawUI();//UIを表示

  //ボタンが押されているときだけtickがカウントされる
  int firstwaitms = 1000;
    
    // if(btnpms == 0){
    //   btnpF = true; 
    //   // btnptick++;
    //   // textMoveF=true;
    //   // if(btnpms==0){btnpF = true; btnptick++;}//最初の0だけtrue
    //   // else{btnpF = false;pressedBtnID=-1;}

    // }else{

      if(btnpms <= 150)
      {
        btnpF = true;
        textMoveF=true;
      }else{

        if(btnpms%300 >= 150)
        {
          // Serial.println("定期的にtrue");
          btnpF = false;
          
        }else{
          btnpF = true;
        }

        if(btnpF!= prebtnpF)btnptick++;

        if (btnptick!=prebtnptick) {
          
          if(btnptick<=1||btnptick>=5)
          textMoveF=true;
          else
          textMoveF=false;

        }else{
          textMoveF=false;
        }
      }

    btnpms += elapsedTime;
    prebtnpF = btnpF;
    prebtnptick = btnptick;

  //ボタンが押されているときだけtickがカウントされる
  //btnpms//    0123456...
  //btnpF//     |||||___|||||____|||||____|||||____ //一定時間ずつフラグを立てる
  //textMoveF// |____________|___|____|___|____|    //差があった時にtrueになる最初firstwaitms分はフラグたてない

    if(textMoveF)//どのモードでもbtnpに反応する
    {
      ledUpdate();
    }
    
    if( ui.getEvent() == TOUCH ){//TOUCHの時だけ

      if(pressedBtnID != -1){buttonState[pressedBtnID] = -1;}
        // pressedBtnID = -1;//リセット   
    }
    
    if(ui.getEvent() == MOVE){
      if(ui.getTouchBtnID() == -1){
        pressedBtnID = -1;
      }else{
        if(ui.getTouchBtnID()<15)
        pressedBtnID = convUiId[ui.getTouchBtnID()];//convUiIdを使って実際押されたボタンとゲームに送るボタン番号をマップ変換する(最初の25個だけ
        else
        pressedBtnID = ui.getTouchBtnID();
      }
      
    }
    if( ui.getEvent() == RELEASE ){//RELEASEの時だけ
      // ui.setBtnID(-1);//タッチボタンIDをリセット
      // pressedBtnID = ui.getTouchBtnID()+12;//12個分の物理ボタンをタッチボタンIDに足す
      textMoveF=false;
      btnptick = 0;
      btnpms = 0;
      pressedBtnID = -1;//リセット

    }
    
  }

  


      // 経過時間が1/30秒以上経過した場合
if (elapsedTime >= 1000/fps||fps==-1) {


  if( isEditMode == TFT_RUN_MODE ){

    //ゲーム内のprint時の文字設定をしておく
    tft.setTextSize(1);//サイズ
    tft.setFont(&lgfxJapanGothicP_8);//日本語可
    tft.setCursor(0, 0);//位置
    tft.setTextWrap(true);

    // == tune task ==
    // tunes.run();

    // == game task ==
    mode = game->run(remainTime);//exitは1が返ってくる　mode=１ 次のゲームを起動

    if(pressedBtnID == 12){//RUN<-->EDIT切り替え
      
      reboot(appfileName, TFT_EDIT_MODE);//現状rebootしないと初期化が完全にできない
    }


    //ESCボタンで強制終了
    if (pressedBtnID == 0)
    { // reload

      editor.setCursorConfig(0,0,0);//カーソルの位置を保存
      delay(50);

      appfileName = "/init/main.lua";

      patternNo = 0;//音楽開始位置を0にリセット

      firstLoopF = true;
      toneflag = false;
      sfxflag = false;
      musicflag = false;

      mode = 1;//exit
      // 星用のベクター配列使用後は要素数を0にする
      bsParamFloat.resize(0);
      bsParamInt8t.resize(0);
      bsParamInt16t.resize(0);
    }

    if (pressedBtnID == 9999)
    { // reload
      mode = 1;//exit
      pressedBtnID = -1;
    }

    if(mode != 0){ // exit request//次のゲームを立ち上げるフラグ値、「modeが１＝次のゲームを起動」であれば
      // tunes.pause();
      game->pause();
      // ui.clearAddBtns();//個別のゲーム内で追加したタッチボタンを消去する
      free(game);
      firstLoopF = true;
      toneflag = false;
      sfxflag = false;
      musicflag = false;
      fps = 60;
      bgmodeNo = 0;//bgなしモードにリセット
      // txtName = appfileName;
      game = nextGameObject(&appfileName, gameState, mapFileName);//ファイルの種類を判別して適したゲームオブジェクトを生成
      game->init();//resume()（再開処理）を呼び出し、ゲームで利用する関数などを準備
      // tunes.resume();
    }
    

    if(bgmodeNo==2)
    {
      //  tft.drawPngFile(SPIFFS, pngimgPath, pngimgX, pngimgY, 160,128,0,0, 160/pngimgW, 128/pngimgH);
      tft.drawPngFile(SPIFFS, pngimgPath, pngimgX, pngimgY,0,0, 0,0);//tftに直接展開する
      // tft.drawPngFile(SPIFFS, pngimgPath, pngimgX, pngimgY, 0,0,0,0, 160/float(pngimgW), 128/float(pngimgH));//tftに直接展開する
    }

    // ui.showTouchEventInfo( tft, 0, 100 );//タッチイベントを視覚化する
    // if(patternNo<10)tft.fillRect(200,120,16,8,TFT_BLACK);
    ui.showInfo( tft, 200, 112 );//ボタン情報、フレームレート情報などを表示します。
    // tft.setCursor(200,120);
    // tft.println(patternNo);

      // if(toolNo != 0){
      //   if(toolNo==1){//カラーパレット
      //     for(int j = 0; j<8; j++){
      //       for(int i = 0; i<2; i++){
      //         tft.fillRect(i*16,j*16,16,16,gethaco3Col(j*2+i));
      //       }
      //     }
      //   }

      //   toolNo = 0;
      // }

  tft.setTextSize(1);
  tft.setFont(&lgfxJapanGothicP_16);
  tft.setTextColor( TFT_WHITE ,TFT_BLACK );
  tft.setCursor( 32,32 );

    // tft.print(gui.getKey());

    // auto osk = gui.getKeyboard();
    tft.print(gkey);

//三次ベジェ曲線： 引数の x y のペアが4つ
    // dx = cos(rad * 3.14) * 60 - 60;
    // dy = sin(rad * 3.14) * -20;
    // delay(1);
    // rad += delta;
    // if (2 < rad) {
    //     rad = 0;
    // }

    // drawSpriteParrot(dx, dy, 255, 255, 255);


    tft.setPivot(0, 0);
    tft.pushRotateZoom(&screen, TFT_POSX , TFT_POSY  , 0, 2, 2);
    // tft.pushRotateZoom(&screen, 0 , 0  , 0, 2, 2);
    

    // tft.pushSprite(&tft2, 0 , 0);//最大1.2倍までしか描画できない//PSRAMを使わないギリギリ
    // tft2.pushRotateZoom(&screen, 0 , 0  , 0, 2, 2);
    
    
    // tft.pushRotateZoom(&screen, 240 , 0  , 0, 2, 2);//最大1.2倍までしか描画できない//PSRAMを使わないギリギリ
    
    // }
    // else if(outputMode == FAST_MODE){
    //   tft.pushSprite(&screen,TFT_OFFSET_X,TFT_OFFSET_Y);//ゲーム画面を小さく高速描画する
    // }

    // if(pressedBtnID == 5){//PAGEUP//キーボードからエディタ再起動
    //   restart(appfileName, 1);//appmodeでリスタートかけるので、いらないかも
    // }

  }
  else if(isEditMode == TFT_EDIT_MODE)
  {
    
    if(textMoveF)
    // if(btnpF)
      {
               if (pressedBtnID == 1) {
          keychar = PS2_LEFTARROW;editor.editorMoveCursor(keychar);
        } else if (pressedBtnID == 2) {
          keychar = PS2_RIGHTARROW;editor.editorMoveCursor(keychar);
        } else if (pressedBtnID == 3) {
          keychar = PS2_UPARROW;editor.editorMoveCursor(keychar);
        } else if (pressedBtnID == 4) {
          keychar = PS2_DOWNARROW;editor.editorMoveCursor(keychar);
        }

        // ledUpdate();

      }else{
        //通常の文字
        if(keychar != 0){
          if(keychar == 0x08){//BSバックスペースのコードが来たら
            keychar = PS2_DELETE;
            editor.editorProcessKeypress(keychar, SPIFFS);
          }else{//通常文字のコードが来たら
            editor.editorProcessKeypress(keychar, SPIFFS);
          }
        }
      }
    
    
    
    editor.editorRefreshScreen(tft);

    float codeunit = 128.0/float(editor.getNumRows());
    float codelen = codeunit * 14;//14は表示行数
    // int codelen = int(codelen_f + 0.5); // 四捨五入して整数に変換する
    
    float curpos = codeunit*editor.getCy();
    float codepos = codeunit * (editor.getCy() - editor.getScreenRow());
    // int codepos = int(codepos_f + 0.5); // 四捨五入して整数に変換する
    
    if(!textMoveF)
    tft.fillRect(156,0, 4,128, HACO3_C5);//コードの全体の長さを表示
    else
    tft.fillRect(156,0, 4,128, HACO3_C9);//コードの全体の長さを表示

    if(!shiftF)
    tft.fillRect(156,int(codepos), 4,codelen, HACO3_C6);//コードの位置と範囲を表示
    else
    tft.fillRect(156,int(codepos), 4,codelen, HACO3_C12);//コードの位置と範囲を表示

    if(curpos>=int(codepos)+codelen)//すこしはみ出たら表示コード内に入れる
    {
      if(codeunit>=1)curpos = int(codepos)+codelen - codeunit;
      else curpos = int(codepos)+codelen - 1;
    }

    if(codeunit>=1){tft.fillRect(156, int(curpos), 4, codeunit, HACO3_C8);}//コードの位置と範囲をスライダ表示
    else{tft.fillRect(156, int(curpos), 4, 1, HACO3_C8);}//１ピクセル未満の時は見えなくなるので１に
    
    //最終出力
    tft.setPivot(0, 0);
    tft.pushRotateZoom(&screen, TFT_POSX , TFT_POSY  , 0, 2, 2);//最大1.2倍までしか描画できない//PSRAMを使わないギリギリ
    // tft.pushRotateZoom(&screen, 0 , 0  , 0, 2, 2);//最大1.2倍までしか描画できない//PSRAMを使わないギリギリ
    
    if(pressedBtnID == 0)//(|)メニュー画面へ
    {
      editor.setCursorConfig(0,0,0);//カーソルの位置を保存
      delay(50);
      restart("/init/main.lua", TFT_RUN_MODE);
    }

    if(pressedBtnID == 12){//RUN<-->EDIT切り替え
      
      editor.editorSave(SPIFFS);//SPIFFSに保存
      delay(100);//ちょっと待つ
      reboot(appfileName, TFT_RUN_MODE);//現状rebootしないと初期化が完全にできない
      // restart(appfileName, 0);//初期化がうまくできない（スプライトなど）
      // broadchat();//ファイルの中身をブロードキャスト送信する（ファイルは消えない）
    }

    // editor.editorRefreshScreen(tft);

    // float codeunit = 128.0/float(editor.getNumRows());
    // float codelen = codeunit*10;
    
    // float curpos = codeunit*editor.getCy();
    // float codepos = codeunit*(editor.getCy() - editor.getScreenRow());
    
    // tft.fillRect(156,0, 4,128, HACO3_C5);//コードの全体の長さを表示
    // tft.fillRect(156,int(codepos), 4,codelen, HACO3_C6);//コードの位置と範囲を表示
    // if(codeunit>=1){tft.fillRect(155, int(curpos), 4, codeunit, HACO3_C8);}//コードの位置と範囲を表示
    // else{tft.fillRect(155, int(curpos), 4, 1, HACO3_C8);}//１ピクセル未満の時は見えなくなるので１に
    
    // // tft.pushSprite(&screen, 60, 0);
    // tft.setPivot(0, 0);
    // tft.pushRotateZoom(&screen, 112 , 0  , 0, 2, 2);//最大1.2倍までしか描画できない//PSRAMを使わないギリギリ
    
    // if(pressedBtnID == 0)//ESC
    // {
    //   editor.setCursorConfig(0,0,0);//カーソルの位置を保存
    //   delay(50);
    //   restart("/init/main.lua", 0);
    // }

    // if(pressedBtnID == 6){//PAGEDOWN
    //   editor.editorSave(SPIFFS);//SPIFFSに保存
    //   delay(100);//ちょっと待つ
    //   reboot(appfileName, TFT_RUN_MODE);//現状rebootしないと初期化が完全にできない
    //   // restart(appfileName, 0);//初期化がうまくできない（スプライトなど）
    //   // broadchat();//ファイルの中身をブロードキャスト送信する（ファイルは消えない）
    // }

  }
  else if(isEditMode == TFT_WIFI_MODE)
  {
    // editor.editorRefreshScreen(tft);

    //ESCボタンで強制終了
    // if (pressedBtnID == 0)
    // { // reload
    //   safeReboot();
    // }


    // tft.setTextSize(1);//サイズ
    // tft.setFont(&lgfxJapanGothicP_8);//日本語可
    // tft.setCursor(0, 0);//位置
    // tft.setTextWrap(true);
    // tft.setTextScroll(true);


    if(pressedBtnID == 6){//PAGEDOWN
      // editor.editorSave(SPIFFS);//SPIFFSに保存
      // delay(100);//ちょっと待つ
      // broadchat();
    }

    tft.pushSprite(&screen,40,0);
  }

    startTime = currentTime;
  }

  // for(int n=0; n<LED_NUM; n++)
  // {
  //   if(isSoftLED[n]==true){
  //     screen.fillCircle(8+16*n,16,8,TFT_RED);
  //   }else{
  //     screen.fillCircle(8+16*n,16,8,TFT_DARKGRAY);
  //   }
  //   screen.drawCircle(8+16*n,16,8,TFT_WHITE);
  // }
  

  

  frame++;
  if(frame > 18446744073709551615)frame = 0;

  delay(1);
  
}
