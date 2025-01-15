/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#include "quantum.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // デフォルトレイヤー
  [0] = LAYOUT_universal(
    KC_ESC ,         KC_Q  , KC_W  , KC_E  , KC_R  , KC_T  ,               KC_Y  , KC_U  , KC_I     , KC_O    , KC_P ,      KC_BSPC ,
    LCTL_T(KC_TAB) , KC_A  , KC_S  , KC_D  , KC_F  , KC_G  ,               KC_H  , KC_J  , KC_K     , KC_L    , KC_LBRC ,   RCTL_T(KC_ENT) ,
    KC_LSFT ,        KC_Z  , KC_X  , KC_C  , KC_V  , KC_B  ,               KC_N  , KC_M  , KC_COMM  , KC_DOT  , S(KC_SLSH) ,   RSFT_T(KC_INT1) ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    KC_LGUI  , KC_LALT       , MO(3) ,  LT(2,KC_SPC) , KC_BTN1 ,           KC_LNG2  , LT(1,KC_LNG1)    , XXXXXXX  , XXXXXXX     , KC_MINS
  ),

  // マウス、設定レイヤー
  [1] = LAYOUT_universal(
    _______ ,   _______  , _______   , _______  , KBC_RST  , _______  ,          _______  , _______  , _______  , _______  , _______ ,   _______  ,
    _______ ,   AML_TO   , KBC_SAVE  , _______  , _______  , _______  ,          KC_BTN4  , KC_BTN1  , KC_BTN2  , KC_BTN5  , _______ ,   _______  ,
    _______ ,   _______  , _______  , _______   , _______  , _______  ,          _______  , _______  , _______  , _______  , _______ ,   _______  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , KC_BTN2  ,               _______  , _______        , XXXXXXX , XXXXXXX        , _______
  ),

  // 数字、Fnレイヤー（スクロールモード）
  [2] = LAYOUT_universal(
    KC_F11 ,    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5   ,          KC_F6       , KC_F7       , KC_F8    , KC_F9       , KC_F10 ,    KC_F12  ,
    _______ ,   KC_1     , KC_2     , KC_3     , KC_4     , KC_5    ,          KC_6        , KC_7        , KC_8     , KC_9        , KC_0 ,      _______  ,
    _______ ,   _______  , _______  , _______  , _______  , _______ ,          S(KC_MINS)  , S(KC_SCLN)  , KC_MINS  , S(KC_QUOT)  , KC_SLSH ,   KC_INT3  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , _______  ,              _______  , MO(4)        , XXXXXXX , XXXXXXX        , _______
  ),

   // 記号レイヤー（スクロールモード）
  [3] = LAYOUT_universal(
    _______ ,     _______  , _______  , _______  , _______  , _______ ,      _______  , S(KC_LBRC)  , KC_RBRC     , KC_NUHS     , _______ ,      _______  ,
    KC_EQL ,      S(KC_1)  , S(KC_2)  , S(KC_3)  , S(KC_4)  , S(KC_5) ,      S(KC_6)  , S(KC_7)     , S(KC_8)     , S(KC_9)     , KC_QUOT ,      KC_SCLN  ,
    S(KC_EQL) ,   _______  , _______  , _______  , _______  , _______ ,      _______  , _______     , S(KC_RBRC)  , S(KC_NUHS)  , _______ ,   S(KC_INT3)  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , _______  ,              _______  , _______        , XXXXXXX , XXXXXXX        , _______
  ),

  // 移動レイヤー（スクロールモード）
  [4] = LAYOUT_universal(
    _______ ,   _______  , _______  , _______  , _______  , _______  ,          KC_PGUP  , KC_HOME   , KC_END  , KC_PGDN  , _______ ,   _______  ,
    _______ ,   _______  , _______  , _______  , _______  , _______  ,          KC_LEFT  , KC_DOWN  , KC_UP    , KC_RGHT  , _______ ,   _______  ,
    _______ ,   _______  , _______  , _______  , _______  , _______  ,          _______  , _______  , _______  , _______  , _______ ,   _______  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , _______  ,               _______  , _______        , XXXXXXX , XXXXXXX        , _______
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // スクロールモード
    // NOTE: マウスレイヤーよりも後ろのレイヤーを設定すること
    // オートマウスとスクロールモードのレイヤーが同時に有効になると後ろのレイヤーが優先させるため、スクロールモードが前にあるとスクロールされない
    keyball_set_scroll_mode(get_highest_layer(state) >= 2);
    return state;
}

#ifdef OLED_ENABLE
# include "lib/oledkit/oledkit.h"
void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif