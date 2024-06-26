#include "LovyanGFX_DentaroUI.hpp"
using namespace std;

#define FLICK_DIST 4
#define HOLDING_SEC 160000
#define TAP_TO_TAP_SEC 220000

#define CALIBRATION_FILE "/init/param/caldata.txt"

#include "FS.h"
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true

// 閾値の数
const int numThresholds = 5;
// 方向を示すビット列
byte directionBits = 0;

// 閾値のリスト
float statebtn_vals[6] = {
    1150, 1050, 560, 180, 20, 0
};

LovyanGFX_DentaroUI::LovyanGFX_DentaroUI(LGFX* _lcd)
{
  lcd = _lcd;
}

int LovyanGFX_DentaroUI::getPhVol(int n)
{
  return phbtnState[n];
}

int LovyanGFX_DentaroUI::getPhVolDir(int n){
  // 音声ボリュームの値
  float volume = getPhVol(n);
  // 閾値を元に方向を判断する
  for (int i = 0; i < numThresholds; i++) {
    float threshold = (statebtn_vals[i]+statebtn_vals[i+1])/2;
      if (volume >= threshold) {
          directionBits = i;  // 閾値を超えたら該当する方向を示すビット列をセット
          break;  // 最初の該当する閾値を見つけたらループを終了
      }
  }
  if(volume == 0){
    directionBits = -1;
  }
  return directionBits;
}

int LovyanGFX_DentaroUI::getPhVolVec(int n1, int n2) {
  // 音声ボリュームの値
  float x = getPhVol(n1);
  float y = getPhVol(n2);

  // 基準値を引いて調整
  x -= 1950;
  y -= 1904;

  // 値の制限
  if (x > 1900) {
    x = 1900;
  } else if (x < -1900) {
    x = -1900;
  }
  if (y > 1900) {
    y = 1900;
  } else if (y < -1900) {
    y = -1900;
  }
// 角度を計算（ラジアン）
  float angle = atan2(y, x);

  // 角度を0~360度に変換
  if (angle < 0) {
    angle += 2 * M_PI;
  }
  float degrees = angle * 180.0 / M_PI;

  // 方向を判定
  int direction = -1; // デフォルトは方向がないという意味

  if (abs(x) < 50 && abs(y) < 50) {
    direction = -1; // 方向がない
  } else if (degrees >= 22.5 && degrees < 67.5) {
    direction = 0; // 右上
  } else if (degrees >= 67.5 && degrees < 112.5) {
    direction = 1; // 上
  } else if (degrees >= 112.5 && degrees < 157.5) {
    direction = 2; // 左上
  } else if (degrees >= 157.5 && degrees < 202.5) {
    direction = 3; // 左
  } else if (degrees >= 202.5 && degrees < 247.5) {
    direction = 4; // 左下
  } else if (degrees >= 247.5 && degrees < 292.5) {
    direction = 5; // 下
  } else if (degrees >= 292.5 && degrees < 337.5) {
    direction = 6; // 右下
  } else if ((degrees >= 337.5 && degrees <= 360) || (degrees >= 0 && degrees < 22.5)) {
    direction = 7; // 右
  }

  if (abs(x) < 50 && abs(y) < 50) {
    direction = -1; // 方向がない
  }

  return direction;
}

void LovyanGFX_DentaroUI::touchCalibration(LGFX& _lcd){
  // touchCalibrationF = _CalF;
// タッチが使用可能な場合のキャリブレーションを行います。（省略可）
  if(touchCalibrationF == true){
    if (_lcd.touch())
    {

      if (_lcd.width() < _lcd.height()) _lcd.setRotation(_lcd.getRotation() ^ 1);

      // 画面に案内文章を描画します。
      // _lcd.setTextDatum(textdatum_t::middle_center);
      // _lcd.drawString("touch the arrow marker.", _lcd.width()>>1, _lcd.height() >> 1);
      // _lcd.setTextDatum(textdatum_t::top_left);

      // タッチを使用する場合、キャリブレーションを行います。画面の四隅に表示される矢印の先端を順にタッチしてください。
      std::uint16_t fg = TFT_WHITE;
      std::uint16_t bg = TFT_BLACK;
      if (_lcd.isEPD()) std::swap(fg, bg);
      // _lcd.calibrateTouch(nullptr, fg, bg, std::max(_lcd.width(), _lcd.height()) >> 3);
      _lcd.calibrateTouch(calData, fg, bg, std::max(_lcd.width(), _lcd.height()) >> 3);


    if (!SPIFFS.begin()) {
      _lcd.println("Formating file system"); 
      SPIFFS.format(); 
      SPIFFS.begin(); 
    }
    
File f = SPIFFS.open(CALIBRATION_FILE, "a"); // "a"モードを使用してファイルを開く
if (f) {
  // 最初の8つの値をカンマ区切りでファイルに書き出す
  for (int i = 0; i < 8; ++i) {
    f.print(calData[i]);
    f.print(",");
  }
  f.println(); // 改行を追加する
  f.close();
} else {
  _lcd.println("Failed to open file for writing"); // ファイルが開けなかった場合のエラーメッセージ
}

    _lcd.fillRect(0, 0, 128, 128, TFT_GREEN); 
    _lcd.setTextSize(1); 
    _lcd.setTextColor(TFT_WHITE, TFT_BLUE); 
    _lcd.setCursor(0, 0); 
    _lcd.println(calData[0]); 
    _lcd.println(calData[1]); 
    _lcd.println(calData[2]); 
    _lcd.println(calData[3]); 
    _lcd.println(calData[4]); 
    _lcd.println(calData[5]); 
    _lcd.println(calData[6]); 
    _lcd.println(calData[7]);
  
  delay(2000);
  
  _lcd.setTouchCalibrate(calData);

      // _lcd.fillScreen(TFT_BLACK);

      // _lcd.setColorDepth(COL_DEPTH);


          // Serial.printf("heap_caps_get_free_size(MALLOC_CAP_SPIRAM)            : %6d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM) );
          // Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DMA):%d\n", heap_caps_get_free_size(MALLOC_CAP_DMA) );
          // Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA):%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
          // Serial.printf("Width:%d, Height:%d\n", 256, 256);

          // Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DMA):%d\n", heap_caps_get_free_size(MALLOC_CAP_DMA) );
          // Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA):%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
      }
    }
  }
  
void LovyanGFX_DentaroUI::showSavedCalData(LGFX& _lcd){

  // タッチが使用可能な場合のキャリブレーション値の可視化を行います。（省略可）
  if(touchCalibrationF == true)
  {
    if (!SPIFFS.begin()) {
    _lcd.println("Formating file system"); 
    SPIFFS.format(); 
    SPIFFS.begin(); 
    }
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
    _lcd.fillRect(0, 0, 128, 128, TFT_GREEN); 
    _lcd.setTextSize(1); 
    _lcd.setTextColor(TFT_WHITE, TFT_BLUE); 
    _lcd.setCursor(0, 0); 
    _lcd.println(calData[0]); 
    _lcd.println(calData[1]); 
    _lcd.println(calData[2]); 
    _lcd.println(calData[3]); 
    _lcd.println(calData[4]); 
    _lcd.println(calData[5]); 
    _lcd.println(calData[6]); 
    _lcd.println(calData[7]);
  }
  delay(2000);
  _lcd.setTouchCalibrate(calData);
}


void LovyanGFX_DentaroUI::setCalF(bool _calibF){
  touchCalibrationF = _calibF;
}

bool LovyanGFX_DentaroUI::getCalF(){
  return touchCalibrationF;
}

void LovyanGFX_DentaroUI::begin( LGFX& _lcd, int _colBit, int _rotateNo, bool _calibF)
{
  _lcd.init();
  _lcd.begin();
  _lcd.startWrite();//CSアサート開始
  _lcd.setColorDepth( _colBit );
  touchCalibrationF = _calibF;
  begin(_lcd);
  
  _lcd.setRotation( _rotateNo );
  // showSavedCalData(_lcd);//タッチキャリブレーションの値を表示

}

int LovyanGFX_DentaroUI::getTouchZoom(){
  return touchZoom;
}

void LovyanGFX_DentaroUI::setTouchZoom(int _touchzoom){
  touchZoom = _touchzoom;
}



void LovyanGFX_DentaroUI::begin( LGFX& _lcd, String _host, int _shiftNum, int _colBit, int _rotateNo, bool _calibF )
{
	host = _host;
   SD.end();
  // SDカードがマウントされているかの確認

  if(!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 20000000)){
    Serial.println("Card Mount Failed");
    while (1) {}
  }

  // カードタイプの取得
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    // 画面に案内文章を描画します。
    _lcd.setTextDatum(textdatum_t::middle_center);
    _lcd.drawString("No SD card attached.", _lcd.width()>>1, _lcd.height() >> 1);
    _lcd.setTextDatum(textdatum_t::top_left);
    while (1) {}
  }

  begin( _lcd, _shiftNum, _colBit, _rotateNo, _calibF );

}

void LovyanGFX_DentaroUI::begin( LGFX& _lcd, int _shiftNum, int _colBit, int _rotateNo, bool _calibF ){

  // for(int i=0; i<18;i++){
  //   uiBoxes.push_back(*new UiContainer);
  // }

  _lcd.init();
  _lcd.begin();
  // _lcd.startWrite();//CSアサート開始
  _lcd.setColorDepth( _colBit );
  touchCalibrationF = _calibF;
  shiftNum = _shiftNum;

  //シフト数分パネルを確保
  for(int i = 0; i < shiftNum; i++){
    flickPanels[i] = new FlickPanel;
  }

  use_flickUiSpriteF = true;
  // flickUiSprite = _flickUiSprite;
  begin(_lcd);
  _lcd.setRotation( _rotateNo );
}

void LovyanGFX_DentaroUI::begin( LGFX& _lcd)
{
 lcdPos.x = 0;
 lcdPos.y = 0;

//  uiMode = _mode;
  if(uiMode == TOUCH_MODE){

  }else if(uiMode == PHYSICAL_MODE){
    selectBtnID = 0;
  }

}

// void LovyanGFX_DentaroUI::update( LGFX& _lcd )
// {
//   _lcd.getTouch(&tp);
//   for(int i = 0; i < 4; i++)
//   {
//     clist[i+1] = clist[i];
//   }
//   if(eventState != NO_EVENT) preEventState = eventState;//前のイベント状態を保持

//   //以下イベント取得処理
//   eventState = NO_EVENT;//イベント初期化
//   flickState = NO_EVENT;//-1で初期化
//   obj_ret.reset();

//   //1ビット左にシフト
//   touchState = touchState<<1;

//   clist[0] = TFT_WHITE;

//   if ( _lcd.getTouch( &tp ) )
//   {
//     bitSet(touchState, 0);//右０ビット目を１（Set）に
//   }else{
//     bitClear(touchState, 0);//右０ビット目を0（Clear）に
//   }
//   //下位２ビットをみてイベントを振り分け
//   touchState &= B00000011;//マスク
//   if (touchState == B_TOUCH){
//     sp = tp; touchStartTime =  micros();
//     eventState = TOUCH;//7
//     setEventBit(TOUCH,1);
//   }
//   else if (touchState == B_MOVE) {
//     clist[0] = TFT_GREEN;
//     unsigned long holdingTime = micros() - touchStartTime;

//     eventState = WAIT;
//     setEventBit(WAIT,1);
//     if(holdingTime > HOLDING_SEC){
//       clist[0] = TFT_YELLOW;
//       eventState = MOVE;//8
//       setEventBit(MOVE,1);
//     }
//   }
//   else if (touchState == B_RELEASE)
//   {
//     clist[0] = TFT_BLUE;
//     unsigned long holdingTime = micros() - touchStartTime;
//     float dist = sqrt( pow((tp.x - sp.x),2) + pow((tp.y - sp.y), 2) );

//     if(holdingTime > HOLDING_SEC - 10000 && holdingTime <= HOLDING_SEC){
//       if(dist <= FLICK_DIST){
//         eventState = WAIT;//5
//         setEventBit(WAIT,1);
//       }
//     }
//     else if(holdingTime > HOLDING_SEC)
//     {
//       eventState = DRAG;//2
//       setEventBit(DRAG,1);
//     }
//     else if(dist <= FLICK_DIST)
//     {
//       float dist2 = sqrt( pow((tp.x - sp2.x),2) + pow((tp.y - sp2.y),2) );

//       if(preTouchStartTime != 0)
//       {
//         unsigned long tappingTime  = touchStartTime - preTouchStartTime;
//         if( tappingTime < TAP_TO_TAP_SEC ){//前とのタップ間の時間調整, 位置判定
//           if(dist2 < FLICK_DIST) tapCount++;//短距離、短時間なら加算
//         }
//         else if( tappingTime >= TAP_TO_TAP_SEC )//時間が空いていたらリセット
//         {

//           lastTapCount = tapCount;
//           if(lastTapCount>0){
//             eventState = WTAPPED;
//             setEventBit(WTAPPED, 1);
//           }else if(lastTapCount==0){
//             eventState = TAPPED;
//             setEventBit(TAPPED, 1);
//           }
//           tapCount = 0;

//         }
//       }

//       //tapCount++;//今回タップした分を足す
//       lastTappedTime = micros();
//       Serial.print(tapCount);
//       Serial.print(":");
//       Serial.print(micros());
//       Serial.print(":");
//       Serial.println(micros() - firstTappedTime);


//       // if(endTapCount)

//       if(tapCount == 0){
//         firstTappedTime = touchStartTime;//tap番号0のタップスタート時間を格納
//         eventState = TAPPED;
//         setEventBit(TAPPED, 1);
//       }

//       if(tapCount == 1){
//         eventState = WTAPPED;//4
//         setEventBit(WTAPPED, 1);
//       }


//       sp2 = tp;
//       preTouchStartTime = touchStartTime;
//     }
//     else if(dist > FLICK_DIST)
//     {
//        if(holdingTime <= HOLDING_SEC){
//         float angle = getAngle(sp, tp);
//   //      Serial.println(angle);

//           if(angle <= 22.5 || angle > 337.5){
//             eventState = RIGHT_FLICK;//0
//             setEventBit(RIGHT_FLICK, 1);
//             flickState = RIGHT_FLICK;//0
//           }else if(angle <= 67.5 && angle > 22.5){
//             eventState = R_D_FLICK;//7
//             setEventBit(R_D_FLICK, 1);
//             flickState = R_D_FLICK;//7
//           }else if(angle <= 112.5 && angle > 67.5){
//             eventState = DOWN_FLICK;//6
//             setEventBit(DOWN_FLICK, 1);
//             flickState = DOWN_FLICK;//6
//           }else if(angle <= 157.5 && angle > 112.5){
//             eventState = D_L_FLICK;//5
//             setEventBit(D_L_FLICK, 1);
//             flickState = D_L_FLICK;//5
//           }else if(angle <= 202.5 && angle > 157.5){
//             eventState = LEFT_FLICK;//4
//             setEventBit(LEFT_FLICK, 1);
//             flickState = LEFT_FLICK;//4
//           }else if(angle <= 247.5 && angle > 202.5){
//             eventState = L_U_FLICK;//3
//             setEventBit(L_U_FLICK, 1);
//             flickState = L_U_FLICK;//3
//           }else if(angle <= 292.5 && angle > 247.5){
//             eventState = UP_FLICK;//2
//             setEventBit(UP_FLICK, 1);
//             flickState = UP_FLICK;//2
//           }else if(angle <= 337.5 && angle > 292.5){
//             eventState = U_R_FLICK;//1
//             setEventBit(U_R_FLICK, 1);
//             flickState = U_R_FLICK;//1
//           }

//        }
//     }
//     eventState = RELEASE;//9
//     setEventBit(RELEASE,1);



//   }
//   else if (touchState == B_UNTOUCH)
//   {
//     clist[0] = TFT_DARKGREEN;

//   // if(micros() - firstTappedTime < 200000 && jadgeF == false){
//   //   jadgeF = true;
//   // }

//     // if(micros() - firstTappedTime > 200000 && jadgeF == true){
//     //   eventState = TAPPED;//
//     //   setEventBit(TAPPED, 1);
//     //   jadgeF = false;
//     // }
//   }




//   // if(micros() - firstTappedTime < 320000 && jadgeF == false){
//   //   jadgeF = true;
//   // }
//   // if(jadgeF == true){//タップ番号０時点から現在の時間までの経過時間がｗTapの判定時間を超えたら
//   //   if(tapCount == 0){
//   //     eventState = TAPPED;//4
//   //     setEventBit(TAPPED, 1);
//   //   }else if(tapCount == 1){
//   //     eventState = WTAPPED;//4
//   //     setEventBit(WTAPPED, 1);
//   //   }
//   //   jadgeF = false;
//   // }



//   for(int i = 0; i < uiBoxes_num; i++){
//     for(int j = 0; j<uiBoxes[i].b_num; j++){
//       addHandler(uiBoxes[i].b_sNo + j, j, ret0_DG, uiBoxes[i].eventNo, uiBoxes[i].parentID);
//       addHandler(uiBoxes[i].b_sNo + j, j, ret1_DG, uiBoxes[i].eventNo, uiBoxes[i].parentID);
//     }
//   }
// }



void LovyanGFX_DentaroUI::update( LGFX& _lcd )
{
  // constantGetF = true;
  _lcd.getTouch(&tp);
  
  
  for(int i = 0; i < 4; i++)
  {
    clist[i+1] = clist[i];
  }
  if(eventState != NO_EVENT) preEventState = eventState;//前のイベント状態を保持

  //以下イベント取得処理
  eventState = NO_EVENT;//イベント初期化
  flickState = NO_EVENT;//-1で初期化
  obj_ret.reset();

  //1ビット左にシフト
  touchState = touchState<<1;

  clist[0] = TFT_WHITE;

  if ( _lcd.getTouch( &tp ) )
  {
    bitSet(touchState, 0);//右０ビット目を１（Set）に
  }else{
    bitClear(touchState, 0);//右０ビット目を0（Clear）に
  }
  //下位２ビットをみてイベントを振り分け
  touchState &= B00000011;
  if (touchState == B_TOUCH){
    sp = tp; sTime =  micros();
    eventState = TOUCH;//6
  }
  else if (touchState == B_MOVE) {
    clist[0] = TFT_GREEN;
    unsigned long touchedTime = micros() - sTime;
    eventState = WAIT;
    if(touchedTime > 160000){
      clist[0] = TFT_YELLOW;
      eventState = MOVE;//8
    }
  }
  else if (touchState == B_RELEASE)
  {
    clist[0] = TFT_BLUE;
    unsigned long touchedTime = micros() - sTime;
    float dist = sqrt( pow((tp.x - sp.x),2) + pow((tp.y - sp.y),2) );

    if(touchedTime > 120000&&touchedTime <= 160000){
      if(dist <= FLICK_DIST){
        eventState = WAIT;//5
      }
    }
    else if(touchedTime > 160000)
    {
      eventState = DRAG;//2
    }
    else if(dist <= FLICK_DIST){
      eventState = TAPPED;//4

      float dist2 = sqrt( pow((tp.x - sp2.x),2) + pow((tp.y - sp2.y),2) );
      if(sTime - tappedTime < 200000 && dist2 < FLICK_DIST ){//ダブルタップの間の時間調整, 位置判定
        eventState = WTAPPED;//5
      }
      tappedTime = micros();
      sp2 = tp;
    }
    else if(dist > FLICK_DIST)
    {
       if(touchedTime <= 160000){
        float angle = getAngle(sp, tp);

          if(angle <= 22.5 || angle > 337.5){
            eventState = RIGHT_FLICK;//0
            flickState = RIGHT_FLICK;//0
          }else if(angle <= 67.5 && angle > 22.5){
            eventState = R_D_FLICK;//7
            flickState = R_D_FLICK;//7
          }else if(angle <= 112.5 && angle > 67.5){
            eventState = DOWN_FLICK;//6
            flickState = DOWN_FLICK;//6
          }else if(angle <= 157.5 && angle > 112.5){
            eventState = D_L_FLICK;//5
            flickState = D_L_FLICK;//5
          }else if(angle <= 202.5 && angle > 157.5){
            eventState = LEFT_FLICK;//4
            flickState = LEFT_FLICK;//4
          }else if(angle <= 247.5 && angle > 202.5){
            eventState = L_U_FLICK;//3
            flickState = L_U_FLICK;//3
          }else if(angle <= 292.5 && angle > 247.5){
            eventState = UP_FLICK;//2
            flickState = UP_FLICK;//2
          }else if(angle <= 337.5 && angle > 292.5){
            eventState = U_R_FLICK;//1
            flickState = U_R_FLICK;//1
          }

       }
    }
    eventState = RELEASE;//9
  }else if (touchState == B_UNTOUCH) {
    clist[0] = TFT_DARKGREEN;
  }

  for(int i = 0; i < panels.size(); i++){//パネルの数
    // Serial.print(panels[i]->id);
    for(int j = 0; j < panels[i]->b_num; j++){//パネル内のボタンの数
      addHandler(i, j, setBtnID_ret_DG, panels[i]->eventNo, panels[i]->parentID, constantGetF);
      addHandler(i, j, setBtnNO_ret_DG, panels[i]->eventNo, panels[i]->parentID, constantGetF);
    }

  }

  if(constantGetF){
    if(getEvent() == NO_EVENT){
      obj_ret.setBtnID_ret(-1);//‐１（ボタンが離れている状態）にクリア
    }
    // if(getEvent() != TOUCH)obj_ret.setBtnID_ret(-1);//‐１（ボタンが離れている状態）にクリア
  }

  
  // constantBtnID = obj_ret.btnID;
}


void LovyanGFX_DentaroUI::flickSetup(){

 //  if(use_flickUiSpriteF){
 //
 //   createFlick9Btns(_flickUiSprite);//フリック用のボタンを生成
 // }
 //createLayout タッチ位置を決めている
  createLayout( 0 , 0, 240, 240, MULTI_EVENT );//LAYOUT_0

  createBtns(   0 , 80+18, 240, 30,  1, 1, MULTI_EVENT );//BTN_1

  createFlicks( 48, 80+48,  144, 192, 3, 4, MULTI_EVENT );//FLICK_2

  setFlickPanels();//フリックキーボード用プリセット
// //ここでフリックパネルのカスタマイズができます。
// //引数は(フリックパネル番号、ボタン番号、登録したい５文字、または９文字)になっています。
// //  ui.setFlickPanel(0, 0, "あいうえおかきくけ");
// //  ui.setFlickPanel(0, 1, "臨兵闘者皆陣列前行");

  createBtns( 0,   80+48,  48, 192, 1, 4, MULTI_EVENT );//BTN_3
  createBtns( 192, 80+48,  48, 192, 1, 4, MULTI_EVENT );//BTN_4


  setFlick( JP , getUiID("BTN_1"), getUiID("FLICK_2"), getUiID("BTN_3"), getUiID("BTN_4"));//立ち上げ時のパネル指定　JP/EN/NUMERIC
  setBtnName( 0, "↓");

  // Serial.println(getUiID("FLICK_2"));



}
void LovyanGFX_DentaroUI::flickUpdate(  LGFX& _lcd )
{
  if( getEvent() != NO_EVENT)
  {
    if( getEvent() == TOUCH )
    {
      if( getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo + 2 )//フリックパネルのシフト uiBoxes[LeftBtnUiID].b_sNo+2 = 14
      {
          fpNo++;
          fpNo %= SHIFT_NUM;//フリックパネルのシフト数
          if(fpNo==0||fpNo==1)setCharMode(CHAR_3_BYTE);//日本語パネル（３バイト）
          else if(fpNo==2||fpNo==3)setCharMode(CHAR_1_BYTE);//英語パネル（１バイト）
          setUiLabels( FlickUiID, fpNo);
          drawFlicks( FlickUiID, _lcd );
      }
      else if(getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo+ 1 )
      { //大小文字モード切替 uiBoxes[LeftBtnUiID].b_sNo+1 = 13
        if(fpNo!=2&&fpNo!=3)fpNo=2;//英語モードでなければ切り替える
          else if(fpNo==2)fpNo=3;
          else if(fpNo==3)fpNo=2;
          if(fpNo==2||fpNo==3){
            setCharMode( CHAR_1_BYTE );//英語パネル（１バイト）
          }
          setUiLabels( FlickUiID, fpNo );
          drawFlicks( FlickUiID, _lcd );
      }

      // _layoutSprite.fillScreen(TFT_BLACK);
      // _lcd.fillRect(0,80,240,240,TFT_BLACK);
      drawBtns( TopBtnUiID, _lcd );
      drawBtns( LeftBtnUiID, _lcd );
      drawBtns( RightBtnUiID, _lcd );
      drawFlicks( FlickUiID, _lcd );
      // drawFlickBtns(_lcd, getTouchBtnID(), getTouchBtnNo(), true, true );
      // drawLayOut( _lcd );
    }

    if( getEvent() == RELEASE )
    {
      // _layoutSprite.fillScreen(TFT_BLACK);
      // _lcd.fillRect(0,80,240,240,TFT_BLACK);

      if( getTouchBtnID() == uiBoxes[TopBtnUiID].b_sNo)
      {
        if(uiOpenF){
          setLayoutPos( 0, 80 );
          }
        else if(!uiOpenF){setLayoutPos( 0, 290 ); }
        _lcd.fillScreen(TFT_BLACK);
        uiOpenF = !uiOpenF;
      }

      // _layoutSprite.fillScreen(TFT_BLACK);
      drawBtns( TopBtnUiID, _lcd );
      drawBtns( LeftBtnUiID, _lcd );
      drawBtns( RightBtnUiID, _lcd );
      drawFlicks( FlickUiID, _lcd );

      if(getTouchBtnID() >= getUiFirstNo(FlickUiID) && getTouchBtnID() < getUiFirstNo(FlickUiID) + 12 ){
      curKanaRowNo = getTouchBtnID() - getUiFirstNo(FlickUiID);//押されたボタンを行番号として渡す。
      }

      if(getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo)
      {//次へNxt uiBoxes[LeftBtnUiID].b_sNo = 12
        selectModeF =false;
        if(charMode == CHAR_3_BYTE ){//日本語入力の時
          finalChar = "";
          curKanaColNo++;
          curKanaColNo%=5;
          finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,kanaShiftNo);

          while(String("　") == finalChar){
            curKanaColNo++;
            curKanaColNo%=5;
            finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,kanaShiftNo);
          }

          if(finalChar!="無"){
            flickStr = delEndChar(flickStr, 3);
            flickStr += finalChar;
            flickStrDel += finalChar+"\n";
          }
        }else if(charMode == CHAR_1_BYTE ){//英語入力の時
          finalChar = "";
          curKanaColNo++;
          curKanaColNo%=5;
          finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,0);
          while(String(" ") == finalChar){
            curKanaColNo++;
            curKanaColNo%=5;
            finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,0);
          }
          if(finalChar!="無"){
            delChar();
            // flickStr = delEndChar(flickStr, 1);
            flickStr += finalChar;
            flickStrDel += finalChar+"\n";
          }
        }
      }
      if( getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo + 3 )
      {//変換
        selectModeF =true;
        if( charMode == CHAR_3_BYTE ){
          kanaShiftNo++;
          kanaShiftNo%=3;
          finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,kanaShiftNo);
          if(finalChar!="無"){
            flickStr = delEndChar(flickStr, 3);
            flickStr += finalChar;
            flickStrDel += finalChar+"\n";
          }
        }
      }
      else if(getTouchBtnID() == uiBoxes[RightBtnUiID].b_sNo)//delete
      {
        //最後の文字のバイト数を判定する
        setlocale(LC_ALL, "");
        std::vector<std::string> ret = split_mb(flickStrDel.c_str(),"\n");
        if(ret.size() >= 1)
        {
          //バイト数分消去
          flickStr = delEndChar(flickStr, ret[ret.size()-1].length());//一字分のバイト数削る
          //flickStrDelも更新（最後の字+ "\n"を削除）
          flickStrDel = "";
          for (size_t i = 0; i < ret.size()-1; i++) {
            flickStrDel += String(ret[i].c_str()) + "\n";
          }
        }

      }else if(getTouchBtnID() == uiBoxes[RightBtnUiID].b_sNo+1){
        if( charMode == CHAR_3_BYTE ){//全角空白（日本語のとき）
          finalChar = "　";
          flickStr += finalChar;
          }
        else if( charMode == CHAR_1_BYTE ){//半角空白
          finalChar = " ";
          flickStr += finalChar;
        }
        // flickStrDel = flickStrDel + finalChar + "\n";//通らない
        flickStrDel = String(flickStrDel) + finalChar + "\n";//通る
        // flickStrDel += finalChar+"\n";//なぜか通らない

      }else if(getTouchBtnID() == uiBoxes[RightBtnUiID].b_sNo+2){flickStr = ""; flickStrDel = "";//クリアfinalStr = "";finalStrDel = "";
      }else if(flickStr.length() >= 72){flickStr = "";//24文字を超えたらfinalStr = "";
      }else
      {
        if(selectModeF){
          kanaShiftNo = 0;
        }
        finalChar = getFlickStr();
        flickStr += finalChar;
        flickStrDel += finalChar+"\n";
        // drawLayOut( _lcd );
      }
    }
  }
}

void LovyanGFX_DentaroUI::setConstantGetF(bool _constantGetF){
  constantGetF = _constantGetF;
}

void LovyanGFX_DentaroUI::delChar(){
  //最後の文字のバイト数を判定する
  setlocale(LC_ALL, "");
  std::vector<std::string> ret = split_mb(flickStrDel.c_str(),"\n");

  if(ret.size() >= 1){
  //バイト数分消去
  flickStr = delEndChar(flickStr, ret[ret.size()-1].length());//一字分のバイト数削る

  //flickStrDelも更新（最後の字+ "\n"を削除）
    flickStrDel = "";
    for (size_t i = 0; i < ret.size()-1; i++) {
      flickStrDel += String(ret[i].c_str()) + "\n";
    }
  }
}

String LovyanGFX_DentaroUI::getKana(int _panelID, int _rowID, int _colID, int _transID){
  String str="";
  std::string stdstr = "";
  stdstr = flickPanels[showFlickPanelNo]->text_list[_rowID].c_str();

  if( charMode == CHAR_3_BYTE )
  {
    String planeStr ="";

    planeStr = stdstr.substr(_colID*3, 3).c_str();
    str = stdstr.substr(_colID*3, 3).c_str();

    if(str != "ま"||planeStr != "な"){
      if(_transID == 1||_transID == 2){
        int i = 0;
        while(planeStr!= getHenkanChar(i, 0)||i>HENKAN_NUM-1){
          i++;
          if(i>HENKAN_NUM-1)break;
        }
        if(i < HENKAN_NUM && getHenkanChar(i, _transID).c_str()!=NULL)str = getHenkanChar(i, _transID).c_str();
        else str = "無";
      }
    }

  }
  else if( charMode == CHAR_1_BYTE )
  {
    str = stdstr.substr(_colID, 1).c_str();
  }

  return str;
}


//次のマルチバイト文字へのポインタを取得
const char* LovyanGFX_DentaroUI::next_c_mb(const char* c) {
  int L = mblen(c, 10);
  return c + L;
}

//マルチバイト文字を一文字取り出す
void LovyanGFX_DentaroUI::ngetc(char* const dst,const char* src) {
  int L = mblen(src, 10);
  memcpy(dst, src, L);
  dst[L] = '\0';
}

//マルチバイト文字を比較する
bool LovyanGFX_DentaroUI::nchr_cmp(const char* c1, const char* c2) {
  int K = mblen(c1, 10);
  int L = mblen(c2, 10);

  if (K != L)
    return false;

  bool issame = (strncmp(c1, c2, K) == 0);
  return issame;
}

std::vector<std::string> LovyanGFX_DentaroUI::split_mb(const char* src, const char* del) {

  char tmp[10];

  std::vector<std::string> result;

  std::string tmps;
  while (*src) {

    //デリミタを飛ばす
    // const char* p = src;
    while (nchr_cmp(src, del) == true && *src != '\0')
      src= next_c_mb(src);

    //デリミタに遭遇するまで文字を追加し続ける
    while (nchr_cmp(src, del) != true && *src != '\0') {
      ngetc(tmp, src);//一文字取り出す
      tmps += tmp;
      src = next_c_mb(src);
    }
    if (tmps.size()) {
      result.push_back(tmps);
    }
    tmps.clear();
  }

  return result;
}

void LovyanGFX_DentaroUI::updateSelectBtnID(int _selectBtnID){
 selectBtnID = _selectBtnID;
}

void LovyanGFX_DentaroUI::showTouchEventInfo(LovyanGFX& _lcd, int _x, int _y){
  for(int i = 0; i < 5; i++){
    _lcd.fillRect(_x + 1 + 10*i, _y, 9, 9, clist[0]);
  }
}

void LovyanGFX_DentaroUI::createToggles(int _x, int _y, int _w,int _h, int _row, int _col, int _eventNo){
  toggle_mode = true;
  createBtns( _x, _y, _w, _h, _row, _col, _eventNo);
  toggle_mode = false;
}

void LovyanGFX_DentaroUI::createFlick9Btns(LGFX_Sprite& _flickUiSprite)
{
  flickPanel.b_sNo = 0;
  flickPanel.id  = 0;
  flickPanel.x   = 0;
  flickPanel.y   = 0;
  //  flickPanel.w   = 144;
  //  flickPanel.h   = 144;
  flickPanel.row = 3;
  flickPanel.col = 3;
  // flickPanel.eventNo = _eventNo;

  //  _lcd.setPsram( USE_PSRAM );
  // _lcd.setPsram(false);//UNUSE_PSRAM
  // _lcd.setColorDepth( COL_DEPTH );
  // _lcd.createSprite( 144, 144 );
  //  _lcd.createSprite( flickPanel.w, flickPanel.h );

  //btnNum = _row * _col ;
  int b_w = 48;
  int b_h = 48;

  for(int j= 0; j < 3; j++)
  {
    for(int i= 0; i < 3; i++)
    {
      // if( i*flickPanel.col + j > 0 ){
      //    flick_touch_btn_list.push_back( new TouchBtn() );
      // }
      // else
      // {
      //   flick_touch_btn_list[ i*flickPanel.col + j ] = NULL;
      //   flick_touch_btn_list[ 0 ] = new TouchBtn();
      // }

      flick_touch_btn_list[ i*flickPanel.col + j ] = NULL;
      flick_touch_btn_list[ i*flickPanel.col + j ] = new TouchBtn();


      flick_touch_btn_list[ i*flickPanel.col + j ]->initBtn( i * flickPanel.col + j,"a",
      i*b_w,
      j*b_h,
      b_w,
      b_h,
      String( i*flickPanel.col + j ),
      getTouchPoint(0, 0),
      // getTouchPoint(0, 0),
      TOUCH_FLICK_MODE);

      //ベータ版は基本true
      flick_touch_btn_list[ i * flickPanel.col + j ]->setVisibleF( true );
      flick_touch_btn_list[ i * flickPanel.col + j ]->setAvailableF( true );
    }
  }
  flickPanel.b_num =  9;//UIのボタン数をセット
}

void LovyanGFX_DentaroUI::changeBtnMode(int _uiID, int _btnID, int _btn_mode){
  //flick_touch_btn_list[ _btnID ]->btn_mode = _btn_mode;
}

// void LovyanGFX_DentaroUI::setFlickPanel(int _flickPanelID, int _btnID, String _btnsString, int _btn_mode){
//   // flick_touch_btn_list[ _btnID ]->btn_mode = _btn_mode;
//   // setFlickPanel(_flickPanelID, _btnID, _btnsString);
// }

void LovyanGFX_DentaroUI::setFlickPanel( int _flickPanelID, int _btnID, String _btnsString ){

  flickPanels[_flickPanelID]->new_text_list(_btnID, _btnsString);
  // flickPanels[_flickPanelID]->text_list[_btnID] = _btnsString;
  // Serial.println(_btnID);
}

String LovyanGFX_DentaroUI::getFlickStr(){
  if(getTouchBtnID()!= NO_EVENT)
  {
    //押されたボタンのモードがFLICKだったら
    if(touch_btn_list[getTouchBtnID()]->getBtnMode() == TOUCH_FLICK_MODE){

      std::string gettingText = flickPanels[showFlickPanelNo]->text_list[getTouchBtnID() - getUiFirstNo(FlickUiID)].c_str();
      std::string trimText = "";
      flickString ="";
      trimText = gettingText;

      float angle = getAngle(sp, tp);
  //      Serial.println(angle);
      float dist = getDist(sp, tp);
      if(charMode == CHAR_3_BYTE){//日本語3ビットの場合
        //５文字の場合
        if(dist>24){

          if(gettingText.size() == 15)//5文字以下
          {
            charNumMode  = CHAR_3_BYTE_5;//5文字
            if(angle <= 22.5 || angle > 337.5){
              flickString = trimText.substr(3*3,3).c_str();//え
              curKanaColNo = 3;
            }else if(angle <= 112.5 && angle > 67.5){
              flickString = trimText.substr(4*3,3).c_str();//お
              curKanaColNo = 4;
            }else if(angle <= 202.5 && angle > 157.5){
              flickString = trimText.substr(1*3,3).c_str();//い
              curKanaColNo = 1;
            }else if(angle <= 292.5 && angle > 247.5){
              flickString = trimText.substr(2*3,3).c_str();//う
              curKanaColNo = 2;
            }
          }
          else if(gettingText.size() == 27)//9文字
          {
            charNumMode  = CHAR_3_BYTE_9;//9文字
            if(angle <= 22.5 || angle > 337.5){
              flickString = trimText.substr(5*3,3).c_str();//か
              curKanaColNo = 5;
            }else if(angle <= 112.5 && angle > 67.5){
              flickString = trimText.substr(7*3,3).c_str();//く
              curKanaColNo = 7;
            }else if(angle <= 202.5 && angle > 157.5){
              flickString = trimText.substr(1*3,3).c_str();//い
              curKanaColNo = 1;
            }else if(angle <= 292.5 && angle > 247.5){
              flickString = trimText.substr(3*3,3).c_str();//え
              curKanaColNo = 3;
            }else if(angle <= 67.5 && angle > 22.5){
              flickString = trimText.substr(6*3,3).c_str();//き
              curKanaColNo = 6;
            }else if(angle <= 157.5 && angle > 112.5){
              flickString = trimText.substr(8*3,3).c_str();//け
              curKanaColNo = 8;
            }else if(angle <= 247.5 && angle > 202.5){
              flickString = trimText.substr(2*3,3).c_str();//う
              curKanaColNo = 2;
            }else if(angle <= 337.5 && angle > 292.5){
              flickString = trimText.substr(4*3,3).c_str();//お
              curKanaColNo = 4;
            }
          }

        }else if(dist<=24){
          flickString = trimText.substr(0,3).c_str();//あ
          curKanaColNo = 0;
        }
        return flickString;
      }
      else if(charMode == CHAR_1_BYTE){//英数１ビットの場合
        if(dist>24){

          if(gettingText.size() <= 5)//1バイト文字5文字（英語）以下の場合
          {
            charNumMode  = CHAR_1_BYTE_5;//5文字
            flickString = "";
              if(angle <= 22.5 || angle > 337.5){
                if(trimText.length()>=4)
                flickString = trimText.substr(3,1).c_str();//D
              }

              if(angle <= 112.5 && angle > 67.5&&trimText.length()>=5){
                flickString = trimText.substr(4,1).c_str();//E
              }

              if(angle <= 202.5 && angle > 157.5&&trimText.length()>=2){
                flickString = trimText.substr(1,1).c_str();//B
              }

              if(angle <= 292.5 && angle > 247.5&&trimText.length()>=3){
                flickString = trimText.substr(2,1).c_str();//C
              }
          }

          if(gettingText.size() == 9)//1バイト文字5文字（英語）以下の場合
          {
            charNumMode  = CHAR_1_BYTE_9;//9文字
            flickString = "";
              // if(angle <= 22.5 || angle > 337.5){
              //   if(trimText.length()>=4)
              //   flickString = trimText.substr(3,1).c_str();//D
              // }

              // if(angle <= 112.5 && angle > 67.5&&trimText.length()>=5){
              //   flickString = trimText.substr(4,1).c_str();//E
              // }

              // if(angle <= 202.5 && angle > 157.5&&trimText.length()>=2){
              //   flickString = trimText.substr(1,1).c_str();//B
              // }

              // if(angle <= 292.5 && angle > 247.5&&trimText.length()>=3){
              //   flickString = trimText.substr(2,1).c_str();//C
              // }

            if(angle <= 22.5 || angle > 337.5){
              flickString = trimText.substr(5,1).c_str();//かF
              // curKanaColNo = 5;
            }else if(angle <= 112.5 && angle > 67.5){
              flickString = trimText.substr(7,1).c_str();//くH
              // curKanaColNo = 7;
            }else if(angle <= 202.5 && angle > 157.5){
              flickString = trimText.substr(1,1).c_str();//いB
              // curKanaColNo = 1;
            }else if(angle <= 292.5 && angle > 247.5){
              flickString = trimText.substr(3,1).c_str();//えD
              // curKanaColNo = 3;
            }else if(angle <= 67.5 && angle > 22.5){
              flickString = trimText.substr(6,1).c_str();//きG
              // curKanaColNo = 6;
            }else if(angle <= 157.5 && angle > 112.5){
              flickString = trimText.substr(8,1).c_str();//けI
              // curKanaColNo = 8;
            }else if(angle <= 247.5 && angle > 202.5){
              flickString = trimText.substr(2,1).c_str();//うC
              // curKanaColNo = 2;
            }else if(angle <= 337.5 && angle > 292.5){
              flickString = trimText.substr(4,1).c_str();//おE
              // curKanaColNo = 4;
            }

          }

        }else if(dist<=24){
          flickString = trimText.substr(0,1).c_str();//A
        }
        return flickString;

      }
    }
  }
  return "";

}

void LovyanGFX_DentaroUI::drawFlickBtns( LovyanGFX& _lcd, int _btnID, int _btnNo, bool _visible, bool _available )
{

  if(_btnID != NO_EVENT)
  {

    //押されたボタンのモードがFLICKだったら
    if(touch_btn_list[getTouchBtnID()]->getBtnMode() == TOUCH_FLICK_MODE)
    {
      //if(_visible){
      // _lcd.setPivot( 0, 0 );//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

      // _lcd.fillScreen(TFT_RED);

      std::string panelText ="０１２３４５６７８";//デフォルトの文字セット
      //std::string prePanelText ="";
      std::string gettingText = flickPanels[showFlickPanelNo]->text_list[_btnNo].c_str();//ArduinoのString型をc_str()で、std::string型に直してから、渡す
      std::string trimText = "";


      if(gettingText.size() <= 5)//1バイト文字5文字（英語）以下の場合
      {
        panelText ="";
        for(int i=0; i<9; i++){
          int swapArray[9] =
          {5,2,6,
           1,0,3,
           8,4,7};
          if(swapArray[i] < gettingText.size())
          {
            trimText = gettingText; panelText += trimText.substr(swapArray[i],1);
            panelText += " ";panelText += " ";
          }else{
            panelText += "　";
          }
        }
      }
      else if(gettingText.size() == 9)//1バイト文字9文字（英語）以下の場合
      {
        panelText ="";
        for(int i=0; i<9; i++){
          int swapArray[9] =
          {2,3,4,
           1,0,5,
           8,7,6};
          if(swapArray[i] < gettingText.size())
          {
            trimText = gettingText; panelText += trimText.substr(swapArray[i],1);
            panelText += " ";panelText += " ";
          }else{
            panelText += "　";
          }
        }
      }
      else if(gettingText.size() == 15)//２バイト文字（日本語）5文字の場合
      {
        panelText ="";

        panelText += "　";//"５";//gettingText.substr(5*3,3).c_str();か
        trimText = gettingText; panelText += trimText.substr(2*3,3);//う
        panelText += "　";//"８";//gettingText.substr(8*3,3).c_str();け

        trimText = gettingText; panelText += trimText.substr(1*3,3);//い
        trimText = gettingText; panelText += trimText.substr(0,3);//あ
        trimText = gettingText; panelText += trimText.substr(3*3,3);//え

        panelText += "　";//"６";//gettingText.substr(6*3,3).c_str();き
        trimText = gettingText; panelText += trimText.substr(4*3,3);//お
        panelText += "　";//"７";//gettingText.substr(7*3,3).c_str();く
      }
      else if(gettingText.size() == 27)//9文字の場合
      {
        panelText ="";

        trimText = gettingText; panelText += trimText.substr(2*3,3);//う
        trimText = gettingText; panelText += trimText.substr(3*3,3);//え
        trimText = gettingText; panelText += trimText.substr(4*3,3);//お

        trimText = gettingText; panelText += trimText.substr(1*3,3);//い
        trimText = gettingText; panelText += trimText.substr(0,3);//あ
        trimText = gettingText; panelText += gettingText.substr(5*3,3);//か

        trimText = gettingText; panelText += gettingText.substr(8*3,3);//け
        trimText = gettingText; panelText += gettingText.substr(7*3,3);//く
        trimText = gettingText; panelText += gettingText.substr(6*3,3);//き
      }

      //panelText.replace( 0, gettingText.size(), gettingText );//　"あああ"　がきたら　"あああ３４５６７８"　のように置き換える

      //以下なぜか動かず、、、
      // if(panelText.size()<27){//3バイト*9文字以下なら
      //   panelText.replace(0,gettingText.size(), gettingText);//　"あああ"　がきたら　"あああ３４５６７８"　のように置き換える
      // }else{
      //   panelText.substr(0,27);//3バイト*9文字のみ切り出す
      // }

      //フリックパネルの文字を描画
        for(int j = 0; j < 3; j++){
          for(int i = 0; i < 3; i++){
            _lcd.setTextSize(1);

            flick_touch_btn_list[ j*3+i ]->setBtnName(panelText.substr((j*3+i)*3,3).c_str());//std::string型の状態で一文字切り出して、Stringに渡す
            // flick_touch_btn_list[ j*3+i ]->setBtnName(panelText.substr(3,3).c_str());//std::string型の状態で一文字切り出して、Stringに渡す

            // String btn_name = flickPanels[showFlickPanelNo]->btn_name.c_str();
            String btn_name = flick_touch_btn_list[ j*3+i ]->getBtnName();

            // Serial.print(panelText.substr((j*3+i)*3,3).c_str());
            // Serial.println(btn_name);

            //flick_touch_btn_list[i*3+j]->btn_mode = BTN_MODE_FLICK;
            // flick_touch_btn_list[j*3+i]->flickDraw( _lcd ); //使わない
            _lcd.fillRoundRect(
              // flick_touch_btn_list[i]->getBtnPos().x,
              // flick_touch_btn_list[i]->getBtnPos().y,
                48*i,
                48*j,
                48,//flick_touch_btn_list[i]->getBtnSize().w,
                48,//flick_touch_btn_list[i]->getBtnSize().h,
                10, TFT_WHITE);

            if(btn_name == NULL){
              btn_name = String(j*3+i);
            }

            _lcd.setTextColor( TFT_BLACK );
            _lcd.setFont(&lgfxJapanGothicP_20);
            int b_hw = 48/2;
            int b_hh = 48/2;
            float b_str_hw = _lcd.textWidth( btn_name ) / 2;
            // _lcd.drawString( flick_touch_btn_list[ j*3+i ]->getBtnName(), 48*i + b_hw - b_str_hw , 48*j + b_hh - 4 );
            _lcd.drawString( btn_name, 48*i + b_hw - b_str_hw , 48*j + b_hh - 4 );
          }
        }

      // _lcd.fillRoundRect(0, 0, 50, 50, 10, TFT_WHITE);
      // _lcd.pushSprite(&_lcd, _x, _y);
    // }
    // else if(!_visible){
      //_lcd.fillRoundRect(flick_touch_btn_list[i]->, 0, 48, 48, 10, TFT_WHITE);
      // _lcd.pushSprite(&_lcd, getBtnPos(_btnID).x, getBtnPos(_btnID).y);
      // _lcd.pushSprite(&_lcd, getBtnPos(_btnID).x, getBtnPos(_btnID).y);
    // }
    }

  }

}
void LovyanGFX_DentaroUI::createOBtns(
  int _r0,int _r1, int _a, int _n, int _eventNo)
  {
    int _x = 125;//初期値を適当に入れておく
    int _y = 245;//初期値を適当に入れておく
    createOBtns(_x, _y, _r0, _r1, _a, _n, _eventNo);
  }

void LovyanGFX_DentaroUI::createOBtns(
  int _x, int _y,  int _r0,int _r1, int _a, int _n, int _eventNo)
  {//円形に並ぶ//中心位置XY、外半径r0、内半径r1、スタート角、分割数

  // int _x, int _y,
  // int _w,int _h,
  // int _row, int _col,
  // LGFX_Sprite& _uiSprite, int _eventNo)
  uiBoxes.push_back(*new UiContainer);

  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "BTN_" + String(uiID);
  Serial.println("BTN_" + String(uiID)  + "=[" + String(uiID) + "]" + String(gBtnID)+"~");

  int _startId = gBtnID;//スタート時のボタンIDをセット
  uiBoxes[uiID].b_sNo = gBtnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _x;// - _r1;
  uiBoxes[uiID].y   = _y;// - _r1;

  // uiBoxes[uiID].w   = _w;
  // uiBoxes[uiID].h   = _h;
  // uiBoxes[uiID].row = _row;
  // uiBoxes[uiID].col = _col;

  uiBoxes[uiID].r0 = _r0;
  uiBoxes[uiID].r1 = _r1;
  uiBoxes[uiID].a  =  _a;
  uiBoxes[uiID].n  =  _n;

  uiBoxes[uiID].eventNo = _eventNo;
  uiBoxes[uiID].toggle_mode = toggle_mode;

  // _uiSprite.setPsram(false);//UNUSE_PSRAM
  // _uiSprite.setPsram(USE_PSRAM);
  // _uiSprite.setColorDepth( COL_DEPTH );
  // _uiSprite.createSprite( 2*uiBoxes[uiID].r1, 2*uiBoxes[uiID].r1 );
  // _uiSprite.createSprite( 150, 150 );

  int b_w = int(uiBoxes[uiID].r0 - uiBoxes[uiID].r1);//ボタン幅を計算

  for(int i= 0; i < uiBoxes[uiID].n; i++)
    {

      int p_btnID = _startId + i;//事前に計算

      touch_btn_list[p_btnID] = NULL;
      touch_btn_list[p_btnID] = new TouchBtn();

      touch_btn_list[p_btnID]->initOBtn(
        p_btnID,
        String(p_btnID),
        i,
        uiBoxes[uiID].a,//描画のスタート角（右から時計回り）
        uiBoxes[uiID].x,
        uiBoxes[uiID].y,
        uiBoxes[uiID].r1,
        uiBoxes[uiID].r0,
        uiBoxes[uiID].n,//分割数
        String(p_btnID),
        layoutSpritePos,
        getTouchPoint(_x, _y),
        TOUCH_OBTN_MODE);

      touch_btn_list[ p_btnID ]->setVisibleF( true );
      touch_btn_list[ p_btnID ]->setAvailableF( true );

      gBtnID++;//ボタンを更新
  }
  uiBoxes[uiID].b_num =  gBtnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
}


void LovyanGFX_DentaroUI::createPanel(
  int _x, int _y,
  int _w,int _h,
  int _row, int _col,
  int _eventNo, int _touchZoom)
{

  touchZoom = _touchZoom;
  addBtnNum += _row *_col;

  Panel* panel = new Panel();  // 新しい Panel オブジェクトを動的に確保
  panel->label = "BTN_" + String(gPanelID);
  Serial.println("BTN_" + String(gPanelID)  + "=[" + String(gPanelID) + "]" + String(gBtnID) + "~");

  // int _startId = gBtnID;
  panel->b_sNo = gBtnID;
  panel->id = gPanelID;
  panel->x = _x * _touchZoom;
  panel->y = _y * _touchZoom;
  panel->w = _w * _touchZoom;
  panel->h = _h * _touchZoom;
  panel->row = _row;
  panel->col = _col;
  panel->eventNo = _eventNo;

  int b_w = int(panel->w / _row);
  int b_h = int(panel->h / _col);

  for (int j = 0; j < _col; j++) {
    for (int i = 0; i < _row; i++) {
      panel->addTouchBtn(gBtnID, panel->x + i * b_w, 
                                 panel->y + j * b_h, b_w, b_h);  // TouchBtn オブジェクトを Panel オブジェクトに追加
      panel->b_num++;//パネルが持つボタンの数を記録
      gBtnID++;
    }
  }
  // gBtnID += panel->b_num;
  // その他のパネルのプロパティの設定など
  // ...

  panels.push_back(panel);  // Panel オブジェクトを panels ベクターに追加

  // panel->b_num =  gBtnID - panel->b_sNo;//UIのボタン数をセット
  Serial.println("e");

  // uiBoxes_num++;
  gPanelID++;

}

void LovyanGFX_DentaroUI::createBtns(
  int _x, int _y,
  int _w,int _h,
  int _row, int _col,
  int _eventNo)
  {//縦方向に並ぶ
  uiBoxes.push_back(*new UiContainer);

  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "BTN_" + String(uiID);
  Serial.println("BTN_" + String(uiID)  + "=[" + String(uiID) + "]" + String(gBtnID)+"~");

  int _startId = gBtnID;//スタート時のボタンIDをセット
  uiBoxes[uiID].b_sNo = gBtnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _x;
  uiBoxes[uiID].y   = _y;
  uiBoxes[uiID].w   = _w;
  uiBoxes[uiID].h   = _h;
  uiBoxes[uiID].row = _row;
  uiBoxes[uiID].col = _col;
  uiBoxes[uiID].eventNo = _eventNo;

  uiBoxes[uiID].toggle_mode = toggle_mode;


  // uiBoxes[uiID].parentID = _parentID;

  // _uiSprite.setPsram(USE_PSRAM);
  // _uiSprite.setPsram(false);//UNUSE_PSRAM
  // _uiSprite.setColorDepth( COL_DEPTH );
  // _uiSprite.createSprite( uiBoxes[uiID].w, uiBoxes[uiID].h );


  int b_w = int(uiBoxes[uiID].w/uiBoxes[uiID].row);
  int b_h = int(uiBoxes[uiID].h/uiBoxes[uiID].col);

  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      int p_btnID = _startId + j * uiBoxes[uiID].row + i;//事前に計算

      touch_btn_list[p_btnID] = NULL;
      touch_btn_list[p_btnID] = new TouchBtn();

      if(toggle_mode == false){
        touch_btn_list[p_btnID]->initBtn(p_btnID, String(p_btnID),
        i*b_w,
        j*b_h,
        b_w,
        b_h,
        String(p_btnID),
        layoutSpritePos,
        // getTouchPoint(_x, _y),
        TOUCH_BTN_MODE);
      }
      else if(toggle_mode == true){
        touch_btn_list[p_btnID]->initBtn(p_btnID, String(p_btnID),
        i*b_w,
        j*b_h,
        b_w,
        b_h,
        String(p_btnID),
        layoutSpritePos,
        // getTouchPoint(_x, _y),
        TOUCH_TOGGLE_MODE);
      }



      touch_btn_list[ p_btnID ]->setVisibleF( true );
      touch_btn_list[ p_btnID ]->setAvailableF( true );

      gBtnID++;//ボタンを更新
    }
  }
  uiBoxes[uiID].b_num =  gBtnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
}

void LovyanGFX_DentaroUI::setAvailableF(int uiID, int _btnID, bool _available){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  touch_btn_list[_startId + _btnID]->setAvailableF(_available);
}

void LovyanGFX_DentaroUI::setAllBtnAvailableF(int uiID, bool _available){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
      for(int j= 0; j < uiBoxes[uiID].col; j++)
      {
        touch_btn_list[_startId + j*uiBoxes[uiID].row + i]->setAvailableF(_available);
      }
    }
}

void LovyanGFX_DentaroUI::setVisibleF(int uiID, int _btnID, bool _visible){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  touch_btn_list[_startId + _btnID]->setVisibleF(_visible);
}

void LovyanGFX_DentaroUI::setAllBtnVisibleF(int uiID, bool _visible){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
      for(int j= 0; j < uiBoxes[uiID].col; j++)
      {
        touch_btn_list[_startId + j*uiBoxes[uiID].row + i]->setVisibleF(_visible);
      }
    }
}

void LovyanGFX_DentaroUI::createBtns(
  int _x, int _y,
  int _w,int _h,int
  _row, int _col,
  int _eventNo, bool _colMode)
  {//縦方向に並ぶ
  if(!_colMode)
  {
    createBtns( _x, _y, _w, _h, _row, _col, _eventNo);
  }
  else if(!_colMode)
  {
    uiBoxes.push_back(*new UiContainer);
    uiBoxes_num++;
    uiID++;
    uiBoxes[uiID].label = "BTN_" + String(uiID);
    Serial.println("BTN_"+String(uiID)  + "=[" + String(uiID) + "]");
    int _startId = gBtnID;//スタート時のボタンIDをセット
    uiBoxes[uiID].b_sNo = gBtnID;
    uiBoxes[uiID].id  = uiID;
    uiBoxes[uiID].x   = _x;
    uiBoxes[uiID].y   = _y;
    uiBoxes[uiID].w   = _w;
    uiBoxes[uiID].h   = _h;
    uiBoxes[uiID].row = _row;
    uiBoxes[uiID].col = _col;
    uiBoxes[uiID].eventNo = _eventNo;
    uiBoxes[uiID].toggle_mode = toggle_mode;

    //btnNum = _row * _col ;
    int b_w = int( uiBoxes[uiID].w / uiBoxes[uiID].row );
    int b_h = int( uiBoxes[uiID].h / uiBoxes[uiID].col );

    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      for(int i= 0; i < uiBoxes[uiID].row; i++)
      {
        //int p_btnID = _startId + j * uiBoxes[uiID].row + i;//事前に計算
        int p_btnID = _startId + i * uiBoxes[uiID].col + j;
        touch_btn_list[p_btnID] = NULL;
        touch_btn_list[p_btnID] = new TouchBtn();

        touch_btn_list[p_btnID]->initBtn( p_btnID, String(p_btnID),
        i*b_w,
        j*b_h,
        b_w,
        b_h,
        String(_startId),
        layoutSpritePos,
        // getTouchPoint(uiBoxes[uiID].x, uiBoxes[uiID].y),
        TOUCH_BTN_MODE);

        // touch_btn_list[ _startId + i * uiBoxes[uiID].col + j ]->setVisibleF( _btnVisibleF );
        // touch_btn_list[ _startId + i * uiBoxes[uiID].col + j ]->setAvailableF( _btnAvailableF );

        //ベータ版は基本true
        touch_btn_list[ p_btnID ]->setVisibleF( true );
        touch_btn_list[ p_btnID ]->setAvailableF( true );
        gBtnID++;
      }
    }
    uiBoxes[uiID].b_num =  gBtnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
  }
}

void LovyanGFX_DentaroUI::setLayoutPos( int _x, int _y )
{
  // uiBoxes[uiID].x   = _x;//タッチ座標
  // uiBoxes[uiID].y   = _y;

  layoutSpritePos.x = _x;//表示用レイアウト座標
  layoutSpritePos.y = _y;
  layoutSprite_w =240;
  layoutSprite_h =240;

  setLayoutPosToAllBtn(getTouchPoint(_x, _y ));
}

void LovyanGFX_DentaroUI::createLayout( int _x, int _y, int _w, int _h, int _eventNo = MULTI_EVENT){
  uiBoxes.push_back(*new UiContainer);
    uiBoxes_num++;
    uiID++;
    Serial.println("LAYOUT_" + String(uiID)  + "=[" + String(uiID) + "]" + String(gBtnID)+"~");
    uiBoxes[uiID].label = "LAYOUT_" + String(uiID);
    uiBoxes[uiID].b_sNo = 0;
    uiBoxes[uiID].id  = uiID;
    uiBoxes[uiID].x   = _x;
    uiBoxes[uiID].y   = _y;
    uiBoxes[uiID].w   = _w;
    uiBoxes[uiID].h   = _h;

    uiBoxes[uiID].eventNo = _eventNo;
    // uiBoxes[uiID].parentID = _parentID;

    layoutSpritePos.x = _x;
    layoutSpritePos.y = _y;
    layoutSprite_w = _w;
    layoutSprite_h = _h;

    // _layoutSprite.setPsram(false);
    // _layoutSprite.setPsram(true);
    // // _layoutSprite.setColorDepth(TILE_COL_DEPTH);//親スプライトの色深度１６
    // _layoutSprite.createSprite(layoutSprite_w, layoutSprite_h);
    // _layoutSprite.setPaletteGrayscale();
}

void LovyanGFX_DentaroUI::createSliders(int _x, int _y, int _w, int _h,  int _row, int _col,
int _xy_mode,
int _eventNo)
{
  uiBoxes.push_back(*new UiContainer);
  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "SLIDER_" + String(uiID);
  Serial.println("SLIDER_" + String(uiID) + "=[" + String(uiID) + "]");
  int _startId = gBtnID;
  uiBoxes[uiID].b_sNo = gBtnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _x;
  uiBoxes[uiID].y   = _y;
  uiBoxes[uiID].w   = _w;
  uiBoxes[uiID].h   = _h;
  uiBoxes[uiID].row = _row;
  uiBoxes[uiID].col = _col;
  uiBoxes[uiID].eventNo = _eventNo;

  //sliderNum = _row * _col ;
  int s_w = int( _w/_row );
  int s_h = int( _h/_col );

  for( int i= 0; i < _row; i++ )
    {
    for( int j= 0; j < _col; j++ )
    {
    gBtnID = _startId + j*_row + i;
    touch_btn_list[_startId + j*_row + i] = NULL;
    touch_btn_list[_startId + j*_row + i] = new TouchBtn();
    touch_btn_list[_startId + j*_row + i]->initSlider( _startId + j*_row + i,
    i*s_w,
    j*s_h,
    s_w,
    s_h,
    String( _startId + j*_row + i ),
    layoutSpritePos,
    getTouchPoint(_x, _y),
    _xy_mode);

    gBtnID++;
    }
  }
  uiBoxes[uiID].b_num =  gBtnID - uiBoxes[uiID].b_sNo;
}



void LovyanGFX_DentaroUI::setQWERTY(int uiID, String _btnsString, LGFX_Sprite& _sprite)
{
  int charsNum = _btnsString.length();
  const char* btnChars = _btnsString.c_str();
  int _id = uiBoxes[uiID].b_sNo;

  for(int i=0; i<charsNum; i++){
    touch_btn_list[_id + i]->setVisibleF(true);
    // touch_btn_list[_id + i]->setAvailableF(true);
    touch_btn_list[_id + i]->setBtnName(String(btnChars[i]));
    //touch_btn_list[_id + i]->setColor(_bgColIndex);
  }
}

void LovyanGFX_DentaroUI::setBtnName( int _btnNo, String _btnName, String _btnNameFalse){
  int _btnId = _btnNo;
  touch_btn_list[_btnId]->setBtnName(_btnName);
  touch_btn_list[_btnId]->setBtnNameFalse(_btnNameFalse);//トグルのfalse状態の時のラベルを設定
}

void LovyanGFX_DentaroUI::setBtnName( int _btnNo, String _btnName)
{
  int _btnId = _btnNo;
  touch_btn_list[_btnId]->setAvailableF(true);
  touch_btn_list[_btnId]->setVisibleF(true);
  // touch_btn_list[_btnId]->setAvailableF(true);
  touch_btn_list[_btnId]->setBtnName(_btnName);
  //touch_btn_list[_btnId]->setColor(_bgColIndex);
}

// void LovyanGFX_DentaroUI::drawBtns(int _uiID, LovyanGFX *_lcd, LGFX_Sprite& _uiSprite)
// {
//   drawBtns(_uiID, _lcd, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
// }

void LovyanGFX_DentaroUI::drawBtns( int uiID, LovyanGFX& _lcd, int _x, int _y)
{
  // lgfx = _lcd;
  if( getEvent() != NO_EVENT )
  {
  if(getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
  {
    int _id = uiBoxes[uiID].b_sNo;
    int _btnNum = uiBoxes[uiID].b_num;
    // _uiSprite.setPivot( 0, 0 );//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

    for(int i= _id; i < _id + _btnNum; i++)
    {
      touch_btn_list[i]->setSelectBtnF(false);
      if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      else touch_btn_list[i]->setSelectBtnF(false);
      touch_btn_list[i]->btnDraw(_lcd, uiBoxes[uiID].x, uiBoxes[uiID].y);

      // if(_showMethod == SHOW_ALL)
      // {
      //   if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      //   else touch_btn_list[i]->setSelectBtnF(false);
      //   touch_btn_list[i]->btnDraw(_uiSprite);
      // }
      // else if(_showMethod == SHOW_NAMED)
      // {
      //   if(touch_btn_list[i]->getAvailableF() == true){
      //     if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      //     else touch_btn_list[i]->setSelectBtnF(false);
      //     touch_btn_list[i]->btnDraw(_uiSprite);
      //   }
      // }
    }
    // _uiSprite.pushSprite(&_lcd, uiBoxes[uiID].x, uiBoxes[uiID].y);
    }
  }
}

void LovyanGFX_DentaroUI::drawSliders(int _uiID, LovyanGFX& _lcd)
{
  drawSliders( _uiID, _lcd, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawSliders(int uiID, LovyanGFX& _lcd, int _x, int _y)
{
  if( getEvent() != NO_EVENT ){
    if( getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
    {
      int _id = uiBoxes[uiID].b_sNo;
      int _btnNum = uiBoxes[uiID].b_num;
      // _uiSprite.setPivot(0, 0);//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

      for(int i = _id; i < _id + _btnNum; i++)
      {
        //    touch_btn_list[i].setSelectBtnF(false);
        touch_btn_list[i]->sliderDraw(_lcd, tp, _x, _y);

        //    if(_showMethod == SHOW_ALL)
        //    {
        //      if(selectBtnID == i)touch_btn_list[i].setSelectBtnF(true);
        //      else touch_btn_list[i].setSelectBtnF(false);
        //      touch_btn_list[i].sliderDraw(_uiSprite);
        //    }
        //    else if(_showMethod == SHOW_NAMED)
        //    {
        //      if(touch_btn_list[i].getAvailableF()==true){
        //        if(selectBtnID == i)touch_btn_list[i].setSelectBtnF(true);
        //        else touch_btn_list[i].setSelectBtnF(false);
        //        touch_btn_list[i].sliderDraw(_uiSprite);
        //      }
        //    }
      }
      // _uiSprite.pushSprite( &_lcd, _x, _y );
    }
  }
  //_layoutSprite.pushSprite(&_lcd, layoutSpritePos.x, layoutSpritePos.y);//最終的な出力
}

void LovyanGFX_DentaroUI::drawBtns(int _uiID, LovyanGFX& _lcd){
  toggle_mode = false;
  drawBtns( _uiID, _lcd, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawToggles(int _uiID, LovyanGFX& _lcd){
  if( getEvent() == TOUCH ){
    if( _uiID >= 0 ){ //NULLを除外
      switchToggleVal();
      drawToggles(_uiID, _lcd, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
    }else{

    }
  }
}

void LovyanGFX_DentaroUI::drawOBtns( int uiID, LovyanGFX& _lcd, int _x, int _y )
{

  if( getEvent() != NO_EVENT )
  {
    if(getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
    {
      int _id = uiBoxes[uiID].b_sNo;
      int _btnNum = uiBoxes[uiID].b_num;
      // _uiSprite.setPivot( 0, 0 );//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

        for(int i= _id; i < _id + _btnNum; i++)
        {
          touch_btn_list[i]->setSelectBtnF(false);
          if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
          else touch_btn_list[i]->setSelectBtnF(false);
          touch_btn_list[i]->btnDraw( _lcd, 0,0); //ボタンエリアの半径
          // touch_btn_list[i]->btnDraw(_uiSprite, uiBoxes[uiID].x, uiBoxes[uiID].y); //スプライト経由で描画する
        }
    }

    int _id = uiBoxes[uiID].b_sNo;

      for(int i= 0; i < uiBoxes[uiID].n; i++)
      {
        int p_btnID = _id + i;//事前に計算
        //タッチポイントをUI表示位置にずらす
        // uiBoxes[uiID].x = _x + _uiSprite.width()/2-uiBoxes[uiID].r0;
        // uiBoxes[uiID].y = _y +_uiSprite.height()/2-uiBoxes[uiID].r0;
        touch_btn_list[p_btnID]->setOBtnPos(
          _x + 180/2,
          _y + 180/2);
      }
  }
}



void LovyanGFX_DentaroUI::drawToggles(int _uiID, LovyanGFX& _lcd, int _x, int _y){

  // if(uiBoxes[_uiID].toggle_mode  == true){
  //   drawBtns(_uiID, _lcd, _uiSprite, _x, _y);
  // }
  toggle_mode = true;
  drawBtns(_uiID, _lcd, _x, _y);
  toggle_mode = false;
}

// void LovyanGFX_DentaroUI::createTile( LGFX_Sprite& _layoutSprite, int _layoutUiID, int _eventNo, int _spriteNo ){//横方向に並ぶ
//   uiBoxes.push_back(*new UiContainer);
//   uiBoxes_num++;
//   uiID++;
//   Serial.println("TILE_" + String(uiID) + "=[" + String(uiID) + "]");
//   int _startId = btnID;
//   uiBoxes[uiID].b_sNo = btnID;
//   uiBoxes[uiID].id  = uiID;//いる？
//   uiBoxes[uiID].x   = uiBoxes[_layoutUiID].x;
//   uiBoxes[uiID].y   = uiBoxes[_layoutUiID].y;
//   uiBoxes[uiID].w   = uiBoxes[_layoutUiID].w;
//   uiBoxes[uiID].h   = uiBoxes[_layoutUiID].h;
//   uiBoxes[uiID].row = 1;
//   uiBoxes[uiID].col = 1;
//   uiBoxes[uiID].eventNo = _eventNo;
//   // uiBoxes[uiID].parentID = _parentID;

//   touch_btn_list[_spriteNo] = NULL;
//   touch_btn_list[_spriteNo] = new TouchBtn();

//   touch_btn_list[_spriteNo] -> initTile(
//     _startId,
//     String(_startId),
//     getTouchPoint(uiBoxes[uiID].x, uiBoxes[uiID].y),
//     uiBoxes[uiID].w,
//     uiBoxes[uiID].h,
//     _layoutSprite,
//     g_basic_sprite_list[_spriteNo]);

//    uiBoxes[uiID].b_num =  1;
// }

void LovyanGFX_DentaroUI::createFlicks(int _x, int _y, int _w,int _h,int _row, int _col, int _eventNo){//縦方向に並ぶ
uiBoxes.push_back(*new UiContainer);
  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "FLICK_" + String(uiID);
  Serial.println("FLICK_" + String(uiID)  + "=[" + String(uiID) + "]" + String(gBtnID)+"~");

  int _startId = gBtnID;//スタート時のボタンIDをセット
  uiBoxes[uiID].b_sNo = gBtnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _x;
  uiBoxes[uiID].y   = _y;
  uiBoxes[uiID].w   = _w;
  uiBoxes[uiID].h   = _h;
  uiBoxes[uiID].row = _row;
  uiBoxes[uiID].col = _col;
  uiBoxes[uiID].eventNo = _eventNo;
  uiBoxes[uiID].toggle_mode = false;

  int b_w = int(uiBoxes[uiID].w/uiBoxes[uiID].row);
  int b_h = int(uiBoxes[uiID].h/uiBoxes[uiID].col);

  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      int p_btnID = _startId + j * uiBoxes[uiID].row + i;
      // std::string stdstr = flickPanels[showFlickPanelNo]->text_list[j * uiBoxes[uiID].row + i].c_str();
      // String str = stdstr.substr(0,3).c_str();//先頭文字をラベルにする

      // touch_btn_list.push_back(new TouchBtn());

      touch_btn_list[p_btnID] = NULL;
      touch_btn_list[p_btnID] = new TouchBtn();
      touch_btn_list[p_btnID]->initBtn(p_btnID, String(p_btnID),
      i*b_w,
      j*b_h,
      b_w,
      b_h,
      "あ",//str, //先頭文字をラベルにする
      layoutSpritePos,
      // getTouchPoint(_x, _y),
      TOUCH_FLICK_MODE);


      // touch_btn_list[ p_btnID ]->setVisibleF( _btnVisibleF );
      // touch_btn_list[ p_btnID ]->setAvailableF( _btnAvailableF );

      //ベータ版は基本true
      touch_btn_list[ p_btnID ]->setVisibleF( true );
      touch_btn_list[ p_btnID ]->setAvailableF( true );

      gBtnID++;//ボタンを更新
    }
  }
  uiBoxes[uiID].b_num =  gBtnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
}

void LovyanGFX_DentaroUI::drawFlicks(int _uiID, LovyanGFX& _lcd){
  toggle_mode = false;

  drawFlicks( _uiID, _lcd, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawFlicks( int uiID, LovyanGFX& _lcd, int _x, int _y)
{
  // lgfx = _lcd;
  if( getEvent() != NO_EVENT ){
    if(getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
    {
    int _id = uiBoxes[uiID].b_sNo;
    int _btnNum = uiBoxes[uiID].b_num;
    // _uiSprite.setPivot( 0, 0 );//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

    for(int i= _id; i < _id + _btnNum; i++)
    {
      touch_btn_list[i]->setSelectBtnF(false);
      if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      else touch_btn_list[i]->setSelectBtnF(false);
      touch_btn_list[i]->btnDraw(_lcd, _x, _y);
    }
    // _uiSprite.pushSprite(&_lcd, _x, _y);
    // _uiSprite.pushSprite(&_lcd, uiBoxes[uiID].x, uiBoxes[uiID].y);
    // _uiSprite.pushSprite(&_lcd, uiBoxes[uiID].x, uiBoxes[uiID].y);
    }
  }
}

// void LovyanGFX_DentaroUI::setPngTile( fs::FS &fs, String _m_url, int _spriteNo ){
//   this->g_basic_sprite_list[_spriteNo].drawPngFile(fs, _m_url,
//                                 0, 0,
//                                 256, 256,
//                                 0, 0, 1.0, 1.0,
//                                 datum_t::top_left);
// }

// void LovyanGFX_DentaroUI::drawTile(int uiID, LovyanGFX* _lcd, LGFX_Sprite& _layoutSprite, uint8_t _bgColIndex, int _spriteNo )//スプライトに描画
// {
//   if( getEvent() != NO_EVENT ){
//     // Serial.println("alive");
//     if( getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
//     {
//       int _id = uiBoxes[uiID].b_sNo;
//       touch_btn_list[_id]->setVisibleF(true);
//       // touch_btn_list[_id]->setAvailableF(true);
//       touch_btn_list[_id]->tileDraw( _lcd, _layoutSprite, layoutSpritePos, sp, _bgColIndex, g_basic_sprite_list[_spriteNo]);
//     }
//   }
// }

void LovyanGFX_DentaroUI::drawLayOut(LGFX& _lcd){
  // if(getEvent() == uiBoxes[uiID].eventNo)
  // {
    // _layoutSprite.pushSprite( &_lcd, layoutSpritePos.x, layoutSpritePos.y );//最終的な出力
  // }
}

bool LovyanGFX_DentaroUI::isAvailable( int _btnID ){
  return touch_btn_list[_btnID]->getAvailableF();
}


void LovyanGFX_DentaroUI::addHandler( int _panelNo, int _btnNo, DelegateBase2* _func, uint16_t _runEventNo, int _parentID, bool _constantGetF){
  runEventNo = _runEventNo;
  _parentID = 0;//ベータ版ではとりあえず強制的にLCDのみのイベントをとる

  int _btnID = panels[_panelNo]->touchBtns[_btnNo]->getBtnID();
  
  panels[_panelNo]->touchBtns[_btnNo]->addHandler(_func);

  if(!_constantGetF){
         if( parentID == PARENT_LCD )    panels[_panelNo]->touchBtns[_btnNo]->run2( _btnID, _btnNo, sp, tp, eventState, runEventNo);//ボタン内のイベントかチェック
    else if( parentID == PARENT_SPRITE ) panels[_panelNo]->touchBtns[_btnNo]->run2( _btnID, _btnNo, sp, tp, eventState, runEventNo);//ボタン内のイベントかチェック
  }
  else if(_constantGetF){//spにtpを入れて、常時取得モードにする
         if( parentID == PARENT_LCD )    panels[_panelNo]->touchBtns[_btnNo]->run2( _btnID, _btnNo, tp, tp, eventState, runEventNo);//ボタン内のイベントかチェック
    else if( parentID == PARENT_SPRITE ) panels[_panelNo]->touchBtns[_btnNo]->run2( _btnID, _btnNo, tp, tp, eventState, runEventNo);//ボタン内のイベントかチェック
  }

  panels[_panelNo]->touchBtns[_btnNo]->delHandlers2();
  
}

// void LovyanGFX_DentaroUI::addHandler( int _btnID, int _btnNo, DelegateBase2* _func, uint16_t _runEventNo, int _parentID){

//   runEventNo = _runEventNo;
//   //btnID = _btnID;
//   _parentID = 0;//ベータ版ではとりあえず強制的にLCDのみのイベントをとる

//   touch_btn_list[_btnID]->addHandler(_func);

//   if( parentID == PARENT_LCD ) touch_btn_list[_btnID]->run2( _btnID, _btnNo, sp, tp, eventState, runEventNo);//ボタン内のイベントかチェック
//   else if( parentID == PARENT_SPRITE ) touch_btn_list[_btnID]->run2( _btnID, _btnNo, sp, tp, eventState, runEventNo);//ボタン内のイベントかチェック

//   touch_btn_list[_btnID]->delHandlers2();
// }

float LovyanGFX_DentaroUI::getAngle(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b ){

  float r = atan2( b.y - a.y, b.x - a.x );
  if (r < 0) {
        r = r + 2 * M_PI;
    }
    return r * 360 / (2 * M_PI);
}

float LovyanGFX_DentaroUI::getDist(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b ){
   float d = sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
    return d;
}

int LovyanGFX_DentaroUI::getPreEvent()
{
  return preEventState;
}

void LovyanGFX_DentaroUI::circle(LovyanGFX* _lcd,  uint16_t c, int fillF)
{
  if(fillF){
    _lcd->fillCircle(tp.x, tp.y, 10, c);
    _lcd->fillCircle(tp.x, tp.y, 10, c);
  }else{
    _lcd->drawCircle(tp.x, tp.y, 10, c);
    _lcd->drawCircle(tp.x, tp.y, 10, c);
  }
}

void LovyanGFX_DentaroUI::rect(LovyanGFX* _lcd, uint16_t c, int fillF)
{
  if(fillF){
    _lcd->fillRect(tp.x, tp.y, 10, 10, c);
    _lcd->fillRect(tp.x, tp.y, 10, 10, c);
  }else{
    _lcd->drawRect(tp.x, tp.y, 10, 10, c);
    _lcd->drawRect(tp.x, tp.y, 10, 10, c);
  }
}

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getPos()
{
  return tp;

}

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getStartPos()
{
  return sp;
}

void LovyanGFX_DentaroUI::setPos(int _x, int _y){
  tp.x = _x;
  tp.y = _y;
}

void LovyanGFX_DentaroUI::setStartPos(int _x, int _y){
  sp.x = _x;
  sp.y = _y;
}

int LovyanGFX_DentaroUI::getTouchBtnID(){
  return obj_ret.btnID;
}


int LovyanGFX_DentaroUI::getTouchBtnNo(){
  return getTouchBtnID() - getUiFirstNo(FlickUiID);
}

int LovyanGFX_DentaroUI::getEvent(){
  return eventState;
}

int LovyanGFX_DentaroUI::getFlickEvent(){
  return flickState;
}

// LGFX_Sprite LovyanGFX_DentaroUI::getTileSprite(int _btnID)
// {
//   return g_basic_sprite;
// }

// void LovyanGFX_DentaroUI::setTilePos(int _id, int _x, int _y)
// {
//   touch_btn_list[_id]->setTilePos(getTouchPoint(_x - layoutSpritePos.x, _y - layoutSpritePos.y));
// }

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getTouchPoint(int _x, int _y){
  lgfx::v1::touch_point_t tp;
  tp.x = _x;
  tp.y = _y;
  return tp;
}

float LovyanGFX_DentaroUI::getSliderVal(int uiID, int _btnNo){
  float retVal = 1.0;
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  if(touch_btn_list[_btnID]->get_xy_mode() == X_VAL){retVal = touch_btn_list[_btnID]->getSliderValx();}
  else if(touch_btn_list[_btnID]->get_xy_mode() == Y_VAL){retVal = touch_btn_list[_btnID]->getSliderValy();}
  return retVal;
}

float LovyanGFX_DentaroUI::getSliderVal(int uiID,int _btnNo, int _xy){
  float retVal = 1.0;
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  if(_xy == X_VAL){retVal = touch_btn_list[_btnID]->getSliderValx();}
  else if(_xy == Y_VAL){retVal = touch_btn_list[_btnID]->getSliderValy();}
  return retVal;
}

Vec2 LovyanGFX_DentaroUI::getSliderVec2(int uiID, int _btnNo){//XYを一気に取得
  Vec2  retPos;
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  if(touch_btn_list[_btnID]->get_xy_mode() == XY_VAL){
    retPos.x = touch_btn_list[_btnID]->getSliderValx();
    retPos.y = touch_btn_list[_btnID]->getSliderValy();
  }
  return retPos;
}

void LovyanGFX_DentaroUI::setSliderVal(int uiID, int _btnNo, float _x, float _y){
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  touch_btn_list[_btnID]->setSliderVal(_x, _y);
}


void LovyanGFX_DentaroUI::setCharMode(int _charMode){
  charMode = _charMode;
}

int LovyanGFX_DentaroUI::getCharMode(){
  return charMode;
}

bool LovyanGFX_DentaroUI::getToggleVal(int _uiID, int _btnNo){
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  // touch_btn_list[_btnID]->switchToggleVal();//値をスイッチしておいて
  return touch_btn_list[_btnID]->getToggleVal();//スイッチした値をリターンする
}

// bool LovyanGFX_DentaroUI::getToggleVal(int _btnID){
//   // touch_btn_list[_btnID]->switchToggleVal();//値をスイッチしておいて
//   return touch_btn_list[_btnID]->getToggleVal();
// }

// bool LovyanGFX_DentaroUI::getToggleVal2(){
//   return obj_ret.toggleVal;
// }

int LovyanGFX_DentaroUI::getUiFirstNo(int uiID){
  return uiBoxes[uiID].b_sNo;
}

int LovyanGFX_DentaroUI::getflickPanelBtnNo(int uiID){
  return flickPanels[uiID]->b_No;
}

int LovyanGFX_DentaroUI::getUiBtnNum(int uiID){
  return uiBoxes[uiID].b_num;
}

int LovyanGFX_DentaroUI::getUiBoxNum(){
//  return uiBoxes.size();
  return uiBoxes_num;
}

int LovyanGFX_DentaroUI::getAllBtnNum(){
  return gBtnID;
}

int LovyanGFX_DentaroUI::getParentID(){
  return parentID;
}

int LovyanGFX_DentaroUI::getUiID( const char* _uiLabel){
  int i = 0;
  while(uiBoxes[i].label != String(_uiLabel)){
    i++;
  }
  return i;
}

void LovyanGFX_DentaroUI::setBtnID(int _btnID){
  obj_ret.setBtnID_ret(_btnID);
}


lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getBtnPos(int _btnID){
  return getTouchPoint(touch_btn_list[_btnID]->getBtnPos().x, touch_btn_list[_btnID]->getBtnPos().y);
}

void LovyanGFX_DentaroUI::setUiLabels(int uiID, int _showFlickPanelNo)
{
  showFlickPanelNo = _showFlickPanelNo;

  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      // int p_btnID = uiBoxes[uiID].b_sNo + j * uiBoxes[uiID].row + i;
      int p_btnID = uiBoxes[uiID].b_sNo + j * uiBoxes[ uiID ].row + i;
      std::string stdstr = flickPanels[ showFlickPanelNo ]->text_list[ j * uiBoxes[uiID].row + i ].c_str();
      String str = stdstr.substr( 0, 3 ).c_str();//先頭文字をラベルにする
      touch_btn_list[ p_btnID ]->setBtnName( str );
    }
  }
}

// void LovyanGFX_DentaroUI::showInfo(LGFX& _lcd , int _infox, int _infoy){
//   //showTouchEventInfo( _lcd, _lcd.width() - 100, 0 );//タッチイベントを視覚化する
//   //フレームレート情報などを表示します。
//   _lcd.fillRect( _infox, _infoy, 100, 10, TFT_BLACK );
//   _lcd.setTextSize(2);
//   _lcd.setTextColor( TFT_WHITE );
//   _lcd.setCursor( _infox + 1, _infoy + 1 );
//   _lcd.print( fps );
//   _lcd.print( ":" );
//   _lcd.print( String( getEvent() ) );
//   _lcd.print( "[" );
//   _lcd.print( String( getPreEvent() ) );
//   _lcd.print( "]:BTN" );
//   _lcd.println( String( getTouchBtnID() ) );
//   ++frame_count;sec = millis() / 1000;
//   if ( psec != sec ) {
//     psec = sec; fps = frame_count;
//     frame_count = 0;
//     _lcd.setAddrWindow( 0, 0, _lcd.width(), _lcd.height() );
//     //_lcd.setAddrWindow( 0, 0, 240, 320 );
//   }
// }


void LovyanGFX_DentaroUI::showInfo(LovyanGFX& _lcd , int _infox, int _infoy){
  //フレームレート情報などを表示します。
  _lcd.setTextSize(1);
  _lcd.setFont(&lgfxJapanGothicP_8);
  _lcd.fillRect( 0, 0, 150, 10, TFT_BLACK );
  _lcd.setTextColor( TFT_WHITE );
  _lcd.setCursor( 1,1 );
  _lcd.print( fps );
  _lcd.print( ":" );
  _lcd.print( String( getEvent() ) );
  _lcd.print( "[" );
  _lcd.print( String( getPreEvent() ) );
  _lcd.print( "]:BTN" );
  _lcd.print( "[" );
  _lcd.print( String( getFlickEvent() ) );
  _lcd.print( "]:BTN" );
  _lcd.print( String( getTouchBtnID() ) );
  _lcd.print( ":" );
  // _lcd.println( String( getToggleVal(1,0) ) );
  ++frame_count;sec = millis() / 1000;
  if ( psec != sec ) {
    psec = sec; fps = frame_count;
    frame_count = 0;
    _lcd.setAddrWindow( 0, 0, _lcd.width(), _lcd.height() );
  }
}

String LovyanGFX_DentaroUI::getHenkanChar(int _henkanListNo, int _kanaShiftNo){
return kanalist[_henkanListNo][_kanaShiftNo];
}


void LovyanGFX_DentaroUI::setFlickPanels(){
  //パネル番号、ボタン番号、表示文字９個
  //ui.setFlickPanel(0, 0, "あいうえおかきくけ");//9文字はいまのところ未対応

  // int _b_sNo = uiBoxes[FlickUiID].b_sNo;//FlickUiIDは1
  // int _b_sNo = getUiFirstNo(FlickUiID);
  //Serial.println(getUiFirstNo(getUiID("FLICK_2")));
  // Serial.println(getUiID("FLICK_2"));
  int _b_sNo = uiBoxes[getUiID("FLICK_2")].b_sNo;
  // int _b_sNo = 1;//

  setCharMode(CHAR_3_BYTE);
  // //全角5文字になるように空白を入れて文字を登録
  // flickPanels[0]->new_text_list(_b_sNo, "あいうえお");

  setFlickPanel(0, _b_sNo,   "あいうえお");//第一引数はSHIFT_ID
  setFlickPanel(0, _b_sNo+1, "かきくけこ");
  setFlickPanel(0, _b_sNo+2, "さしすせそ");
  setFlickPanel(0, _b_sNo+3, "たちつてと");
  setFlickPanel(0, _b_sNo+4, "なにぬねの");
  setFlickPanel(0, _b_sNo+5, "はひふへほ");
  setFlickPanel(0, _b_sNo+6, "まみむめも");
  setFlickPanel(0, _b_sNo+7, "や「ゆ」よ");
  setFlickPanel(0, _b_sNo+8, "らりるれろ");
  setFlickPanel(0, _b_sNo+9, "～（・）　");
  setFlickPanel(0, _b_sNo+10, "わをんー　");
  setFlickPanel(0, _b_sNo+11, "、。？！　");

  setFlickPanel(1, _b_sNo+0, "アイウエオ");
  setFlickPanel(1, _b_sNo+1, "カキクケコ");
  setFlickPanel(1, _b_sNo+2, "サシスセソ");
  setFlickPanel(1, _b_sNo+3, "タチツテト");
  setFlickPanel(1, _b_sNo+4, "ナニヌネノ");
  setFlickPanel(1, _b_sNo+5, "ハヒフヘホ");
  setFlickPanel(1, _b_sNo+6, "マミムメモ");
  setFlickPanel(1, _b_sNo+7, "ヤ「ユ」ヨ");
  setFlickPanel(1, _b_sNo+8, "ラリルレロ");
  setFlickPanel(1, _b_sNo+9, "％＆￥―　");
  setFlickPanel(1, _b_sNo+10, "ワヲンー　");
  setFlickPanel(1, _b_sNo+11, "、。？！　");

  setCharMode(CHAR_1_BYTE);
  //半角5文字になるように空白を入れて文字を登録
  setFlickPanel(2, _b_sNo+0, "@#|&_");
  setFlickPanel(2, _b_sNo+1, "ABC  ");
  setFlickPanel(2, _b_sNo+2, "DEF  ");
  setFlickPanel(2, _b_sNo+3, "GHI  ");
  setFlickPanel(2, _b_sNo+4, "JKL  ");
  setFlickPanel(2, _b_sNo+5, "MNO  ");
  setFlickPanel(2, _b_sNo+6, "PQRS ");
  setFlickPanel(2, _b_sNo+7, "TUV  ");
  setFlickPanel(2, _b_sNo+8, "WXYZ ");
  setFlickPanel(2, _b_sNo+9, "^[$] ");//toggleボタンに
  setFlickPanel(2, _b_sNo+10, "'<\"> ");
  setFlickPanel(2, _b_sNo+11, ".,?! ");

  setFlickPanel(3, _b_sNo+0, "@#%&_");
  setFlickPanel(3, _b_sNo+1, "abc");
  setFlickPanel(3, _b_sNo+2, "def  ");
  setFlickPanel(3, _b_sNo+3, "ghi  ");
  setFlickPanel(3, _b_sNo+4, "jkl  ");
  setFlickPanel(3, _b_sNo+5, "mno  ");
  setFlickPanel(3, _b_sNo+6, "pqrs ");
  setFlickPanel(3, _b_sNo+7, "tuv  ");
  setFlickPanel(3, _b_sNo+8, "wxyz ");
  setFlickPanel(3, _b_sNo+9, "-(/) ");
  setFlickPanel(3, _b_sNo+10, ":;'\" ");
  setFlickPanel(3, _b_sNo+11, ".,?! ");

  setFlickPanel(4, _b_sNo+0, "0    ");
  setFlickPanel(4, _b_sNo+1, "1    ");
  setFlickPanel(4, _b_sNo+2, "2    ");
  setFlickPanel(4, _b_sNo+3, "3    ");
  setFlickPanel(4, _b_sNo+4, "4    ");
  setFlickPanel(4, _b_sNo+5, "5    ");
  setFlickPanel(4, _b_sNo+6, "6    ");
  setFlickPanel(4, _b_sNo+7, "7    ");
  setFlickPanel(4, _b_sNo+8, "8    ");
  setFlickPanel(4, _b_sNo+9, "9    ");
  setFlickPanel(4, _b_sNo+10, "+-*/ ");
  setFlickPanel(4, _b_sNo+11, ".(=) ");
  }

  void LovyanGFX_DentaroUI::setFlick( int _TopBtnUiID, int _FlickUiID, int _LeftBtnUiID, int _RightBtnUiID )
  {
    setFlick( CHAR_3_BYTE, _TopBtnUiID, _FlickUiID, _LeftBtnUiID, _RightBtnUiID );
  }

  void LovyanGFX_DentaroUI::setFlick( int _charMode, int _TopBtnUiID, int _FlickUiID, int _LeftBtnUiID, int _RightBtnUiID )
  {
    TopBtnUiID = _TopBtnUiID;
    FlickUiID = _FlickUiID;
    LeftBtnUiID = _LeftBtnUiID;
    RightBtnUiID = _RightBtnUiID;

    setBtnName( uiBoxes[LeftBtnUiID].b_sNo,   "Nxt"  );
    setBtnName( uiBoxes[LeftBtnUiID].b_sNo+1, "a/A"  );
    setBtnName( uiBoxes[LeftBtnUiID].b_sNo+2, "SFT"  );
    setBtnName( uiBoxes[LeftBtnUiID].b_sNo+3, "゛小" );

    setBtnName( uiBoxes[RightBtnUiID].b_sNo,   "del" );
    setBtnName( uiBoxes[RightBtnUiID].b_sNo+1, "_"   );
    setBtnName( uiBoxes[RightBtnUiID].b_sNo+2, "Clr" );
    setBtnName( uiBoxes[RightBtnUiID].b_sNo+3, "Ret" );

    if(_charMode == CHAR_1_BYTE){
      fpNo=2;//英語モードに切り替える
    }else if(_charMode == NUMERIC){
      fpNo=4;//数値モードに切り替える
    }else{
      setCharMode(_charMode);//指定のパネルモードに切り替える
    }

    setUiLabels( FlickUiID, fpNo );
    // drawFlicks( getUiID("FLICK_2"), _layoutSprite, ui_sprite0 );
  }

String LovyanGFX_DentaroUI::getInvisibleFlickStrings(){
  String invisibleStr = "";
  //最後の文字のバイト数を判定する
  setlocale(LC_ALL, "");
  std::vector<std::string> ret = split_mb(flickStrDel.c_str(),"\n");

  if(ret.size() >= 1){

  for (size_t i = 0; i < ret.size()-1; i++) {
    invisibleStr += "*";
  }

  invisibleStr += String(ret[ret.size()-1].c_str());
  }

  return invisibleStr;
}

String LovyanGFX_DentaroUI::getFlickString(bool _visibleMode){
  if(_visibleMode == INVISIBLE)return getInvisibleFlickStrings();
  else return flickStr;
}

String LovyanGFX_DentaroUI::getFlickString(){
  return flickStr;
}

String LovyanGFX_DentaroUI::getFlickChar(){

  String RetChar = "";
  if(!selectModeF){
    RetChar = preFlickChar;
  }else if(selectModeF){
    RetChar = previewFlickChar;
  }
  return RetChar;
}

String LovyanGFX_DentaroUI::delEndChar(String _str, int _ByteNum){
  std::string stdFlickStr = _str.c_str();
  for( int i = 0; i < _ByteNum; i++ ){
    stdFlickStr.erase( --stdFlickStr.end() );
  }
  return stdFlickStr.c_str();
}

void LovyanGFX_DentaroUI::setLayoutPosToAllBtn( lgfx::v1::touch_point_t  _layoutPos ){
  for(int i=0; i < getAllBtnNum(); i++){
    touch_btn_list[ i ]->setlayoutSpritePos(_layoutPos);
  }
}

void LovyanGFX_DentaroUI::switchToggleVal()
{
  if(getTouchBtnID()>=0){ //NULLを除外
    touch_btn_list[getTouchBtnID()]->switchToggleVal();
  }else{

  }

}

//---MAP用関数

void LovyanGFX_DentaroUI::nowLoc(LovyanGFX& _lcd)
{
    _lcd.fillTriangle(110, 106, 130, 106, 120, 120, TFT_RED);
    _lcd.fillCircle(120, 120 - 18, 10, TFT_RED);
    _lcd.fillCircle(120, 120 - 18, 6, TFT_WHITE);
}

void LovyanGFX_DentaroUI::drawTile(int uiID, LovyanGFX& _lcd, LGFX_Sprite& _layoutSprite, uint8_t _bgColIndex, int _spriteNo )//スプライトに格納
{
  if( getEvent() != NO_EVENT )
  {
    if( getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
    {
      int _id = uiBoxes[uiID].b_sNo;
      touch_btn_list[_id]->setVisibleF(true);
      touch_btn_list[_id]->tileDraw( _lcd, _layoutSprite, layoutSpritePos, sp, _bgColIndex, *MapTiles[_spriteNo]->getSpritePtr());
    }
  }
}

void LovyanGFX_DentaroUI::drawTileAuto(int uiID, LovyanGFX& _lcd, LGFX_Sprite& _layoutSprite, int _bgColIndex, int _spriteNo )//スプライトに格納
{
  int _id = uiBoxes[uiID].b_sNo;
  // touch_btn_list[_id]->setVisibleF(false);
  touch_btn_list[_id]->setVisibleF(true);

   touch_btn_list[_id]->setDrawFinishF(false);

  touch_btn_list[_id]->tileDraw( _lcd, _layoutSprite, layoutSpritePos, sp, _bgColIndex, *MapTiles[_spriteNo]->getSpritePtr());

  while(touch_btn_list[_id]->getDrawFinishF() == false)
          {
          //読み込み完了を待つだけ
            delay(1);
          }
  touch_btn_list[_id]->setDrawFinishF(true);
}

int LovyanGFX_DentaroUI::getPositionNo(int _addXTileNo, int _addYTileNo)//スプライトに高速描画
{
  int _posNo = 0;
       if(_addXTileNo ==  0 && _addYTileNo ==  0){_posNo = 0;}
  else if(_addXTileNo ==  1 && _addYTileNo ==  0){_posNo = 1;}
  else if(_addXTileNo ==  0 && _addYTileNo ==  1){_posNo = 2;}
  else if(_addXTileNo == -1 && _addYTileNo ==  0){_posNo = 3;}
  else if(_addXTileNo ==  0 && _addYTileNo == -1){_posNo = 4;}
  else if(_addXTileNo ==  1 && _addYTileNo == -1){_posNo = 5;}
  else if(_addXTileNo ==  1 && _addYTileNo ==  1){_posNo = 6;}
  else if(_addXTileNo == -1 && _addYTileNo ==  1){_posNo = 7;}
  else if(_addXTileNo == -1 && _addYTileNo == -1){_posNo = 8;}
  return _posNo;
}

int LovyanGFX_DentaroUI::getAddX(int _spriteNo)
{
  int addX = MapTiles[_spriteNo]->getAddX();
  return addX;
}

int LovyanGFX_DentaroUI::getAddY(int _spriteNo)
{
  int addY = MapTiles[_spriteNo]->getAddY();
  return addY;
}

void LovyanGFX_DentaroUI::setAddX(int _objId,  int _xtileNo)
{
  MapTiles[_objId]->setAddX(_xtileNo);
}

void LovyanGFX_DentaroUI::setAddY(int _objId,  int _ytileNo)
{
  MapTiles[_objId]->setAddY(_ytileNo);
}

//---------

int LovyanGFX_DentaroUI::getPreAddX(int _spriteNo)
{
  int addX = MapTiles[_spriteNo]->getPreAddX();
  return addX;
}

int LovyanGFX_DentaroUI::getPreAddY(int _spriteNo)
{
  int addY = MapTiles[_spriteNo]->getPreAddY();
  return addY;
}

void LovyanGFX_DentaroUI::setPreAddX(int _objId,  int _xtileNo)
{
  MapTiles[_objId]->setPreAddX(_xtileNo);
}

void LovyanGFX_DentaroUI::setPreAddY(int _objId,  int _ytileNo)
{
  MapTiles[_objId]->setPreAddY(_ytileNo);
}

//---------

int LovyanGFX_DentaroUI::get_gPosId()
{
  return gPosId;
}

void LovyanGFX_DentaroUI::set_gPosId(int _gPosId)
{
  gPosId = _gPosId;
}

//---------

void LovyanGFX_DentaroUI::setDrawFinishF(int _objId, bool _drawFinishF){
  touch_btn_list[_objId]->setDrawFinishF(_drawFinishF);
}

bool LovyanGFX_DentaroUI::getDrawFinishF(int _objId){
  return touch_btn_list[_objId]->getDrawFinishF();
}

//---------

void LovyanGFX_DentaroUI::setXtileNo(int _objId,  int _xtileNo)
{
  MapTiles[_objId]->setXtileNo(_xtileNo);
}

void LovyanGFX_DentaroUI::setPreXtileNo(int _objId,  int _preXtileNo)
{
  MapTiles[_objId]->setPreXtileNo(_preXtileNo);
}

void LovyanGFX_DentaroUI::setYtileNo(int _objId,  int _ytileNo)
{
  MapTiles[_objId]->setYtileNo(_ytileNo);
}

void LovyanGFX_DentaroUI::setPreYtileNo(int _objId,  int _preYtileNo)
{
  MapTiles[_objId]->setPreYtileNo(_preYtileNo);
}

void LovyanGFX_DentaroUI::setTileNo(int _objId, int _xtileNo, int _ytileNo)
{
  MapTiles[_objId]->setXtileNo(_xtileNo);
  MapTiles[_objId]->setYtileNo(_ytileNo);
}

// void LovyanGFX_DentaroUI::setMapName(int objId,  int _xtileNo,  int _ytileNo){
//   // MapTiles[_objId]->setXtileNo(_xtileNo);
//   // MapTiles[_objId]->setYtileNo(_ytileNo);
//   // MapTiles[_objId]->setMapName(_xtileNo, _ytileNo);
// }

void LovyanGFX_DentaroUI::setXtilePos(int _objId,  int _xtilePos)
{
  MapTiles[_objId]->setXtilePos(_xtilePos);
}

void LovyanGFX_DentaroUI::setYtilePos(int _objId,  int _ytilePos)
{
  MapTiles[_objId]->setYtilePos(_ytilePos);
}


int LovyanGFX_DentaroUI::getXtile(){
  return xtile;
}

int LovyanGFX_DentaroUI::getYtile(){
  return ytile;
}

int LovyanGFX_DentaroUI::getZtile(){
  return ztile;
}

int LovyanGFX_DentaroUI::getXtileNo(){
  return xtileNo;
}

int LovyanGFX_DentaroUI::getYtileNo(){
  return ytileNo;
}

int LovyanGFX_DentaroUI::getXtileNo(int _objNo){
  return MapTiles[_objNo]->getXtileNo();
}

int LovyanGFX_DentaroUI::getYtileNo(int _objNo){
  return MapTiles[_objNo]->getYtileNo();
}

int LovyanGFX_DentaroUI::getPreXtileNo(int _objNo){
  return MapTiles[_objNo]->getPreXtileNo();
}

int LovyanGFX_DentaroUI::getPreYtileNo(int _objNo){
  return MapTiles[_objNo]->getPreYtileNo();
}

int LovyanGFX_DentaroUI::getPreXtileNo(){
  return preXtileNo;
}

int LovyanGFX_DentaroUI::getPreYtileNo(){
  return preYtileNo;
}

int LovyanGFX_DentaroUI::getPreXtile(){
  return preXtile;
}

int LovyanGFX_DentaroUI::getPreYtile(){
  return preYtile;
}


int LovyanGFX_DentaroUI::getPreDirID(){
  return preDirID;
}

int LovyanGFX_DentaroUI::getVx(){
  return vx;
}

int LovyanGFX_DentaroUI::getVy(){
  return vy;
}

void LovyanGFX_DentaroUI::setExistF(int _objId, bool _existF){
  MapTiles[_objId]->setExistF(_existF);
}

bool LovyanGFX_DentaroUI::getExistF(int _objId){
  return MapTiles[_objId]->getExistF();
}

void LovyanGFX_DentaroUI::setPreDirID(int _dirID){
  preDirID = _dirID;
}

int LovyanGFX_DentaroUI::getDirID(){


    vx = getXtile() - getPreXtile();
    vy = getYtile() - getPreYtile();

    float r = atan2( vy, vx );
    if (r < 0) { r = r + 2 * M_PI; }
    float vecAngle = r * 360 / (2 * M_PI);

    // preDirID = dirID;

         if(vecAngle < 15||vecAngle >= 345)   { dirID = 1;}
    else if(vecAngle >= 15  && vecAngle < 75 ){ dirID = 2; }
    else if(vecAngle >= 75  && vecAngle < 105){ dirID = 3; }
    else if(vecAngle >= 105 && vecAngle < 165){ dirID = 4; }
    else if(vecAngle >= 165 && vecAngle < 195){ dirID = 5; }
    else if(vecAngle >= 195 && vecAngle < 255){ dirID = 6; }
    else if(vecAngle >= 255 && vecAngle < 285){ dirID = 7; }
    else if(vecAngle >= 285 && vecAngle < 345){ dirID = 8; }

    return dirID;
}


bool LovyanGFX_DentaroUI::getDownloadF()
{
  return DownloadF;
}

void LovyanGFX_DentaroUI::setDownloadF(bool _b)
{
  DownloadF = _b;
}

// int LovyanGFX_DentaroUI::get_gPosId()
// {
//   return gPosId;
// }


// int LovyanGFX_DentaroUI::getXtileNo(int _objNo){
//   return MapTiles[_objNo]->getXtileNo();
// }

// int LovyanGFX_DentaroUI::getYtileNo(int _objNo){
//   return MapTiles[_objNo]->getYtileNo();
// }

void LovyanGFX_DentaroUI::setPngTile( fs::FS &fs, String _m_url, int _spriteNo )
{
  MapTiles[_spriteNo]->getSpritePtr()->drawPngFile(fs, _m_url,
                                0, 0,
                                256, 256,
                                0, 0, 1.0, 1.0,
                                datum_t::top_left);
}

void LovyanGFX_DentaroUI::task2_setPngTile(int _posId)
{
    ROI_m_url = "/tokyo/" + String(getZtile()) + "/"+String(getXtileNo(_posId)) + "/"+String(getYtileNo(_posId)) + ".png";
    setPngTile( SD, ROI_m_url, _posId );//SDからの地図の読み込み

}

void LovyanGFX_DentaroUI::drawMaps(LGFX& _lcd, double _walkLatPos, double _walkLonPos, int _tileZoom){

  for(int objId = 0; objId < BUF_PNG_NUM; objId++)
    {
      setPreXtileNo(objId, getXtileNo(objId));//過去の名前（位置）を保存
      setPreYtileNo(objId, getYtileNo(objId));//過去の名前（位置）を保存
    }

  getTilePos(_walkLatPos, _walkLonPos, _tileZoom);//経緯度からタイル座標を計算

   //9枚のマップ座標取得
   for(int objId = 0; objId < BUF_PNG_NUM; objId++){
     setAddX(objId, addTPosList[objId][0]);//相対位置情報を登録
     setAddY(objId, addTPosList[objId][1]);//相対位置情報を登録
     setXtileNo(objId, getXtileNo() + getAddX(objId));//名前（位置）を登録
     setYtileNo(objId, getYtileNo() + getAddY(objId));//名前（位置）を登録

     matrix_list[objId][2] = 120 + (getAddX(objId)*255) - getXtile()%255;
     matrix_list[objId][5] = 120 + (getAddY(objId)*255) - getYtile()%255;
     setXtilePos(objId,  matrix_list[objId][2]);
     setYtilePos(objId,  matrix_list[objId][5]);
   }

   if(getXtileNo() != getPreXtileNo()||getYtileNo() != getPreYtileNo()){
     mataidaF = true;
     //Serial.println("mataida!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
   }else{
     mataidaF = false;
   }

   for(int objId = 0; objId < BUF_PNG_NUM; objId++)
   {
     //ディスプレイ内にはいっていて
     //まだ読み込んでいない場合
     setExistF(objId, false);
       for(int i=0; i < 9; i++){
         if(getXtileNo(objId) == preReadXtileNo[i] && getYtileNo(objId) == preReadYtileNo[i])
         {

           setExistF(objId, true);
           break;
         }else{

           setExistF(objId, false);
         }
       }

       if(getExistF(objId)==false)
       {

         preReadXtileNo[objId] = -1;
         preReadYtileNo[objId] = -1;

       }
     }

//---------------------------------------

   if(!mataidaF){
     //地図をまたいでいない時の処理
     //描画
     for(int objId = 0; objId < BUF_PNG_NUM; objId++)
     {
        for(int j = 0; j<6; j++ )matrix[j] = matrix_list[objId][j];//位置差分番号からスプライトを置く位置番号を特定し、実際の位置を登録
        layoutSprite_list[objId].pushAffine( &_lcd, matrix );//0~9までのスプライトを差分位置に描画
     }
   }

//---------------------------------------

   for(int objId = 0; objId < BUF_PNG_NUM; objId++)
   {
         if(mataidaF){
           //読み込む
           set_gPosId(objId);

           //上書き可能リストに、データ読み込み
           //マップID取得

           setDownloadF(true);//trueにするだけでタスク２が１回作動する

           while(getDownloadF() == true)
           {
           //読み込み完了を待つだけ
             delay(1);
           }

           drawTileAuto(
             objId + BUF_PNG_NUM,
             _lcd,
             layoutSprite_list[objId],
             TFT_ORANGE,
             objId);

        //  Serial.print(objId);
        Serial.print("[");
        Serial.print(getXtileNo());
        Serial.print(":");
        Serial.print(getYtileNo());
        Serial.print("]");
        Serial.println("");

         preReadXtileNo[objId] = getXtileNo(objId);
         preReadYtileNo[objId] = getYtileNo(objId);
         setExistF(objId, true);
         }
  }

}

//経緯度からタイル座標を求める
void LovyanGFX_DentaroUI::getTilePos(double _lat, double _lon, int zoom_level)
{
  //setLatLonPos(lat, lon);
  // _lat = lat;
  // _lon = lon;

  // //座標を含むタイル番号を計算
  // double lat_rad = _lat * (M_PI/180);
  // double n = _lcdpow(2, zoom_level);
  // xtileNo = int((_lon + 180.0) / 360.0 * n);
  // ytileNo = int((1.0 - log(tan(lat_rad) + (1 / cos(lat_rad))) / M_PI) / 2.0 * n);
  // ztile = zoom_level;

  // 緯度経度からタイル座標に変換
  double x = (_lon / 180 + 1) * pow(2, zoom_level) / 2;
  xtile = int(x*255);
  double y = ((-log(tan((45 + _lat / 2) * M_PI / 180)) + M_PI) * pow(2, zoom_level) / (2 * M_PI));
  ytile = int(y*255);
  ztile = zoom_level;

  // //座標を含むタイル番号を計算
  //x = (_lon / 180 + 1) * pow(2, zoom_level-1) / 2;
  // xtileNo = int(x);
  //y = ((-log(tan((45 + _lat / 2) * M_PI / 180)) + M_PI) * pow(2, zoom_level-1) / (2 * M_PI));
  // ytileNo = int(y);

// //現在地を含むタイル番号を計算
  double lat_rad = _lat * (M_PI/180);
  double n = pow(2, zoom_level);
  xtileNo = int((_lon + 180.0) / 360.0 * n);
  ytileNo = int((1.0 - log(tan(lat_rad) + (1 / cos(lat_rad))) / M_PI) / 2.0 * n);
}

void LovyanGFX_DentaroUI::updateOBtnSlider( int uiID, int _x, int _y )
{
uiBoxes[uiID].c_d = sqrt( ( pow( tp.x - (_x + uiBoxes[uiID].r1 ), 2.0 ) + pow( tp.y- (_y + uiBoxes[uiID].r1 ), 2.0 ) ));
if(uiBoxes[uiID].c_d > uiBoxes[uiID].r0 && uiBoxes[uiID].c_d < uiBoxes[uiID].r1)
  {
    if( getEvent() == MOVE )
    {


    uiBoxes[uiID].c_a = int( ( atan2( tp.y - (_y + uiBoxes[uiID].r1 ) ,
                                      tp.x - (_x + uiBoxes[uiID].r1 ) ) * 180/M_PI ) );//現在のボタン中心の角度を登録

    if(uiBoxes[uiID].c_a < 0) uiBoxes[uiID].c_a = 180 + ( 180 + uiBoxes[uiID].c_a );//0~360の値に収める

    // if( abs(uiBoxes[uiID].c_a - uiBoxes[uiID].p_a) > 0 ){

      uiBoxes[uiID].diff_a = uiBoxes[uiID].c_a - uiBoxes[uiID].p_a;//差分を計算する

      // Serial.println(uiBoxes[uiID].diff_a);

      // if( getEvent() == TOUCH ){
        // uiBoxes[uiID].p_a = uiBoxes[uiID].c_a;//タッチした時のボタン中心の角度を登録しておく
      // }


      if(uiBoxes[uiID].diff_a > 180){
        uiBoxes[uiID].diff_a -= 360;
      }else if(uiBoxes[uiID].diff_a < -180){
        uiBoxes[uiID].diff_a += 360;
      }

      // Serial.println(uiBoxes[uiID].diff_a);

      uiBoxes[uiID].p_a = uiBoxes[uiID].c_a;
      // Serial.print(uiBoxes[uiID].p_a);
      // Serial.print(":");
    }
    else if( getEvent() == RELEASE )
    {
      uiBoxes[uiID].diff_a = 0;
    }

  }
}

int LovyanGFX_DentaroUI::getCurrentAngle(int uiID){
    return uiBoxes[uiID].c_a;
}

int LovyanGFX_DentaroUI::getOBtnDiffAngle(int uiID){
    return uiBoxes[uiID].diff_a;
  // diff_a = c_a - s_a;

  // if(diff_a > 180){
  //   diff_a -= 360;
  // }else if(diff_a < -180){
  //   diff_a += 360;
  // }

  // return diff_a;
}

void LovyanGFX_DentaroUI::setEventBit(int bitNo, bool inputBit){

  if ( inputBit == 1 )
  {
    bitSet(eventBits, bitNo-1);//右０ビット目を１（Set）に
  }
  else if ( inputBit == 0 )
  {
    bitClear(eventBits, bitNo);//右０ビット目を0（Clear）に
  }

}

bool LovyanGFX_DentaroUI::getEventBit(int bitNo)
{
  uint32_t b = eventBits;//コピー
  //uint32_t mb = 0b1;//マスク用
  bool rb = 0b1 & (b >> bitNo-1);//ビットシフトして、マスクで１ビット目を抽出してboolにキャスト
  return  rb;
}

void LovyanGFX_DentaroUI::resetEventBits()
{
  eventBits = 0b00000000000000000000000000000000;
}

void LovyanGFX_DentaroUI::clearAddBtns() {
  int count = uiBoxes.size();
  int count2 = touch_btn_list.size();

  Serial.print("uiBoxes:");
  Serial.println(count);
  Serial.print("touch_btn_list:");
  Serial.println(count2);

  // uiID = 6;
  // btnID = 9;
}