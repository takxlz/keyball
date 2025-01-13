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
    KC_ESC           , LT(4,KC_Q)  , KC_W  , KC_E  , KC_R  , KC_T  ,          KC_Y  , KC_U  , KC_I     , KC_O    , KC_P           , KC_BSPC  ,
    LCTL_T(KC_TAB)   , LT(1,KC_A)  , KC_S  , KC_D  , KC_F  , KC_G  ,          KC_H  , KC_J  , KC_K     , KC_L    , LT(1,KC_SCLN)  , RCTL_T(KC_ENT)  ,
    KC_LSFT          , LT(3,KC_Z)  , KC_X  , KC_C  , KC_V  , KC_B  ,          KC_N  , KC_M  , KC_COMM  , KC_DOT  , LT(3,KC_MINS)  , RSFT_T(KC_INT1)  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    KC_LALT  , KC_LGUI       , LT(1,KC_LNG2) , KC_SPC  , LT(2,KC_ENT) ,          LT(2,KC_BSPC)  , LT(1,KC_LNG1)    , XXXXXXX  , XXXXXXX     , KC_RGUI
  ),

  // 数字、記号レイヤー（スクロールモード）
  [1] = LAYOUT_universal(
    _______  , KC_1     , KC_2     , KC_3     , KC_4     , KC_5 ,             KC_6     , KC_7  , KC_8  , KC_9  , KC_0     , KC_LBRC  ,
    _______  , _______  , _______  , KC_ASTR  , KC_LPRN  , _______ ,          KC_0     , KC_4  , KC_5  , KC_6  , KC_QUOT  , KC_EQL  ,
    _______  , _______  , _______  , KC_RBRC  , KC_NUHS  , _______ ,          _______  , KC_1  , KC_2  , KC_3  , KC_SLSH  , RSFT_T(KC_INT3)  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , _______  ,              _______  , _______        , XXXXXXX , XXXXXXX        , _______
  ),

  // 移動レイヤー
  [2] = LAYOUT_universal(
    _______  , _______  , _______  , _______  , _______  , _______  ,          KC_HOME  , KC_END   , KC_PGUP  , KC_PGDN  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,          KC_LEFT  , KC_DOWN  , KC_UP    , KC_RGHT  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,          _______  , _______  , _______  , _______  , _______  , _______  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , _______  ,               _______  , _______        , XXXXXXX , XXXXXXX        , _______
  ),

  // マウスレイヤー
  [3] = LAYOUT_universal(
    _______  , _______  , _______  , _______  , _______  , _______  ,          _______  , _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,          KC_BTN4  , KC_BTN1  , KC_BTN2  , KC_BTN5  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,          _______  , _______  , _______  , _______  , _______  , _______  ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , _______  ,               _______  , _______        , XXXXXXX , XXXXXXX        , _______
  ),

  // 設定、ファンクションレイヤー
  [4] = LAYOUT_universal(
    _______  , _______  , _______   , _______  , KBC_RST  , _______  ,          KC_F10  , KC_F7  , KC_F8  , KC_F9  , KC_PSCR  , KC_DEL ,
    _______  , AML_TO  , KBC_SAVE   , _______  , _______  , _______  ,          KC_F11  , KC_F4  , KC_F5  , KC_F6  , _______  , _______ ,
    _______  , _______  , _______   , _______  , _______  , _______  ,          KC_F12  , KC_F1  , KC_F2  , KC_F3  , _______  , _______ ,
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    _______  , _______       , _______  ,  _______  , _______  ,                _______  , _______        , XXXXXXX , XXXXXXX        , _______
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 1
    keyball_set_scroll_mode(get_highest_layer(state) == 1);
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