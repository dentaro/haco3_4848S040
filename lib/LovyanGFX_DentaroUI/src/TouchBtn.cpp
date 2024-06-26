#include "TouchBtn.hpp"
#include "LovyanGFX_DentaroUI.hpp"
using namespace std;
//TouchBtn::TouchBtn(LGFX& _lcd)
//{
//// lcd = _lcd;
//}

TouchBtn::TouchBtn()
{
}

//TouchBtn::TouchBtn(LGFX& _lcd): lcd(_lcd) {lcd = _lcd;}

void TouchBtn::setlayoutSpritePos(lgfx::v1::touch_point_t _layoutSpritePos)
{
  layoutSpritePos = _layoutSpritePos;
}

void TouchBtn::initBtn(int _btnID, String _btnIDlabel, int _b_x, int _b_y, int _b_w, int _b_h, String _btn_name,
lgfx::v1::touch_point_t _lcdPos,
int _btn_mode)
{
  btnID = _btnID;
  btnIDlabel = _btnIDlabel;
  uiSpritePos = _lcdPos;
  b_x = _b_x;
  b_y = _b_y;
  b_w = _b_w;
  b_h = _b_h;
  b_hw = b_w/2;
  b_hh = b_h/2;
  btn_name = _btn_name;
  btn_mode = _btn_mode;
  // layoutSpritePos = _layoutSpritePos;
}

void TouchBtn::setOBtnPos( int _b_x, int _b_y){
  b_x = _b_x;
  b_y = _b_y;
}

void TouchBtn::initOBtn(int _btnID, String _btnIDlabel, int _btnNo, int _btns_starAngle, int _b_x, int _b_y, int _b_r0, int _b_r1, int _b_n, String _btn_name,
lgfx::v1::touch_point_t _layoutSpritePos,
lgfx::v1::touch_point_t _lcdPos,
int _btn_mode)
{


  btnID = _btnID;
  btnIDlabel = _btnIDlabel;
  btnNo = _btnNo;
  btns_starAngle = _btns_starAngle;
  uiSpritePos = _lcdPos;
  b_x = _b_x;
  b_y = _b_y;

    b_r0 = _b_r0;
    b_r1 = _b_r1;
    b_n = _b_n;

  b_a = 360/_b_n;

  b_a0 = (b_a * btnNo + btns_starAngle)%360;
  b_a1 = b_a0 + b_a;

  b_hw = b_w/2;
  b_hh = b_h/2;

  btn_name = _btn_name;
  btn_mode = _btn_mode;
  layoutSpritePos = _layoutSpritePos;
}

void TouchBtn::initSlider( int _btnID, int _s_x, int _s_y, int _s_w, int _s_h, String _btn_name,
lgfx::v1::touch_point_t _layoutSpritePos,
lgfx::v1::touch_point_t _lcdPos,
int _xy_mode )
{
  btnID = _btnID;
  btn_mode = TOUCH_SLIDER_MODE;
  uiSpritePos = _lcdPos;
  s_x = _s_x;
  s_y = _s_y;
  s_w = _s_w;
  s_h = _s_h;
  s_hw = s_w/2;
  s_hh = s_h/2;
  btn_name = _btn_name;
  xy_mode = _xy_mode;
  sliderPosx = _s_x + (_s_w/2);
  sliderPosy = _s_x + (_s_h/2);
  layoutSpritePos = _layoutSpritePos;
}

// void TouchBtn::initTile(int _btnID,String _btn_name,
// lgfx::v1::touch_point_t _layoutSpritePos,int _layoutSprite_w, int _layoutSprite_h,
// LGFX_Sprite& _layoutSprite,
// LGFX_Sprite& _g_basic_sprite)
// {
//   btnID = _btnID;
//   btn_mode = TOUCH_TILE_MODE;
//   b_x  = _layoutSpritePos.x;
//   b_y  = _layoutSpritePos.y;
//   b_w  = _layoutSprite_w;
//   b_h  = _layoutSprite_h;
//   b_hw = _layoutSprite_w/2;
//   b_hh = _layoutSprite_h/2;
//   b_qw = _layoutSprite_w/4;
// //  b_qh = h/4;
//   btn_name = _btn_name;
//   divSprite0.setPsram( false );//DMA利用のためPSRAMは切る
//   // divSprite0.setPsram( true );
//   divSprite0.setColorDepth(COL_DEPTH);
//   divSprite0.createSprite( b_qw, _layoutSprite_h );//子スプライトメモリ確保
// }

void TouchBtn::setBtnNameFalse(String _btnNameFalse)
{
  btn_nameFalse = _btnNameFalse;
}

void TouchBtn::btnDraw(LovyanGFX&  _lcd)
{
  btnDraw(_lcd, -1, -1 );
}

void TouchBtn::btnDraw(LovyanGFX&  _lcd, int _xx, int _yy )
{
  int _x=_xx+this->b_x;
  int _y=_yy+this->b_y;
  _lcd.setTextSize(1);
  _lcd.setFont(&lgfxJapanGothicP_20);
  String drawName = "";

  if(this->btn_mode == TOUCH_TOGGLE_MODE)//トグルボタンの時
  {
    if(getToggleVal() == true){
      _lcd.fillRoundRect(_x, _y, this->b_w, this->b_h, 10, TFT_WHITE);
      _lcd.setTextColor(TFT_BLACK);
      drawName = btn_name;
      }
    else if(getToggleVal() == false){
      _lcd.fillRoundRect(_x, _y, this->b_w, this->b_h, 10, TFT_BLACK);
      _lcd.setTextColor(TFT_WHITE);
      drawName = btn_nameFalse;
      }
    b_str_hw = _lcd.textWidth(drawName)/2;
    _lcd.drawString(drawName, _x + b_hw - b_str_hw , _y + b_hh - 8);
  }
  else if(this->btn_mode == TOUCH_BTN_MODE)//普通のボタンの時
  {
    if( visibleF == true ){
      _lcd.fillRoundRect(_x, _y, this->b_w, this->b_h, 10, TFT_WHITE);
      _lcd.setTextColor(TFT_BLACK);
      //_lcd.setFont(&lgfxJapanGothicP_20);
      // drawName = btn_name;
      b_str_hw = _lcd.textWidth(btn_name)/2;
      _lcd.drawString(btn_name, _x + b_hw - b_str_hw , _y + b_hh - 8);
    }
  }
  else if(this->btn_mode == TOUCH_OBTN_MODE)//円形ボタンの時
  {
    if( visibleF == true ){
      if(_x == -1 && _y == -1)//描画座標が指定されていなければ(スプライトの場合)
      {
        _x = _lcd.width()/2;
        _y = _lcd.height()/2;
      }

      if(this->b_n > 1){//分割数が2以上なら

        _lcd.setColor(TFT_WHITE);//強制的に白にする
        _lcd.fillArc(_x, _y, this->b_r0, this->b_r1, this->b_a0, this->b_a1);

        // _lcd.setTextColor(TFT_BLACK);
        //_lcd.setFont(&lgfxJapanGothicP_20);
        // drawName = btn_name;
        b_str_hw = _lcd.textWidth(btn_name)/2;
        float middle_angle = (this->b_a0 + this->b_a1 + 0.1) /2;//角度が0にならないように0.1度を加えている//this->b_a * this->btnNo + this->b_a/2 - this->btns_starAngle;
        float middle_radius = (this->b_r0 + this->b_r1)/2;
        float str_x = cos(middle_angle/ 180.0 * M_PI) * middle_radius;
        float str_y = sin(middle_angle/ 180.0 * M_PI) * middle_radius;
        // _lcd.setTextColor(TFT_BLACK);
        // _lcd.setColor(TFT_WHITE);
        _lcd.drawString(btn_name,
                            _x + str_x + b_hw - b_str_hw-1,
                            _y + str_y + b_hh - 11 );

        _lcd.setColor(TFT_BLACK);
        _lcd.drawArc(_x, _y, this->b_r0, this->b_r1, this->b_a0, this->b_a1);

      }else if(this->b_n == 1){//分割数が1なら
        // _lcd.setColor(TFT_RED);
        _lcd.fillCircle(_x, _y,  this->b_r0);
        // _lcd.setTextColor(TFT_BLACK);
        b_str_hw = _lcd.textWidth(btn_name)/2;
        _lcd.drawString(btn_name, _x + b_hw - b_str_hw-1 , _y + b_hh - 11);

        _lcd.setColor(TFT_BLACK);
        _lcd.drawCircle(_x, _y,  this->b_r0);

      }
      else if(this->b_n == 0){//分割数が0=スライダなら

       // _lcd.setColor(TFT_RED);
        _lcd.fillCircle(_x, _y,  this->b_r0);
        // _lcd.setTextColor(TFT_BLACK);
        b_str_hw = _lcd.textWidth(btn_name)/2;
        _lcd.drawString(btn_name, _x + b_hw - b_str_hw-1 , _y + b_hh - 11);

        _lcd.setColor(TFT_BLACK);
        _lcd.drawCircle(_x, _y,  this->b_r0);

        // _lcd.setColor(TFT_WHITE);//強制的に白にする
        // _lcd.fillArc(_x, _y, this->b_r0, this->b_r1, this->b_a0, this->b_a1);

        // b_str_hw = _lcd.textWidth(btn_name)/2;
        // float middle_angle = (this->b_a0 + this->b_a1 + 0.1) /2;//角度が0にならないように0.1度を加えている//this->b_a * this->btnNo + this->b_a/2 - this->btns_starAngle;
        // float middle_radius = (this->b_r0 + this->b_r1)/2;
        // float str_x = cos(middle_angle/ 180.0 * M_PI) * middle_radius;
        // float str_y = sin(middle_angle/ 180.0 * M_PI) * middle_radius;

        // _lcd.drawString(btn_name,
        //                     _x + str_x + b_hw - b_str_hw-1,
        //                     _y + str_y + b_hh - 11 );
        // _lcd.setColor(TFT_BLACK);
        // _lcd.drawArc(_x, _y, this->b_r0, this->b_r1, this->b_a0, this->b_a1);

      }



    }
  }
  else if( this->btn_mode == TOUCH_FLICK_MODE )//フリックボタンの時
  {
    if( visibleF == true )
    {
      _lcd.fillRoundRect( _x, _y, this->b_w, this->b_h, 10, TFT_WHITE );
      _lcd.setTextColor( TFT_BLACK );
      //_lcd.setFont(&lgfxJapanGothicP_20);
      // drawName = btnIDlabel;//btn_name;
      b_str_hw = _lcd.textWidth( btn_name ) / 2;
      _lcd.drawString( btn_name, _x + b_hw - b_str_hw , _y + b_hh - 8 );
    }
  }

  if(selectBtnF){
    _lcd.drawRoundRect(_x, _y, this->b_w, this->b_h, 10, TFT_WHITE);
  }else{
    _lcd.drawRoundRect(_x, _y, this->b_w, this->b_h, 10, TFT_BLACK);
  }
}

void TouchBtn::flickDraw(LovyanGFX& _lcd, int _x, int _y )
{
  _lcd.setTextSize(1);
  String drawName = "";

  _lcd.fillRoundRect( _x, _y, this->b_w, this->b_h, 10, TFT_WHITE );

  _lcd.fillRoundRect(
          _x,
          _y,
          48,//flick_touch_btn_list[i]->getBtnSize().w,
          48,//flick_touch_btn_list[i]->getBtnSize().h,
          10, TFT_WHITE);
  _lcd.setTextColor( TFT_BLACK );
  drawName = btn_name;
  b_str_hw = _lcd.textWidth( drawName ) / 2;
  _lcd.drawString( drawName, b_x + b_hw - b_str_hw , b_y + b_hh - 4 );
}

void TouchBtn::sliderDraw(LovyanGFX& _lcd, lgfx::v1::touch_point_t _tp, int _x, int _y)
{
  _x = _x+this->s_x;
  _y = _y+this->s_y;
  _lcd.fillRect(_x, _y, this->s_w, this->s_h, TFT_BLACK);
  _lcd.setTextSize(1);
  _lcd.setTextColor(TFT_WHITE);
  this->sliderPosx = _x + sliderValx * this->s_w;
  this->sliderPosy = _y + sliderValy * this->s_h;

  _lcd.fillCircle(this->sliderPosx,
                         this->sliderPosy, 5, TFT_WHITE);

  _lcd.drawRoundRect(_x, _y, this->s_w, this->s_h, 10, TFT_WHITE);
  if( xy_mode == Y_VAL||xy_mode==XY_VAL ){
    //_lcd.drawString(String(sliderValy), _x + 4 , _y + 16);
    _lcd.drawLine(_x,
                      this->sliderPosy,
                      _x + this->s_w,
                      this->sliderPosy, TFT_WHITE);
  }
  if( xy_mode == X_VAL||xy_mode==XY_VAL ){
    //_lcd.drawString(String(sliderValx), _x + 4 , _y + 4);
    _lcd.drawLine(sliderPosx,
                      _y,
                      sliderPosx,
                      _y + this->s_h , TFT_WHITE);
  }
  //_lcd.drawLine(_x, _x, _x + this->s_w, _x, TFT_WHITE);

  if( btn_mode == TOUCH_BTN_MODE ){
    b_str_hw = _lcd.textWidth(btn_name)/2;
    _lcd.drawString(btn_name, _x + this->s_hw - b_str_hw , _y + this->s_hh -4);
  }


}

void TouchBtn::setSelectBtnF(bool _selectBtnF){
  selectBtnF = _selectBtnF;
}

void TouchBtn::tileDraw(LovyanGFX& _lgfx, LGFX_Sprite& _layoutSprite,
lgfx::v1::touch_point_t _layoutSpritePos, lgfx::v1::touch_point_t _sp,
uint8_t _bgColIndex, LGFX_Sprite& _g_basic_sprite)
{

  // Serial.println("aliveccc");

  // _layoutSprite.setColorDepth(TILE_COL_DEPTH);//親スプライト16
  //_layoutSprite.fillScreen(_bgColIndex);
  // divSprite0.setColorDepth(TILE_COL_DEPTH);//親スプライト16

  // _layoutSprite.setColorDepth(TILE_COL_DEPTH);//親スプライト16
  // divSprite0.setColorDepth(TILE_COL_DEPTH);//親スプライト16

  _g_basic_sprite.setPivot(_g_basic_sprite.width()/2.0, _g_basic_sprite.height()/2.0);//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

//  divSprite0.fillScreen(_bgColIndex);

  _g_basic_sprite.pushSprite(&divSprite0, tilePos.x, tilePos.y);
  divSprite0.pushSprite(&_layoutSprite, 0, 0);//親を指定して出力

//  divSprite0.fillScreen(_bgColIndex);

  _g_basic_sprite.pushSprite(&divSprite0, tilePos.x-b_qw, tilePos.y);
  divSprite0.pushSprite(&_layoutSprite, b_qw, 0);//親を指定して出力

//  divSprite0.fillScreen(_bgColIndex);

  _g_basic_sprite.pushSprite(&divSprite0, tilePos.x-b_qw*2, tilePos.y);
  divSprite0.pushSprite(&_layoutSprite, b_qw*2, 0);//親を指定して出力

  //divSprite0.fillScreen(_bgColIndex);

  _g_basic_sprite.pushSprite(&divSprite0, tilePos.x-b_qw*3, tilePos.y);
  divSprite0.pushSprite(&_layoutSprite, b_qw*3, 0);//親を指定して出力
}

void TouchBtn::setAvailableF(bool _availableF){
  availableF = _availableF;
}

bool TouchBtn::getAvailableF(){
  return availableF;
}

void TouchBtn::setVisibleF(bool _visibleF){
  visibleF = _visibleF;
}

bool TouchBtn::getVisibleF(){
  return visibleF;
}

int TouchBtn::getBtnMode(){
  return btn_mode;
}

void TouchBtn::addHandler(DelegateBase2* _func){
  lim2.push_back(_func);
}

void TouchBtn::delHandlers2(){
      std::list<DelegateBase2*>::iterator ite2 =  lim2.begin();
      while( ite2 != lim2.end() )
      {
        // DelegateBase2 *ptr = (*ite2);
        lim2.pop_front();//
      }
}

// id TouchBtn::run2(int _btnID, int _btnNo, lgfx::v1::touch_point_t _sp, lgfx::v1::touch_point_t _tp, int _eventState, int _runEventNo)
// {
//   run2(_btnID, _btnNo, _sp.x, _sp.y, _tp.x, _tp.y, _eventState, _runEventNo);
// }

// void TouchBtn::run2(int _btnID, int _btnNo, int _sx, int _sy, int _tx, int _ty, int _eventState, int _runEventNo){
//       sp.x = _sx;
//       sp.y = _sy;
//       tp.x = _tx;
//       tp.y = _ty;

//       eventState = _eventState;
//       runEventNo = _runEventNo;

//       std::list<DelegateBase2*>::iterator ite2 =  lim2.begin();
//       while( ite2 != lim2.end() )
//       {
//         DelegateBase2 *ptr = (*ite2);


//         if(btn_mode == TOUCH_TILE_MODE){
//           if(sp.x > b_x
//           && sp.x < b_x + b_w
//           && sp.y > b_y
//           && sp.y < b_y + b_h){//ボタンの領域内に入っていれば
//             (*ptr)( _btnID );    // 関数を実行！
//           }
//         }
//         else if(btn_mode == TOUCH_BTN_MODE||btn_mode == TOUCH_TOGGLE_MODE)
//         {//普通のボタン用の判定
//           if(sp.x > b_x     + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.x < b_x+b_w + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.y > b_y     + layoutSpritePos.y + this->uiSpritePos.y
//           && sp.y < b_y+b_h + layoutSpritePos.y + this->uiSpritePos.y)
//           {//ボタンの領域内に入っていれば
//           if(availableF == true){
//             // this->switchToggleVal();
//             (*ptr)( _btnID );    // 関数を実行！
//           }
//             // if(eventState == runEventNo)this->switchToggleVal();
//             // if(eventState == runEventNo && eventState != NO_EVENT)this->switchToggleVal();
//           }
//         }
//         else if(btn_mode == TOUCH_OBTN_MODE)
//         {//円形ボタンの領域判定
//           // if(sp.x > b_x     + layoutSpritePos.x + this->uiSpritePos.x
//           // && sp.x < b_x+b_w + layoutSpritePos.x + this->uiSpritePos.x
//           // && sp.y > b_y     + layoutSpritePos.y + this->uiSpritePos.y
//           // && sp.y < b_y+b_h + layoutSpritePos.y + this->uiSpritePos.y)

//           int d = sqrt( ( pow( sp.x- this->b_x, 2.0 ) + pow( sp.y- this->b_y, 2.0 ) ));
//           int a = int((atan2(sp.y- this->b_y, sp.x- this->b_x)*180/M_PI ));
//           if(a<0) a += 360;//マイナス値をなくす
//           int div_a = (this->b_a * this->btnNo);

//           if(d > this->b_r1 && d < this->b_r0)
//           {//円形ボタンの領域内に入っていれば
//             if((360 + a -this->btns_starAngle)%360 > div_a && (360 + a -this->btns_starAngle)%360 < div_a + this->b_a)
//             {//分割角と一致する角度であれば
//             if(availableF == true){
//               (*ptr)( _btnID );    // 関数を実行！
//             }
//           }
//           }
//         }
//         else if(btn_mode == TOUCH_FLICK_MODE)//FLICK_MODEもこちらで処理
//         {//普通のボタン用の判定
//           if(sp.x > b_x     + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.x < b_x+b_w + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.y > b_y     + layoutSpritePos.y + this->uiSpritePos.y
//           && sp.y < b_y+b_h + layoutSpritePos.y + this->uiSpritePos.y)
//           {//ボタンの領域内に入っていれば
//           if(availableF == true){
//             (*ptr)( _btnID );    // 関数を実行！
//           }
//             // if(eventState == runEventNo)this->switchToggleVal();
//             // if(eventState == runEventNo && eventState != NO_EVENT)this->switchToggleVal();
//           }
//         }
//         else if(btn_mode == TOUCH_SLIDER_MODE)
//         {//スライダボタン用の判定
//           if(sp.x > this->s_x + this->uiSpritePos.x
//           && sp.x < this->s_x + this->uiSpritePos.x + this->s_w
//           && sp.y > this->s_y + this->uiSpritePos.y
//           && sp.y < this->s_y + this->uiSpritePos.y + this->s_h)
//           {//ボタンの領域内に入っていれば
//             (*ptr)( _btnID );// 関数を実行！
//             if( eventState == runEventNo||runEventNo == MULTI_EVENT ){
//                 this->sliderPosx = tp.x - uiSpritePos.x;
//                 this->sliderPosy = tp.y - uiSpritePos.y;

//                 this->sliderValx = float( tp.x - s_x - uiSpritePos.x )/s_w;
//                 this->sliderValy = float( tp.y - s_y - uiSpritePos.y )/s_h;
//             }
//             if( (tp.x - uiSpritePos.x) >= s_x && (tp.x - uiSpritePos.x) <= s_x + s_w ){
//               if( (tp.y - uiSpritePos.y) >= s_y && (tp.y - uiSpritePos.y) <= s_y + s_h ){
//                 this->sliderPosx = tp.x - uiSpritePos.x;
//                 this->sliderPosy = tp.y - uiSpritePos.y;

//                 this->sliderValx = float(tp.x - s_x - uiSpritePos.x)/s_w;
//                 this->sliderValy = float(tp.y - s_y - uiSpritePos.y)/s_h;
//               }
//             }
//           }
//         }
//         ite2++;
//         lim2.pop_front();//メソッドを実行したら削除
//       }
// }

// void TouchBtn::run2(int _btnID, int _btnNo, lgfx::v1::touch_point_t _sp, lgfx::v1::touch_point_t _tp, int _eventState, int _runEventNo)
// {
//   // this->switchToggleVal();
//   run2(_btnID, _btnNo, _sp.x, _sp.y, _tp.x, _tp.y, _eventState, _runEventNo);
// }

// void TouchBtn::run2(int _btnID, int _btnNo, int _sx, int _sy, int _tx, int _ty, int _eventState, int _runEventNo){
//       sp.x = _sx;
//       sp.y = _sy;
//       tp.x = _tx;
//       tp.y = _ty;

//       eventState = _eventState;
//       runEventNo = _runEventNo;

//       std::list<DelegateBase2*>::iterator ite2 =  lim2.begin();
//       while( ite2 != lim2.end() )
//       {
//         DelegateBase2 *ptr = (*ite2);


//         if(btn_mode == TOUCH_TILE_MODE){
//           if(sp.x > b_x
//           && sp.x < b_x + b_w
//           && sp.y > b_y
//           && sp.y < b_y + b_h){//ボタンの領域内に入っていれば
//             (*ptr)( _btnID );    // 関数を実行！
//           }
//         }
//         else if(btn_mode == TOUCH_BTN_MODE||btn_mode == TOUCH_TOGGLE_MODE)
//         {//普通のボタン用の判定
//           if(sp.x > b_x     + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.x < b_x+b_w + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.y > b_y     + layoutSpritePos.y + this->uiSpritePos.y
//           && sp.y < b_y+b_h + layoutSpritePos.y + this->uiSpritePos.y)
//           {//ボタンの領域内に入っていれば
//           if(availableF == true){
//             // this->switchToggleVal();
//             (*ptr)( _btnID );    // 関数を実行！
//           }
//             // if(eventState == runEventNo)this->switchToggleVal();
//             // if(eventState == runEventNo && eventState != NO_EVENT)this->switchToggleVal();
//           }
//         }
//         else if(btn_mode == TOUCH_OBTN_MODE)
//         {//円形ボタンの領域判定
//           // if(sp.x > b_x     + layoutSpritePos.x + this->uiSpritePos.x
//           // && sp.x < b_x+b_w + layoutSpritePos.x + this->uiSpritePos.x
//           // && sp.y > b_y     + layoutSpritePos.y + this->uiSpritePos.y
//           // && sp.y < b_y+b_h + layoutSpritePos.y + this->uiSpritePos.y)

//           int d = sqrt( ( pow( sp.x- this->b_x, 2.0 ) + pow( sp.y- this->b_y, 2.0 ) ));
//           int a = int((atan2(sp.y- this->b_y, sp.x- this->b_x)*180/M_PI ));
//           if(a<0) a += 360;//マイナス値をなくす
//           int div_a = (this->b_a * this->btnNo);

//           if(d > this->b_r1 && d < this->b_r0)
//           {//円形ボタンの領域内に入っていれば
//             if((360 + a -this->btns_starAngle)%360 > div_a && (360 + a -this->btns_starAngle)%360 < div_a + this->b_a)
//             {//分割角と一致する角度であれば
//             if(availableF == true){
//               (*ptr)( _btnID );    // 関数を実行！
//             }
//           }
//           }
//         }
//         else if(btn_mode == TOUCH_FLICK_MODE)//FLICK_MODEもこちらで処理
//         {//普通のボタン用の判定
//           if(sp.x > b_x     + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.x < b_x+b_w + layoutSpritePos.x + this->uiSpritePos.x
//           && sp.y > b_y     + layoutSpritePos.y + this->uiSpritePos.y
//           && sp.y < b_y+b_h + layoutSpritePos.y + this->uiSpritePos.y)
//           {//ボタンの領域内に入っていれば
//           if(availableF == true){
//             (*ptr)( _btnID );    // 関数を実行！
//           }
//             // if(eventState == runEventNo)this->switchToggleVal();
//             // if(eventState == runEventNo && eventState != NO_EVENT)this->switchToggleVal();
//           }
//         }
//         else if(btn_mode == TOUCH_SLIDER_MODE)
//         {//スライダボタン用の判定
//           if(sp.x > this->s_x + this->uiSpritePos.x
//           && sp.x < this->s_x + this->uiSpritePos.x + this->s_w
//           && sp.y > this->s_y + this->uiSpritePos.y
//           && sp.y < this->s_y + this->uiSpritePos.y + this->s_h)
//           {//ボタンの領域内に入っていれば
//             (*ptr)( _btnID );// 関数を実行！
//             if( eventState == runEventNo||runEventNo == MULTI_EVENT ){
//                 this->sliderPosx = tp.x - uiSpritePos.x;
//                 this->sliderPosy = tp.y - uiSpritePos.y;

//                 this->sliderValx = float( tp.x - s_x - uiSpritePos.x )/s_w;
//                 this->sliderValy = float( tp.y - s_y - uiSpritePos.y )/s_h;
//             }
//             if( (tp.x - uiSpritePos.x) >= s_x && (tp.x - uiSpritePos.x) <= s_x + s_w ){
//               if( (tp.y - uiSpritePos.y) >= s_y && (tp.y - uiSpritePos.y) <= s_y + s_h ){
//                 this->sliderPosx = tp.x - uiSpritePos.x;
//                 this->sliderPosy = tp.y - uiSpritePos.y;

//                 this->sliderValx = float(tp.x - s_x - uiSpritePos.x)/s_w;
//                 this->sliderValy = float(tp.y - s_y - uiSpritePos.y)/s_h;
//               }
//             }
//           }
//         }
//         ite2++;
//         lim2.pop_front();//メソッドを実行したら削除
//       }
// }

void TouchBtn::run2(int _btnID, int _btnNo, lgfx::v1::touch_point_t _sp, lgfx::v1::touch_point_t _tp, int _eventState, int _runEventNo)
{
  run2(_btnID, _btnNo, _sp.x, _sp.y, _tp.x, _tp.y, _eventState, _runEventNo);
}



void TouchBtn::run2(int _btnID, int _btnNo, int _sx, int _sy, int _tx, int _ty, int _eventState, int _runEventNo){
      sp.x = _sx;
      sp.y = _sy;
      tp.x = _tx;
      tp.y = _ty;

      eventState = _eventState;
      runEventNo = _runEventNo;
      
      std::list<DelegateBase2*>::iterator ite2 =  lim2.begin();
      while( ite2 != lim2.end() )
      {
        DelegateBase2 *ptr = (*ite2);
        

        // if(btn_mode == TOUCH_TILE_MODE){
          if(sp.x > b_x 
          && sp.x < b_x + b_w 
          && sp.y > b_y 
          && sp.y < b_y + b_h){//ボタンの領域内に入っていれば
            (*ptr)( _btnID );    // 関数を実行！
          }
   
        ite2++;
        lim2.pop_front();//メソッドを実行したら削除
      }
}
int TouchBtn::getBtnID(){
    return btnID;
}

void TouchBtn::setColor(uint16_t _color){
  color = _color;
}
void TouchBtn::setBgColorIndex(uint16_t _bgColorIndex){
  bgColorIndex = _bgColorIndex;
}

lgfx::v1::touch_point_t TouchBtn::getTouchPoint(int _x, int _y){
  lgfx::v1::touch_point_t tp;
  tp.x = _x;
  tp.y = _y;
  return tp;
}

void TouchBtn::switchToggleVal(){
  if(this->toggleVal == true)this->toggleVal = false;
  else if(this->toggleVal == false)this->toggleVal = true;
  else ;
}

float TouchBtn::getSliderValx(){
  return this->sliderValx;
}

float TouchBtn::getSliderValy(){
  return this->sliderValy;
}

void TouchBtn::setSliderVal(float _x, float _y){
  this->sliderValx = _x;
  this->sliderValy = _y;
}

bool TouchBtn::getToggleVal(){
  return this->toggleVal;
}

// lgfx::v1::touch_point_t TouchBtn::getTilePos(){
//   return tilePos;
// }

void TouchBtn::setTilePos(lgfx::v1::touch_point_t _pos){
  tilePos = _pos;
}

int TouchBtn::get_xy_mode(){
  return xy_mode;
}

lgfx::v1::touch_point_t TouchBtn::getBtnPos(){
  return getTouchPoint(this->b_x, this->b_y);
}


void TouchBtn::setBtnName(String _btn_name){
  btn_name = _btn_name;
}

String TouchBtn::getBtnName()
{
  return btn_name;
}


void TouchBtn::setDrawFinishF(bool _drawFinishF){
  drawFinishF = _drawFinishF;
}

bool TouchBtn::getDrawFinishF(){
  return drawFinishF;
}
