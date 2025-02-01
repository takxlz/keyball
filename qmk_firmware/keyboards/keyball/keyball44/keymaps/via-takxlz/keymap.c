#include QMK_KEYBOARD_H
#include "quantum.h"
#ifdef OS_DETECTION_ENABLE
  #include "os_detection.h"
#endif

/************************************************************ OLED ************************************************************/
#ifdef OLED_ENABLE
# include "lib/oledkit/oledkit.h"
void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif


/************************************************************ 各種定義 ************************************************************/
// レイヤー
enum {
  _BASE = 0, // ベース（QWERTY）
  _MOUSE,    // マウス
  _NUM,      // 数字
  _SYM,      // 記号
  _MV,       // 移動
  _FN,       // ファンクション
};

// カスタムキーコード
// enum custom_keycodes {
//     TK_LNG1 = SAFE_RANGE, // かな
//     TK_LNG2,              // 英数
// };

// タップダンス
#ifdef TAP_DANCE_ENABLE
// TDキーコード定義
enum {
  TD_LANG = 0, // 言語切替
  TD_NMSM,     // 数字・記号切替
  TD_MBTN,     // マウス
};

// TD種別
enum {
  SINGLE_TAP = 1,    // タップ
  SINGLE_HOLD,       // ホールド
  DOUBLE_TAP,        // ダブルタップ
  SINGLE_TAP_HOLD,   // タップ＆ホールド
  DOUBLE_SINGLE_TAP, // appleのようなpp入力とダブルタップを区別
};
#endif

/************************************************************ タップダンス ************************************************************/
#ifdef TAP_DANCE_ENABLE
// TD状態
static int td_state = 0;

// TD種別判定関数
int cur_dance(tap_dance_state_t *state) {
  if (state->count ==1) {
    if (!state->pressed) return SINGLE_TAP;
    else return SINGLE_HOLD;
  }
  else if (state->count == 2) {
    if (state->interrupted) return DOUBLE_SINGLE_TAP;
    else if (state->pressed) return SINGLE_TAP_HOLD;
    else return DOUBLE_TAP;
  }
  else return 9;
}

// [TD_LANG]
void td_lang_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  // 英数
  if (td_state == SINGLE_TAP || td_state == SINGLE_HOLD) {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG2);
      tap_code(KC_F13);
    } else {
      tap_code(KC_LNG2);
    }
  // かな
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG1);
      tap_code(KC_F14);
    } else {
      tap_code(KC_LNG1);
    }
  }
}
void td_lang_reset(tap_dance_state_t *state, void *user_data) {
  td_state = 0;
}

// [TD_NMSM]
void td_nmsm_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  // 数字レイヤー
  if (td_state == SINGLE_HOLD) {
    layer_on(_NUM);
  // 記号レイヤー
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_SYM);
  }
}
void td_nmsm_reset(tap_dance_state_t *state, void *user_data) {
  // 数字レイヤー
  if (td_state == SINGLE_HOLD) {
    layer_off(_NUM);
  // 記号レイヤー
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_SYM);
  }
  td_state = 0;
}

// [TD_MBTN]
void td_mbtn_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  // マウスレイヤー
  if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_MOUSE);
  // マウスボタン1
  } else if (td_state == SINGLE_HOLD) {
    register_code(KC_BTN1);
  } else if (td_state == SINGLE_TAP) {
    tap_code(KC_BTN1);
  } else if (td_state == DOUBLE_TAP) {
    tap_code(KC_BTN1);
    tap_code(KC_BTN1);
  }
}
void td_mbtn_reset(tap_dance_state_t *state, void *user_data) {
  // マウスレイヤー
  if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_MOUSE);
  } else if (td_state == SINGLE_HOLD) {
    unregister_code(KC_BTN1);
  }
  td_state = 0;
}

// TD割り当て
tap_dance_action_t tap_dance_actions[] = {
    [TD_LANG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lang_finished, td_lang_reset),
    [TD_NMSM] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_nmsm_finished, td_nmsm_reset),
    [TD_MBTN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_mbtn_finished, td_mbtn_reset),
};
#endif


/************************************************************ キー割り当て ************************************************************/
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // デフォルト
  [_BASE] = LAYOUT_universal(
    KC_ESC         , KC_Q , KC_W , KC_E , KC_R , KC_T ,               KC_Y  , KC_U , KC_I    , KC_O   , KC_P            , KC_BSPC                 ,
    LCTL_T(KC_TAB) , KC_A , KC_S , KC_D , KC_F , KC_G ,               KC_H  , KC_J , KC_K    , KC_L   , KC_MINS/* -= */ , KC_ENT                  ,
    KC_LSFT        , KC_Z , KC_X , KC_C , KC_V , KC_B ,               KC_N  , KC_M , KC_COMM , KC_DOT , KC_SLSH/* /? */ , RSFT_T(KC_INT1)/* \_ */ ,
    KC_LALT , KC_LGUI     , TD(TD_NMSM) , KC_SPC , TD(TD_MBTN) ,      MO(_MV) , TD(TD_LANG) , XXXXXXX , XXXXXXX  , LT(_MV, KC_BTN1)
  ),

  // マウス
  [_MOUSE] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,          _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,          KC_BTN4 , KC_BTN1 , KC_BTN2 , KC_BTN5 , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,          _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______           , _______ , _______ , _______ ,          _______ , _______ , XXXXXXX , XXXXXXX , _______
  ),

  // 数字（スクロールモード）
  [_NUM] = LAYOUT_universal(
    _______ , _______ , _______ , _______  , _______ , _______ ,         S(KC_2)/* " */    , KC_7/* ' */       , KC_8/* ( */       , KC_9/* ) */     , KC_EQL/* ^~ */  , KC_BSPC          ,
    _______ , _______ , KC_LSFT , MO(_SYM) , MO(_FN) , _______ ,         S(KC_7)/* ' */    , KC_4/* $ */       , KC_5/* % */       , KC_6/* & */     , KC_SCLN/* ;+ */ , KC_QUOT/* :* */  ,
    _______ , _______ , _______ , _______  , _______ , KC_SPC  ,         KC_LBRC/* @` */   , KC_1/* ! */       , KC_2/* " */       , KC_3/* # */     , KC_SLSH/* /? */ , KC_EJCT          ,
    _______ , _______           , _______  , _______ , _______ ,         C(KC_UP) , KC_0   , XXXXXXX , XXXXXXX , _______
  ),

  // 記号（スクロールモード）
  [_SYM] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,          S(KC_2)/* " */    , S(KC_8)/* ( */    , S(KC_9)/* ) */    , KC_MINS/* -= */  , KC_EQL/* ^~ */  , KC_BSPC         ,
    _______ , _______ , KC_LSFT , _______ , _______ , _______ ,          S(KC_7)/* ' */    , KC_RBRC/* [{ */   , KC_NUHS/* ]} */   , S(KC_6)/* & */   , KC_SCLN/* ;+ */ , KC_QUOT/* :* */ ,
    _______ , _______ , _______ , _______ , _______ , KC_SPC  ,          S(KC_LBRC)/* ` */ , S(KC_RBRC)/* { */ , S(KC_NUHS)/* } */ , KC_INT3/* ¥| */ , KC_SLSH/* /? */ , KC_INT1/* \_ */ ,
    _______ , _______           , _______ , _______ , _______ ,          C(KC_UP) , _______ , XXXXXXX , XXXXXXX , _______
  ),

  // 移動（スクロールモード）
  [_MV] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,          KC_PGUP , KC_HOME , KC_END  , KC_PGDN , _______ , KC_DEL  ,
    _______ , _______ , _______ , _______ , _______ , _______ ,          KC_LEFT , KC_DOWN , KC_UP   , KC_RGHT , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,          _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______           , _______ , _______ , _______ ,          _______ , _______ , XXXXXXX , XXXXXXX , _______
  ),

  // FN（スクロールモード）
  [_FN] = LAYOUT_universal(
    _______ , _______ , _______  , _______ , KBC_RST , _______ ,          KC_F10 , KC_F7 , KC_F8 , KC_F9 , KC_PSCR , KC_DEL  ,
    _______ , AML_TO  , KBC_SAVE , _______ , _______ , _______ ,          KC_F11 , KC_F4 , KC_F5 , KC_F6 , _______ , _______ ,
    _______ , _______ , _______  , _______ , _______ , _______ ,          KC_F12 , KC_F1 , KC_F2 , KC_F3 , _______ , _______ ,
    _______ , _______            , _______ , _______ , _______ ,          _______ , _______ , XXXXXXX , XXXXXXX , _______
  ),

};
// clang-format on


/************************************************************ カスタムキー ************************************************************/
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  return true;
};


layer_state_t layer_state_set_user(layer_state_t state) {
    // スクロールモード
    // NOTE: マウスレイヤーよりも後ろのレイヤーを設定すること
    // オートマウスとスクロールモードのレイヤーが同時に有効になると後ろのレイヤーが優先させるため、スクロールモードが前にあるとスクロールされない
    keyball_set_scroll_mode(get_highest_layer(state) >= 2);
    return state;
}