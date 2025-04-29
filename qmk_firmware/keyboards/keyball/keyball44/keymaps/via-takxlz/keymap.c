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
  _NS,       // 数字・記号
  _NSS,      // 数字・記号（サブ）
  _MV,       // 移動
  _FN,       // ファンクション
};

// カスタムキーコード
enum custom_keycodes {
    TK_FWRD = SAFE_RANGE, // 進む
    TK_BACK,              // 戻る
    TK_MCTL,              // ミッションコントロール
    TK_COLN,              // コロン
};

// タップダンス
#ifdef TAP_DANCE_ENABLE
// TDキーコード定義
enum {
  TD_LANG = 0, // 言語切替  ※使用していない
  TD_ENNS,     // 英数、数字・記号
  TD_JPNS,     // かな、数字・記号
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

// [TD_ENNS]
void td_enns_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
 if (td_state == SINGLE_HOLD) {
    layer_on(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_NSS);
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG2);
      tap_code(KC_F13);
    } else {
      tap_code(KC_LNG2);
    }
  }
}
void td_enns_reset(tap_dance_state_t *state, void *user_data) {
  if (td_state == SINGLE_HOLD) {
    layer_off(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_NSS);
  }
  td_state = 0;
}

// [TD_JPNS]
void td_jpns_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  if (td_state == SINGLE_HOLD) {
    layer_on(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_NSS);
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG1);
      tap_code(KC_F14);
    } else {
      tap_code(KC_LNG1);
    }
  }
}
void td_jpns_reset(tap_dance_state_t *state, void *user_data) {
  if (td_state == SINGLE_HOLD) {
    layer_off(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_NSS);
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
    [TD_ENNS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_enns_finished, td_enns_reset),
    [TD_JPNS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_jpns_finished, td_jpns_reset),
    [TD_MBTN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_mbtn_finished, td_mbtn_reset),
};
#endif


/************************************************************ キー割り当て ************************************************************/
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // デフォルト
  [_BASE] = LAYOUT_universal(
    KC_ESC         , KC_Q , KC_W , KC_E , KC_R , KC_T ,                 KC_Y , KC_U , KC_I    , KC_O   , KC_P            , KC_BSPC                 ,
    KC_LCTL        , KC_A , KC_S , KC_D , KC_F , KC_G ,                 KC_H , KC_J , KC_K    , KC_L   , KC_MINS/* -= */ , KC_ENT                  ,
    LSFT_T(KC_TAB) , KC_Z , KC_X , KC_C , KC_V , KC_B ,                 KC_N , KC_M , KC_COMM , KC_DOT , KC_SLSH/* /? */ , RSFT_T(KC_INT1)/* \_ */ ,
    KC_LALT , KC_LGUI , /***/ TD(TD_ENNS) , KC_SPC , TD(TD_MBTN) ,      MO(_MV) , TD(TD_JPNS) , XXXXXXX , XXXXXXX  , LT(_MV, KC_BTN1)
  ),

  // マウス
  [_MOUSE] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,           _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,           KC_BTN4 , KC_BTN1 , KC_BTN2 , KC_BTN5 , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,           _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , /***/ _______ , _______ , _______ ,               _______ , _______ , XXXXXXX , XXXXXXX , _______
  ),

  // 数字・記号（スクロールモード）
  [_NS] = LAYOUT_universal(
    _______ , _______ , S(KC_8)/* ( */    , S(KC_9)/* ) */    , S(KC_2)/* " */  , _______ ,         S(KC_SCLN)/* + */  , KC_7/* ' */ , KC_8/* ( */ , KC_9/* ) */     , KC_EQL/* ^~ */ , KC_BSPC          ,
    _______ , _______ , KC_RBRC/* [{ */   , KC_NUHS/* ]} */   , S(KC_7)/* ' */  , _______ ,         S(KC_QUOT)/* * */  , KC_4/* $ */ , KC_5/* % */ , KC_6/* & */     , KC_MINS/* -= */ , TK_COLN/* ;: */ ,
    _______ , _______ , S(KC_RBRC)/* { */ , S(KC_NUHS)/* } */ , KC_LBRC/* @` */ , _______ ,         KC_0 , KC_1/* ! */ , KC_2/* " */ , KC_3/* # */ , KC_SLSH/* /? */ , KC_INT1/* \_ */ ,
    _______ , _______ , /***/ MO(_FN) , _______ , _______ ,                                                _______ , MO(_FN) , XXXXXXX , XXXXXXX , KC_INT3/* ¥| */
  ),

  // 数字・記号（サブ）（スクロールモード）
  [_NSS] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,        _______ , _______ , _______ , _______          , _______         , _______         ,
    _______ , _______ , _______ , _______ , _______ , _______ ,        _______ , _______ , _______ , _______          , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______  ,        _______ , _______ , _______ , _______ , _______         , _______         ,
    _______ , _______ , /***/ _______ , _______ , _______ ,            _______ , _______ , XXXXXXX , XXXXXXX , _______
  ),

  // 移動（スクロールモード）
  [_MV] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,           KC_PGUP , KC_HOME , KC_END  , KC_PGDN , _______ , KC_DEL  ,
    _______ , _______ , _______ , TK_BACK , TK_FWRD , _______ ,           KC_LEFT , KC_DOWN , KC_UP   , KC_RGHT , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,           _______ , _______ , _______ , _______ , _______ , KC_EJCT ,
    _______ , _______ , /***/ _______ , _______ , _______ ,               _______ , _______ , XXXXXXX , XXXXXXX , _______
  ),

  // FN（スクロールモード）
  [_FN] = LAYOUT_universal(
    _______ , _______ , _______  , _______ , KBC_RST , _______ ,       KC_F10 , KC_F7 , KC_F8 , KC_F9 , KC_PSCR , KC_DEL  ,
    _______ , AML_TO  , KBC_SAVE , _______ , TK_MCTL , _______ ,       KC_F11 , KC_F4 , KC_F5 , KC_F6 , _______ , _______ ,
    _______ , _______ , _______  , _______ , _______ , _______ ,       KC_F12 , KC_F1 , KC_F2 , KC_F3 , _______ , _______ ,
    _______ , _______ , /***/  _______ , _______ , _______ ,           _______ , _______ , XXXXXXX , XXXXXXX , _______
  ),

};
// clang-format on


/************************************************************ カスタムキー ************************************************************/
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // 進む
  if (keycode == TK_FWRD && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      register_code(KC_LALT);
      tap_code(KC_RGHT);
      unregister_code(KC_LALT);
    } else {
      register_code(KC_LGUI);
      tap_code(KC_RGHT);
      unregister_code(KC_LGUI);
    }
    return false;
  }
  // 戻る
  if (keycode == TK_BACK && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      register_code(KC_LALT);
      tap_code(KC_LEFT);
      unregister_code(KC_LALT);
    } else {
      register_code(KC_LGUI);
      tap_code(KC_LEFT);
      unregister_code(KC_LGUI);
    }
    return false;
  }
  // ミッションコントロール
  if (keycode == TK_MCTL && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      register_code(KC_LCTL);
      register_code(KC_LALT);
      tap_code(KC_TAB);
      unregister_code(KC_LALT);
      unregister_code(KC_LCTL);
    } else {
      register_code(KC_LCTL);
      tap_code(KC_UP);
      unregister_code(KC_LCTL);
    }
    return false;
  }
  // コロン
  if (keycode == TK_COLN && record -> event.pressed) {
    bool shift = get_mods() & MOD_MASK_SHIFT;
    if (shift) {
      uint8_t mods = get_mods();    // 現在のModifierの状態を保存
      del_mods(MOD_MASK_SHIFT);     // Shiftを外す
      tap_code(KC_QUOT);            // アスタリスク（*）ではなくコロン（:）を送る
      set_mods(mods);               // 元のModifier状態に戻す
    } else {
      tap_code(KC_SCLN);
    }
    return false;
  }
  return true;
};


layer_state_t layer_state_set_user(layer_state_t state) {
    // スクロールモード
    // NOTE: マウスレイヤーよりも後ろのレイヤーを設定すること
    // オートマウスとスクロールモードのレイヤーが同時に有効になると後ろのレイヤーが優先させるため、スクロールモードが前にあるとスクロールされない
    keyball_set_scroll_mode(get_highest_layer(state) >= 2);
    return state;
}